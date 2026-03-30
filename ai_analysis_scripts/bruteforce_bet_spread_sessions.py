#!/usr/bin/env python3
"""
Brute-force blackjack bet spread search with session risk estimation.

This script searches over monotone TC->bet-unit ramps, ranks them by
expected session profit using EV-per-TC Monte Carlo output, then simulates
sessions to estimate one-session bankroll ruin probability.

The search space is intentionally practical:
- TC <= 0 always bets the table minimum
- Positive TC thresholds follow the engine-style bucket rule:
  `(0, 1] -> TC1`, `(1, 2] -> TC2`, and so on
- Integer TC buckets from `start_tc` to `top_tc - 1` are searched
- TC >= top_tc uses the last searched unit value
- Units must be non-decreasing as TC increases

This gives us an exhaustive search over a compact family of realistic bet
ramps without exploding the number of candidate spreads.
"""

from __future__ import annotations

import argparse
import csv
import math
import random
from bisect import bisect_left
from dataclasses import dataclass
from itertools import combinations_with_replacement
from typing import Dict, Iterable, List, Sequence, Tuple

from ror_analysis import analyze_ror


@dataclass(frozen=True)
class TCBucket:
    tc: float
    probability: float
    ev_per_dollar: float
    wager_multiplier: float
    variance_per_dollar: float


@dataclass(frozen=True)
class CandidateSpread:
    units_by_tc: Dict[int, int]
    min_bet: float

    def units_for_tc(self, tc: float) -> int:
        if tc <= 0:
            return 1
        bucket = math.ceil(tc)
        if bucket in self.units_by_tc:
            return self.units_by_tc[bucket]
        max_key = max(self.units_by_tc)
        if bucket >= max_key:
            return self.units_by_tc[max_key]
        # Defensive fallback for gaps; should not happen in generation.
        last_value = 1
        for key in sorted(self.units_by_tc):
            if bucket < key:
                break
            last_value = self.units_by_tc[key]
        return last_value

    def bet_for_tc(self, tc: float) -> float:
        return self.units_for_tc(tc) * self.min_bet

    def spread_label(self, start_tc: int, top_tc: int) -> str:
        parts = ["TC<=0:1"]
        for tc in range(start_tc, top_tc):
            parts.append(f"TC{tc}:{self.units_by_tc[tc]}")
        parts.append(f"TC>={top_tc}:{self.units_by_tc[top_tc - 1]}")
        return " | ".join(parts)


@dataclass
class CandidateMetrics:
    spread: CandidateSpread
    expected_profit_per_hand: float
    stddev_per_hand: float
    expected_profit_per_session: float
    approx_session_stddev: float
    approx_ror_normal: float
    approx_trip_ror: float
    approx_infinite_ror: float
    sim_ror: float = float("nan")
    sim_avg_profit: float = float("nan")
    sim_median_profit: float = float("nan")
    sim_p10_profit: float = float("nan")
    sim_p90_profit: float = float("nan")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Brute-force TC bet ramps and estimate one-session bankroll ruin."
    )
    parser.add_argument(
        "csv_file",
        help="EV-per-TC CSV produced by simulator (columns like TrueCount, HandsPlayed, EVPerDollar)",
    )
    parser.add_argument("--bankroll", type=float, default=10000, help="Total bankroll in dollars")
    parser.add_argument(
        "--session-bankroll",
        type=float,
        default=None,
        help="Session bankroll brought to the table. Defaults to --bankroll.",
    )
    parser.add_argument("--min-bet", type=float, default=25, help="Table minimum bet")
    parser.add_argument("--max-bet", type=float, default=500, help="Hard max bet")
    parser.add_argument(
        "--start-tc",
        type=int,
        default=1,
        help="First positive integer TC bucket included in the brute-force search",
    )
    parser.add_argument(
        "--top-tc",
        type=int,
        default=6,
        help="Search integer buckets up to TC=top_tc-1, with TC>=top_tc using the final bucket value",
    )
    parser.add_argument(
        "--max-units",
        type=int,
        default=12,
        help="Maximum spread size in table-minimum units",
    )
    parser.add_argument(
        "--min-hands",
        type=int,
        default=5000,
        help="Ignore sparse TC rows with fewer than this many observed hands",
    )
    parser.add_argument(
        "--session-hands",
        type=int,
        default=100,
        help="Number of hands per simulated session",
    )
    parser.add_argument(
        "--candidates-to-sim",
        type=int,
        default=25,
        help="Simulate this many top analytical candidates in detail",
    )
    parser.add_argument(
        "--sessions",
        type=int,
        default=20000,
        help="Monte Carlo sessions per detailed candidate",
    )
    parser.add_argument(
        "--target-ror",
        type=float,
        default=0.05,
        help="Ruin ceiling used when reporting the best practical spread",
    )
    parser.add_argument(
        "--ruin-mode",
        choices=["anytime", "end"],
        default="anytime",
        help="Count ruin when bankroll falls below min bet at any time, or only at session end",
    )
    parser.add_argument(
        "--seed",
        type=int,
        default=42,
        help="RNG seed for reproducible session simulation",
    )
    return parser.parse_args()


