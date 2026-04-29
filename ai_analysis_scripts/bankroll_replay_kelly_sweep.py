#!/usr/bin/env python3
"""
Kelly Sweep Matrix — parallel multi-regime bankroll replay.

Sweeps a hard-coded matrix of (table_min, table_max, bankroll) regimes against
a per-regime Kelly grid, all driven off a single strategy EV CSV + shoe trace.
Ruin mode is selectable via --ruin-mode (anytime|end, default anytime);
end-bankroll percentiles are exported for reconstructing outcome ranges.

Loads the shoe library + EV table once, then dispatches all
(bankroll, kelly) jobs to a multiprocessing Pool. On Linux the workers inherit
the numpy arrays read-only via fork copy-on-write (no pickle cost).

Usage:
    python3 ai_analysis_scripts/bankroll_replay_kelly_sweep.py \
        --trace stats/shoetraces/hand_traces_HiLoStrategy_6deck_75pen_H17_DAS_NoRAS_NoSurrender_3to2.csv.zst \
        --ev-csv data/ev_per_tc_data/evPerTC/HiLoStrategy/ev_per_tc_HiLoStrategy_6deck_70pen_H17_DAS_NoRAS_NoSurrender_3to2.csv \
        --shoes 100 --replays 10000 \
        --output kelly_matrix_hilo_6d75pen_h17

The matrix summary JSON is named ``{stem}_shoes{N}_ruin-{anytime|end}.json``
(``stem`` is the output folder name with trailing ``_ruin-*`` and legacy
``_shoes*_replay*`` segments removed so shoes/ruin appear once).
"""

from __future__ import annotations

import argparse
import csv
import json
import multiprocessing as mp
import os
import re
import sys
import time
from dataclasses import dataclass, fields as dc_fields
from pathlib import Path
from typing import Optional, TextIO

import numpy as np

sys.path.insert(0, str(Path(__file__).resolve().parent))
from bankroll_replay import (  # noqa: E402
    ReplayConfig,
    ReplayResult,
    load_shoe_library,
    replay_once,
)
from bankroll_replay_kelly import (  # noqa: E402
    KellyEVTable,
    KellySizer,
    KellySummary,
    _TeeTextIO,
    compute_kelly_summary,
    print_kelly_summary,
)


# ── fixed configuration ────────────────────────────────────────────────────────

def _unique_sorted(values: tuple[float, ...]) -> tuple[float, ...]:
    return tuple(sorted(set(float(v) for v in values)))


END_BANKROLL_PERCENTILES: tuple[float, ...] = _unique_sorted(
    (0.001,0.01, 0.025, 0.975, 0.99,0.999)
    + tuple(i / 100.0 for i in range(5, 100, 5))
)

# The matrix report uses percentile bankrolls instead of P(end < fraction * B0).
THRESHOLDS: tuple[float, ...] = ()

KELLY_LOW = (0.25, 0.5, 0.75, 1.0, 1.25, 1.5, 2.0, 2.5, 3.0, 4.0, 5.0, 6.0, 7.0)
KELLY_MID = (0.25, 0.5, 0.75, 1.0, 1.25, 1.5, 2.0, 2.5, 3.0, 4.0)

# (table_min, table_max, [bankrolls], [kelly_grid])
SWEEP_MATRIX: tuple[tuple[float, float, tuple[float, ...], tuple[float, ...]], ...] = (
    (5.0,   500.0,  (200.0, 500.0, 1000.0),                 KELLY_LOW),
    (10.0,  1000.0,  (500.0, 1000.0, 2000.0),                 KELLY_LOW),
    (25.0,  2000.0, (1000.0, 2000.0, 5000.0, 10000.0),      KELLY_MID),
    (50.0,  2000.0, (1000.0, 2000.0, 5000.0, 10000.0),      KELLY_MID),
    (100.0, 5000.0, (10000.0, 50000.0, 100000.0),           KELLY_MID),
)


# ── job dataclasses ────────────────────────────────────────────────────────────

@dataclass(frozen=True)
class JobKey:
    table_min: float
    table_max: float
    bankroll: float
    kelly: float


@dataclass
class Job:
    key: JobKey
    seed: int
    shoes_target: int
    num_replays: int


# ── worker (module-level for picklability; uses fork-inherited globals) ────────

_LIB = None
_EV_TABLE: Optional[KellyEVTable] = None
_RUIN_MODE: str = "anytime"


