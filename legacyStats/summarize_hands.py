#!/usr/bin/env python3
"""Summarize hands played before/after a true-count threshold and compute EV.

Usage:
    python stats/summarize_hands.py --file stats/strategy_1_user_16_dealer_10.csv --threshold 1
"""

import argparse
import csv
from pathlib import Path


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Sum Hands Played split by true count threshold")
    parser.add_argument(
        "--file",
        type=Path,
        required=True,
        help="Path to the CSV file (expects columns TrueCount and Hands Played)",
    )
    parser.add_argument(
        "--threshold",
        type=float,
        default=1.0,
        help="True count threshold; counts > threshold are considered 'after', others 'before'",
    )
    return parser.parse_args()


def summarize(csv_path: Path, threshold: float) -> dict:
    before_hands = 0
    after_hands = 0
    before_hit_ev_sum = 0.0
    after_stand_ev_sum = 0.0
    after_hit_ev_sum = 0.0

    with csv_path.open(newline="", encoding="utf-8") as fh:
        reader = csv.DictReader(fh)
        for row in reader:
            try:
                true_count = float(row["TrueCount"])
                hands = int(row["Hands Played"])
                stand_ev = float(row["Stand EV"])
                hit_ev = float(row["Hit EV"])
            except (KeyError, ValueError) as exc:
                raise ValueError(f"Invalid row encountered: {row}") from exc

            if true_count > threshold:
                after_hands += hands
                after_hit_ev_sum += hit_ev * hands
                after_stand_ev_sum += stand_ev * hands

            else:
                before_hands += hands
                before_hit_ev_sum += hit_ev * hands

    total_hands = before_hands + after_hands

    stand_ev = after_stand_ev_sum / after_hands if after_hands else 0.0
    hit_ev = after_hit_ev_sum / after_hands if after_hands else 0.0
    delta_weighted = 0.0
    if total_hands:
        delta_weighted = (before_hit_ev_sum + after_stand_ev_sum) / total_hands - (after_hit_ev_sum + before_hit_ev_sum) / total_hands

    return {
        "before_hands": before_hands,
        "after_hands": after_hands,
        "total_hands": total_hands,
        "before_hit_ev_sum": before_hit_ev_sum,
        "after_hit_ev_sum": after_hit_ev_sum,
        "stand_ev": stand_ev,
        "hit_ev": hit_ev,
        "weighted_ev_no_deviation": (before_hit_ev_sum + after_hit_ev_sum) / total_hands,
        "weighted_ev_with_deviation": (before_hit_ev_sum + after_stand_ev_sum) / total_hands,
        "delta_vs_always_stand": delta_weighted,
        "threshold": threshold,
    }


def main() -> None:
    args = parse_args()
    if not args.file.exists():
        raise FileNotFoundError(f"CSV not found: {args.file}")

    result = summarize(args.file, args.threshold)
    print(f"Threshold:                {result['threshold']}")
    print(f"Hands at/below threshold: {result['before_hands']}")
    print(f"Hands above threshold:    {result['after_hands']}")
    print(f"Total hands:              {result['total_hands']}")
    print("--- Segment EVs ---")
    if result['before_hands']:
        print(
            f"Hit EV (<= threshold):  {result['before_hit_ev_sum'] / result['before_hands']:.6f}"
        )
    if result['after_hands']:
        print(f"Stand EV (> threshold):   {result['stand_ev']:.6f}")
        print(f"Hit EV (> threshold):     {result['hit_ev']:.6f}")
    print("--- Weighted EV ---")
    print(
        f"With deviation (stand<=, hit>): {result['weighted_ev_with_deviation']:.6f}"
    )
    print(
        f"No deviation (always stand):    {result['weighted_ev_no_deviation']:.6f}"
    )
    print(
        f"Delta vs always stand:          {result['delta_vs_always_stand']:.6f}"
    )


if __name__ == "__main__":
    main()
