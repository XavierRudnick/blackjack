"""
aggregate_shoe_traces.py
------------------------
Reads a hand trace CSV produced by runShoeTraceSim() and outputs:
  1. aggregate_*.csv  — statistics per (tc_bucket, sp_bucket)
  2. transitions_*.csv — state transition probabilities between consecutive hands

Usage:
    python3 ai_analysis_scripts/aggregate_shoe_traces.py \
        stats/shoetraces/hand_traces_HiLo_6deck_75pen_H17_DAS_NoRAS_NoSurrender_3to2.csv.zst

Or to process all hand trace files in a directory:
    python3 ai_analysis_scripts/aggregate_shoe_traces.py stats/shoetraces/
"""

import sys
import os
import glob
import numpy as np
import pandas as pd
from scipy.stats import skew, kurtosis


# ── helpers ────────────────────────────────────────────────────────────────────

def bucket_tc(tc_series: pd.Series, increment: float = 0.5) -> pd.Series:
    """Round TC to nearest 0.5 increment."""
    return (tc_series / increment).round() * increment


def bucket_shoe_progress(sp_series: pd.Series, increment: float = 0.05) -> pd.Series:
    """Round shoe progress to nearest 5% bucket."""
    return (sp_series / increment).round() * increment


def read_trace(path: str) -> pd.DataFrame:
    """
    Read a hand trace CSV — plain .csv or Zstandard-compressed (.zst, .csv.zst).

    Uses the zstandard library for .zst so we do not rely on pandas optional
    compression backends (which still require zstandard to be installed).
    """
    kwargs = {"low_memory": False}
    if not path.endswith(".zst"):
        return pd.read_csv(path, **kwargs)

    try:
        import zstandard as zstd
    except ImportError as e:
        raise ImportError(
            "Reading Zstandard (.zst) trace files requires the 'zstandard' package. "
            "Install with: python3 -m pip install zstandard"
        ) from e

    # Prefer zstd.open (text) when available; older releases only have decompressor API.
    open_fn = getattr(zstd, "open", None)
    if open_fn is not None:
        with open_fn(path, mode="rt", encoding="utf-8", newline="") as handle:
            return pd.read_csv(handle, **kwargs)

    import io

    with open(path, "rb") as raw:
        dctx = zstd.ZstdDecompressor()
        with dctx.stream_reader(raw) as strm:
            text = io.TextIOWrapper(strm, encoding="utf-8", newline="")
            return pd.read_csv(text, **kwargs)


def strip_trace_ext(path: str) -> str:
    """Return the basename with .csv.zst, .csv, or .zst stripped (order matters)."""
    base = os.path.basename(path)
    for ext in (".csv.zst", ".csv", ".zst"):
        if base.endswith(ext):
            return base[:-len(ext)]
    return base


def normalize_trace_columns(df: pd.DataFrame) -> pd.DataFrame:
    """
    Align trace CSV columns with what aggregate/transitions expect.

    Current C++ writer (runShoeTraceSim) uses ``tc_bucket`` for the 0.5 TC bucket;
    older files used ``true_count_bucket`` or raw ``true_count_before_hand_raw``.
    """
    if "true_count_bucket" not in df.columns:
        if "tc_bucket" in df.columns:
            df = df.rename(columns={"tc_bucket": "true_count_bucket"})
        elif "true_count_before_hand_raw" in df.columns:
            df = df.copy()
            df["true_count_bucket"] = bucket_tc(df["true_count_before_hand_raw"])
        else:
            raise KeyError(
                "Trace CSV must include tc_bucket, true_count_bucket, or "
                "true_count_before_hand_raw"
            )
    return df


# ── aggregate table ────────────────────────────────────────────────────────────

