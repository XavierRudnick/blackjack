#!/usr/bin/env python3
"""
Run bankroll_replay_kelly_sweep.py over the full Kelly matrix plan:

  4 strategies x 7 (decks, pen) x 2 ruin modes x 3 session lengths = 168 invocations.

Paths match kelly_matrix/NAMING.txt and INPUT_AUDIT.txt at repo root.

Replay counts are hardcoded by (deck count, shoes/session):
  6-deck: 1000 shoes -> 1k replays; 100 shoes -> 8k; 10 shoes -> 40k.
  2-deck: 1000 shoes -> 2k replays; 100 shoes -> 16k; 10 shoes -> 80k.

Examples:
  python3 ai_analysis_scripts/run_kelly_matrix_batch.py --dry-run
  python3 ai_analysis_scripts/run_kelly_matrix_batch.py --seed 42
  python3 ai_analysis_scripts/run_kelly_matrix_batch.py --only-strategy HiLoStrategy --shoes 10
  python3 ai_analysis_scripts/run_kelly_matrix_batch.py --force  # re-run even if summary JSON exists
"""

from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[1]
SCRIPT_DIR = Path(__file__).resolve().parent
SWEEP = REPO_ROOT / "ai_analysis_scripts" / "bankroll_replay_kelly_sweep.py"

if str(SCRIPT_DIR) not in sys.path:
    sys.path.insert(0, str(SCRIPT_DIR))
from bankroll_replay_kelly_sweep import (  # noqa: E402
    _output_dir_with_ruin_mode,
    matrix_summary_json_paths_for_skip,
)


def sweep_python() -> str:
    """Prefer repo .venv so numpy/matplotlib match project tooling."""
    for rel in (".venv/bin/python3", ".venv/bin/python"):
        p = REPO_ROOT / rel
        if p.is_file():
            return str(p)
    return sys.executable

STRATEGIES = (
    "HiLoStrategy",
    "NoStrategy",
    "ZenCountStrategy",
    "OmegaIIStrategyAceCount",
)
RULES_ID = "H17_DAS_NoRAS_NoSurrender_3to2"
# (num_decks, penetration_percent_int)
DECK_PENS = ((6, 55), (6, 65), (6, 75), (2, 50), (2, 60), (2, 70), (2, 80))
RUIN_MODES = ("anytime", "end")
SHOE_TARGETS = (10, 100, 1000)


def trace_path(strategy: str, num_decks: int, pen_pct: int) -> Path:
    return REPO_ROOT / (
        f"stats/shoetraces/hand_traces_{strategy}_{num_decks}deck_{pen_pct}pen_{RULES_ID}.csv.zst"
    )


def ev_csv_path(strategy: str, num_decks: int, pen_pct: int) -> Path:
    return REPO_ROOT / (
        f"data/ev_per_tc_data/evPerTC/{strategy}/"
        f"ev_per_tc_{strategy}_{num_decks}deck_{pen_pct}pen_{RULES_ID}.csv"
    )


def output_base(strategy: str, num_decks: int, pen_pct: int, shoes: int) -> Path:
    return REPO_ROOT / "kelly_matrix" / strategy / f"{num_decks}deck_{pen_pct}pen" / f"shoes{shoes}"


def matrix_replays(num_decks: int, shoes: int) -> int:
    """Monte Carlo replays per (bankroll, Kelly) job for this matrix (hardcoded)."""
    if shoes not in SHOE_TARGETS:
        raise ValueError(f"matrix_replays: unsupported shoes={shoes}")
    if num_decks == 6:
        return {1000: 1000, 100: 8000, 10: 40_000}[shoes]
    if num_decks == 2:
        return {1000: 2000, 100: 16_000, 10: 80_000}[shoes]
    raise ValueError(f"matrix_replays: unsupported num_decks={num_decks}")


