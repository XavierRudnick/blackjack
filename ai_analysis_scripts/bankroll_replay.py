#!/usr/bin/env python3
"""
Bankroll Outcome Engine — Shoe-Based Monte Carlo Replay

Estimates bankroll outcome distribution after X shoes by sampling random shoes
from a simulated shoe library and replaying bankroll paths.

All saved shoe traces were generated with initial_bet = 1 (unit bet).
net_profit is stored in units; total_money_committed is stored in units.

During replay:
    bet_h     = Spread(true_count_bucket_h)          [dollars]
    profit_h  = bet_h * net_profit_h                 [dollars]
    B_{h+1}   = B_h + profit_h

Usage:
    python3 ai_analysis_scripts/bankroll_replay.py \\
        --trace stats/shoetraces/hand_traces_HiLoStrategy_6deck_75pen_H17_DAS_NoRAS_NoSurrender_3to2.csv.zst \\
        --spread "1:25,2:75,3:150" \\
        --min-bet 25 \\
        --start-bankroll 10000 \\
        --shoes 100 \\
        --replays 10000

Spread format:  "K:amount,K:amount,..."
    K         = integer TC bucket  (1, 2, 3, ...)
    amount    = bet size in dollars for TC in (K-1, K]
    TC <= 0   always uses --min-bet
    TC >= max_K uses the largest-K bet

Example: "1:25,2:75,3:150"
    TC <= 0  -> $25 (min-bet)
    TC = 1   -> $25
    TC = 2   -> $75
    TC >= 3  -> $150
"""

from __future__ import annotations

import argparse
import csv as csv_mod
import io
import math
import sys
import time
from dataclasses import dataclass, fields
from typing import Optional

import numpy as np
import pandas as pd


# ── trace I/O (copied from aggregate_shoe_traces.py) ──────────────────────────

def read_trace(path: str) -> pd.DataFrame:
    """Read a hand trace CSV — plain .csv or Zstandard-compressed (.zst, .csv.zst)."""
    kwargs: dict = {"low_memory": False}
    if not path.endswith(".zst"):
        return pd.read_csv(path, **kwargs)

    try:
        import zstandard as zstd
    except ImportError as exc:
        raise ImportError(
            "Reading Zstandard (.zst) trace files requires the 'zstandard' package. "
            "Install with: python3 -m pip install zstandard"
        ) from exc

    open_fn = getattr(zstd, "open", None)
    if open_fn is not None:
        with open_fn(path, mode="rt", encoding="utf-8", newline="") as handle:
            return pd.read_csv(handle, **kwargs)

    with open(path, "rb") as raw:
        dctx = zstd.ZstdDecompressor()
        with dctx.stream_reader(raw) as strm:
            text = io.TextIOWrapper(strm, encoding="utf-8", newline="")
            return pd.read_csv(text, **kwargs)


def normalize_trace_columns(df: pd.DataFrame) -> pd.DataFrame:
    """Ensure the TC column is named 'true_count_bucket'."""
    if "true_count_bucket" not in df.columns:
        if "tc_bucket" in df.columns:
            df = df.rename(columns={"tc_bucket": "true_count_bucket"})
        elif "true_count_before_hand_raw" in df.columns:
            df = df.copy()
            df["true_count_bucket"] = (
                (df["true_count_before_hand_raw"] / 0.5).round() * 0.5
            )
        else:
            raise KeyError(
                "Trace CSV must include tc_bucket, true_count_bucket, or "
                "true_count_before_hand_raw"
            )
    return df


# ── shoe library ───────────────────────────────────────────────────────────────

@dataclass
class ShoeLibrary:
    """
    Compact, flat-array shoe store.

    All hands from all shoes are concatenated into three 1-D numpy arrays.
    Per-shoe boundaries are tracked via 'starts' and 'lengths'.
    """
    tc: np.ndarray               # float32, shape (total_hands,)
    profit: np.ndarray           # float64, shape (total_hands,)
    money_committed: np.ndarray  # float64, shape (total_hands,) — units wagered per hand
    starts: np.ndarray           # int64,   shape (n_shoes,)
    lengths: np.ndarray          # int32,   shape (n_shoes,)
    n_shoes: int

    def get_shoe(self, idx: int) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
        s = int(self.starts[idx])
        l = int(self.lengths[idx])
        return self.tc[s:s + l], self.profit[s:s + l], self.money_committed[s:s + l]


