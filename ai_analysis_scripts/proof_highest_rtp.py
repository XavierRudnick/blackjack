#!/usr/bin/env python3
"""
Dinkelbach + DP optimizer for blackjack bet ramp b(tc) to maximize RTP subject to:
- discrete bets (multiples of MIN_BET)
- min/max bet bounds
- monotone nondecreasing b(tc) in tc (optional)
- quadratic "risk" constraint: sum p(tc) * (b(tc)*m(tc))^2 <= R

Case 1 (recommended):
- e(tc) = EVPerDollar = TotalPayout(tc) / TotalMoneyWagered(tc)  (profit per $ actually wagered)
- m(tc) = TotalMoneyWagered(tc) / HandsPlayed(tc) / base_bet     (wager multiplier per $ initial bet)
- effective wager in a bucket is w(tc) = b(tc)*m(tc)

RTP(b) = 1 + E[Profit]/E[Wager]
E[Wager]  = sum p * b * m
E[Profit] = sum p * e * b * m

We optimize the ratio E[Profit]/E[Wager] via Dinkelbach:
maximize N(b)/D(b) where N = sum p*e*b*m and D = sum p*b*m.
"""

import csv
import sys
import math
from dataclasses import dataclass
from typing import List, Tuple, Optional

# -------------------------
# User constants / defaults
# -------------------------
MIN_BET = 25
MAX_BET = 2000
BET_STEP = 25

# Filtering (recommended to avoid noisy extreme buckets)
MIN_HANDS_PER_BUCKET = 1_000_000
TC_MIN = -20.0
TC_MAX = 20.0

# DP / Risk discretization
RISK_STEPS = 800  # increase for accuracy, decrease for speed (400-1200 is typical)
EPS = 1e-10
MAX_DINKELBACH_ITERS = 25

# If you want a monotone ramp b(tc) nondecreasing in tc
ENFORCE_MONOTONE = True


# -------------------------
# Example baseline bet rules
# -------------------------
def bet_piecewise(tc: float) -> int:
    """Your piecewise ramp."""
    if tc < 1.0:
        return 25
    elif tc <= 1.5:
        return 100
    elif tc < 3.0:
        return 300
    elif tc < 4.0:
        return 500
    elif tc < 5.0:
        return 1000
    elif tc < 6.0:
        return 1600
    else:
        return 2000


def bet_kelly_linear(tc: float, BR=50_000, kelly=0.75, evPerTC=0.004854187311, avgVol=1.32,
                    TC_THRESHOLD=0.5, min_bet=MIN_BET, max_bet=MAX_BET) -> int:
    """
    A simple Kelly-ish linear ramp (matches your earlier structure more than your piecewise).
    Note: this ignores an intercept; if you want one, add it.
    """
    unitSize = (BR * kelly * evPerTC) / avgVol
    eff = tc - TC_THRESHOLD
    if eff <= 0:
        return min_bet
    raw = round((unitSize * eff) / min_bet) * min_bet
    return int(max(min_bet, min(max_bet, raw)))


# -------------------------
# Data structures
# -------------------------
@dataclass
class Bucket:
    tc: float
    p: float      # probability of being in this tc bucket
    e: float      # EV per $ actually wagered
    m: float      # wager multiplier: $ wagered per $1 initial bet
    hands: int
    wagered: float


def load_buckets(csv_file: str,
                 min_hands: int = MIN_HANDS_PER_BUCKET,
                 tc_min: float = TC_MIN,
                 tc_max: float = TC_MAX) -> List[Bucket]:
    rows = []
    with open(csv_file, "r") as f:
        reader = csv.DictReader(f)
        for r in reader:
            rows.append(r)

    # Parse and filter
    parsed = []
    for r in rows:
        tc = float(r["TrueCount"])
        hands = int(float(r["HandsPlayed"]))
        if hands < min_hands:
            continue
        if not (tc_min <= tc <= tc_max):
            continue
        wagered = float(r["TotalMoneyWagered"])
        e = float(r["EVPerDollar"])  # profit per $ actually wagered (Case 1)
        # Assume base initial bet in this file is $1; otherwise divide by base_bet.
        m = wagered / hands if hands > 0 else 1.0
        parsed.append((tc, hands, wagered, e, m))

    if not parsed:
        raise ValueError("No buckets left after filtering; loosen filters.")

    # Compute p(tc) from HandsPlayed (more reliable than HandsPlayedPct rounding)
    total_hands = sum(h for _, h, _, _, _ in parsed)

    buckets = []
    for tc, hands, wagered, e, m in sorted(parsed, key=lambda x: x[0]):
        p = hands / total_hands
        buckets.append(Bucket(tc=tc, p=p, e=e, m=m, hands=hands, wagered=wagered))

    # Sanity: p sums to 1
    psum = sum(b.p for b in buckets)
    if abs(psum - 1.0) > 1e-6:
        # Renormalize just in case of float drift
        for b in buckets:
            b.p /= psum
    return buckets


