#!/usr/bin/env python3
"""
Blackjack Monte Carlo Deviation Analyzer

This script analyzes Monte Carlo simulation results to find optimal playing deviations.
It identifies the True Count threshold where deviating from basic strategy becomes +EV.

Features:
- Ingests all CSV files from simulation folders
- Finds optimal deviation points (crossover true counts)
- Computes 95% confidence intervals
- Weighted aggregation across hands played
- Outputs consolidated deviation tables

Example:
    python3 analyze_deviations.py
    python3 analyze_deviations.py --folders 6deck_80pen_s17_stats
    python3 analyze_deviations.py --strategy HiLoStrategy --output deviations.csv
"""

from __future__ import annotations

import argparse
import csv
import glob
import os
import re
import sys
import warnings
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List, Optional, Tuple
import math

import numpy as np
import pandas as pd
from scipy import stats

# Suppress numpy warnings for invalid operations (we handle NaN explicitly)
warnings.filterwarnings('ignore', category=RuntimeWarning)


# ============================================================================
# Configuration and Decision Type Definitions
# ============================================================================

@dataclass
class DecisionType:
    """Defines a decision comparison (e.g., Hit vs Stand)."""
    name: str
    action_a: str          # Alternative action (deviation)
    action_b: str          # Basic strategy action
    ev_col_a: str          # EV column for action A
    ev_col_b: str          # EV column for action B
    deviation_direction: str  # 'above' or 'below' - when A beats B

DECISION_TYPES = {
    "Hit_vs_Stand": DecisionType(
        name="Hit_vs_Stand",
        action_a="Hit", action_b="Stand",
        ev_col_a="Hit EV", ev_col_b="Stand EV",
        deviation_direction="variable"  # depends on context
    ),
    "Hit_vs_Double": DecisionType(
        name="Hit_vs_Double",
        action_a="Double", action_b="Hit",
        ev_col_a="Double EV", ev_col_b="Hit EV",
        deviation_direction="above"  # Double better at high counts
    ),
    "Surrender_vs_Hit": DecisionType(
        name="Surrender_vs_Hit",
        action_a="Surrender", action_b="Hit",
        ev_col_a="Surrender EV", ev_col_b="Hit EV",
        deviation_direction="below"  # Surrender when count low
    ),
    "InsuranceAccept_vs_Decline": DecisionType(
        name="InsuranceAccept_vs_Decline",
        action_a="Accept", action_b="Decline",
        ev_col_a="Insurance Accept EV", ev_col_b="Insurance Decline EV",
        deviation_direction="above"  # Insurance +EV at high counts
    ),
    "Split_vs_Stand_Pair10s": DecisionType(
        name="Split_vs_Stand_Pair10s",
        action_a="Split", action_b="Stand",
        ev_col_a="Split EV", ev_col_b="Stand EV",
        deviation_direction="above"  # Split 10s at high counts
    ),
}


# ============================================================================
# Data Structures
# ============================================================================

@dataclass
class AggregatedPoint:
    """Stores aggregated EV data at a specific true count."""
    true_count: float
    hands: int
    ev_a_sum: float  # Sum of EV_A * hands
    ev_b_sum: float  # Sum of EV_B * hands
    # For variance estimation (online algorithm)
    ev_a_sq_sum: float = 0.0  # Sum of EV_A^2 * hands
    ev_b_sq_sum: float = 0.0  # Sum of EV_B^2 * hands
    ev_diff_sq_sum: float = 0.0  # Sum of (EV_A - EV_B)^2 * hands
    
    @property
    def ev_a(self) -> float:
        return self.ev_a_sum / self.hands if self.hands > 0 else 0.0
    
    @property
    def ev_b(self) -> float:
        return self.ev_b_sum / self.hands if self.hands > 0 else 0.0
    
    @property
    def ev_diff(self) -> float:
        """EV advantage of action A over action B."""
        return self.ev_a - self.ev_b
    
    @property
    def var_diff(self) -> float:
        """Variance of EV difference."""
        if self.hands <= 1:
            return float('inf')
        # Weighted variance: Var = E[X^2] - E[X]^2
        mean_sq = self.ev_diff_sq_sum / self.hands
        sq_mean = self.ev_diff ** 2
        return max(0, mean_sq - sq_mean)
    
    @property
    def std_diff(self) -> float:
        """Standard deviation of EV difference."""
        return math.sqrt(self.var_diff)
    
    @property
    def sem_diff(self) -> float:
        """Standard error of the mean for EV difference."""
        if self.hands <= 1:
            return float('inf')
        return self.std_diff / math.sqrt(self.hands)
    
    def ci_95(self) -> Tuple[float, float]:
        """95% confidence interval for EV difference."""
        z = 1.96  # 95% CI
        margin = z * self.sem_diff
        return (self.ev_diff - margin, self.ev_diff + margin)