def _pool_init(lib, ev_table: KellyEVTable, ruin_mode: str) -> None:
    global _LIB, _EV_TABLE, _RUIN_MODE
    _LIB = lib
    _EV_TABLE = ev_table
    _RUIN_MODE = ruin_mode


def _run_job(job: Job) -> tuple[JobKey, KellySummary]:
    assert _LIB is not None and _EV_TABLE is not None
    config = ReplayConfig(
        start_bankroll=job.key.bankroll,
        shoes_target=job.shoes_target,
        num_replays=job.num_replays,
        table_min=job.key.table_min,
        table_max=job.key.table_max,
        ruin_mode=_RUIN_MODE,
        seed=job.seed,
    )
    sizer = KellySizer(
        table=_EV_TABLE,
        kelly=job.key.kelly,
        start_bankroll=job.key.bankroll,
        table_min=job.key.table_min,
        table_max=job.key.table_max,
    )
    rng = np.random.default_rng(job.seed)
    results: list[ReplayResult] = []
    for _ in range(job.num_replays):
        results.append(replay_once(_LIB, sizer, config, rng))
    summary = compute_kelly_summary(
        results,
        job.key.bankroll,
        job.key.kelly,
        THRESHOLDS,
        END_BANKROLL_PERCENTILES,
    )
    return job.key, summary


# ── job assembly ───────────────────────────────────────────────────────────────

def _build_jobs(base_seed: int, shoes_target: int, num_replays: int) -> list[Job]:
    """Deterministic per-job seeds via SeedSequence.spawn()."""
    flat_keys: list[JobKey] = []
    for table_min, table_max, bankrolls, kelly_grid in SWEEP_MATRIX:
        for br in bankrolls:
            for k in kelly_grid:
                flat_keys.append(JobKey(table_min, table_max, br, k))

    ss = np.random.SeedSequence(base_seed)
    children = ss.spawn(len(flat_keys))
    jobs: list[Job] = []
    for key, child in zip(flat_keys, children):
        seed = int(child.generate_state(1, dtype=np.uint32)[0])
        jobs.append(
            Job(key=key, seed=seed, shoes_target=shoes_target, num_replays=num_replays)
        )
    return jobs


# ── per-regime / per-bankroll output ───────────────────────────────────────────

def _row_label(table_min: float, table_max: float, bankroll: float) -> str:
    return (
        f"min${table_min:g} / max${table_max:g}  "
        f"|  B0=${bankroll:,.0f}"
    )


def _figure_path(out_dir: Path, table_min: float, table_max: float,
                 bankroll: float, num_replays: int, shoes_target: int) -> str:
    return str(
        out_dir
        / f"kelly_sweep_min{int(table_min)}_max{int(table_max)}"
          f"_br{int(bankroll)}_replays{int(num_replays)}_shoes{int(shoes_target)}.png"
    )


def _output_dir_with_ruin_mode(out_dir: Path, ruin_mode: str) -> Path:
    suffix = f"_ruin-{ruin_mode}"
    if out_dir.name.endswith(suffix):
        return out_dir
    return out_dir.with_name(f"{out_dir.name}{suffix}")


def _canonical_matrix_stem(dirname: str) -> str:
    """Strip trailing folder decorations so the JSON basename is not redundant.

    Removes a trailing ``_ruin-anytime`` / ``_ruin-end`` suffix, then a trailing
    ``_shoes{N}_replay...`` segment (legacy output folder names) so the file
    name carries ``shoes`` / ``ruin`` once.
    """
    name = dirname
    for suf in ("_ruin-anytime", "_ruin-end"):
        if name.endswith(suf):
            name = name[: -len(suf)]
            break
    m = re.match(r"^(.+)_shoes(\d+)_replay(\d+k?)$", name, re.IGNORECASE)
    if m:
        name = m.group(1)
    return name


def matrix_summary_json_name(dirname: str, shoes_target: int, ruin_mode: str) -> str:
    """Full matrix summary filename: ``{stem}_shoes{N}_ruin-{mode}.json``."""
    stem = _canonical_matrix_stem(dirname)
    return f"{stem}_shoes{int(shoes_target)}_ruin-{ruin_mode}.json"


