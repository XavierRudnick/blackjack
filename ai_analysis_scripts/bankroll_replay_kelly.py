#!/usr/bin/env python3
"""
Bankroll Outcome Engine — Kelly-Sized Bets (fixed unit from start bankroll)

Mirrors bankroll_replay.py (shoe sampling, trace I/O, P&L math, ruin rules,
summary structure) but replaces the fixed spread with Kelly sizing derived
from an ev_per_tc CSV.

Sizing rule — NO bankroll-path feedback:
    bet_h = round_to_nearest( clip(B0 * k * mu(tc_h) / sigma2(tc_h), table_min, table_max),
                              unit = table_min )

    Rounding: every stake is a multiple of ``table_min`` (nearest chip), then
    capped to the largest such multiple not above ``table_max`` when a cap is set.

Only the STARTING bankroll B0 enters the sizing formula. Bets are NOT
recomputed from the evolving bankroll after wins/losses.

    mu(tc)       = EVPerDollar from the ev_per_tc CSV
    sigma2(tc)   = (StdErrorPerDollar * sqrt(HandsPlayed))^2  (matches
                   calculate_optimal_rtp.py)

Dependencies:
    pip install numpy pandas matplotlib

Usage:
python3 ai_analysis_scripts/bankroll_replay_kelly.py \
        --trace stats/shoetraces/hand_traces_HiLoStrategy_6deck_75pen_H17_DAS_NoRAS_NoSurrender_3to2.csv.zst \
        --ev-csv data/ev_per_tc_data/evPerTC/HiLoStrategy/ev_per_tc_HiLoStrategy_6deck_75pen_H17_DAS_NoRAS_NoSurrender_3to2.csv \
        --start-bankroll 100000 \
        --table-min 100 --table-max 5000 \
        --shoes 1000 --replays 10000 \
        --kelly-grid "0.25,0.5,0.75,1.0,,1.5,2,2.5,3,4" \
        --bankroll-thresholds "0.9,0.75,0.5,0.25,0.1" \
        --output kelly_sweep_hilo_6d75pen_h17.png
"""

from __future__ import annotations

import argparse
import sys
import time
from dataclasses import dataclass, fields
from pathlib import Path
from typing import Optional, Sequence, TextIO

import numpy as np
import pandas as pd

# Allow running as `python3 ai_analysis_scripts/bankroll_replay_kelly.py`
# without installing the package — reuse helpers from bankroll_replay.py.
sys.path.insert(0, str(Path(__file__).resolve().parent))
from bankroll_replay import (  # noqa: E402
    ReplayConfig,
    ReplayResult,
    Summary as BaseSummary,
    compute_summary as base_compute_summary,
    load_shoe_library,
    replay_once,
)


class _TeeTextIO:
    """Duplicate ``write`` / ``flush`` to two text streams (e.g. TTY + log file)."""

    def __init__(self, primary: TextIO, secondary: TextIO) -> None:
        self._primary = primary
        self._secondary = secondary

    def write(self, s: str) -> int:
        self._primary.write(s)
        self._secondary.write(s)
        self._secondary.flush()
        return len(s)

    def flush(self) -> None:
        self._primary.flush()
        self._secondary.flush()

    def isatty(self) -> bool:
        return self._primary.isatty()


# ── Kelly EV table (μ, σ² per TC bucket) ───────────────────────────────────────

TC_STEP = 0.5  # true-count buckets are on 0.5 steps