@dataclass
class DeviationResult:
    """Result of a deviation analysis for one hand configuration."""
    # Identification
    game_config: str       # e.g., "6deck_80pen_s17"
    strategy: str          # e.g., "HiLoStrategy"
    decision_type: str     # e.g., "Hit_vs_Stand"
    player_hand: str       # e.g., "12" or "A,7"
    dealer_upcard: int
    
    # Deviation point
    basic_strategy_action: str
    deviation_action: str
    crossover_tc: Optional[float]  # True count where deviation becomes +EV
    crossover_tc_ci_low: Optional[float]
    crossover_tc_ci_high: Optional[float]
    
    # At crossover point statistics
    ev_advantage: Optional[float]  # EV gain from deviating at crossover
    ev_advantage_ci_low: Optional[float]
    ev_advantage_ci_high: Optional[float]
    
    # Data quality
    total_hands: int
    tc_range_low: float
    tc_range_high: float
    confidence: str  # "high", "medium", "low" based on sample size
    
    # Additional statistics
    ev_at_tc_0: Optional[float]  # EV difference at TC=0
    ev_at_tc_minus5: Optional[float]  # EV difference at TC=-5
    ev_at_tc_plus5: Optional[float]   # EV difference at TC=+5
    
    # Raw data for plotting
    tc_points: List[float] = field(default_factory=list)
    ev_diffs: List[float] = field(default_factory=list)
    ev_diff_ci_lows: List[float] = field(default_factory=list)
    ev_diff_ci_highs: List[float] = field(default_factory=list)
    hands_per_tc: List[int] = field(default_factory=list)


# ============================================================================
# CSV Parsing Functions
# ============================================================================

def parse_filename(filepath: str) -> Tuple[str, str, str]:
    """
    Parse filename to extract strategy and decision type.
    
    Returns: (strategy_name, decision_type, game_config)
    """
    basename = os.path.basename(filepath)
    dirname = os.path.basename(os.path.dirname(filepath))
    
    # Extract game config from directory name
    game_config = dirname.replace("_stats", "") if dirname.endswith("_stats") else dirname
    
    # Parse filename pattern: {Strategy}_{DecisionType}.csv
    match = re.match(r"(.+?)_(Hit_vs_Stand|Hit_vs_Double|Surrender_vs_Hit|"
                     r"InsuranceAccept_vs_Decline|Split_vs_Stand_Pair10s)\.csv", basename)
    if match:
        return match.group(1), match.group(2), game_config
    
    return "Unknown", "Unknown", game_config


def load_csv_to_dataframe(filepath: str) -> pd.DataFrame:
    """Load a CSV file into a pandas DataFrame with proper types."""
    df = pd.read_csv(filepath)
    
    # Rename columns if needed for consistency
    column_mapping = {
        'User Value': 'UserValue',
        'Dealer Value': 'DealerValue',
        'True Count': 'TrueCount',
        'Hands': 'Hands Played',
    }
    df = df.rename(columns={k: v for k, v in column_mapping.items() if k in df.columns})
    
    # Ensure numeric types
    numeric_cols = ['UserValue', 'DealerValue', 'TrueCount', 'Hit EV', 'Stand EV', 
                    'Double EV', 'Split EV', 'Surrender EV', 
                    'Insurance Accept EV', 'Insurance Decline EV', 'Hands Played']
    for col in numeric_cols:
        if col in df.columns:
            df[col] = pd.to_numeric(df[col], errors='coerce')
    
    return df