def _plot_kelly_sweep_titled(
    summaries: list[KellySummary],
    out_path: str,
    title: str,
) -> None:
    """Local copy of plot_kelly_sweep that accepts a custom title."""
    try:
        import matplotlib.pyplot as plt
    except ImportError:
        print("matplotlib not installed — skipping figure.", file=sys.stderr)
        return
    if not summaries:
        return

    ks = np.array([s.kelly for s in summaries])
    exp_profit = np.array([s.base.mean_end_profit for s in summaries])
    p_ruin = np.array([s.base.prob_ruin for s in summaries])
    p_down = np.array([s.base.prob_finish_down for s in summaries])
    order = np.argsort(ks)
    ks, exp_profit, p_ruin, p_down = ks[order], exp_profit[order], p_ruin[order], p_down[order]

    fig, ax1 = plt.subplots(figsize=(9.0, 5.5))
    color_profit = "tab:blue"
    ax1.set_xlabel("Fractional Kelly  k")
    ax1.set_ylabel("Expected profit  E[end − B0]  ($)", color=color_profit)
    ax1.plot(ks, exp_profit, color=color_profit, marker="o", label="E[profit]")
    ax1.axhline(0, color=color_profit, linestyle=":", alpha=0.3)
    ax1.tick_params(axis="y", labelcolor=color_profit)
    ax1.grid(True, alpha=0.3)

    ax2 = ax1.twinx()
    color_ruin = "tab:red"
    ax2.set_ylabel("Probability", color=color_ruin)
    ax2.plot(ks, p_ruin, color=color_ruin, marker="s", label="P(ruin)")
    ax2.plot(ks, p_down, color="tab:orange", marker="^", linestyle="--",
             alpha=0.8, label="P(finish down)")
    ax2.tick_params(axis="y", labelcolor=color_ruin)
    probs_all = np.concatenate([p_ruin, p_down])
    ax2.set_ylim(0, max(0.05, float(np.max(probs_all)) * 1.15))

    ax1.set_title(title)
    lines1, labels1 = ax1.get_legend_handles_labels()
    lines2, labels2 = ax2.get_legend_handles_labels()
    ax2.legend(lines1 + lines2, labels1 + labels2, loc="best")

    fig.tight_layout()
    fig.savefig(out_path, dpi=120)
    plt.close(fig)
    print(f"\nFigure saved to: {out_path}")


def _kelly_spread_for_job(key: JobKey, ev_table: KellyEVTable) -> dict:
    """Serialize the practical Kelly spread for integer TC +1 through +10."""
    sizer = KellySizer(
        table=ev_table,
        kelly=key.kelly,
        start_bankroll=key.bankroll,
        table_min=key.table_min,
        table_max=key.table_max,
    )
    return {
        f"tc_{tc}": float(sizer.bet_for_tc(float(tc)))
        for tc in range(1, 11)
    }


def _end_bankroll_percentiles(s: KellySummary) -> list[dict]:
    """Inverse-CDF points useful for plotting the outcome range."""
    b0 = s.base.start_bankroll
    return [
        {
            "percentile": float(prob * 100.0),
            "probability": float(prob),
            "end_bankroll": float(bankroll),
            "end_profit": float(bankroll - b0),
        }
        for prob, bankroll in zip(s.percentile_probs, s.percentile_bankrolls)
    ]


def _summary_row(key: JobKey, s: KellySummary, ev_table: KellyEVTable) -> dict:
    """Flatten one job's summary into a single row of plain Python scalars."""
    row: dict = {
        "table_min": key.table_min,
        "table_max": key.table_max,
        "bankroll": key.bankroll,
        "kelly": key.kelly,
        "num_replays": s.base.num_replays,
    }
    for f in dc_fields(s.base):
        if f.name in ("num_replays", "start_bankroll"):
            continue
        row[f.name] = getattr(s.base, f.name)
    row["bet_spread"] = _kelly_spread_for_job(key, ev_table)
    row["end_bankroll_percentiles"] = _end_bankroll_percentiles(s)
    return row