def load_tc_buckets(path: str, min_hands: int) -> List[TCBucket]:
    rows: List[Tuple[float, int, float, float, float]] = []
    total_hands = 0

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

            wager_multiplier = wagered / hands if hands > 0 else 1.0
            variance_per_dollar = (stderr * math.sqrt(hands)) ** 2

            rows.append((tc, hands, ev, wager_multiplier, variance_per_dollar))
            total_hands += hands

    if total_hands == 0:
        raise SystemExit("No usable rows after filtering; try lowering --min-hands.")

    buckets = [
        TCBucket(
            tc=tc,
            probability=hands / total_hands,
            ev_per_dollar=ev,
            wager_multiplier=wager_multiplier,
            variance_per_dollar=max(variance_per_dollar, 1e-9),
        )
        for tc, hands, ev, wager_multiplier, variance_per_dollar in sorted(rows)
    ]
    return buckets


def generate_candidate_spreads(
    start_tc: int,
    top_tc: int,
    max_units: int,
    min_bet: float,
    max_bet: float,
) -> Iterable[CandidateSpread]:
    searched_tcs = list(range(start_tc, top_tc))
    max_units_by_table = max(1, int(max_bet // min_bet))
    capped_max_units = min(max_units, max_units_by_table)

    for combo in combinations_with_replacement(range(1, capped_max_units + 1), len(searched_tcs)):
        units_by_tc = {tc: units for tc, units in zip(searched_tcs, combo)}
        yield CandidateSpread(units_by_tc=units_by_tc, min_bet=min_bet)


def approx_session_outcome(
    spread: CandidateSpread,
    buckets: Sequence[TCBucket],
    session_hands: int,
    bankroll_buffer: float,
) -> CandidateMetrics:
    ror = analyze_ror(
        spread=spread,
        buckets=buckets,
        bankroll_buffer=bankroll_buffer,
        hands=session_hands,
    )
    expected_profit_per_hand = ror.mu
    variance_per_hand = ror.sigma2
    stddev_per_hand = math.sqrt(variance_per_hand)

    expected_profit_per_session = expected_profit_per_hand * session_hands
    approx_session_stddev = stddev_per_hand * math.sqrt(session_hands)

    return CandidateMetrics(
        spread=spread,
        expected_profit_per_hand=expected_profit_per_hand,
        stddev_per_hand=stddev_per_hand,
        expected_profit_per_session=expected_profit_per_session,
        approx_session_stddev=approx_session_stddev,
        approx_ror_normal=ror.session_end_ror,
        approx_trip_ror=ror.trip_ror,
        approx_infinite_ror=ror.infinite_ror,
    )


def build_sampler(buckets: Sequence[TCBucket]) -> Tuple[List[float], Sequence[TCBucket]]:
    cumulative: List[float] = []
    running = 0.0
    for bucket in buckets:
        running += bucket.probability
        cumulative.append(running)
    cumulative[-1] = 1.0
    return cumulative, buckets


def sample_bucket(
    cumulative: Sequence[float], buckets: Sequence[TCBucket], rng: random.Random
) -> TCBucket:
    idx = bisect_left(cumulative, rng.random())
    return buckets[idx]


def simulate_candidate(
    metrics: CandidateMetrics,
    buckets: Sequence[TCBucket],
    session_bankroll: float,
    min_bet: float,
    session_hands: int,
    sessions: int,
    ruin_mode: str,
    rng: random.Random,
) -> None:
    cumulative, sampler_buckets = build_sampler(buckets)
    ruined = 0
    profits: List[float] = []

    for _ in range(sessions):
        bankroll = session_bankroll
        start_bankroll = bankroll
        ruined_this_session = False

        for _hand in range(session_hands):
            if bankroll < min_bet:
                ruined_this_session = True
                break

            bucket = sample_bucket(cumulative, sampler_buckets, rng)
            bet = min(metrics.spread.bet_for_tc(bucket.tc), bankroll)

            mean = bet * bucket.wager_multiplier * bucket.ev_per_dollar
            stddev = abs(bet * bucket.wager_multiplier) * math.sqrt(bucket.variance_per_dollar)
            outcome = rng.gauss(mean, stddev)
            bankroll += outcome

            if bankroll < min_bet:
                ruined_this_session = True
                break

        if ruin_mode == "anytime":
            if ruined_this_session:
                ruined += 1
        elif bankroll < min_bet:
            ruined += 1
        profits.append(bankroll - start_bankroll)

    profits.sort()
    metrics.sim_ror = ruined / sessions
    metrics.sim_avg_profit = sum(profits) / sessions
    metrics.sim_median_profit = percentile_from_sorted(profits, 0.50)
    metrics.sim_p10_profit = percentile_from_sorted(profits, 0.10)
    metrics.sim_p90_profit = percentile_from_sorted(profits, 0.90)


def percentile_from_sorted(values: Sequence[float], pct: float) -> float:
    if not values:
        return float("nan")
    idx = max(0, min(len(values) - 1, int(round(pct * (len(values) - 1)))))
    return values[idx]


def choose_best_practical(
    candidates: Sequence[CandidateMetrics], target_ror: float
) -> CandidateMetrics | None:
    eligible = [c for c in candidates if c.sim_ror <= target_ror]
    if not eligible:
        return None
    return max(eligible, key=lambda c: c.sim_avg_profit)


def build_shortlist(
    evaluated: Sequence[CandidateMetrics],
    candidates_to_sim: int,
    target_ror: float,
    ruin_mode: str,
) -> List[CandidateMetrics]:
    if candidates_to_sim >= len(evaluated):
        return list(evaluated)

    def analytical_ror(candidate: CandidateMetrics) -> float:
        return candidate.approx_trip_ror if ruin_mode == "anytime" else candidate.approx_ror_normal

    by_ev = sorted(
        evaluated,
        key=lambda c: (c.expected_profit_per_session, -analytical_ror(c)),
        reverse=True,
    )
    by_safety = sorted(
        evaluated,
        key=lambda c: (-analytical_ror(c), c.expected_profit_per_session),
    )
    by_target_feasible = sorted(
        [c for c in evaluated if analytical_ror(c) <= target_ror * 1.5],
        key=lambda c: c.expected_profit_per_session,
        reverse=True,
    )
    by_sharpe = sorted(
        evaluated,
        key=lambda c: (
            c.expected_profit_per_session / c.approx_session_stddev
            if c.approx_session_stddev > 0
            else float("-inf")
        ),
        reverse=True,
    )

    ordered_groups = [by_ev, by_target_feasible, by_sharpe, by_safety]
    seen = set()
    shortlist: List[CandidateMetrics] = []

    max_len = max((len(group) for group in ordered_groups), default=0)
    for idx in range(max_len):
        for group in ordered_groups:
            if idx >= len(group):
                continue
            candidate = group[idx]
            label = tuple(sorted(candidate.spread.units_by_tc.items()))
            if label in seen:
                continue
            seen.add(label)
            shortlist.append(candidate)
            if len(shortlist) >= candidates_to_sim:
                return shortlist

    return shortlist


def print_header(args: argparse.Namespace, buckets: Sequence[TCBucket], total_candidates: int) -> None:
    print("=" * 90)
    print("BRUTE-FORCE BET SPREAD SEARCH")
    print("=" * 90)
    print(f"CSV:                 {args.csv_file}")
    print(f"Bankroll:            ${args.bankroll:,.0f}")
    print(f"Session bankroll:    ${session_bankroll(args):,.0f}")
    print(f"Table min/max:       ${args.min_bet:.0f} / ${args.max_bet:.0f}")
    print(f"Session length:      {args.session_hands} hands")
    print(f"Ruin mode:           {args.ruin_mode}")
    print(f"Filtered TC buckets: {len(buckets)}")
    print(f"Brute-force space:   {total_candidates:,} spreads")
    print(f"Search buckets:      TC {args.start_tc} to {args.top_tc - 1}, TC>={args.top_tc} capped")
    print(f"Max spread:          1-{min(args.max_units, int(args.max_bet // args.min_bet))}")
    print()


def session_bankroll(args: argparse.Namespace) -> float:
    return args.bankroll if args.session_bankroll is None else args.session_bankroll


def main() -> None:
    args = parse_args()
    if args.start_tc < 1:
        raise SystemExit("--start-tc must be >= 1")
    if args.top_tc <= args.start_tc:
        raise SystemExit("--top-tc must be greater than --start-tc")
    if args.min_bet <= 0 or args.max_bet < args.min_bet:
        raise SystemExit("Invalid min/max bet values")

    buckets = load_tc_buckets(args.csv_file, args.min_hands)

    all_spreads = list(
        generate_candidate_spreads(
            start_tc=args.start_tc,
            top_tc=args.top_tc,
            max_units=args.max_units,
            min_bet=args.min_bet,
            max_bet=args.max_bet,
        )
    )
    print_header(args, buckets, len(all_spreads))

    bankroll_floor = session_bankroll(args)
    bankroll_buffer = max(0.0, bankroll_floor - args.min_bet)
    evaluated = [
        approx_session_outcome(
            spread=spread,
            buckets=buckets,
            session_hands=args.session_hands,
            bankroll_buffer=bankroll_buffer,
        )
        for spread in all_spreads
    ]

    shortlist = build_shortlist(
        evaluated=evaluated,
        candidates_to_sim=min(args.candidates_to_sim, len(evaluated)),
        target_ror=args.target_ror,
        ruin_mode=args.ruin_mode,
    )
    rng = random.Random(args.seed)
    for idx, candidate in enumerate(shortlist, start=1):
        simulate_candidate(
            metrics=candidate,
            buckets=buckets,
            session_bankroll=bankroll_floor,
            min_bet=args.min_bet,
            session_hands=args.session_hands,
            sessions=args.sessions,
            ruin_mode=args.ruin_mode,
            rng=rng,
        )
        print(
            f"Simulated {idx:>2}/{len(shortlist)}: "
            f"EV/session=${candidate.expected_profit_per_session:+.2f}, "
            f"approx {args.ruin_mode} RoR="
            f"{(candidate.approx_trip_ror if args.ruin_mode == 'anytime' else candidate.approx_ror_normal):.2%}, "
            f"sim RoR={candidate.sim_ror:.2%}, "
            f"{candidate.spread.spread_label(args.start_tc, args.top_tc)}"
        )

    print()
    print_top_results(shortlist, args)

    best = choose_best_practical(shortlist, args.target_ror)
    print()
    print("=" * 90)
    print("BEST PRACTICAL SPREAD")
    print("=" * 90)
    if best is None:
        print(f"No simulated candidate met the target ruin ceiling of {args.target_ror:.2%}.")
    else:
        print(f"Target ruin ceiling: {args.target_ror:.2%}")
        print(f"Spread:              {best.spread.spread_label(args.start_tc, args.top_tc)}")
        print(f"Avg profit/session:  ${best.sim_avg_profit:+.2f}")
        print(f"Session ruin:        {best.sim_ror:.2%}")
        print(f"Median session P/L:  ${best.sim_median_profit:+.2f}")
        print(f"P10 / P90 session:   ${best.sim_p10_profit:+.2f} / ${best.sim_p90_profit:+.2f}")


def print_top_results(shortlist: Sequence[CandidateMetrics], args: argparse.Namespace) -> None:
    print("=" * 90)
    print("TOP SIMULATED CANDIDATES")
    print("=" * 90)
    print(
        f"{'Rank':>4} {'AvgSess$':>10} {'RoR':>8} {'Trip':>8} {'Inf':>8} {'P10$':>10} {'P90$':>10}  Spread"
    )
    print("-" * 90)

    ranked = sorted(
        shortlist,
        key=lambda c: (c.sim_avg_profit, -c.sim_ror),
        reverse=True,
    )
    for idx, candidate in enumerate(ranked[:10], start=1):
        print(
            f"{idx:>4} "
            f"{candidate.sim_avg_profit:>+10.2f} "
            f"{candidate.sim_ror:>7.2%} "
            f"{candidate.approx_trip_ror:>7.2%} "
            f"{candidate.approx_infinite_ror:>7.2%} "
            f"{candidate.sim_p10_profit:>+10.2f} "
            f"{candidate.sim_p90_profit:>+10.2f}  "
            f"{candidate.spread.spread_label(args.start_tc, args.top_tc)}"
        )


if __name__ == "__main__":
    main()