# ============================================================================
# Aggregation Functions
# ============================================================================

def aggregate_by_hand_and_tc(
    df: pd.DataFrame, 
    decision_type: DecisionType,
    min_hands: int = 100
) -> Dict[Tuple[int, int], Dict[float, AggregatedPoint]]:
    """
    Aggregate data by player hand, dealer upcard, and true count.
    
    Returns: Dict[(player_value, dealer_value)] -> Dict[true_count] -> AggregatedPoint
    """
    results = {}
    
    ev_a_col = decision_type.ev_col_a
    ev_b_col = decision_type.ev_col_b
    
    # Check if required columns exist
    if ev_a_col not in df.columns or ev_b_col not in df.columns:
        return results
    
    # Group by player value and dealer value
    for (user_val, dealer_val), group in df.groupby(['UserValue', 'DealerValue']):
        hand_key = (int(user_val), int(dealer_val))
        results[hand_key] = {}
        
        for _, row in group.iterrows():
            tc = row['TrueCount']
            hands = int(row.get('Hands Played', 1))
            
            if hands < 1:
                continue
            
            ev_a = row[ev_a_col]
            ev_b = row[ev_b_col]
            ev_diff = ev_a - ev_b
            
            if tc not in results[hand_key]:
                results[hand_key][tc] = AggregatedPoint(
                    true_count=tc,
                    hands=hands,
                    ev_a_sum=ev_a * hands,
                    ev_b_sum=ev_b * hands,
                    ev_a_sq_sum=(ev_a ** 2) * hands,
                    ev_b_sq_sum=(ev_b ** 2) * hands,
                    ev_diff_sq_sum=(ev_diff ** 2) * hands
                )
            else:
                pt = results[hand_key][tc]
                pt.hands += hands
                pt.ev_a_sum += ev_a * hands
                pt.ev_b_sum += ev_b * hands
                pt.ev_a_sq_sum += (ev_a ** 2) * hands
                pt.ev_b_sq_sum += (ev_b ** 2) * hands
                pt.ev_diff_sq_sum += (ev_diff ** 2) * hands
    
    return results


# ============================================================================
# Deviation Analysis Functions
# ============================================================================

def find_crossover_point(
    tc_data: Dict[float, AggregatedPoint],
    min_hands_per_tc: int = 50
) -> Tuple[Optional[float], Optional[float], Optional[float]]:
    """
    Find the true count where EV difference crosses zero.
    Uses linear interpolation between adjacent points.
    
    Returns: (crossover_tc, ci_low, ci_high)
    """
    # Sort by true count and filter by minimum hands
    sorted_tcs = sorted(
        [(tc, pt) for tc, pt in tc_data.items() 
         if pt.hands >= min_hands_per_tc and np.isfinite(tc) and np.isfinite(pt.ev_diff)],
        key=lambda x: x[0]
    )
    
    if len(sorted_tcs) < 3:
        return None, None, None
    
    crossovers = []
    
    for i in range(len(sorted_tcs) - 1):
        tc1, pt1 = sorted_tcs[i]
        tc2, pt2 = sorted_tcs[i + 1]
        
        diff1 = pt1.ev_diff
        diff2 = pt2.ev_diff
        
        # Skip if either diff is NaN
        if not (np.isfinite(diff1) and np.isfinite(diff2)):
            continue
        
        # Check for sign change
        if diff1 * diff2 < 0:
            # Linear interpolation
            denom = diff2 - diff1
            if abs(denom) < 1e-10:
                t = 0.5
            else:
                t = -diff1 / denom
            crossover = tc1 + t * (tc2 - tc1)
            
            if not np.isfinite(crossover):
                continue
            
            # Estimate uncertainty using error propagation
            # Simplified: use average SEM of the two points
            sem1 = pt1.sem_diff if np.isfinite(pt1.sem_diff) else 0
            sem2 = pt2.sem_diff if np.isfinite(pt2.sem_diff) else 0
            avg_sem = (sem1 + sem2) / 2
            tc_step = tc2 - tc1
            
            # CI based on how uncertain the crossing point is
            if avg_sem > 0 and np.isfinite(avg_sem):
                # Approximate CI for crossover
                slope = denom / tc_step if tc_step > 0 else 1e-10
                if abs(slope) > 1e-10:
                    tc_uncertainty = avg_sem / abs(slope)
                else:
                    tc_uncertainty = tc_step
                crossovers.append((crossover, crossover - 1.96 * tc_uncertainty, 
                                  crossover + 1.96 * tc_uncertainty))
            else:
                crossovers.append((crossover, crossover - tc_step, crossover + tc_step))
    
    if not crossovers:
        return None, None, None
    
    # Return the crossover closest to 0
    valid_crossovers = [(c, l, h) for c, l, h in crossovers if np.isfinite(c)]
    if not valid_crossovers:
        return None, None, None
    
    best = min(valid_crossovers, key=lambda x: abs(x[0]))
    return best