@dataclass
class KellyEVTable:
    """
    Dense lookup of EV-per-dollar (mu) and variance-per-dollar (sigma2) keyed
    by true-count bucket.  Missing buckets are flagged in `present`.
    """
    tc_min_scaled: int           # min(2*TC) present
    tc_max_scaled: int           # max(2*TC) present
    mu: np.ndarray               # float64, length = tc_max_scaled - tc_min_scaled + 1
    var: np.ndarray              # float64, same shape
    present: np.ndarray          # bool,    same shape
    n_buckets: int

    @classmethod
    def from_csv(cls, path: str, min_hands: int = 0) -> "KellyEVTable":
        df = pd.read_csv(path)
        required = {"TrueCount", "HandsPlayed", "EVPerDollar", "StdErrorPerDollar"}
        missing = required - set(df.columns)
        if missing:
            raise SystemExit(f"EV CSV missing columns: {sorted(missing)}")

        if min_hands > 0:
            df = df[df["HandsPlayed"] >= min_hands].copy()
        if df.empty:
            raise SystemExit(
                f"EV CSV has no rows after HandsPlayed >= {min_hands} filter."
            )

        tc_vals = df["TrueCount"].to_numpy(dtype=np.float64)
        mu_src = df["EVPerDollar"].to_numpy(dtype=np.float64)
        hands = df["HandsPlayed"].to_numpy(dtype=np.float64)
        sd_src = df["StdErrorPerDollar"].to_numpy(dtype=np.float64)
        var_src = (sd_src * np.sqrt(hands)) ** 2  # variance per dollar

        tc_scaled = np.round(tc_vals / TC_STEP).astype(np.int64)
        tc_min_scaled = int(tc_scaled.min())
        tc_max_scaled = int(tc_scaled.max())
        span = tc_max_scaled - tc_min_scaled + 1

        mu = np.zeros(span, dtype=np.float64)
        var = np.zeros(span, dtype=np.float64)
        present = np.zeros(span, dtype=bool)
        for i, s in enumerate(tc_scaled):
            j = int(s) - tc_min_scaled
            mu[j] = float(mu_src[i])
            var[j] = float(var_src[i])
            present[j] = True

        return cls(
            tc_min_scaled=tc_min_scaled,
            tc_max_scaled=tc_max_scaled,
            mu=mu,
            var=var,
            present=present,
            n_buckets=int(present.sum()),
        )


class KellySizer:
    """
    Kelly bet sizer — drop-in replacement for `Spread` in replay_once.

    Provides bet_vec(tc: np.ndarray) -> np.ndarray, sized by:
        continuous = clip(B0 * k * mu(tc) / sigma2(tc), table_min, table_max)
        bet = nearest multiple of table_min (at least one unit), then capped to
              whole units of table_min not above table_max when max is finite.

    Fallbacks (always return table_min):
        - TC bucket missing from EV table
        - sigma2 <= 0
        - mu <= 0 (negative edge — Kelly says don't lean in)
        - any non-finite intermediate value
    """

    def __init__(
        self,
        table: KellyEVTable,
        kelly: float,
        start_bankroll: float,
        table_min: float,
        table_max: Optional[float],
    ):
        self.table = table
        self.kelly = float(kelly)
        self.B0 = float(start_bankroll)
        self.table_min = float(table_min)
        self.table_max = float(table_max) if table_max is not None else float("inf")

        # Precompute B0 * k * mu / var per bucket; flag invalid with NaN.
        with np.errstate(divide="ignore", invalid="ignore"):
            raw = self.B0 * self.kelly * table.mu / table.var
        valid = table.present & (table.var > 0) & (table.mu > 0) & np.isfinite(raw)
        self._bucket_bet = np.where(valid, raw, np.nan)

    def _quantize_to_base_units(self, clipped: np.ndarray) -> np.ndarray:
        """
        Round each value to the nearest multiple of ``table_min`` (at least one
        unit). If ``table_max`` is finite, cap to the largest multiple of
        ``table_min`` not exceeding ``table_max``.
        """
        u = self.table_min
        x = np.asarray(clipped, dtype=np.float64)
        k = np.rint(x / u).astype(np.int64)
        k = np.maximum(1, k)
        if np.isfinite(self.table_max):
            k_max = int(np.floor(self.table_max / u))
            k_max = max(1, k_max)
            k = np.minimum(k, k_max)
        return k.astype(np.float64) * u

    def bet_for_tc(self, tc: float) -> float:
        s = int(round(tc / TC_STEP))
        offset = s - self.table.tc_min_scaled
        span = self.table.tc_max_scaled - self.table.tc_min_scaled + 1
        if offset < 0 or offset >= span:
            return self.table_min
        raw = self._bucket_bet[offset]
        if not np.isfinite(raw):
            return float(self.table_min)
        cont = float(np.clip(raw, self.table_min, self.table_max))
        return float(self._quantize_to_base_units(np.array([cont]))[0])

    def bet_vec(self, tc: np.ndarray) -> np.ndarray:
        tc64 = tc.astype(np.float64, copy=False)
        scaled = np.round(tc64 / TC_STEP).astype(np.int64)
        offset = scaled - self.table.tc_min_scaled
        span = self.table.tc_max_scaled - self.table.tc_min_scaled + 1
        in_range = (offset >= 0) & (offset < span)
        clipped = np.where(in_range, offset, 0)
        raw = self._bucket_bet[clipped]          # NaN where invalid
        valid = in_range & np.isfinite(raw)
        out = np.where(valid, raw, self.table_min)
        clipped = np.clip(out, self.table_min, self.table_max)
        return self._quantize_to_base_units(clipped)

    def describe(self) -> str:
        max_txt = "none" if not np.isfinite(self.table_max) else f"${self.table_max:.0f}"
        return (
            f"Kelly k={self.kelly:.3f} | B0=${self.B0:,.0f} | "
            f"min=${self.table_min:.0f} / max={max_txt}"
        )

    def spread_stats_from_ev_grid(self) -> tuple[float, float, float, float, float, int, int, int]:
        """
        Kelly-sized bet at each 0.5 TC step covered by the EV table (same as
        replay sizing). Returns:
            tc_lo, tc_hi, bet_min, bet_max, bet_median, spread_ratio,
            n_at_floor, n_at_cap
        """
        span = self.table.tc_max_scaled - self.table.tc_min_scaled + 1
        scaled = np.arange(span, dtype=np.int64) + self.table.tc_min_scaled
        tc = scaled.astype(np.float64) * TC_STEP
        bets = self.bet_vec(tc)
        bet_min = float(np.min(bets))
        bet_max = float(np.max(bets))
        bet_med = float(np.median(bets))
        lo = float(tc[0])
        hi = float(tc[-1])
        ratio = (bet_max / bet_min) if bet_min > 0 else float("nan")
        eps = 1e-6
        n_floor = int(np.sum(np.abs(bets - self.table_min) < eps))
        if np.isfinite(self.table_max):
            n_cap = int(np.sum(bets >= self.table_max - eps))
        else:
            n_cap = 0
        return lo, hi, bet_min, bet_max, bet_med, ratio, n_floor, n_cap


