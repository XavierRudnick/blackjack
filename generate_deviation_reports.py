#!/usr/bin/env python3
"""
Generate Deviation Reports for Blackjack Simulations

This script processes simulation CSV files and generates a consolidated deviation report
showing the True Count where each deviation becomes optimal for each strategy.

For insurance, it consolidates all player vs dealer values to find the single
true count where taking insurance becomes +EV.

Outputs: deviation_report.csv in each stats folder
"""

import os
import re
import csv
import glob
import math
from dataclasses import dataclass, field
from typing import Dict, List, Optional, Tuple
from collections import defaultdict

import numpy as np


# ============================================================================
# Configuration
# ============================================================================

# Define which stats folders to process
STATS_FOLDERS = [
    "stats_2bil_2deck_65pen",
    "stats_600M_6deck_80pen",
    "splitStats_2bil_2deck_65pen",
    "splitStats_600m_6deck_80pen",
]

# Minimum hands to consider a data point valid
MIN_HANDS_PER_TC = 1000
MIN_HANDS_FOR_CROSSOVER = 500

# Decision type configurations
DECISION_CONFIGS = {
    "Hit_vs_Stand": {
        "ev_col_a": "Hit EV",
        "ev_col_b": "Stand EV",
        "action_a": "Hit",
        "action_b": "Stand",
    },
    "Hit_vs_Double": {
        "ev_col_a": "Double EV",
        "ev_col_b": "Hit EV",
        "action_a": "Double",
        "action_b": "Hit",
    },
    "Surrender_vs_Hit": {
        "ev_col_a": "Surrender EV",
        "ev_col_b": "Hit EV",
        "action_a": "Surrender",
        "action_b": "Hit",
    },
    "InsuranceAccept_vs_Decline": {
        "ev_col_a": "Insurance Accept EV",
        "ev_col_b": "Insurance Decline EV",
        "action_a": "Insurance",
        "action_b": "Decline",
    },
    "Split_vs_Stand_Pair10s": {
        "ev_col_a": "Split EV",
        "ev_col_b": "Stand EV",
        "action_a": "Split",
        "action_b": "Stand",
    },
}


# ============================================================================
# Data Structures
# ============================================================================

@dataclass
class DataPoint:
    """A single data point with EV values at a specific true count."""
    true_count: float
    hands: int
    ev_a: float
    ev_b: float
    
    @property
    def ev_diff(self) -> float:
        """EV difference: A - B (positive means A is better)."""
        return self.ev_a - self.ev_b


@dataclass
class AggregatedData:
    """Aggregated data for hands-weighted averaging."""
    hands: int = 0
    ev_a_sum: float = 0.0
    ev_b_sum: float = 0.0
    
    @property
    def ev_a(self) -> float:
        return self.ev_a_sum / self.hands if self.hands > 0 else 0.0
    
    @property
    def ev_b(self) -> float:
        return self.ev_b_sum / self.hands if self.hands > 0 else 0.0
    
    @property
    def ev_diff(self) -> float:
        return self.ev_a - self.ev_b
    
    def add(self, hands: int, ev_a: float, ev_b: float):
        """Add weighted data point."""
        self.hands += hands
        self.ev_a_sum += ev_a * hands
        self.ev_b_sum += ev_b * hands


@dataclass
class DeviationEntry:
    """A single deviation entry for the report."""
    strategy: str
    rules: str  # H17 or S17
    decision_type: str
    action_a: str
    action_b: str
    user_value: int
    dealer_value: int
    crossover_tc: Optional[float]
    total_hands: int
    ev_at_crossover: Optional[float] = None
    notes: str = ""


# ============================================================================
# CSV Parsing Functions
# ============================================================================

def parse_filename(filepath: str) -> Tuple[str, str, str]:
    """
    Parse filename to extract strategy, decision type, and rules.
    
    Returns: (strategy_name, decision_type, rules)
    """
    basename = os.path.basename(filepath)
    
    # Determine rules from filename
    rules = "H17" if "_H17" in basename else "S17" if "_S17" in basename else "Unknown"
    
    # Remove rules suffix for pattern matching
    clean_name = basename.replace("_H17.csv", "").replace("_S17.csv", "")
    
    # Known decision types to match
    decision_types = [
        "Hit_vs_Stand",
        "Hit_vs_Double", 
        "Surrender_vs_Hit",
        "InsuranceAccept_vs_Decline",
        "Split_vs_Stand_Pair10s",
    ]
    
    for dt in decision_types:
        if dt in clean_name:
            strategy = clean_name.replace(f"_{dt}", "")
            return strategy, dt, rules
    
    return "Unknown", "Unknown", rules