def compute_weighted_regression(
    tc_data: Dict[float, AggregatedPoint],
    min_hands: int = 100
) -> Tuple[Optional[float], Optional[float], Optional[float]]:
    """
    Compute weighted linear regression of EV diff vs True Count.
    
    Returns: (slope, intercept, r_squared)
    """
    points = [(tc, pt) for tc, pt in tc_data.items() 
              if pt.hands >= min_hands and np.isfinite(tc) and np.isfinite(pt.ev_diff)]
    
    if len(points) < 5:
        return None, None, None
    
    tcs = np.array([p[0] for p in points], dtype=np.float64)
    diffs = np.array([p[1].ev_diff for p in points], dtype=np.float64)
    weights = np.array([p[1].hands for p in points], dtype=np.float64)
    
    # Filter out any NaN/Inf values
    mask = np.isfinite(tcs) & np.isfinite(diffs) & np.isfinite(weights) & (weights > 0)
    if np.sum(mask) < 5:
        return None, None, None
    
    tcs = tcs[mask]
    diffs = diffs[mask]
    weights = weights[mask]
    
    # Weighted least squares
    w_sum = np.sum(weights)
    if w_sum < 1e-10:
        return None, None, None
        
    w_tc = np.sum(weights * tcs) / w_sum
    w_diff = np.sum(weights * diffs) / w_sum
    
    # Weighted covariance and variance
    cov = np.sum(weights * (tcs - w_tc) * (diffs - w_diff)) / w_sum
    var_tc = np.sum(weights * (tcs - w_tc) ** 2) / w_sum
    var_diff = np.sum(weights * (diffs - w_diff) ** 2) / w_sum
    
    if var_tc < 1e-10 or not np.isfinite(var_tc):
        return None, None, None
    
    slope = cov / var_tc
    intercept = w_diff - slope * w_tc
    r_squared = (cov ** 2) / (var_tc * var_diff) if var_diff > 1e-10 else 0
    
    # Validate outputs
    if not (np.isfinite(slope) and np.isfinite(intercept)):
        return None, None, None
    
    return slope, intercept, r_squared