# ── summary (base + user-configurable bankroll thresholds) ─────────────────────

@dataclass
class KellySummary:
    kelly: float
    base: BaseSummary
    threshold_fractions: tuple[float, ...]   # as fractions of B0
    threshold_probs: tuple[float, ...]       # P(end < frac * B0)


def compute_kelly_summary(
    results: list[ReplayResult],
    start_bankroll: float,
    kelly: float,
    threshold_fractions: Sequence[float],
) -> KellySummary:
    base = base_compute_summary(results, start_bankroll)
    end_b = np.array([r.end_bankroll for r in results])
    probs = tuple(
        float(np.mean(end_b < frac * start_bankroll)) for frac in threshold_fractions
    )
    return KellySummary(
        kelly=kelly,
        base=base,
        threshold_fractions=tuple(threshold_fractions),
        threshold_probs=probs,
    )


# ── printing ───────────────────────────────────────────────────────────────────

def print_kelly_summary(
    s: KellySummary, config: ReplayConfig, sizer: KellySizer
) -> None:
    W = 70
    print()
    print("=" * W)
    print(f"KELLY BANKROLL REPLAY  —  k = {s.kelly:.3f}")
    print("=" * W)
    print(f"{'Replays':30s} {s.base.num_replays:>20,}")
    print(f"{'Shoes per session':30s} {config.shoes_target:>20,}")
    print(f"{'Starting bankroll (B0)':30s} ${s.base.start_bankroll:>19,.2f}")
    tmax = "none" if config.table_max is None else f"${config.table_max:.0f}"
    print(f"{'Table min / max':30s}  ${config.table_min:.0f} / {tmax}")
    print(f"{'Ruin mode':30s} {config.ruin_mode:>20s}")
    print(f"{'Sizer':30s} {sizer.describe()}")
    print()
    print("CALCULATED BET SPREAD (EV grid, per TC step, clipped)")
    print("-" * W)
    tc_lo, tc_hi, bmin, bmax, bmed, ratio, n_fl, n_cap = (
        sizer.spread_stats_from_ev_grid()
    )
    print(f"{'TC range (CSV grid)':30s} {tc_lo:>+9.1f} .. {tc_hi:>+9.1f}")
    print(f"{'Bet min / max':30s} ${bmin:>9,.0f}  /  ${bmax:>9,.0f}")
    print(f"{'Median bucket bet':30s} ${bmed:>19,.0f}")
    rat_txt = f"{ratio:,.2f}×" if np.isfinite(ratio) else "n/a"
    print(f"{'Spread (max / min)':30s} {rat_txt:>20s}")
    print(f"{'Buckets at table min':30s} {n_fl:>20,}")
    if np.isfinite(sizer.table_max):
        print(f"{'Buckets at table max':30s} {n_cap:>20,}")
    print()
    print("CALCULATED BET BY TC (integer TC +1 .. +10)")
    print("-" * W)
    for tc in range(1, 11):
        b = int(round(sizer.bet_for_tc(float(tc))))
        print(f"{f'TC +{tc}':30s} ${b:>19,}")
    print()
    print("USER PRIORITIES")
    print("-" * W)
    print(f"{'E[profit]  = E[end − B0]':30s} ${s.base.mean_end_profit:>+19,.2f}")
    print(f"{'P(finish down)  (end < B0)':30s} {s.base.prob_finish_down:>20.2%}")
    print(f"{'P(ruin)':30s} {s.base.prob_ruin:>20.2%}")
    for frac, p in zip(s.threshold_fractions, s.threshold_probs):
        label = f"P(end < {frac:g}·B0)"
        print(f"{label:30s} {p:>20.2%}")
    print()
    print("BANKROLL DISTRIBUTION")
    print("-" * W)
    print(f"{'E[ending bankroll]':30s} ${s.base.mean_end_bankroll:>19,.2f}")
    print(f"{'Median ending bankroll':30s} ${s.base.median_end_bankroll:>19,.2f}")
    print(
        f"{'95% range [p2.5 .. p97.5]':30s} "
        f"${s.base.p025_bankroll:>9,.2f}  ..  ${s.base.p975_bankroll:>9,.2f}"
    )
    print(f"{'P(lose > 25%)':30s} {s.base.prob_lose_25pct:>20.2%}")
    print(f"{'P(lose > 50%)':30s} {s.base.prob_lose_50pct:>20.2%}")
    print(f"{'P(double bankroll)':30s} {s.base.prob_double:>20.2%}")
    print()
    print("DRAWDOWN DISTRIBUTION")
    print("-" * W)
    print(f"{'Mean max drawdown':30s} ${s.base.mean_max_drawdown:>19,.2f}")
    print(f"{'Median max drawdown':30s} ${s.base.median_max_drawdown:>19,.2f}")
    print(f"{'95th pct max drawdown':30s} ${s.base.p95_max_drawdown:>19,.2f}")
    print(f"{'99th pct max drawdown':30s} ${s.base.p99_max_drawdown:>19,.2f}")
    print()
    print("RETURN TO PLAYER")
    print("-" * W)
    print(f"{'RTP':30s} {s.base.rtp:>20.4%}")
    print(f"{'Avg session wagered':30s} ${s.base.mean_total_wagered:>19,.2f}")
    print()
    print("SESSION STATS")
    print("-" * W)
    print(f"{'Avg shoes played':30s} {s.base.mean_shoes_used:>20.1f}")
    print(f"{'Avg hands played':30s} {s.base.mean_hands_used:>20.1f}")
    print("=" * W)