def load_csv_data(filepath: str) -> List[Dict]:
    """Load CSV file and return list of row dictionaries."""
    rows = []
    with open(filepath, 'r', newline='') as f:
        reader = csv.DictReader(f)
        for row in reader:
            rows.append(row)
    return rows


def get_ev_columns(decision_type: str) -> Tuple[str, str]:
    """Get EV column names for a decision type."""
    config = DECISION_CONFIGS.get(decision_type)
    if config:
        return config["ev_col_a"], config["ev_col_b"]
    return None, None


# ============================================================================
# Analysis Functions
# ============================================================================

def extract_data_points(
    rows: List[Dict],
    ev_col_a: str,
    ev_col_b: str,
    user_value: Optional[int] = None,
    dealer_value: Optional[int] = None
) -> Dict[float, AggregatedData]:
    """
    Extract and aggregate data points by true count.
    
    If user_value and dealer_value are None, aggregates across all values
    (used for insurance consolidation).
    """
    tc_data = defaultdict(AggregatedData)
    
    for row in rows:
        try:
            uv = int(float(row.get('UserValue', 0)))
            dv = int(float(row.get('DealerValue', 0)))
            tc = float(row.get('TrueCount', 0))
            hands = int(float(row.get('Hands Played', 0)))
            
            ev_a = float(row.get(ev_col_a, 0))
            ev_b = float(row.get(ev_col_b, 0))
            
            # Filter by user/dealer values if specified
            if user_value is not None and uv != user_value:
                continue
            if dealer_value is not None and dv != dealer_value:
                continue
            
            # Skip invalid data
            if hands < 1:
                continue
            if not (np.isfinite(ev_a) and np.isfinite(ev_b)):
                continue
            if ev_a == 0 and ev_b == 0:
                continue
                
            tc_data[tc].add(hands, ev_a, ev_b)
            
        except (ValueError, TypeError):
            continue
    
    return tc_data


def find_crossover_tc(
    tc_data: Dict[float, AggregatedData],
    min_hands: int = MIN_HANDS_FOR_CROSSOVER
) -> Tuple[Optional[float], Optional[float]]:
    """
    Find the True Count where EV difference crosses zero.
    
    Returns: (crossover_tc, ev_at_crossover)
    """
    # Filter by minimum hands and sort by TC
    valid_points = [
        (tc, data) for tc, data in tc_data.items()
        if data.hands >= min_hands and np.isfinite(data.ev_diff)
    ]
    
    if len(valid_points) < 3:
        return None, None
    
    sorted_points = sorted(valid_points, key=lambda x: x[0])
    
    crossovers = []
    
    for i in range(len(sorted_points) - 1):
        tc1, data1 = sorted_points[i]
        tc2, data2 = sorted_points[i + 1]
        
        diff1 = data1.ev_diff
        diff2 = data2.ev_diff
        
        # Check for sign change (crossover)
        if diff1 * diff2 < 0:
            # Linear interpolation to find crossover point
            denom = diff2 - diff1
            if abs(denom) < 1e-10:
                t = 0.5
            else:
                t = -diff1 / denom
            
            crossover = tc1 + t * (tc2 - tc1)
            
            if np.isfinite(crossover):
                # Weight by total hands in both points
                weight = data1.hands + data2.hands
                crossovers.append((crossover, weight, 0.0))  # EV at crossover is 0 by definition
    
    if not crossovers:
        return None, None
    
    # Return the crossover closest to 0 (most relevant for practical use)
    # If multiple, weight by sample size
    best = min(crossovers, key=lambda x: abs(x[0]))
    return round(best[0], 1), 0.0


def get_unique_hand_combinations(rows: List[Dict]) -> List[Tuple[int, int]]:
    """Get list of unique (user_value, dealer_value) combinations."""
    combinations = set()
    for row in rows:
        try:
            uv = int(float(row.get('UserValue', 0)))
            dv = int(float(row.get('DealerValue', 0)))
            combinations.add((uv, dv))
        except (ValueError, TypeError):
            continue
    return sorted(combinations)


# ============================================================================
# Main Processing Functions  
# ============================================================================

