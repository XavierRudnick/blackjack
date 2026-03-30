#!/usr/bin/env python3
"""
Verify one-session bankroll loss for a specific hardcoded bet spread.

This script is formula-first:
- it clamps True Count data to a configurable range (default [-15, 15])
- folds tail probability into the boundary buckets
- computes per-hand mean/variance from the EV-per-TC CSV
- reports closed-form approximations for:
  - session-end ruin
  - anytime-in-session ruin (barrier hit)

It can also run a Monte Carlo check using the same per-hand moment model.

Spread format:
  "1:25,2:75,3:100,4:150,5:175,6:200"

Meaning:
  TC in (0,1]  -> 25
  TC in (1,2]  -> 75
  ...
  TC >= 6      -> 200
  TC <= 0      -> min bet
"""

from __future__ import annotations

import argparse
import csv
import math
import random
from collections import defaultdict
from typing import Dict, List

from bruteforce_bet_spread_sessions import CandidateSpread, TCBucket, simulate_candidate
from ror_analysis import (
    analyze_ror,
    compute_hand_moments,
    finite_end_ruin_probability,
    finite_trip_ruin_probability,
)


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Estimate one-session bankroll ruin for a specific hardcoded spread."
    )
    parser.add_argument("csv_file", help="EV-per-TC CSV produced by simulator")
    parser.add_argument(
        "--spread",
        required=True,
        help='Spread spec like "1:25,2:75,3:100,4:150,5:175,6:200"',
    )
    parser.add_argument("--session-bankroll", type=float, default=1000)
    parser.add_argument("--min-bet", type=float, default=25)
    parser.add_argument("--session-hands", type=int, default=80)
    parser.add_argument("--sessions", type=int, default=50000)
    parser.add_argument("--min-hands", type=int, default=5000)
    parser.add_argument("--tc-min", type=float, default=-15.0)
    parser.add_argument("--tc-max", type=float, default=15.0)
    parser.add_argument(
        "--fold-tails",
        action="store_true",
        help="Fold TC probability outside [tc-min, tc-max] into the boundary buckets",
    )
    parser.add_argument(
        "--ruin-mode",
        choices=["anytime", "end"],
        default="anytime",
        help="Monte Carlo ruin definition for the optional simulation check",
    )
    parser.add_argument("--seed", type=int, default=42)
    return parser.parse_args()


def parse_spread(spread_spec: str, min_bet: float) -> CandidateSpread:
    units_by_tc: Dict[int, int] = {}
    for item in spread_spec.split(","):
        item = item.strip()
        if not item:
            continue
        tc_text, bet_text = item.split(":", 1)
        tc = int(tc_text.strip())
        bet = float(bet_text.strip())
        units = int(round(bet / min_bet))
        if units < 1:
            raise ValueError(f"Spread entry {item!r} implies <1 unit")
        units_by_tc[tc] = units

    if not units_by_tc:
        raise ValueError("Spread cannot be empty")

    expected_keys = list(range(1, max(units_by_tc) + 1))
    missing = [k for k in expected_keys if k not in units_by_tc]
    if missing:
        raise ValueError(f"Spread is missing TC thresholds: {missing}")

    return CandidateSpread(units_by_tc=units_by_tc, min_bet=min_bet)


def load_clamped_buckets(
    path: str,
    min_hands: int,
    tc_min: float,
    tc_max: float,
    fold_tails: bool,
) -> tuple[List[TCBucket], float]:
    rows: List[tuple[float, int, float, float, float]] = []
    total_hands_all = 0
    kept_hands = 0

    with open(path, newline="") as f:
        reader = csv.DictReader(f)
        required = {
            "TrueCount",
            "HandsPlayed",
            "TotalMoneyWagered",
            "EVPerDollar",
            "StdErrorPerDollar",
        }
        missing = required.difference(reader.fieldnames or [])
        if missing:
            raise SystemExit(f"Missing required columns: {', '.join(sorted(missing))}")

        for row in reader:
            hands = int(float(row["HandsPlayed"]))
            if hands < min_hands:
                continue

            tc = float(row["TrueCount"])
            wagered = float(row["TotalMoneyWagered"])
            ev = float(row["EVPerDollar"])
            stderr = float(row["StdErrorPerDollar"])
            variance_per_dollar = max((stderr * math.sqrt(hands)) ** 2, 1e-9)

            total_hands_all += hands

            if tc < tc_min:
                if not fold_tails:
                    continue
                tc = tc_min
            elif tc > tc_max:
                if not fold_tails:
                    continue
                tc = tc_max

            kept_hands += hands
            wager_multiplier = wagered / hands if hands > 0 else 1.0
            rows.append((tc, hands, ev, wager_multiplier, variance_per_dollar))

    if kept_hands == 0:
        raise SystemExit("No usable rows after clamping/filtering.")

    grouped: Dict[float, Dict[str, float]] = defaultdict(
        lambda: {"hands": 0.0, "ev_sum": 0.0, "wm_sum": 0.0, "var_sum": 0.0}
    )
    for tc, hands, ev, wager_multiplier, variance_per_dollar in rows:
        g = grouped[tc]
        g["hands"] += hands
        g["ev_sum"] += ev * hands
        g["wm_sum"] += wager_multiplier * hands
        g["var_sum"] += variance_per_dollar * hands

    buckets = []
    for tc in sorted(grouped):
        g = grouped[tc]
        hands = g["hands"]
        buckets.append(
            TCBucket(
                tc=tc,
                probability=hands / kept_hands,
                ev_per_dollar=g["ev_sum"] / hands,
                wager_multiplier=g["wm_sum"] / hands,
                variance_per_dollar=max(g["var_sum"] / hands, 1e-9),
            )
        )

    coverage = kept_hands / total_hands_all if total_hands_all > 0 else 0.0
    return buckets, coverage