# -------------------------
# Objective computations
# -------------------------
def compute_ND(buckets: List[Bucket], bets: List[int]) -> Tuple[float, float]:
    """
    Compute N = sum p*e*b*m and D = sum p*b*m for a bet schedule aligned with buckets.
    """
    N = 0.0
    D = 0.0
    for bkt, bet in zip(buckets, bets):
        w = bet * bkt.m
        D += bkt.p * w
        N += bkt.p * bkt.e * w
    return N, D


def compute_risk(buckets: List[Bucket], bets: List[int]) -> float:
    """
    Quadratic risk proxy: sum p*(b*m)^2.
    """
    R = 0.0
    for bkt, bet in zip(buckets, bets):
        w = bet * bkt.m
        R += bkt.p * (w ** 2)
    return R


# -------------------------
# Inner solve: DP for max sum c_i * b_i under risk + monotone + bounds + discrete bets
# -------------------------
def solve_inner_dp(buckets: List[Bucket],
                   lam: float,
                   risk_budget: float,
                   bet_levels: List[int],
                   enforce_monotone: bool = True,
                   risk_steps: int = RISK_STEPS) -> List[int]:
    """
    Solve:
      max sum_i c_i(lam) * b_i
      s.t. sum_i p_i * (m_i^2) * b_i^2 <= risk_budget
           b_i in bet_levels
           optional monotone nondecreasing in i (tc sorted)

    c_i(lam) = p_i*m_i*(e_i - lam)
    risk coefficient per bucket per b^2: q_i = p_i * m_i^2

    We discretize risk into 'risk_steps' bins.
    """

    n = len(buckets)
    L = len(bet_levels)
    if n == 0:
        return []

    # Precompute coefficients
    c = [bkt.p * bkt.m * (bkt.e - lam) for bkt in buckets]
    q = [bkt.p * (bkt.m ** 2) for bkt in buckets]  # risk multiplier on b^2

    # Discretize risk
    # risk_bin_size chosen so max budget maps to risk_steps
    risk_bin = risk_budget / risk_steps
    if risk_bin <= 0:
        raise ValueError("risk_budget must be > 0")

    # Precompute risk cost (in bins) and reward for each (i, j)
    # cost_ij = ceil(q_i * b^2 / risk_bin)
    # val_ij  = c_i * b
    cost = [[0] * L for _ in range(n)]
    val = [[0.0] * L for _ in range(n)]
    for i in range(n):
        for j, b in enumerate(bet_levels):
            cost_ij = int(math.ceil((q[i] * (b ** 2)) / risk_bin))
            cost[i][j] = cost_ij
            val[i][j] = c[i] * b

    # DP arrays:
    # dp[j][k] = best objective up to current i with last bet index j and risk used k
    # We'll roll i dimension to save memory.
    NEG_INF = -1e300

    dp_prev = [[NEG_INF] * (risk_steps + 1) for _ in range(L)]
    parent = [[[(-1, -1)] * (risk_steps + 1) for _ in range(L)] for _ in range(n)]
    # parent[i][j][k] = (prev_j, prev_k)

    # Initialize for i=0
    for j in range(L):
        k0 = cost[0][j]
        if k0 <= risk_steps:
            dp_prev[j][k0] = val[0][j]
            parent[0][j][k0] = (-2, -2)  # start marker

    # DP transitions
    for i in range(1, n):
        dp_cur = [[NEG_INF] * (risk_steps + 1) for _ in range(L)]
        for prev_j in range(L):
            # prune: if this row has no reachable risk states, skip
            row = dp_prev[prev_j]
            if all(x <= NEG_INF/2 for x in row):
                continue
            for prev_k in range(risk_steps + 1):
                base = row[prev_k]
                if base <= NEG_INF/2:
                    continue

                # next bet index range: enforce monotone if requested
                j_start = prev_j if enforce_monotone else 0
                for j in range(j_start, L):
                    k = prev_k + cost[i][j]
                    if k > risk_steps:
                        continue
                    cand = base + val[i][j]
                    if cand > dp_cur[j][k]:
                        dp_cur[j][k] = cand
                        parent[i][j][k] = (prev_j, prev_k)

        dp_prev = dp_cur

    # Extract best end state
    best_val = NEG_INF
    best_j = 0
    best_k = 0
    for j in range(L):
        for k in range(risk_steps + 1):
            if dp_prev[j][k] > best_val:
                best_val = dp_prev[j][k]
                best_j, best_k = j, k

    # Backtrack
    bets_idx = [0] * n
    j, k = best_j, best_k
    for i in range(n - 1, -1, -1):
        bets_idx[i] = j
        pj, pk = parent[i][j][k]
        if pj == -2 and pk == -2:
            break
        if pj < 0:
            # Shouldn't happen; fallback
            pj, pk = (0, 0)
        j, k = pj, pk

    bets = [bet_levels[idx] for idx in bets_idx]
    return bets