def process_standard_file(
    filepath: str,
    strategy: str,
    decision_type: str,
    rules: str
) -> List[DeviationEntry]:
    """
    Process a standard (non-insurance) simulation file.
    
    Returns list of deviation entries, one per player/dealer combination.
    """
    entries = []
    
    config = DECISION_CONFIGS.get(decision_type)
    if not config:
        return entries
    
    ev_col_a = config["ev_col_a"]
    ev_col_b = config["ev_col_b"]
    action_a = config["action_a"]
    action_b = config["action_b"]
    
    rows = load_csv_data(filepath)
    hand_combos = get_unique_hand_combinations(rows)
    
    for user_val, dealer_val in hand_combos:
        tc_data = extract_data_points(
            rows, ev_col_a, ev_col_b,
            user_value=user_val, dealer_value=dealer_val
        )
        
        if not tc_data:
            continue
        
        total_hands = sum(d.hands for d in tc_data.values())
        crossover_tc, ev_at_cross = find_crossover_tc(tc_data)
        
        entry = DeviationEntry(
            strategy=strategy,
            rules=rules,
            decision_type=decision_type,
            action_a=action_a,
            action_b=action_b,
            user_value=user_val,
            dealer_value=dealer_val,
            crossover_tc=crossover_tc,
            total_hands=total_hands,
            ev_at_crossover=ev_at_cross,
        )
        entries.append(entry)
    
    return entries


def process_insurance_file(
    filepath: str,
    strategy: str,
    rules: str
) -> Optional[DeviationEntry]:
    """
    Process an insurance file - consolidate ALL player/dealer combinations
    to find the single true count where insurance becomes +EV.
    
    Insurance is a side bet that doesn't depend on player hand, only on
    the composition of the remaining deck (count).
    """
    config = DECISION_CONFIGS["InsuranceAccept_vs_Decline"]
    ev_col_a = config["ev_col_a"]
    ev_col_b = config["ev_col_b"]
    action_a = config["action_a"]
    action_b = config["action_b"]
    
    rows = load_csv_data(filepath)
    
    # Aggregate ALL data across all player/dealer combinations
    tc_data = extract_data_points(rows, ev_col_a, ev_col_b)
    
    if not tc_data:
        return None
    
    total_hands = sum(d.hands for d in tc_data.values())
    crossover_tc, ev_at_cross = find_crossover_tc(tc_data)
    
    entry = DeviationEntry(
        strategy=strategy,
        rules=rules,
        decision_type="InsuranceAccept_vs_Decline",
        action_a=action_a,
        action_b=action_b,
        user_value=0,  # 0 indicates consolidated
        dealer_value=11,  # Insurance always vs Ace (11)
        crossover_tc=crossover_tc,
        total_hands=total_hands,
        ev_at_crossover=ev_at_cross,
        notes="Consolidated across all player hands"
    )
    
    return entry


def process_folder(folder_path: str) -> List[DeviationEntry]:
    """
    Process all CSV files in a stats folder.
    
    Returns list of all deviation entries.
    """
    entries = []
    
    # Get all CSV files
    csv_pattern = os.path.join(folder_path, "*.csv")
    csv_files = glob.glob(csv_pattern)
    
    print(f"  Found {len(csv_files)} CSV files")
    
    for filepath in csv_files:
        strategy, decision_type, rules = parse_filename(filepath)
        
        if strategy == "Unknown" or decision_type == "Unknown":
            continue
        
        if decision_type == "InsuranceAccept_vs_Decline":
            entry = process_insurance_file(filepath, strategy, rules)
            if entry:
                entries.append(entry)
        else:
            file_entries = process_standard_file(filepath, strategy, decision_type, rules)
            entries.extend(file_entries)
    
    return entries


def write_deviation_report(entries: List[DeviationEntry], output_path: str):
    """Write deviation entries to a CSV report."""
    
    # Sort entries for readability
    entries = sorted(entries, key=lambda e: (
        e.strategy,
        e.rules,
        e.decision_type,
        e.user_value,
        e.dealer_value
    ))
    
    with open(output_path, 'w', newline='') as f:
        writer = csv.writer(f)
        
        # Header
        writer.writerow([
            "Strategy",
            "Rules",
            "Decision",
            "Action A",
            "Action B", 
            "Player Value",
            "Dealer Upcard",
            "Deviation TC",
            "Total Hands",
            "Notes"
        ])
        
        for entry in entries:
            tc_str = f"{entry.crossover_tc:.1f}" if entry.crossover_tc is not None else "N/A"
            
            writer.writerow([
                entry.strategy,
                entry.rules,
                entry.decision_type,
                entry.action_a,
                entry.action_b,
                entry.user_value if entry.user_value != 0 else "ALL",
                entry.dealer_value,
                tc_str,
                entry.total_hands,
                entry.notes
            ])
    
    print(f"  Wrote {len(entries)} entries to {output_path}")


