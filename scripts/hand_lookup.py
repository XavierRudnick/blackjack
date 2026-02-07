#!/usr/bin/env python3
"""Generate per-strategy lookup tables for hands played per true count."""

from __future__ import annotations

import argparse
import csv
import re
from collections import defaultdict
from pathlib import Path
from typing import Iterable


DEFAULT_OUTPUT_DIR = Path("lookup_tables")
DEFAULT_EV_ROOTS = (
    Path("stats_H17_150mil/evPerTC"),
    Path("stats_S17_150mill/evPerTC"),
)

FILENAME_PATTERN = re.compile(
    r"^ev_per_tc_(?P<strategy>[A-Za-z0-9]+)_(?P<deck>\d+)deck_(?P<pen>\d+)pen_(?P<rest>.+)\.csv$"
)

RULE_TOKENS = {"H17", "S17"}
DROP_TOKENS = {"DAS", "NoRAS", "NoSurrender", "3to2"}


def parse_file_metadata(path: Path) -> tuple[str, str, str, str, tuple[str, ...]]:
    """Return the strategy, rule, deck, penetration, and extra tags from the filename."""

    match = FILENAME_PATTERN.match(path.name)
    if not match:
        raise ValueError(f"unable to parse file metadata from {path.name}")

    rest_tokens = match["rest"].split("_")
    rule = next((token for token in rest_tokens if token in RULE_TOKENS), "Unknown")
    extras = tuple(
        token
        for token in rest_tokens
        if token not in RULE_TOKENS and token not in DROP_TOKENS
    )

    return (
        match["strategy"],
        rule,
        match["deck"],
        match["pen"],
        extras,
    )


def build_column_label(rule: str, deck: str, pen: str, extras: tuple[str, ...]) -> str:
    """Craft a human-readable column label for a deck/penetration combo."""

    label = f"{rule}-{deck}deck-{pen}pen"
    if extras:
        label += "-" + "-".join(extras)
    return label


def collect_stats(ev_dirs: Iterable[Path]):
    stats = defaultdict(lambda: defaultdict(lambda: defaultdict(int)))
    true_counts = defaultdict(set)
    processed_files = 0

    for ev_root in ev_dirs:
        if not ev_root.exists():
            print(f"warning: directory {ev_root} does not exist, skipping")
            continue

        for csv_path in sorted(ev_root.rglob("ev_per_tc_*.csv")):
            try:
                strategy, rule, deck, pen, extras = parse_file_metadata(csv_path)
            except ValueError as exc:
                print(f"skipping {csv_path}: {exc}")
                continue

            column_label = build_column_label(rule, deck, pen, extras)
            with csv_path.open(newline="") as fh:
                reader = csv.DictReader(fh)
                for row in reader:
                    try:
                        tc_value = float(row["TrueCount"])
                        hands = int(float(row["HandsPlayed"]))
                    except (KeyError, ValueError):
                        continue
                    stats[strategy][column_label][tc_value] += hands
                    true_counts[strategy].add(tc_value)

            processed_files += 1

    return stats, true_counts, processed_files


def format_true_count(tc_value: float) -> str:
    """Format the true count so it always shows a single decimal."""

    return f"{tc_value:.1f}"


def write_tables(output_dir: Path, stats, true_counts):
    output_dir.mkdir(parents=True, exist_ok=True)

    written = []
    for strategy, columns in sorted(stats.items()):
        if not columns:
            continue

        column_labels = sorted(columns.keys())
        sorted_tcs = sorted(true_counts[strategy])

        out_path = output_dir / f"{strategy}_hands_lookup.csv"
        with out_path.open("w", newline="") as fh:
            writer = csv.writer(fh)
            writer.writerow(["TrueCount", *column_labels])

            for tc_value in sorted_tcs:
                row = [format_true_count(tc_value)]
                for label in column_labels:
                    row.append(columns[label].get(tc_value, 0))
                writer.writerow(row)

            totals = [sum(columns[label].values()) for label in column_labels]
            writer.writerow(["Total", *totals])

        written.append(out_path)

    return written


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Build hands-played lookup tables for each counting strategy."
    )
    parser.add_argument(
        "--ev-dirs",
        nargs="+",
        default=[str(path) for path in DEFAULT_EV_ROOTS],
        help="List of evPerTC directories to scan (defaults to the two 150m folders).",
    )
    parser.add_argument(
        "--output-dir",
        "-o",
        default=str(DEFAULT_OUTPUT_DIR),
        help="Directory to write the strategy lookup CSVs.",
    )

    args = parser.parse_args()
    ev_dirs = [Path(path) for path in args.ev_dirs]

    stats, true_counts, processed_files = collect_stats(ev_dirs)
    if not stats:
        raise SystemExit("no statistics files were processed")

    written_files = write_tables(Path(args.output_dir), stats, true_counts)

    print(
        f"Processed {processed_files} ev_per_tc CSVs and wrote {len(written_files)} tables to {args.output_dir}"
    )
    for path in written_files:
        print(f"  - {path}")


if __name__ == "__main__":
    main()