# -------------------------
# Dinkelbach outer loop
# -------------------------
def dinkelbach_optimize(buckets: List[Bucket],
                        baseline_bets: List[int],
                        bet_levels: List[int],
                        enforce_monotone: bool = True,
                        risk_steps: int = RISK_STEPS,
                        eps: float = EPS,
                        max_iters: int = MAX_DINKELBACH_ITERS) -> Tuple[List[int], float]:
    """
    Maximize ratio N(b)/D(b) using Dinkelbach:
      lam_{t+1} = N(b_t)/D(b_t) where b_t solves max (N(b) - lam_t D(b)) subject to constraints.
    Here "constraints" include risk_budget fixed to baseline risk.

    We set risk_budget = risk(baseline_bets) by default (apples-to-apples "same risk").
    """

    # Risk budget from baseline schedule (common & recommended)
    risk_budget = compute_risk(buckets, baseline_bets)

    # Initial lambda from baseline
    N0, D0 = compute_ND(buckets, baseline_bets)
    lam = (N0 / D0) if D0 > 0 else 0.0

    best_bets = baseline_bets[:]
    best_ratio = lam

    for it in range(max_iters):
        # Solve inner problem for current lambda
        bets = solve_inner_dp(
            buckets=buckets,
            lam=lam,
            risk_budget=risk_budget,
            bet_levels=bet_levels,
            enforce_monotone=enforce_monotone,
            risk_steps=risk_steps,
        )

        N, D = compute_ND(buckets, bets)
        if D <= 0:
            raise RuntimeError("Denominator D became non-positive; check inputs.")

        ratio = N / D
        gap = N - lam * D  # Dinkelbach optimality gap

        # Track best
        if ratio > best_ratio:
            best_ratio = ratio
            best_bets = bets

        # Convergence check
        if abs(gap) < eps:
            lam = ratio
            break

        lam = ratio

    # Return RTP = 1 + ratio
    return best_bets, 1.0 + best_ratio


# -------------------------
# Main
# -------------------------
def main():
    if len(sys.argv) != 2:
        print("Usage: python optimize_bets_dinkelbach_dp.py <ev_per_tc_csv_file>")
        sys.exit(1)

    csv_file = sys.argv[1]
    buckets = load_buckets(csv_file)

    # Discrete bet levels
    bet_levels = list(range(MIN_BET, MAX_BET + 1, BET_STEP))

    # Baseline bets: choose one (piecewise or kelly-ish)
    baseline = [bet_kelly_linear(b.tc) for b in buckets]  # change to bet_piecewise if you want
    # baseline = [bet_piecewise(b.tc) for b in buckets]

    # Baseline RTP
    N0, D0 = compute_ND(buckets, baseline)
    rtp0 = 1.0 + (N0 / D0)
    risk0 = compute_risk(buckets, baseline)
    print(f"Baseline RTP: {rtp0:.9f}   (edge={(rtp0-1)*100:.4f}%)")
    print(f"Baseline risk (sum p*(b*m)^2): {risk0:.6f}")
    print(f"Buckets used: {len(buckets)}  TC range [{buckets[0].tc}, {buckets[-1].tc}]")
    print()

    # Optimize
    opt_bets, opt_rtp = dinkelbach_optimize(
        buckets=buckets,
        baseline_bets=baseline,
        bet_levels=bet_levels,
        enforce_monotone=ENFORCE_MONOTONE,
        risk_steps=RISK_STEPS,
    )

    # Report
    N1, D1 = compute_ND(buckets, opt_bets)
    risk1 = compute_risk(buckets, opt_bets)
    print(f"Optimized RTP: {opt_rtp:.9f}   (edge={(opt_rtp-1)*100:.4f}%)")
    print(f"Optimized risk: {risk1:.6f}  (should be <= baseline risk)")
    print()

    # Show ramp (only where tc in a readable region)
    print("TC      p(tc)      e(tc)      m(tc)    baseline  optimized")
    print("-" * 68)
    for bkt, b0, b1 in zip(buckets, baseline, opt_bets):
        if -12.0 <= bkt.tc <= 12.0:
            print(f"{bkt.tc:>5.1f}  {bkt.p:>9.6f}  {bkt.e:>9.6f}  {bkt.m:>8.4f}  {b0:>8d}  {b1:>9d}")


if __name__ == "__main__":
    main()