def load_shoe_library(path: str, max_shoes: Optional[int] = None) -> ShoeLibrary:
    """
    Load shoe traces from a CSV (plain or .zst compressed) into a ShoeLibrary.

    Only the columns needed for replay are retained:
        shoe_id, hand_number_in_shoe, true_count_bucket, net_profit

    If max_shoes is given, only the first max_shoes unique shoe IDs are loaded
    (IDs are sorted ascending before truncation).
    """
    print(f"Loading traces from: {path}")
    t0 = time.time()

    df = read_trace(path)
    df = normalize_trace_columns(df)

    required = {"shoe_id", "hand_number_in_shoe", "true_count_bucket", "net_profit",
                "total_money_committed"}
    missing = required - set(df.columns)
    if missing:
        raise SystemExit(f"Trace file is missing required columns: {sorted(missing)}")

    df = df[["shoe_id", "hand_number_in_shoe", "true_count_bucket", "net_profit",
             "total_money_committed"]]

    # Sort to guarantee hand order within each shoe
    df = df.sort_values(["shoe_id", "hand_number_in_shoe"]).reset_index(drop=True)

    # Apply shoe cap before building the library
    if max_shoes is not None:
        unique_ids = np.sort(df["shoe_id"].unique())[:max_shoes]
        df = df[df["shoe_id"].isin(unique_ids)].reset_index(drop=True)

    # Build flat arrays + per-shoe offsets
    tc_flat = df["true_count_bucket"].to_numpy(dtype=np.float32)
    profit_flat = df["net_profit"].to_numpy(dtype=np.float64)
    committed_flat = df["total_money_committed"].to_numpy(dtype=np.float64)

    shoe_ids = df["shoe_id"].to_numpy()
    _, first_occurrences, counts = np.unique(
        shoe_ids, return_index=True, return_counts=True
    )
    starts = first_occurrences.astype(np.int64)
    lengths = counts.astype(np.int32)
    n_shoes = len(starts)

    elapsed = time.time() - t0
    total_hands = len(tc_flat)
    avg_hands = total_hands / n_shoes if n_shoes > 0 else 0
    print(
        f"  Loaded {n_shoes:,} shoes | {total_hands:,} hands "
        f"(avg {avg_hands:.1f} hands/shoe) in {elapsed:.1f}s"
    )

    return ShoeLibrary(
        tc=tc_flat,
        profit=profit_flat,
        money_committed=committed_flat,
        starts=starts,
        lengths=lengths,
        n_shoes=n_shoes,
    )


# ── spread ─────────────────────────────────────────────────────────────────────