def _write_summary_data(
    out_dir: Path,
    rows: list[dict],
    meta: dict,
) -> None:
    """Emit summary.csv + ``{stem}_shoes{N}_ruin-{mode}.json`` with all per-job fields."""
    csv_path = out_dir / "summary.csv"
    json_path = out_dir / matrix_summary_json_name(
        out_dir.name,
        int(meta["shoes_per_session"]),
        str(meta["ruin_mode"]),
    )
    json_only_keys = {
        "bet_spread",
        "end_bankroll_percentiles",
    }

    if rows:
        csv_rows = [
            {k: v for k, v in r.items() if k not in json_only_keys}
            for r in rows
        ]
        # Union of all keys, but stable order: take first row's keys, then
        # append any extras from later rows (different threshold counts etc).
        seen: set[str] = set()
        ordered: list[str] = []
        for r in csv_rows:
            for k in r.keys():
                if k not in seen:
                    seen.add(k)
                    ordered.append(k)
        with open(csv_path, "w", newline="") as fh:
            w = csv.DictWriter(fh, fieldnames=ordered)
            w.writeheader()
            for r in csv_rows:
                w.writerow(r)

    with open(json_path, "w", encoding="utf-8") as fh:
        json.dump({"meta": meta, "rows": rows}, fh, indent=2)

    print(f"\nSummary data saved to:\n  {csv_path}\n  {json_path}")


def _emit_row_outputs(
    out_dir: Path,
    table_min: float,
    table_max: float,
    bankroll: float,
    summaries: list[KellySummary],
    ev_table: KellyEVTable,
    shoes_target: int,
    ruin_mode: str,
) -> None:
    summaries = sorted(summaries, key=lambda s: s.kelly)

    W = 70
    print()
    print("#" * W)
    print(f"# REGIME: {_row_label(table_min, table_max, bankroll)}")
    print("#" * W)

    for s in summaries:
        config = ReplayConfig(
            start_bankroll=bankroll,
            shoes_target=shoes_target,
            num_replays=s.base.num_replays,
            table_min=table_min,
            table_max=table_max,
            ruin_mode=ruin_mode,
        )
        sizer = KellySizer(
            table=ev_table,
            kelly=s.kelly,
            start_bankroll=bankroll,
            table_min=table_min,
            table_max=table_max,
        )
        # NOTE: print_kelly_summary uses config.shoes_target only for display.
        # We don't have it here per-regime, but the value isn't critical for a
        # printed log; pass the real one we ran with via summary's session stats.
        print_kelly_summary(s, config, sizer)

    fig_path = _figure_path(out_dir, table_min, table_max, bankroll, s.base.num_replays, shoes_target)
    title = (
        f"Kelly sweep — min ${table_min:g} / max ${table_max:g}, "
        f"B0 ${bankroll:,.0f}\n"
        f"{summaries[0].base.num_replays:,} replays x "
        f"{shoes_target:,} shoes/session  ({len(summaries)} k values)"
    )
    _plot_kelly_sweep_titled(summaries, fig_path, title)


# ── main ───────────────────────────────────────────────────────────────────────