def build_aggregate(df: pd.DataFrame) -> pd.DataFrame:
    """
    Group by (true_count_bucket, shoe_progress_bucket) and compute per-bucket statistics.
    ruleset/strategy identity lives in the filename, not repeated in every row.
    """
    df = df.copy()
    df["shoe_progress_bucket"] = bucket_shoe_progress(df["shoe_progress_before_hand"])

    def agg_stats(g: pd.DataFrame) -> pd.Series:
        net = g["net_profit"].values
        committed = g["total_money_committed"].values
        n = len(net)
        ddof = 1 if n > 1 else 0
        return pd.Series({
            "hands_count":         n,
            "avg_net_profit":      float(np.mean(net)),
            "variance_per_dollar": float(np.var(net, ddof=ddof)),
            "stddev_per_dollar":   float(np.std(net, ddof=ddof)),
            "avg_total_committed": float(np.mean(committed)),
            "skewness_net_profit": float(skew(net, bias=False) if n > 2 else np.nan),
            "kurtosis_net_profit": float(kurtosis(net, bias=False) if n > 3 else np.nan),
        })

    group_keys = ["true_count_bucket", "shoe_progress_bucket"]
    agg = df.groupby(group_keys, sort=True).apply(agg_stats).reset_index()
    return agg


# ── transition table ───────────────────────────────────────────────────────────

def build_transitions(df: pd.DataFrame) -> pd.DataFrame:
    """
    For each shoe (sorted by hand_number_in_shoe), build consecutive hand pairs
    and compute transition probabilities between state buckets.

    State = (true_count_bucket, shoe_progress_bucket)
    """
    df = df.copy()
    df["shoe_progress_bucket"] = bucket_shoe_progress(df["shoe_progress_before_hand"])

    df = df.sort_values(["shoe_id", "hand_number_in_shoe"])

    grouped = df.groupby("shoe_id", sort=False)
    df["next_tc_bucket"] = grouped["true_count_bucket"].shift(-1)
    df["next_sp_bucket"] = grouped["shoe_progress_bucket"].shift(-1)

    df = df.dropna(subset=["next_tc_bucket", "next_sp_bucket"])

    trans_keys = [
        "true_count_bucket", "shoe_progress_bucket",
        "next_tc_bucket",    "next_sp_bucket",
    ]
    counts = df.groupby(trans_keys, sort=True).size().reset_index(name="transition_count")

    current_keys = ["true_count_bucket", "shoe_progress_bucket"]
    totals = counts.groupby(current_keys)["transition_count"].transform("sum")
    counts["transition_probability"] = counts["transition_count"] / totals

    return counts


# ── main ───────────────────────────────────────────────────────────────────────

def process_file(trace_path: str) -> None:
    print(f"Processing: {trace_path}")
    df = normalize_trace_columns(read_trace(trace_path))

    dirname  = os.path.dirname(trace_path)
    basename = strip_trace_ext(trace_path).replace("hand_traces_", "", 1)

    agg_path   = os.path.join(dirname, f"aggregate_{basename}.csv")
    trans_path = os.path.join(dirname, f"transitions_{basename}.csv")

    print("  Building aggregate table…")
    agg = build_aggregate(df)
    agg.to_csv(agg_path, index=False)
    print(f"  → {agg_path}  ({len(agg):,} rows)")

    print("  Building transition table…")
    trans = build_transitions(df)
    trans.to_csv(trans_path, index=False)
    print(f"  → {trans_path}  ({len(trans):,} rows)")


def main() -> None:
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(1)

    target = sys.argv[1]

    if os.path.isdir(target):
        files = sorted(
            set(
                glob.glob(os.path.join(target, "hand_traces_*.csv"))
                + glob.glob(os.path.join(target, "hand_traces_*.csv.zst"))
                + glob.glob(os.path.join(target, "hand_traces_*.zst"))
            )
        )
        if not files:
            print(f"No hand_traces_*.csv / *.csv.zst / *.zst files found in {target}")
            sys.exit(1)
        for f in files:
            process_file(f)
    elif os.path.isfile(target):
        process_file(target)
    else:
        print(f"Not a file or directory: {target}")
        sys.exit(1)


if __name__ == "__main__":
    main()