@dataclass
class Spread:
    """
    Maps true count → bet size in dollars.

    Spread spec format: "1:25,2:75,3:150"
      Key K (integer) = bet for TC in (K-1, K], i.e. ceil(tc) == K
      TC <= 0         = min_bet
      TC >= max key   = bet of max key
    """
    min_bet: float
    mapping: dict  # {int_ceil_tc: float_bet}

    @classmethod
    def parse(cls, spec: str, min_bet: float) -> "Spread":
        mapping: dict[int, float] = {}
        for item in spec.split(","):
            item = item.strip()
            if not item:
                continue
            tc_text, bet_text = item.split(":", 1)
            tc = int(tc_text.strip())
            if tc < 1:
                raise ValueError(
                    f"Spread key must be >= 1 (TC <= 0 always uses min_bet). Got: {tc}"
                )
            bet = float(bet_text.strip())
            if bet <= 0:
                raise ValueError(f"Bet must be positive. Got: {bet}")
            mapping[tc] = bet

        if not mapping:
            raise ValueError("Spread spec is empty.")

        # Ensure contiguous keys 1 .. max_key
        max_key = max(mapping)
        missing = [k for k in range(1, max_key + 1) if k not in mapping]
        if missing:
            raise ValueError(f"Spread is missing TC thresholds: {missing}")

        return cls(min_bet=min_bet, mapping=mapping)

    def bet_for_tc(self, tc: float) -> float:
        """Return dollar bet for a single true count value."""
        if tc <= 0:
            return self.min_bet
        bucket = math.ceil(tc)
        max_key = max(self.mapping)
        bucket = min(bucket, max_key)
        return self.mapping.get(bucket, self.min_bet)

    def bet_vec(self, tc: np.ndarray) -> np.ndarray:
        """Vectorised: return dollar bet array for a TC array."""
        ceil_tc = np.ceil(tc).clip(min=0).astype(np.int32)
        out = np.full(len(tc), self.min_bet, dtype=np.float64)
        # Apply thresholds in ascending order so higher TCs correctly override
        for threshold in sorted(self.mapping):
            out[ceil_tc >= threshold] = self.mapping[threshold]
        return out

    def describe(self, start_tc: int = 1) -> str:
        parts = [f"TC<=0: ${self.min_bet:.0f}"]
        for k in sorted(self.mapping):
            parts.append(f"TC={k}: ${self.mapping[k]:.0f}")
        return " | ".join(parts)


# ── config / result dataclasses ────────────────────────────────────────────────

@dataclass
class ReplayConfig:
    start_bankroll: float
    shoes_target: int
    num_replays: int
    table_min: float
    table_max: Optional[float] = None
    ruin_mode: str = "anytime"   # "anytime" | "end"
    seed: int = 42


@dataclass
class ReplayResult:
    end_bankroll: float
    end_profit: float
    min_bankroll: float
    max_drawdown: float
    ruin_occurred: bool
    shoes_used: int
    hands_used: int
    total_wagered: float   # sum of bet * total_money_committed across all hands played


# ── single replay ──────────────────────────────────────────────────────────────

def replay_once(
    lib: ShoeLibrary,
    spread: Spread,
    config: ReplayConfig,
    rng: np.random.Generator,
) -> ReplayResult:
    B = config.start_bankroll
    peak = B
    min_B = B
    max_dd = 0.0
    ruin = False
    shoes_used = 0
    hands_used = 0
    total_wagered = 0.0

    table_max_val = config.table_max if config.table_max is not None else np.inf
    shoe_indices = rng.integers(0, lib.n_shoes, size=config.shoes_target)

    for shoe_idx in shoe_indices:
        tc_arr, profit_arr, committed_arr = lib.get_shoe(int(shoe_idx))

        # Compute scaled bets for every hand in the shoe (vectorised)
        bets = spread.bet_vec(tc_arr)
        bets = np.clip(bets, config.table_min, table_max_val)

        # Cumulative bankroll path:
        #   path[0]   = B  (bankroll before hand 0)
        #   path[i]   = bankroll before hand i  (= after hand i-1)
        #   path[-1]  = bankroll after final hand
        dollar_profits = bets * profit_arr
        cumulative = np.empty(len(dollar_profits) + 1, dtype=np.float64)
        cumulative[0] = 0.0
        np.cumsum(dollar_profits, out=cumulative[1:])
        path = B + cumulative

        # Ruin check: bankroll BEFORE each hand < table_min (spec Step 3).
        # Only stop mid-session when ruin_mode == "anytime"; in "end" mode we
        # play every hand of every shoe so full-shoe counts stay efficient.
        if config.ruin_mode == "anytime":
            ruin_mask = path[:-1] < config.table_min
            if ruin_mask.any():
                k = int(np.argmax(ruin_mask))  # first hand player can't afford
                # Stop without playing hand k; bankroll stays at path[k]
                end_path = path[: k + 1]
                hands_this = k
                ruin = True
            else:
                end_path = path
                hands_this = len(tc_arr)
        else:
            end_path = path
            hands_this = len(tc_arr)

        # Update tracking stats (vectorised)
        shoe_peak = float(end_path.max())
        shoe_min = float(end_path.min())
        running_peak = np.maximum.accumulate(end_path)
        shoe_max_dd = float((running_peak - end_path).max())

        if shoe_peak > peak:
            peak = shoe_peak
        if shoe_min < min_B:
            min_B = shoe_min
        if shoe_max_dd > max_dd:
            max_dd = shoe_max_dd

        # Wagered = bet * total_money_committed (accounts for splits/doubles)
        # Only count hands that were actually played (first hands_this hands)
        total_wagered += float(np.sum(bets[:hands_this] * committed_arr[:hands_this]))

        B = float(end_path[-1])
        hands_used += hands_this
        shoes_used += 1

        if ruin:
            break

    # Post-loop ruin check for ruin_mode="end": ruin only if bankroll < table_min at end
    if config.ruin_mode == "end":
        ruin = B < config.table_min

    return ReplayResult(
        end_bankroll=B,
        end_profit=B - config.start_bankroll,
        min_bankroll=min_B,
        max_drawdown=max_dd,
        ruin_occurred=ruin,
        shoes_used=shoes_used,
        hands_used=hands_used,
        total_wagered=total_wagered,
    )