# ── figure ─────────────────────────────────────────────────────────────────────

def plot_kelly_sweep(
    summaries: list[KellySummary], out_path: Optional[str]
) -> None:
    try:
        import matplotlib.pyplot as plt
    except ImportError:
        print(
            "matplotlib not installed — skipping figure. Install with "
            "`pip install matplotlib`.",
            file=sys.stderr,
        )
        return

    if len(summaries) < 1:
        return

    ks = np.array([s.kelly for s in summaries])
    exp_profit = np.array([s.base.mean_end_profit for s in summaries])
    p_ruin = np.array([s.base.prob_ruin for s in summaries])
    p_down = np.array([s.base.prob_finish_down for s in summaries])

    order = np.argsort(ks)
    ks = ks[order]
    exp_profit = exp_profit[order]
    p_ruin = p_ruin[order]
    p_down = p_down[order]

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
    ax2.plot(
        ks, p_down,
        color="tab:orange", marker="^", linestyle="--", alpha=0.8,
        label="P(finish down)",
    )
    ax2.tick_params(axis="y", labelcolor=color_ruin)
    probs_all = np.concatenate([p_ruin, p_down])
    ax2.set_ylim(0, max(0.05, float(np.max(probs_all)) * 1.15))

    ax1.set_title(
        f"Kelly sweep — expected profit vs risk  ({len(summaries)} k values)"
    )
    lines1, labels1 = ax1.get_legend_handles_labels()
    lines2, labels2 = ax2.get_legend_handles_labels()
    ax2.legend(lines1 + lines2, labels1 + labels2, loc="best")

    fig.tight_layout()
    if out_path:
        fig.savefig(out_path, dpi=120)
        print(f"\nFigure saved to: {out_path}")
    else:
        plt.show()
    plt.close(fig)