def analyze_deviation(
    tc_data: Dict[float, AggregatedPoint],
    game_config: str,
    strategy: str,
    decision_type: DecisionType,
    player_hand: int,
    dealer_upcard: int,
    min_hands_per_tc: int = 50
) -> DeviationResult:
    """
    Analyze a single hand configuration for optimal deviation.
    """
    # Get crossover point
    crossover, ci_low, ci_high = find_crossover_point(tc_data, min_hands_per_tc)
    
    # Compute regression for additional insights
    slope, intercept, r_sq = compute_weighted_regression(tc_data)
    
    # Gather statistics
    total_hands = sum(pt.hands for pt in tc_data.values())
    tc_values = [tc for tc in tc_data.keys() if tc_data[tc].hands >= min_hands_per_tc]
    tc_range_low = min(tc_values) if tc_values else 0
    tc_range_high = max(tc_values) if tc_values else 0
    
    # Determine confidence level
    if total_hands >= 100000:
        confidence = "high"
    elif total_hands >= 10000:
        confidence = "medium"
    else:
        confidence = "low"
    
    # EV values at specific true counts
    def get_ev_at_tc(target_tc: float, tolerance: float = 0.5) -> Optional[float]:
        candidates = [(tc, pt) for tc, pt in tc_data.items() 
                      if abs(tc - target_tc) <= tolerance and pt.hands >= min_hands_per_tc]
        if not candidates:
            return None
        best = min(candidates, key=lambda x: abs(x[0] - target_tc))
        return best[1].ev_diff
    
    ev_at_0 = get_ev_at_tc(0)
    ev_at_m5 = get_ev_at_tc(-5)
    ev_at_p5 = get_ev_at_tc(5)
    
    # Determine basic strategy action (typically action with better EV at TC=0)
    if ev_at_0 is not None:
        if ev_at_0 < 0:
            basic_action = decision_type.action_b
            deviation_action = decision_type.action_a
        else:
            basic_action = decision_type.action_a
            deviation_action = decision_type.action_b
    else:
        basic_action = decision_type.action_b
        deviation_action = decision_type.action_a
    
    # EV advantage at crossover
    ev_advantage = None
    ev_adv_ci_low = None
    ev_adv_ci_high = None
    
    if crossover is not None:
        # Find closest point to crossover
        closest_pts = [(tc, pt) for tc, pt in tc_data.items() 
                       if abs(tc - crossover) <= 1.0 and pt.hands >= min_hands_per_tc]
        if closest_pts:
            closest = min(closest_pts, key=lambda x: abs(x[0] - crossover))
            ci = closest[1].ci_95()
            ev_advantage = closest[1].ev_diff
            ev_adv_ci_low, ev_adv_ci_high = ci
    
    # Prepare plot data
    sorted_data = sorted([(tc, pt) for tc, pt in tc_data.items() if pt.hands >= min_hands_per_tc],
                         key=lambda x: x[0])
    tc_points = [x[0] for x in sorted_data]
    ev_diffs = [x[1].ev_diff for x in sorted_data]
    ci_data = [x[1].ci_95() for x in sorted_data]
    ev_diff_ci_lows = [c[0] for c in ci_data]
    ev_diff_ci_highs = [c[1] for c in ci_data]
    hands_per_tc = [x[1].hands for x in sorted_data]
    
    return DeviationResult(
        game_config=game_config,
        strategy=strategy,
        decision_type=decision_type.name,
        player_hand=str(player_hand),
        dealer_upcard=dealer_upcard,
        basic_strategy_action=basic_action,
        deviation_action=deviation_action,
        crossover_tc=crossover,
        crossover_tc_ci_low=ci_low,
        crossover_tc_ci_high=ci_high,
        ev_advantage=ev_advantage,
        ev_advantage_ci_low=ev_adv_ci_low,
        ev_advantage_ci_high=ev_adv_ci_high,
        total_hands=total_hands,
        tc_range_low=tc_range_low,
        tc_range_high=tc_range_high,
        confidence=confidence,
        ev_at_tc_0=ev_at_0,
        ev_at_tc_minus5=ev_at_m5,
        ev_at_tc_plus5=ev_at_p5,
        tc_points=tc_points,
        ev_diffs=ev_diffs,
        ev_diff_ci_lows=ev_diff_ci_lows,
        ev_diff_ci_highs=ev_diff_ci_highs,
        hands_per_tc=hands_per_tc
    )


# ============================================================================
# Main Processing Functions
# ============================================================================

