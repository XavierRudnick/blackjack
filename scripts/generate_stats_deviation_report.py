#!/usr/bin/env python3
"""
Generate Deviation Reports from the stats/ folder

This script processes simulation CSV files from the stats/ folder and generates
a consolidated deviation report showing the True Count where each deviation
becomes optimal for each strategy.

Outputs: deviation_report_stats.csv
"""

import os
import re
import csv
import glob
from dataclasses import dataclass
from typing import Dict, List, Optional, Tuple
from collections import defaultdict

import numpy as np


# ============================================================================
# Configuration
# ============================================================================

STATS_FOLDER = "stats"

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

def parse_filename(filepath: str) -> Tuple[str, str, str, str]:
    """
    Parse filename to extract strategy, decision type, deck size, and rules.
    
    Format: {Strategy}_{Decision}_{decksize}_{rules}.csv
    Example: MentorStrategy_Hit_vs_Stand_6_H17.csv
    
    Returns: (strategy_name, decision_type, deck_size, rules)
    """
    basename = os.path.basename(filepath)
    
    # Determine rules from filename
    if "_H17.csv" in basename:
        rules = "H17"
        clean_name = basename.replace("_H17.csv", "")
    elif "_S17.csv" in basename:
        rules = "S17"
        clean_name = basename.replace("_S17.csv", "")
    else:
        return "Unknown", "Unknown", "Unknown", "Unknown"
    
    # Extract deck size (last number before rules)
    parts = clean_name.split("_")
    deck_size = parts[-1] if parts[-1].isdigit() else "Unknown"
    
    # Remove deck size from clean_name
    clean_name = "_".join(parts[:-1])
    
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
            return strategy, dt, deck_size, rules
    
    return "Unknown", "Unknown", deck_size, rules


def load_csv_data(filepath: str) -> List[Dict]:
    """Load CSV file and return list of row dictionaries."""
    rows = []
    with open(filepath, 'r', newline='') as f:
        reader = csv.DictReader(f)
        for row in reader:
            rows.append(row)
    return rows


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
            
            if np.isfinite(tc1):
                # Weight by total hands in both points
                weight = data1.hands + data2.hands
                crossovers.append((tc1, weight, 0.0))

    if not crossovers:
        return None, None
    
    # Return the crossover closest to 0 (most relevant for practical use)
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
    Process an insurance file - consolidate ALL player/dealer combinations.
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


def process_folder(folder_path: str) -> Dict[str, List[DeviationEntry]]:
    """
    Process all CSV files in a stats folder.
    
    Returns dict keyed by deck size: {"2": entries_2deck, "6": entries_6deck}
    """
    entries_by_deck = defaultdict(list)
    
    csv_pattern = os.path.join(folder_path, "*.csv")
    csv_files = glob.glob(csv_pattern)
    
    print(f"  Found {len(csv_files)} CSV files")
    
    for filepath in csv_files:
        strategy, decision_type, deck_size, rules = parse_filename(filepath)
        
        if strategy == "Unknown" or decision_type == "Unknown":
            print(f"  Skipping unrecognized file: {os.path.basename(filepath)}")
            continue
        
        if decision_type == "InsuranceAccept_vs_Decline":
            entry = process_insurance_file(filepath, strategy, rules)
            if entry:
                entries_by_deck[deck_size].append(entry)
        else:
            file_entries = process_standard_file(filepath, strategy, decision_type, rules)
            entries_by_deck[deck_size].extend(file_entries)
    
    return entries_by_deck


def write_deviation_report(entries: List[DeviationEntry], output_path: str, game_config: str):
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
        
        for entry in entries:
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
    
    print(f"  Wrote {len(entries)} entries to {output_path}")


def main():
    """Main entry point."""
    base_dir = os.path.dirname(os.path.abspath(__file__))
    folder_path = os.path.join(base_dir, STATS_FOLDER)
    
    if not os.path.isdir(folder_path):
        print(f"Error: Folder not found: {folder_path}")
        return
    
    print(f"Processing {STATS_FOLDER}...")
    entries_by_deck = process_folder(folder_path)
    
    # Write separate reports for each deck configuration
    for deck_size, entries in entries_by_deck.items():
        if deck_size == "2":
            game_config = "2deck_65pen_flatbet"
            output_name = "deviation_report_2deck_65pen_flatbet.csv"
        elif deck_size == "6":
            game_config = "6deck_80pen_flatbet"
            output_name = "deviation_report_6deck_80pen_flatbet.csv"
        else:
            game_config = f"{deck_size}deck_flatbet"
            output_name = f"deviation_report_{deck_size}deck_flatbet.csv"
        
        output_path = os.path.join(base_dir, output_name)
        write_deviation_report(entries, output_path, game_config)
    
    print("\nDone!")


if __name__ == "__main__":
    main()