# ── monte carlo loop ───────────────────────────────────────────────────────────

def simulate_distribution(
    lib: ShoeLibrary,
    spread: Spread,
    config: ReplayConfig,
) -> list[ReplayResult]:
    rng = np.random.default_rng(config.seed)
    results: list[ReplayResult] = []
    report_interval = max(1, config.num_replays // 10)
    t0 = time.time()

    for i in range(config.num_replays):
        results.append(replay_once(lib, spread, config, rng))
        if (i + 1) % report_interval == 0 or (i + 1) == config.num_replays:
            pct = (i + 1) / config.num_replays * 100
            elapsed = time.time() - t0
            rate = (i + 1) / elapsed if elapsed > 0 else float("inf")
            print(
                f"  {i + 1:>6,}/{config.num_replays:,}  ({pct:5.1f}%)  "
                f"{rate:,.0f} replays/s",
                flush=True,
            )

    return results


# ── summary statistics ─────────────────────────────────────────────────────────

@dataclass
class Summary:
    num_replays: int
    start_bankroll: float
    # Bankroll distribution
    mean_end_bankroll: float
    median_end_bankroll: float
    mean_end_profit: float
    p025_bankroll: float
    p975_bankroll: float
    # Outcome probabilities
    prob_finish_down: float
    prob_lose_25pct: float
    prob_lose_50pct: float
    prob_double: float
    prob_ruin: float
    # Drawdown
    mean_max_drawdown: float
    median_max_drawdown: float
    p95_max_drawdown: float
    p99_max_drawdown: float
    # RTP
    rtp: float                  # 1 + E[profit] / E[wagered]
    mean_total_wagered: float   # average dollars wagered per session
    # Session stats
    mean_shoes_used: float
    mean_hands_used: float


def compute_summary(results: list[ReplayResult], start_bankroll: float) -> Summary:
    n = len(results)
    end_b = np.array([r.end_bankroll for r in results])
    drawdowns = np.array([r.max_drawdown for r in results])
    ruined = np.array([r.ruin_occurred for r in results], dtype=bool)
    wagered = np.array([r.total_wagered for r in results])
    profits = end_b - start_bankroll

    mean_wagered = float(np.mean(wagered))
    mean_profit = float(np.mean(profits))
    rtp = (1.0 + mean_profit / mean_wagered) if mean_wagered > 0 else float("nan")

    return Summary(
        num_replays=n,
        start_bankroll=start_bankroll,
        mean_end_bankroll=float(np.mean(end_b)),
        median_end_bankroll=float(np.percentile(end_b, 50)),
        mean_end_profit=mean_profit,
        p025_bankroll=float(np.percentile(end_b, 2.5)),
        p975_bankroll=float(np.percentile(end_b, 97.5)),
        prob_finish_down=float(np.mean(end_b < start_bankroll)),
        prob_lose_25pct=float(np.mean(end_b < 0.75 * start_bankroll)),
        prob_lose_50pct=float(np.mean(end_b < 0.50 * start_bankroll)),
        prob_double=float(np.mean(end_b >= 2.0 * start_bankroll)),
        prob_ruin=float(np.mean(ruined)),
        mean_max_drawdown=float(np.mean(drawdowns)),
        median_max_drawdown=float(np.percentile(drawdowns, 50)),
        p95_max_drawdown=float(np.percentile(drawdowns, 95)),
        p99_max_drawdown=float(np.percentile(drawdowns, 99)),
        rtp=rtp,
        mean_total_wagered=mean_wagered,
        mean_shoes_used=float(np.mean([r.shoes_used for r in results])),
        mean_hands_used=float(np.mean([r.hands_used for r in results])),
    )


# ── output helpers ─────────────────────────────────────────────────────────────

def print_summary(s: Summary, config: ReplayConfig, spread: Spread) -> None:
    W = 70
    print()
    print("=" * W)
    print("BANKROLL OUTCOME DISTRIBUTION — SHOE REPLAY MONTE CARLO")
    print("=" * W)
    print(f"{'Replays':30s} {s.num_replays:>20,}")
    print(f"{'Shoes per session':30s} {config.shoes_target:>20,}")
    print(f"{'Starting bankroll':30s} ${s.start_bankroll:>19,.2f}")
    print(f"{'Table min / max':30s}  ${config.table_min:.0f} / "
          f"{'none' if config.table_max is None else f'${config.table_max:.0f}'}")
    print(f"{'Ruin mode':30s} {'':>20s}{'anytime' if config.ruin_mode == 'anytime' else 'end'}")
    print(f"{'Spread':30s} {spread.describe():>20s}")
    print()
    print("BANKROLL DISTRIBUTION")
    print("-" * W)
    print(f"{'E[ending bankroll]':30s} ${s.mean_end_bankroll:>19,.2f}")
    print(f"{'E[profit]':30s} ${s.mean_end_profit:>+19,.2f}")
    print(f"{'Median ending bankroll':30s} ${s.median_end_bankroll:>19,.2f}")
    print(f"{'95% range  [p2.5 .. p97.5]':30s} "
          f"${s.p025_bankroll:>9,.2f}  ..  ${s.p975_bankroll:>9,.2f}")
    print()
    print("OUTCOME PROBABILITIES")
    print("-" * W)
    print(f"{'P(finish down)':30s} {s.prob_finish_down:>20.2%}")
    print(f"{'P(lose > 25%)':30s} {s.prob_lose_25pct:>20.2%}")
    print(f"{'P(lose > 50%)':30s} {s.prob_lose_50pct:>20.2%}")
    print(f"{'P(double bankroll)':30s} {s.prob_double:>20.2%}")
    print(f"{'P(ruin)':30s} {s.prob_ruin:>20.2%}")
    print()
    print("DRAWDOWN DISTRIBUTION")
    print("-" * W)
    print(f"{'Mean max drawdown':30s} ${s.mean_max_drawdown:>19,.2f}")
    print(f"{'Median max drawdown':30s} ${s.median_max_drawdown:>19,.2f}")
    print(f"{'95th pct max drawdown':30s} ${s.p95_max_drawdown:>19,.2f}")
    print(f"{'99th pct max drawdown':30s} ${s.p99_max_drawdown:>19,.2f}")
    print()
    print("RETURN TO PLAYER")
    print("-" * W)
    print(f"{'RTP':30s} {s.rtp:>20.4%}")
    print(f"{'House edge  (1 - RTP)':30s} {1.0 - s.rtp:>+20.4%}")
    print(f"{'Avg session wagered':30s} ${s.mean_total_wagered:>19,.2f}")
    print()
    print("SESSION STATS")
    print("-" * W)
    print(f"{'Avg shoes played':30s} {s.mean_shoes_used:>20.1f}")
    print(f"{'Avg hands played':30s} {s.mean_hands_used:>20.1f}")
    print("=" * W)


def save_results_csv(results: list[ReplayResult], path: str) -> None:
    field_names = [f.name for f in fields(ReplayResult)]
    with open(path, "w", newline="") as fh:
        writer = csv_mod.DictWriter(fh, fieldnames=field_names)
        writer.writeheader()
        for r in results:
            writer.writerow(
                {f.name: getattr(r, f.name) for f in fields(ReplayResult)}
            )
    print(f"Per-replay results saved to: {path}")


# ── CLI ────────────────────────────────────────────────────────────────────────

def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Monte Carlo bankroll replay using shoe trace library.",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        "--trace",
        required=True,
        help="Path to hand trace CSV or .csv.zst file",
    )
    parser.add_argument(
        "--spread",
        required=True,
        help='Spread spec: "1:25,2:75,3:150"  (TC bucket -> dollar bet)',
    )
    parser.add_argument(
        "--min-bet",
        type=float,
        default=25.0,
        help="Bet size for TC <= 0 (also used as table minimum if --table-min not set)",
    )
    parser.add_argument(
        "--start-bankroll",
        type=float,
        default=10000.0,
        help="Starting bankroll in dollars",
    )
    parser.add_argument(
        "--table-min",
        type=float,
        default=None,
        help="Table minimum; defaults to --min-bet",
    )
    parser.add_argument(
        "--table-max",
        type=float,
        default=None,
        help="Table maximum (optional hard cap on bets)",
    )
    parser.add_argument(
        "--shoes",
        type=int,
        default=100,
        dest="shoes_target",
        help="Number of shoes per session (replay)",
    )
    parser.add_argument(
        "--replays",
        type=int,
        default=10000,
        dest="num_replays",
        help="Number of Monte Carlo replays",
    )
    parser.add_argument(
        "--ruin-mode",
        choices=["anytime", "end"],
        default="anytime",
        help="'anytime': ruin when bankroll < table_min at any point; "
             "'end': ruin only if bankroll < table_min at session end",
    )
    parser.add_argument(
        "--seed",
        type=int,
        default=42,
        help="RNG seed for reproducibility",
    )
    parser.add_argument(
        "--max-shoes",
        type=int,
        default=None,
        help="Maximum number of shoes to load from trace file (default: all)",
    )
    parser.add_argument(
        "--output",
        default=None,
        help="Optional path to save per-replay CSV results",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()

    # Resolve table_min default
    table_min = args.table_min if args.table_min is not None else args.min_bet

    # Parse spread
    spread = Spread.parse(args.spread, args.min_bet)

    # Validate
    if args.start_bankroll <= 0:
        raise SystemExit("--start-bankroll must be positive")
    if table_min <= 0:
        raise SystemExit("--table-min must be positive")
    if args.table_max is not None and args.table_max < table_min:
        raise SystemExit("--table-max must be >= table_min")
    if args.shoes_target < 1:
        raise SystemExit("--shoes must be >= 1")
    if args.num_replays < 1:
        raise SystemExit("--replays must be >= 1")

    config = ReplayConfig(
        start_bankroll=args.start_bankroll,
        shoes_target=args.shoes_target,
        num_replays=args.num_replays,
        table_min=table_min,
        table_max=args.table_max,
        ruin_mode=args.ruin_mode,
        seed=args.seed,
    )

    # Load shoe library
    lib = load_shoe_library(args.trace, max_shoes=args.max_shoes)
    if lib.n_shoes == 0:
        raise SystemExit("No shoes loaded — check trace file and --max-shoes.")
    if lib.n_shoes < args.shoes_target:
        print(
            f"Warning: only {lib.n_shoes} shoes available but session requests "
            f"{args.shoes_target}. Sampling with replacement from {lib.n_shoes} shoes.",
            file=sys.stderr,
        )

    # Run simulation
    print(f"\nRunning {args.num_replays:,} replays ({args.shoes_target} shoes/session)...")
    t0 = time.time()
    results = simulate_distribution(lib, spread, config)
    total_time = time.time() - t0
    print(f"\nCompleted in {total_time:.1f}s ({args.num_replays / total_time:,.0f} replays/s)")

    # Summarise and print
    summary = compute_summary(results, args.start_bankroll)
    print_summary(summary, config, spread)

    # Optional CSV output
    if args.output:
        save_results_csv(results, args.output)


if __name__ == "__main__":
    main()