def process_file(
    filepath: str,
    min_hands_per_tc: int = 50
) -> List[DeviationResult]:
    """Process a single CSV file and return deviation results."""
    strategy, decision_name, game_config = parse_filename(filepath)
    
    if decision_name not in DECISION_TYPES:
        print(f"  Warning: Unknown decision type '{decision_name}' in {filepath}")
        return []
    
    decision_type = DECISION_TYPES[decision_name]
    
    try:
        df = load_csv_to_dataframe(filepath)
    except Exception as e:
        print(f"  Error loading {filepath}: {e}")
        return []
    
    # Aggregate data
    aggregated = aggregate_by_hand_and_tc(df, decision_type, min_hands=min_hands_per_tc)
    
    results = []
    for (player_val, dealer_val), tc_data in aggregated.items():
        result = analyze_deviation(
            tc_data=tc_data,
            game_config=game_config,
            strategy=strategy,
            decision_type=decision_type,
            player_hand=player_val,
            dealer_upcard=dealer_val,
            min_hands_per_tc=min_hands_per_tc
        )
        results.append(result)
    
    return results


def process_folder(
    folder_path: str,
    strategy_filter: Optional[str] = None,
    min_hands_per_tc: int = 50
) -> List[DeviationResult]:
    """Process all CSV files in a folder."""
    results = []
    pattern = os.path.join(folder_path, "*.csv")
    
    for filepath in glob.glob(pattern):
        basename = os.path.basename(filepath)
        
        # Apply strategy filter if specified
        if strategy_filter and not basename.startswith(strategy_filter):
            continue
        
        print(f"  Processing: {basename}")
        file_results = process_file(filepath, min_hands_per_tc)
        results.extend(file_results)
    
    return results


def results_to_dataframe(results: List[DeviationResult]) -> pd.DataFrame:
    """Convert list of DeviationResult to a pandas DataFrame."""
    records = []
    
    for r in results:
        records.append({
            'Game Config': r.game_config,
            'Strategy': r.strategy,
            'Decision Type': r.decision_type,
            'Player Hand': r.player_hand,
            'Dealer Upcard': r.dealer_upcard,
            'Basic Strategy': r.basic_strategy_action,
            'Deviation Action': r.deviation_action,
            'Crossover TC': r.crossover_tc,
            'Crossover TC CI Low': r.crossover_tc_ci_low,
            'Crossover TC CI High': r.crossover_tc_ci_high,
            'EV Advantage at Crossover': r.ev_advantage,
            'EV Advantage CI Low': r.ev_advantage_ci_low,
            'EV Advantage CI High': r.ev_advantage_ci_high,
            'Total Hands': r.total_hands,
            'TC Range Low': r.tc_range_low,
            'TC Range High': r.tc_range_high,
            'Confidence': r.confidence,
            'EV Diff @ TC=0': r.ev_at_tc_0,
            'EV Diff @ TC=-5': r.ev_at_tc_minus5,
            'EV Diff @ TC=+5': r.ev_at_tc_plus5,
        })
    
    df = pd.DataFrame(records)
    
    # Sort by decision type, player hand, dealer upcard
    if not df.empty:
        df = df.sort_values(['Game Config', 'Strategy', 'Decision Type', 
                            'Player Hand', 'Dealer Upcard'])
    
    return df


def create_deviation_table(df: pd.DataFrame) -> pd.DataFrame:
    """
    Create a filtered table showing only actionable deviations.
    
    Criteria:
    - Has a valid crossover point
    - Crossover is within a practical range (e.g., -10 to +10)
    - Has reasonable confidence
    """
    mask = (
        df['Crossover TC'].notna() &
        (df['Crossover TC'].abs() <= 15) &
        (df['Total Hands'] >= 1000)
    )
    
    filtered = df[mask].copy()
    
    # Add deviation description
    filtered['Deviation'] = (
        filtered['Player Hand'].astype(str) + ' vs ' + 
        filtered['Dealer Upcard'].astype(str) + ': ' +
        filtered['Deviation Action'] + ' @ TC>=' +
        filtered['Crossover TC'].round(1).astype(str)
    )
    
    return filtered