def create_summary_table(all_entries: Dict[str, List[DeviationEntry]], output_path: str):
    """
    Create a summary table showing deviation TC for each strategy side by side.
    
    Columns: Decision, Player, Dealer, HiLo_H17, HiLo_S17, Mentor_H17, ...
    """
    # Collect all unique strategies and rules combinations
    strategy_rules = set()
    for entries in all_entries.values():
        for e in entries:
            if e.strategy != "NoStrategy":  # Exclude NoStrategy (baseline)
                strategy_rules.add((e.strategy, e.rules))
    
    strategy_rules = sorted(strategy_rules)
    
    # Build lookup: (decision, player, dealer, strategy, rules) -> TC
    lookup = {}
    for folder, entries in all_entries.items():
        for e in entries:
            key = (e.decision_type, e.user_value, e.dealer_value, e.strategy, e.rules)
            lookup[key] = e.crossover_tc
    
    # Collect all unique (decision, player, dealer) combinations
    hand_configs = set()
    for entries in all_entries.values():
        for e in entries:
            hand_configs.add((e.decision_type, e.user_value, e.dealer_value))
    
    hand_configs = sorted(hand_configs)
    
    with open(output_path, 'w', newline='') as f:
        writer = csv.writer(f)
        
        # Header
        header = ["Decision", "Player", "Dealer"]
        for strat, rules in strategy_rules:
            header.append(f"{strat}_{rules}")
        writer.writerow(header)
        
        # Data rows
        for decision, player, dealer in hand_configs:
            row = [
                decision,
                player if player != 0 else "ALL",
                dealer
            ]
            for strat, rules in strategy_rules:
                key = (decision, player, dealer, strat, rules)
                tc = lookup.get(key)
                if tc is not None:
                    row.append(f"{tc:.1f}")
                else:
                    row.append("")
            writer.writerow(row)
    
    print(f"Wrote summary table to {output_path}")


# ============================================================================
# Main Entry Point
# ============================================================================

def create_consolidated_report(
    stats_entries: List[DeviationEntry],
    split_entries: List[DeviationEntry],
    output_path: str,
    game_config: str
):
    """
    Create a consolidated report merging stats and split data.
    
    Uses split stats for Split deviations (has actual Stand EV data),
    and regular stats for all other decisions.
    """
    # Index split entries by (strategy, rules, player, dealer)
    split_lookup = {}
    for e in split_entries:
        if e.decision_type == "Split_vs_Stand_Pair10s":
            key = (e.strategy, e.rules, e.user_value, e.dealer_value)
            split_lookup[key] = e
    
    # Build final entries: use stats, but replace Split with split_entries data
    final_entries = []
    for e in stats_entries:
        if e.decision_type == "Split_vs_Stand_Pair10s":
            # Try to get from split stats
            key = (e.strategy, e.rules, e.user_value, e.dealer_value)
            if key in split_lookup:
                final_entries.append(split_lookup[key])
            else:
                final_entries.append(e)  # fallback
        else:
            final_entries.append(e)
    
    # Sort entries
    final_entries = sorted(final_entries, key=lambda e: (
        e.strategy,
        e.rules,
        e.decision_type,
        e.user_value,
        e.dealer_value
    ))
    
    with open(output_path, 'w', newline='') as f:
        writer = csv.writer(f)
        
        # Header
        writer.writerow([
            "Game Config",
            "Strategy",
            "Rules",
            "Decision",
            "Action A",
            "Action B", 
            "Player Value",
            "Dealer Upcard",
            "Deviation TC",
            "Total Hands",
            "Notes"
        ])
        
        for entry in final_entries:
            tc_str = f"{entry.crossover_tc:.1f}" if entry.crossover_tc is not None else "N/A"
            
            writer.writerow([
                game_config,
                entry.strategy,
                entry.rules,
                entry.decision_type,
                entry.action_a,
                entry.action_b,
                entry.user_value if entry.user_value != 0 else "ALL",
                entry.dealer_value,
                tc_str,
                entry.total_hands,
                entry.notes
            ])
    
    print(f"  Wrote {len(final_entries)} entries to {output_path}")