def main() -> None:
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("--seed", type=int, default=42)
    p.add_argument("--dry-run", action="store_true", help="Print commands only")
    p.add_argument(
        "--continue-on-missing",
        action="store_true",
        help="Skip runs whose trace or EV file is missing instead of aborting",
    )
    p.add_argument("--only-strategy", choices=STRATEGIES, default=None)
    p.add_argument("--only-shoes", type=int, default=None, choices=SHOE_TARGETS)
    p.add_argument(
        "--force",
        action="store_true",
        help="Run even when the matrix summary JSON for that cell already exists",
    )
    args = p.parse_args()

    if not SWEEP.is_file():
        sys.exit(f"missing sweep script: {SWEEP}")

    strategies = (args.only_strategy,) if args.only_strategy else STRATEGIES
    shoe_list = (args.only_shoes,) if args.only_shoes is not None else SHOE_TARGETS

    cmds: list[list[str]] = []
    missing: list[str] = []
    skipped_existing: list[Path] = []

    for strategy in strategies:
        for num_decks, pen_pct in DECK_PENS:
            tr = trace_path(strategy, num_decks, pen_pct)
            ev = ev_csv_path(strategy, num_decks, pen_pct)
            for shoes in shoe_list:
                for ruin in RUIN_MODES:
                    if not tr.is_file():
                        missing.append(f"missing trace: {tr.relative_to(REPO_ROOT)}")
                    if not ev.is_file():
                        missing.append(f"missing EV:   {ev.relative_to(REPO_ROOT)}")
                    out = output_base(strategy, num_decks, pen_pct, shoes)
                    out_dir = _output_dir_with_ruin_mode(out, ruin)
                    json_primary, json_legacy = matrix_summary_json_paths_for_skip(out_dir, shoes, ruin)
                    done = json_primary.is_file() or (
                        json_legacy != json_primary and json_legacy.is_file()
                    )
                    if done and not args.force:
                        hit = json_primary if json_primary.is_file() else json_legacy
                        skipped_existing.append(hit.relative_to(REPO_ROOT))
                        continue
                    replays = matrix_replays(num_decks, shoes)
                    cmds.append(
                        [
                            sweep_python(),
                            str(SWEEP),
                            "--trace",
                            str(tr),
                            "--ev-csv",
                            str(ev),
                            "--shoes",
                            str(shoes),
                            "--replays",
                            str(replays),
                            "--output",
                            str(out),
                            "--ruin-mode",
                            ruin,
                            "--seed",
                            str(args.seed),
                        ]
                    )

    if missing and not args.continue_on_missing:
        print("Input check failed (fix with C++ BLACKJACK_KELLY_MATRIX=… or --continue-on-missing):\n", file=sys.stderr)
        for line in sorted(set(missing)):
            print(" ", line, file=sys.stderr)
        sys.exit(1)

    if args.continue_on_missing:
        # Filter commands with missing inputs
        def ok(cmd: list[str]) -> bool:
            tr_i = cmd.index("--trace") + 1
            ev_i = cmd.index("--ev-csv") + 1
            return Path(cmd[tr_i]).is_file() and Path(cmd[ev_i]).is_file()

        before = len(cmds)
        cmds = [c for c in cmds if ok(c)]
        skipped = before - len(cmds)
        if skipped:
            print(f"Skipping {skipped} runs with missing trace/EV (--continue-on-missing).", file=sys.stderr)

    if skipped_existing:
        print(
            f"Skipping {len(skipped_existing)} runs with existing summary JSON (use --force to re-run).",
            file=sys.stderr,
        )
        for rel in sorted(skipped_existing):
            print(f"  exists: {rel}", file=sys.stderr)

    print(f"Planned runs: {len(cmds)}", file=sys.stderr)

    for i, cmd in enumerate(cmds, start=1):
        print(f"\n[{i}/{len(cmds)}] ", " ".join(cmd), file=sys.stderr)
        if args.dry_run:
            print(" ".join(cmd))
            continue
        r = subprocess.run(cmd, cwd=REPO_ROOT)
        if r.returncode != 0:
            sys.exit(r.returncode)

    if args.dry_run:
        print(f"(dry-run: {len(cmds)} commands)", file=sys.stderr)


if __name__ == "__main__":
    main()