def create_summary_stats(results: List[DeviationResult]) -> pd.DataFrame:
    """Create summary statistics for the analysis."""
    summary_data = []
    
    # Group by game config and strategy
    from collections import defaultdict
    groups = defaultdict(list)
    for r in results:
        groups[(r.game_config, r.strategy)].append(r)
    
    for (game_config, strategy), group_results in groups.items():
        total_hands = sum(r.total_hands for r in group_results)
        valid_crossovers = [r for r in group_results if r.crossover_tc is not None]
        high_confidence = [r for r in group_results if r.confidence == "high"]
        
        summary_data.append({
            'Game Config': game_config,
            'Strategy': strategy,
            'Total Hands Analyzed': total_hands,
            'Hand Configurations': len(group_results),
            'Valid Crossover Points': len(valid_crossovers),
            'High Confidence Results': len(high_confidence),
            'Coverage (%)': len(valid_crossovers) / len(group_results) * 100 if group_results else 0
        })
    
    return pd.DataFrame(summary_data)


def generate_tc_profile(results: List[DeviationResult], output_dir: str) -> None:
    """Generate detailed TC profile CSVs for each hand configuration."""
    profile_dir = os.path.join(output_dir, "tc_profiles")
    os.makedirs(profile_dir, exist_ok=True)
    
    for r in results:
        if not r.tc_points:
            continue
        
        profile_df = pd.DataFrame({
            'True Count': r.tc_points,
            'EV Difference': r.ev_diffs,
            'EV Diff CI Low': r.ev_diff_ci_lows,
            'EV Diff CI High': r.ev_diff_ci_highs,
            'Hands': r.hands_per_tc
        })
        
        filename = f"{r.game_config}_{r.strategy}_{r.decision_type}_{r.player_hand}v{r.dealer_upcard}.csv"
        profile_df.to_csv(os.path.join(profile_dir, filename), index=False)


# ============================================================================
# Insurance-Specific Analysis
# ============================================================================

def analyze_insurance(
    df: pd.DataFrame,
    game_config: str,
    strategy: str
) -> Dict[float, Tuple[float, float, int]]:
    """
    Analyze insurance data, aggregating across all player hands.
    
    Returns: Dict[true_count] -> (ev_diff, ci_margin, total_hands)
    """
    # For insurance, we aggregate across all player hands since the decision
    # only depends on dealer upcard (Ace) and true count
    
    aggregated = {}
    
    for _, row in df.iterrows():
        tc = row['TrueCount']
        hands = int(row.get('Hands Played', 1))
        
        if hands < 1:
            continue
        
        accept_ev = row.get('Insurance Accept EV', 0)
        decline_ev = row.get('Insurance Decline EV', 0)
        ev_diff = accept_ev - decline_ev
        
        if tc not in aggregated:
            aggregated[tc] = AggregatedPoint(
                true_count=tc,
                hands=hands,
                ev_a_sum=accept_ev * hands,
                ev_b_sum=decline_ev * hands,
                ev_diff_sq_sum=(ev_diff ** 2) * hands
            )
        else:
            pt = aggregated[tc]
            pt.hands += hands
            pt.ev_a_sum += accept_ev * hands
            pt.ev_b_sum += decline_ev * hands
            pt.ev_diff_sq_sum += (ev_diff ** 2) * hands
    
    result = {}
    for tc, pt in aggregated.items():
        ci_low, ci_high = pt.ci_95()
        ci_margin = (ci_high - ci_low) / 2
        result[tc] = (pt.ev_diff, ci_margin, pt.hands)
    
    return result


# ============================================================================
# Main Entry Point
# ============================================================================

