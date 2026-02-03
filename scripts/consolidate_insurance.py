#!/usr/bin/env python3
"""Consolidate insurance Monte Carlo results by TrueCount.

Input CSVs are produced by FixedEngine::savetoCSVResults and typically contain rows keyed by:
(UserValue, DealerValue, TrueCount).

This script aggregates across UserValue so you get one row per (DealerValue, TrueCount),
using Hands Played as weights.

Example:
  python3 consolidate_insurance.py stats/HiLoStrategy_InsuranceAccept_vs_Decline.csv

Output:
  Writes a sibling CSV named "*_consolidated.csv" unless --out is provided.
"""

from __future__ import annotations

import argparse
import csv
import os
from dataclasses import dataclass
from typing import Dict, Tuple, Optional


@dataclass
class Accum:
    hands: int = 0
    accept_payout_sum: float = 0.0
    decline_payout_sum: float = 0.0

    def add(self, accept_ev: float, decline_ev: float, hands: int) -> None:
        if hands <= 0:
            return
        self.hands += hands
        self.accept_payout_sum += accept_ev * hands
        self.decline_payout_sum += decline_ev * hands

    def accept_ev(self) -> float:
        return 0.0 if self.hands == 0 else self.accept_payout_sum / self.hands

    def decline_ev(self) -> float:
        return 0.0 if self.hands == 0 else self.decline_payout_sum / self.hands


def _get_col(row: Dict[str, str], name: str) -> Optional[str]:
    # Exact match first; then try a few common variants.
    if name in row:
        return row[name]
    # tolerate minor header changes
    normalized = {k.strip().lower(): k for k in row.keys()}
    key = normalized.get(name.strip().lower())
    return row[key] if key else None


def _pick_hands_played(row: Dict[str, str]) -> int:
    # Preferred: single column
    for col in (
        "Hands Played",
        # Older per-action format (if present)
        "Insurance Accept Hands",
        "Insurance Decline Hands",
        "Hit Hands",
        "Stand Hands",
    ):
        v = _get_col(row, col)
        if v is not None and v != "":
            try:
                return int(float(v))
            except ValueError:
                pass
    return 0


def consolidate(in_path: str, out_path: str) -> None:
    buckets: Dict[Tuple[int, float], Accum] = {}

    with open(in_path, "r", newline="") as f:
        reader = csv.DictReader(f)
        if reader.fieldnames is None:
            raise SystemExit("Input CSV has no header")

        # Required columns
        required = ["DealerValue", "TrueCount", "Insurance Accept EV", "Insurance Decline EV"]
        missing = [c for c in required if (c not in reader.fieldnames)]
        if missing:
            raise SystemExit(
                "Missing required columns: "
                + ", ".join(missing)
                + f"\nFound: {', '.join(reader.fieldnames)}"
            )

        for row in reader:
            try:
                dealer_value = int(float(row["DealerValue"]))
                true_count = float(row["TrueCount"])
                accept_ev = float(row["Insurance Accept EV"])
                decline_ev = float(row["Insurance Decline EV"])
            except ValueError:
                # Skip malformed rows
                continue

            hands = _pick_hands_played(row)
            key = (dealer_value, true_count)
            buckets.setdefault(key, Accum()).add(accept_ev, decline_ev, hands)

    # Write consolidated output
    with open(out_path, "w", newline="") as f:
        writer = csv.writer(f)
        writer.writerow(
            [
                "DealerValue",
                "TrueCount",
                "Insurance Accept EV",
                "Insurance Decline EV",
                "Hands Played",
                "Best Action",
                "EV Diff (Accept-Decline)",
            ]
        )

        for (dealer_value, true_count) in sorted(buckets.keys(), key=lambda k: (k[0], k[1])):
            acc = buckets[(dealer_value, true_count)]
            accept = acc.accept_ev()
            decline = acc.decline_ev()
            best = "Accept" if accept > decline else "Decline" if decline > accept else "Tie"
            writer.writerow(
                [
                    dealer_value,
                    _format_true_count(true_count),
                    f"{accept:.6f}",
                    f"{decline:.6f}",
                    acc.hands,
                    best,
                    f"{(accept - decline):.6f}",
                ]
            )


def _format_true_count(tc: float) -> str:
    # Keep integers looking like integers; otherwise a compact float.
    if tc.is_integer():
        return str(int(tc))
    return repr(tc)


def main() -> None:
    ap = argparse.ArgumentParser()
    ap.add_argument("input", help="Path to an insurance CSV (e.g. *_InsuranceAccept_vs_Decline.csv)")
    ap.add_argument("--out", help="Output CSV path (default: <input>_consolidated.csv)")
    args = ap.parse_args()

    in_path = args.input
    if not os.path.exists(in_path):
        raise SystemExit(f"Input not found: {in_path}")

    out_path = args.out
    if not out_path:
        abs_in_path = os.path.abspath(in_path)
        in_dir = os.path.dirname(abs_in_path)

        # Default behavior: write consolidated outputs into stats/insurance/ when the
        # input file lives in the stats directory.
        out_dir = os.path.join(in_dir, "insurance") if os.path.basename(in_dir) == "stats" else in_dir
        os.makedirs(out_dir, exist_ok=True)

        base = os.path.basename(abs_in_path)
        root, ext = os.path.splitext(base)
        out_path = os.path.join(out_dir, f"{root}_consolidated{ext or '.csv'}")

    consolidate(in_path, out_path)
    print(f"Wrote: {out_path}")


if __name__ == "__main__":
    main()
