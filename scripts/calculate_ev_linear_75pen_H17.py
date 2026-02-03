#!/usr/bin/env python3
"""
Calculate linear regression EV per TC for 75pen H17 stats files.

Outputs CSV matching the format of ev_per_tc_linear_formulas_75pen.csv:
Strategy,Decks,Slope,Slope_Pct,Intercept,Intercept_Pct,Breakeven_TC,R_Squared
"""

import os
import glob
import re
import numpy as np
import pandas as pd
from pathlib import Path


def load_and_filter_csv(filepath: str, min_hands: int = 1000, 
                        tc_min: float = -15, tc_max: float = 20) -> pd.DataFrame:
    """Load CSV and filter by minimum hands and TC range."""
    df = pd.read_csv(filepath)
    
    # Filter by minimum hands and TC range
    df = df[df['HandsPlayed'] >= min_hands]
    df = df[(df['TrueCount'] >= tc_min) & (df['TrueCount'] <= tc_max)]
    
    return df.sort_values('TrueCount').reset_index(drop=True)


def weighted_linear_fit(tc: np.ndarray, ev: np.ndarray, 
                        weights: np.ndarray) -> dict:
    """Perform weighted linear regression and return metrics."""
    # Fit linear (degree 1) with weights
    coeffs = np.polyfit(tc, ev, 1, w=np.sqrt(weights))
    slope, intercept = coeffs
    poly = np.poly1d(coeffs)
    
    # Calculate predictions
    ev_pred = poly(tc)
    
    # Weighted R-squared
    weighted_mean = np.average(ev, weights=weights)
    ss_tot = np.sum(weights * (ev - weighted_mean) ** 2)
    ss_res = np.sum(weights * (ev - ev_pred) ** 2)
    r_squared = 1 - (ss_res / ss_tot) if ss_tot > 0 else 0
    
    # Calculate breakeven TC (where EV = 0)
    breakeven_tc = -intercept / slope if slope != 0 else float('inf')
    
    return {
        'slope': slope,
        'slope_pct': slope * 100,  # Convert to percentage
        'intercept': intercept,
        'intercept_pct': intercept * 100,  # Convert to percentage
        'breakeven_tc': breakeven_tc,
        'r_squared': r_squared
    }


def parse_filename(filepath: str) -> dict:
    """Extract strategy and deck info from filename."""
    filename = os.path.basename(filepath)
    
    # Pattern: ev_per_tc_{Strategy}_{N}deck_{pen}pen_...
    pattern = r'ev_per_tc_(\w+Strategy)_(\d+)deck_(\d+)pen'
    match = re.search(pattern, filename)
    
    if match:
        strategy = match.group(1).replace('Strategy', '')
        decks = f"{match.group(2)}deck"
        pen = f"{match.group(3)}pen"
        return {'strategy': strategy, 'decks': decks, 'pen': pen}
    
    return {'strategy': 'Unknown', 'decks': 'Unknown', 'pen': 'Unknown'}


def find_75pen_h17_files(base_dir: str) -> list:
    """Find all 75pen H17 CSV files in evPerTC directories."""
    pattern = os.path.join(base_dir, '**', '*75pen*H17*.csv')
    files = glob.glob(pattern, recursive=True)
    return sorted(files)


def find_70pen_h17_4_6deck_files(base_dir: str) -> list:
    """Find 4deck and 6deck 70pen H17 CSV files in evPerTC directories."""
    pattern = os.path.join(base_dir, '**', '*70pen*H17*.csv')
    files = glob.glob(pattern, recursive=True)
    # Filter to only 4deck and 6deck
    files = [f for f in files if '4deck' in f or '6deck' in f]
    return sorted(files)


def main():
    # Configuration
    base_dir = 'stats_evpertc_accurate/evPerTC'
    output_file = 'ev_per_tc_linear_formulas_75pen_H17_accurate.csv'
    
    min_hands = 1000
    tc_range = (-15, 20)
    
    # Find all matching files - 75pen H17 + 4/6deck 70pen H17
    files = find_75pen_h17_files(base_dir)
    files_70pen = find_70pen_h17_4_6deck_files(base_dir)
    files = files + files_70pen
    
    if not files:
        print(f"No 75pen H17 files found in {base_dir}")
        return
    
    print(f"Found {len(files)} files to analyze:")
    for f in files:
        print(f"  - {f}")
    print()
    
    results = []
    
    for filepath in files:
        try:
            # Parse metadata from filename
            info = parse_filename(filepath)
            
            # Load and filter data
            df = load_and_filter_csv(filepath, min_hands, tc_range[0], tc_range[1])
            
            if len(df) < 3:
                print(f"  Skipping {filepath}: insufficient data points after filtering")
                continue
            
            # Get arrays for fitting
            tc = df['TrueCount'].values
            ev = df['EVPerDollar'].values
            weights = df['HandsPlayed'].values.astype(float)
            
            # Perform weighted linear regression
            fit_result = weighted_linear_fit(tc, ev, weights)
            
            # Collect result
            results.append({
                'Strategy': info['strategy'],
                'Decks': info['decks'],
                'Penetration': info['pen'],
                'Slope': fit_result['slope'],
                'Slope_Pct': fit_result['slope_pct'],
                'Intercept': fit_result['intercept'],
                'Intercept_Pct': fit_result['intercept_pct'],
                'Breakeven_TC': fit_result['breakeven_tc'],
                'R_Squared': fit_result['r_squared']
            })
            
            print(f"  {info['strategy']} ({info['decks']}, {info['pen']}): "
                  f"slope={fit_result['slope']:.6f}, "
                  f"intercept={fit_result['intercept']:.6f}, "
                  f"R²={fit_result['r_squared']:.4f}")
            
        except Exception as e:
            print(f"  Error processing {filepath}: {e}")
    
    # Create DataFrame and sort by Strategy, then Decks, then Penetration
    df_results = pd.DataFrame(results)
    df_results = df_results.sort_values(['Strategy', 'Decks', 'Penetration']).reset_index(drop=True)
    
    # Save to CSV
    df_results.to_csv(output_file, index=False)
    print(f"\nResults saved to: {output_file}")
    print(f"\nSummary ({len(results)} configurations analyzed):")
    print(df_results.to_string(index=False))


if __name__ == '__main__':
    main()