def session_end_ruin_probability(
    session_bankroll: float,
    min_bet: float,
    session_hands: int,
    mu: float,
    sigma2: float,
) -> float:
    bankroll_buffer = session_bankroll - min_bet
    return finite_end_ruin_probability(bankroll_buffer, session_hands, mu, sigma2)


def session_anytime_ruin_probability(
    session_bankroll: float,
    min_bet: float,
    session_hands: int,
    mu: float,
    sigma2: float,
) -> float:
    bankroll_buffer = session_bankroll - min_bet
    return finite_trip_ruin_probability(bankroll_buffer, session_hands, mu, sigma2)


def run_monte_carlo_check(
    spread: CandidateSpread,
    buckets: List[TCBucket],
    session_bankroll: float,
    min_bet: float,
    session_hands: int,
    sessions: int,
    ruin_mode: str,
    seed: int,
):
    mu, sigma2 = compute_hand_moments(spread, buckets)
    session_mean = session_hands * mu
    session_std = math.sqrt(session_hands * sigma2)

    from bruteforce_bet_spread_sessions import CandidateMetrics

    metrics = CandidateMetrics(
        spread=spread,
        expected_profit_per_hand=mu,
        stddev_per_hand=math.sqrt(sigma2),
        expected_profit_per_session=session_mean,
        approx_session_stddev=session_std,
        approx_ror_normal=session_end_ruin_probability(
            session_bankroll, min_bet, session_hands, mu, sigma2
        ),
        approx_trip_ror=session_anytime_ruin_probability(
            session_bankroll, min_bet, session_hands, mu, sigma2
        ),
        approx_infinite_ror=analyze_ror(
            spread=spread,
            buckets=buckets,
            bankroll_buffer=session_bankroll - min_bet,
            hands=session_hands,
        ).infinite_ror,
    )
    simulate_candidate(
        metrics=metrics,
        buckets=buckets,
        session_bankroll=session_bankroll,
        min_bet=min_bet,
        session_hands=session_hands,
        sessions=sessions,
        ruin_mode=ruin_mode,
        rng=random.Random(seed),
    )
    return metrics


def main() -> None:
    args = parse_args()
    spread = parse_spread(args.spread, args.min_bet)
    buckets, coverage = load_clamped_buckets(
        args.csv_file,
        args.min_hands,
        args.tc_min,
        args.tc_max,
        args.fold_tails,
    )

    mu, sigma2 = compute_hand_moments(spread, buckets)
    session_mean = args.session_hands * mu
    session_std = math.sqrt(args.session_hands * sigma2)
    ror = analyze_ror(
        spread=spread,
        buckets=buckets,
        bankroll_buffer=args.session_bankroll - args.min_bet,
        hands=args.session_hands,
    )
    end_ruin = session_end_ruin_probability(
        args.session_bankroll, args.min_bet, args.session_hands, mu, sigma2
    )
    anytime_ruin = session_anytime_ruin_probability(
        args.session_bankroll, args.min_bet, args.session_hands, mu, sigma2
    )
    metrics = run_monte_carlo_check(
        spread=spread,
        buckets=buckets,
        session_bankroll=args.session_bankroll,
        min_bet=args.min_bet,
        session_hands=args.session_hands,
        sessions=args.sessions,
        ruin_mode=args.ruin_mode,
        seed=args.seed,
    )

    print("=" * 80)
    print("BET SPREAD RUIN VERIFICATION")
    print("=" * 80)
    print(f"CSV:                 {args.csv_file}")
    print(f"Spread:              {args.spread}")
    print(f"TC clamp:            [{args.tc_min:.0f}, {args.tc_max:.0f}]")
    print(f"Fold tails:          {args.fold_tails}")
    print(f"Coverage used:       {coverage:.4%}")
    print(f"Session bankroll:    ${args.session_bankroll:,.0f}")
    print(f"Min bet:             ${args.min_bet:.0f}")
    print(f"Session hands:       {args.session_hands}")
    print(f"Monte Carlo runs:    {args.sessions:,}")
    print()
    print(f"Per-hand EV:         ${mu:+.4f}")
    print(f"Per-hand SD:         ${math.sqrt(sigma2):.4f}")
    print(f"Session EV:          ${session_mean:+.2f}")
    print(f"Session SD:          ${session_std:.2f}")
    print()
    print(f"Infinite RoR:        {ror.infinite_ror:.4%}")
    print(f"Formula end ruin:    {end_ruin:.4%}")
    print(f"Formula anytime:     {anytime_ruin:.4%}")
    print()
    print(f"Simulated ruin rate: {metrics.sim_ror:.4%}  ({args.ruin_mode})")
    print(f"Avg session P/L:     ${metrics.sim_avg_profit:+.2f}")
    print(f"Median session P/L:  ${metrics.sim_median_profit:+.2f}")
    print(f"P10 / P90 session:   ${metrics.sim_p10_profit:+.2f} / ${metrics.sim_p90_profit:+.2f}")


if __name__ == "__main__":
    main()