# ── simulation loop ────────────────────────────────────────────────────────────

def simulate_distribution(
    lib,
    sizer: KellySizer,
    config: ReplayConfig,
) -> list[ReplayResult]:
    rng = np.random.default_rng(config.seed)
    results: list[ReplayResult] = []
    report_interval = max(1, config.num_replays // 10)
    t0 = time.time()
    for i in range(config.num_replays):
        results.append(replay_once(lib, sizer, config, rng))
        if (i + 1) % report_interval == 0 or (i + 1) == config.num_replays:
            pct = (i + 1) / config.num_replays * 100
            elapsed = time.time() - t0
            rate = (i + 1) / elapsed if elapsed > 0 else float("inf")
            print(
                f"    {i + 1:>6,}/{config.num_replays:,}  ({pct:5.1f}%)  "
                f"{rate:,.0f} replays/s",
                flush=True,
            )
    return results


# ── CLI argument parsing helpers ───────────────────────────────────────────────

def parse_kelly_grid(args: argparse.Namespace) -> list[float]:
    """
    Resolve the Kelly fractions to sweep. --kelly-grid wins if given;
    otherwise fall back to --kelly-min / --kelly-max / --kelly-steps.
    """
    if args.kelly_grid:
        vals = [float(x) for x in args.kelly_grid.split(",") if x.strip()]
    else:
        if args.kelly_min is None or args.kelly_max is None or args.kelly_steps is None:
            raise SystemExit(
                "Provide --kelly-grid OR all of --kelly-min/--kelly-max/--kelly-steps."
            )
        if args.kelly_steps < 1:
            raise SystemExit("--kelly-steps must be >= 1")
        if args.kelly_steps == 1:
            vals = [float(args.kelly_min)]
        else:
            vals = list(np.linspace(args.kelly_min, args.kelly_max, args.kelly_steps))

    vals = [float(v) for v in vals]
    bad = [v for v in vals if v <= 0]
    if bad:
        raise SystemExit(f"All Kelly fractions must be positive. Got: {bad}")
    # De-dup while preserving order
    seen: set[float] = set()
    out: list[float] = []
    for v in vals:
        if v not in seen:
            seen.add(v)
            out.append(v)
    return out


def parse_thresholds(spec: str, start_bankroll: float) -> list[float]:
    """
    Parse --bankroll-thresholds.

    Values <= 1 are treated as fractions of B0.
    Values > 1 are treated as absolute dollar amounts and converted to fractions.
    Returns a list of fractions (applied as `P(end < frac * B0)`).
    """
    if not spec:
        return []
    out: list[float] = []
    for item in spec.split(","):
        item = item.strip()
        if not item:
            continue
        v = float(item)
        if v <= 0:
            raise SystemExit(f"Threshold must be > 0, got {v}")
        frac = v if v <= 1.0 else v / start_bankroll
        out.append(frac)
    return out


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Monte Carlo bankroll replay sized by fractional Kelly from a "
            "per-TC EV CSV.  Uses only the STARTING bankroll for sizing — no "
            "bankroll-path feedback."
        ),
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        "--trace",
        required=True,
        help="Hand trace CSV or .csv.zst (same format as bankroll_replay.py).",
    )
    parser.add_argument(
        "--ev-csv",
        required=True,
        help="Per-TC EV CSV under data/ev_per_tc_data/... "
             "(TrueCount, HandsPlayed, EVPerDollar, StdErrorPerDollar).",
    )
    parser.add_argument(
        "--start-bankroll", "--bankroll",
        type=float, default=10000.0, dest="start_bankroll",
        help="Starting bankroll B0 (dollars). Also the only bankroll used in "
             "the Kelly sizing formula.",
    )
    parser.add_argument(
        "--table-min", "--min-bet",
        type=float, default=25.0, dest="table_min",
        help="Table minimum bet / floor on Kelly sizing.",
    )
    parser.add_argument(
        "--table-max",
        type=float, default=None,
        help="Table maximum bet (hard cap).",
    )
    parser.add_argument(
        "--shoes",
        type=int, default=100, dest="shoes_target",
        help="Shoes per replay session.",
    )
    parser.add_argument(
        "--replays",
        type=int, default=10000, dest="num_replays",
        help="Number of Monte Carlo replays per Kelly fraction.",
    )

    # Kelly sweep — either explicit grid or min/max/steps
    parser.add_argument(
        "--kelly-grid", default="0.5,1.0",
        help='Comma-separated Kelly fractions, e.g. "0.25,0.5,0.75,1.0".',
    )
    parser.add_argument("--kelly-min", type=float, default=None)
    parser.add_argument("--kelly-max", type=float, default=None)
    parser.add_argument("--kelly-steps", type=int, default=None)

    parser.add_argument(
        "--bankroll-thresholds",
        default="0.9,0.75,0.5",
        help="Comma-separated thresholds for P(end < threshold). "
             "Values <=1 are fractions of B0, values >1 are absolute dollars.",
    )
    parser.add_argument(
        "--min-hands-per-tc",
        type=int, default=0,
        help="Drop TC buckets with fewer than this many HandsPlayed from the EV CSV.",
    )
    parser.add_argument(
        "--ruin-mode", choices=["anytime", "end"], default="anytime",
        help="'anytime': ruin whenever pre-hand bankroll < table_min; "
             "'end': ruin only if end-of-session bankroll < table_min.",
    )
    parser.add_argument("--seed", type=int, default=42)
    parser.add_argument(
        "--max-shoes", type=int, default=None,
        help="Cap on shoes loaded from the trace file.",
    )
    parser.add_argument(
        "--output", default=None,
        help="Optional figure output path (e.g. kelly_sweep.png). "
             "Omit to display interactively (if matplotlib backend allows). "
             "When set, the same basename with a .md extension receives a copy "
             "of everything printed to stdout.",
    )
    return parser.parse_args()