def parse_args() -> argparse.Namespace:
    p = argparse.ArgumentParser(
        description=(
            "Sweep a hard-coded matrix of (bankroll, table-min, table-max) "
            "regimes against per-regime Kelly grids, in parallel."
        ),
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    p.add_argument("--trace", required=True)
    p.add_argument("--ev-csv", required=True)
    p.add_argument("--shoes", type=int, required=True, dest="shoes_target",
                   help="Shoes per replay session.")
    p.add_argument("--replays", type=int, required=True, dest="num_replays",
                   help="Monte Carlo replays per (bankroll, kelly) job.")
    p.add_argument("--output", required=True,
                   help="Output folder base name. The selected ruin mode is "
                        "appended before creation. Receives PNGs, "
                        "session_log.md, summary.csv, and a matrix JSON named "
                        "{stem}_shoes{N}_ruin-{anytime|end}.json.")
    p.add_argument("--seed", type=int, default=42)
    p.add_argument("--max-shoes", type=int, default=None)
    p.add_argument("--min-hands-per-tc", type=int, default=0)
    p.add_argument("--workers", type=int, default=None,
                   help="Worker processes (defaults to os.cpu_count()).")
    p.add_argument("--ruin-mode", choices=["anytime", "end"], default="anytime",
                   help="'anytime': ruin whenever pre-hand bankroll < table_min; "
                        "'end': ruin only if end-of-session bankroll < table_min.")
    return p.parse_args()


def _run(args: argparse.Namespace) -> None:
    if args.shoes_target < 1:
        raise SystemExit("--shoes must be >= 1")
    if args.num_replays < 1:
        raise SystemExit("--replays must be >= 1")

    print(f"Loading EV table from: {args.ev_csv}")
    ev_table = KellyEVTable.from_csv(args.ev_csv, min_hands=args.min_hands_per_tc)
    tc_lo = ev_table.tc_min_scaled * 0.5
    tc_hi = ev_table.tc_max_scaled * 0.5
    print(
        f"  {ev_table.n_buckets} TC buckets in [{tc_lo:+.1f} .. {tc_hi:+.1f}] "
        f"(HandsPlayed >= {args.min_hands_per_tc})"
    )

    lib = load_shoe_library(args.trace, max_shoes=args.max_shoes)
    if lib.n_shoes == 0:
        raise SystemExit("No shoes loaded — check --trace and --max-shoes.")
    if lib.n_shoes < args.shoes_target:
        print(
            f"Warning: only {lib.n_shoes} shoes available but session requests "
            f"{args.shoes_target}. Sampling with replacement.",
            file=sys.stderr,
        )

    jobs = _build_jobs(args.seed, args.shoes_target, args.num_replays)
    n_jobs = len(jobs)
    workers = args.workers or os.cpu_count() or 1
    workers = max(1, min(workers, n_jobs))
    workers = 12

    print(
        f"\nDispatching {n_jobs} jobs across {workers} workers "
        f"(replays={args.num_replays:,}, shoes/session={args.shoes_target})"
    )

    ctx = mp.get_context("fork")
    results: dict[JobKey, KellySummary] = {}
    t0 = time.time()
    with ctx.Pool(
        processes=workers,
        initializer=_pool_init,
        initargs=(lib, ev_table, args.ruin_mode),
    ) as pool:
        for i, (key, summary) in enumerate(
            pool.imap_unordered(_run_job, jobs, chunksize=1), start=1
        ):
            results[key] = summary
            elapsed = time.time() - t0
            rate = i / elapsed if elapsed > 0 else float("inf")
            print(
                f"  [{i:>3}/{n_jobs}]  "
                f"min${key.table_min:g}/max${key.table_max:g}  "
                f"B0=${key.bankroll:>7,.0f}  k={key.kelly:>4.2f}  "
                f"({rate:.2f} jobs/s, elapsed {elapsed:.1f}s)",
                flush=True,
            )

    total = time.time() - t0
    print(
        f"\nAll {n_jobs} jobs done in {total:.1f}s "
        f"({n_jobs / total:.2f} jobs/s avg)"
    )

    # Group by (table_min, table_max, bankroll) for output.
    out_dir = Path(args.output)
    summary_rows: list[dict] = []
    for table_min, table_max, bankrolls, kelly_grid in SWEEP_MATRIX:
        for br in bankrolls:
            row = [
                results[JobKey(table_min, table_max, br, k)]
                for k in kelly_grid
            ]
            _emit_row_outputs(
                out_dir, table_min, table_max, br, row, ev_table,
                args.shoes_target, args.ruin_mode,
            )
            for k in kelly_grid:
                key = JobKey(table_min, table_max, br, k)
                summary_rows.append(_summary_row(key, results[key], ev_table))

    meta = {
        "trace": args.trace,
        "ev_csv": args.ev_csv,
        "shoes_per_session": args.shoes_target,
        "num_replays": args.num_replays,
        "ruin_mode": args.ruin_mode,
        "percentiles": list(END_BANKROLL_PERCENTILES),
        "seed": args.seed,
        "min_hands_per_tc": args.min_hands_per_tc,
        "max_shoes": args.max_shoes,
        "n_shoes_loaded": int(lib.n_shoes),
        "ev_tc_min": float(ev_table.tc_min_scaled * 0.5),
        "ev_tc_max": float(ev_table.tc_max_scaled * 0.5),
        "n_ev_buckets": int(ev_table.n_buckets),
    }
    _write_summary_data(out_dir, summary_rows, meta)


def main() -> None:
    args = parse_args()
    args.output = str(_output_dir_with_ruin_mode(Path(args.output), args.ruin_mode))

    out_dir = Path(args.output)
    out_dir.mkdir(parents=True, exist_ok=True)
    log_path = out_dir / "session_log.md"
    log_file: TextIO = open(log_path, "w", encoding="utf-8")
    real_stdout = sys.stdout
    sys.stdout = _TeeTextIO(real_stdout, log_file)

    try:
        _run(args)
    finally:
        sys.stdout = real_stdout
        log_file.close()
        print(f"\nSession log saved to: {log_path}", file=real_stdout)


if __name__ == "__main__":
    main()