def main():
    parser = argparse.ArgumentParser(
        description="Analyze blackjack Monte Carlo simulation results for optimal deviations."
    )
    parser.add_argument(
        "--folders", "-f",
        nargs="+",
        default=None,
        help="Specific folders to analyze (default: all *_stats folders)"
    )
    parser.add_argument(
        "--strategy", "-s",
        default=None,
        help="Filter by strategy name (e.g., HiLoStrategy)"
    )
    parser.add_argument(
        "--output", "-o",
        default="deviations_analysis.csv",
        help="Output CSV filename (default: deviations_analysis.csv)"
    )
    parser.add_argument(
        "--min-hands", "-m",
        type=int,
        default=100,
        help="Minimum hands per true count for analysis (default: 100)"
    )
    parser.add_argument(
        "--profiles",
        action="store_true",
        help="Generate detailed TC profile CSVs"
    )
    parser.add_argument(
        "--base-dir",
        default=".",
        help="Base directory to search for stats folders (default: current directory)"
    )
    
    args = parser.parse_args()
    
    base_dir = args.base_dir
    
    # Find stats folders
    if args.folders:
        folders = [os.path.join(base_dir, f) for f in args.folders]
    else:
        # Auto-discover folders ending with _stats
        folders = glob.glob(os.path.join(base_dir, "*_stats"))
        folders = [f for f in folders if os.path.isdir(f)]
    
    if not folders:
        print("No stats folders found. Use --folders to specify or run from project directory.")
        sys.exit(1)
    
    print(f"Analyzing {len(folders)} folder(s)...")
    print(f"Min hands per TC: {args.min_hands}")
    print()
    
    all_results = []
    
    for folder in sorted(folders):
        print(f"Processing folder: {os.path.basename(folder)}")
        results = process_folder(
            folder, 
            strategy_filter=args.strategy,
            min_hands_per_tc=args.min_hands
        )
        all_results.extend(results)
        print(f"  Found {len(results)} hand configurations")
        print()
    
    if not all_results:
        print("No results found.")
        sys.exit(1)
    
    # Convert to DataFrames
    full_df = results_to_dataframe(all_results)
    actionable_df = create_deviation_table(full_df)
    summary_df = create_summary_stats(all_results)
    
    # Output results
    output_base = os.path.splitext(args.output)[0]
    
    # Full results
    full_path = f"{output_base}_full.csv"
    full_df.to_csv(full_path, index=False, float_format='%.6f')
    print(f"Full results saved to: {full_path}")
    
    # Actionable deviations only
    actionable_path = f"{output_base}_actionable.csv"
    actionable_df.to_csv(actionable_path, index=False, float_format='%.6f')
    print(f"Actionable deviations saved to: {actionable_path}")
    
    # Summary statistics
    summary_path = f"{output_base}_summary.csv"
    summary_df.to_csv(summary_path, index=False, float_format='%.2f')
    print(f"Summary saved to: {summary_path}")
    
    # Generate TC profiles if requested
    if args.profiles:
        profile_dir = os.path.dirname(args.output) or "."
        generate_tc_profile(all_results, profile_dir)
        print(f"TC profiles saved to: {profile_dir}/tc_profiles/")
    
    # Print summary
    print()
    print("=" * 60)
    print("ANALYSIS SUMMARY")
    print("=" * 60)
    print(f"Total hand configurations analyzed: {len(all_results)}")
    print(f"Configurations with valid crossover: {len(actionable_df)}")
    print()
    
    # Show top actionable deviations
    if not actionable_df.empty:
        print("TOP ACTIONABLE DEVIATIONS (sorted by absolute crossover TC):")
        print("-" * 60)
        
        top_deviations = actionable_df.nsmallest(20, 'Crossover TC', keep='first')
        top_deviations = top_deviations.sort_values('Crossover TC')
        
        for _, row in top_deviations.head(15).iterrows():
            tc = row['Crossover TC']
            ci_low = row.get('Crossover TC CI Low', tc)
            ci_high = row.get('Crossover TC CI High', tc)
            ci_str = f"[{ci_low:.1f}, {ci_high:.1f}]" if pd.notna(ci_low) else ""
            
            print(f"  {row['Strategy']}: {row['Player Hand']} vs {row['Dealer Upcard']}")
            print(f"    {row['Basic Strategy']} -> {row['Deviation Action']} @ TC >= {tc:.1f} {ci_str}")
            print(f"    ({row['Decision Type']}, {row['Total Hands']:,} hands, {row['Confidence']} confidence)")
            print()
    
    print("=" * 60)
    print("Analysis complete!")


if __name__ == "__main__":
    main()