def _run_session(args: argparse.Namespace) -> None:
    kelly_vals = parse_kelly_grid(args)
    thresholds = parse_thresholds(args.bankroll_thresholds, args.start_bankroll)

    config = ReplayConfig(
        start_bankroll=args.start_bankroll,
        shoes_target=args.shoes_target,
        num_replays=args.num_replays,
        table_min=args.table_min,
        table_max=args.table_max,
        ruin_mode=args.ruin_mode,
        seed=args.seed,
    )

    print(f"Loading EV table from: {args.ev_csv}")
    ev_table = KellyEVTable.from_csv(args.ev_csv, min_hands=args.min_hands_per_tc)
    tc_lo = ev_table.tc_min_scaled * TC_STEP
    tc_hi = ev_table.tc_max_scaled * TC_STEP
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

    summaries: list[KellySummary] = []
    for k in kelly_vals:
        sizer = KellySizer(
            table=ev_table,
            kelly=k,
            start_bankroll=args.start_bankroll,
            table_min=args.table_min,
            table_max=args.table_max,
        )
        print(
            f"\n→ Kelly k={k:.3f}   "
            f"({args.num_replays:,} replays × {args.shoes_target} shoes)"
        )
        t0 = time.time()
        results = simulate_distribution(lib, sizer, config)
        print(f"  done in {time.time() - t0:.1f}s")
        s = compute_kelly_summary(results, args.start_bankroll, k, thresholds)
        summaries.append(s)
        print_kelly_summary(s, config, sizer)

    plot_kelly_sweep(summaries, args.output)


# ── main ───────────────────────────────────────────────────────────────────────

def main() -> None:
    args = parse_args()

    if args.start_bankroll <= 0:
        raise SystemExit("--start-bankroll must be positive")
    if args.table_min <= 0:
        raise SystemExit("--table-min must be positive")
    if args.table_max is not None and args.table_max < args.table_min:
        raise SystemExit("--table-max must be >= --table-min")
    if args.shoes_target < 1:
        raise SystemExit("--shoes must be >= 1")
    if args.num_replays < 1:
        raise SystemExit("--replays must be >= 1")

    log_path: Optional[Path] = None
    log_file: Optional[TextIO] = None
    real_stdout = sys.stdout
    if args.output:
        log_path = Path(args.output).with_suffix(".md")
        log_file = open(log_path, "w", encoding="utf-8")
        sys.stdout = _TeeTextIO(real_stdout, log_file)

    try:
        _run_session(args)
    finally:
        if log_file is not None:
            sys.stdout = real_stdout
            log_file.close()
            print(f"\nSession log (stdout) saved to: {log_path}", file=real_stdout)


if __name__ == "__main__":
    main()