def create_strategy_comparison_csv(
    stats_entries: List[DeviationEntry],
    split_entries: List[DeviationEntry],
    output_path: str,
    game_config: str
):
    """
    Create a CSV with strategies as columns for easy comparison.
    """
    # Index split entries
    split_lookup = {}
    for e in split_entries:
        if e.decision_type == "Split_vs_Stand_Pair10s":
            key = (e.strategy, e.rules, e.user_value, e.dealer_value)
            split_lookup[key] = e
    
    # Merge entries
    all_entries = []
    for e in stats_entries:
        if e.decision_type == "Split_vs_Stand_Pair10s":
            key = (e.strategy, e.rules, e.user_value, e.dealer_value)
            if key in split_lookup:
                all_entries.append(split_lookup[key])
            else:
                all_entries.append(e)
        else:
            all_entries.append(e)
    
    # Get all unique strategies (excluding NoStrategy)
    strategies = sorted(set(
        e.strategy for e in all_entries 
        if e.strategy != "NoStrategy"
    ))
    
    # Get all unique rules
    rules_list = sorted(set(e.rules for e in all_entries))
    
    # Build lookup: (decision, player, dealer, strategy, rules) -> TC
    lookup = {}
    for e in all_entries:
        key = (e.decision_type, e.user_value, e.dealer_value, e.strategy, e.rules)
        lookup[key] = e.crossover_tc
    
    # Collect all unique (decision, player, dealer) combinations
    hand_configs = sorted(set(
        (e.decision_type, e.user_value, e.dealer_value) 
        for e in all_entries
    ))
    
    with open(output_path, 'w', newline='') as f:
        writer = csv.writer(f)
        
        # Header
        header = ["Decision", "Player", "Dealer"]
        for strat in strategies:
            for rules in rules_list:
                header.append(f"{strat}_{rules}")
        writer.writerow(header)
        
        # Data rows
        for decision, player, dealer in hand_configs:
            row = [
                decision.replace("_vs_", " vs ").replace("_Pair10s", " 10s"),
                player if player != 0 else "ALL",
                dealer
            ]
            for strat in strategies:
                for rules in rules_list:
                    key = (decision, player, dealer, strat, rules)
                    tc = lookup.get(key)
                    if tc is not None:
                        row.append(f"{tc:.1f}")
                    else:
                        row.append("")
            writer.writerow(row)
    
    print(f"  Wrote strategy comparison to {output_path}")


def main():
    """Main entry point."""
    base_dir = os.path.dirname(os.path.abspath(__file__))
    
    all_entries = {}
    
    for folder_name in STATS_FOLDERS:
        folder_path = os.path.join(base_dir, folder_name)
        
        if not os.path.isdir(folder_path):
            print(f"Warning: Folder not found: {folder_path}")
            continue
        
        print(f"\nProcessing {folder_name}...")
        entries = process_folder(folder_path)
        all_entries[folder_name] = entries
        
        # Write individual report for this folder
        output_path = os.path.join(folder_path, "deviation_report.csv")
        write_deviation_report(entries, output_path)
    
    # Create consolidated reports for each game configuration
    # 2-deck 65% penetration
    if "stats_2bil_2deck_65pen" in all_entries and "splitStats_2bil_2deck_65pen" in all_entries:
        print("\nCreating consolidated 2-deck report...")
        consolidated_path = os.path.join(base_dir, "deviation_report_2deck_65pen.csv")
        create_consolidated_report(
            all_entries["stats_2bil_2deck_65pen"],
            all_entries["splitStats_2bil_2deck_65pen"],
            consolidated_path,
            "2deck_65pen"
        )
        
        comparison_path = os.path.join(base_dir, "deviation_comparison_2deck_65pen.csv")
        create_strategy_comparison_csv(
            all_entries["stats_2bil_2deck_65pen"],
            all_entries["splitStats_2bil_2deck_65pen"],
            comparison_path,
            "2deck_65pen"
        )
    
    # 6-deck 80% penetration
    if "stats_600M_6deck_80pen" in all_entries and "splitStats_600m_6deck_80pen" in all_entries:
        print("\nCreating consolidated 6-deck report...")
        consolidated_path = os.path.join(base_dir, "deviation_report_6deck_80pen.csv")
        create_consolidated_report(
            all_entries["stats_600M_6deck_80pen"],
            all_entries["splitStats_600m_6deck_80pen"],
            consolidated_path,
            "6deck_80pen"
        )
        
        comparison_path = os.path.join(base_dir, "deviation_comparison_6deck_80pen.csv")
        create_strategy_comparison_csv(
            all_entries["stats_600M_6deck_80pen"],
            all_entries["splitStats_600m_6deck_80pen"],
            comparison_path,
            "6deck_80pen"
        )
    
    print("\nDone!")


if __name__ == "__main__":
    main()
