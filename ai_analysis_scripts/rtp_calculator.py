#!/usr/bin/env python3
"""
RTP Calculator for HiLo Strategy

Calculates RTP using EV-per-TC data and HiLo betting formula.

Usage: python rtp_calculator.py <ev_per_tc_csv_file>

Example: python rtp_calculator.py ev_per_tc_HiLoStrategy_2deck_80pen_H17_DAS_NoRAS_NoSurrender_3to2.csv
"""

import csv
import sys
import math

# --- Hardcoded HiLo Strategy Constants ---
BR = 50_000
kelly = 0.75
evPerTC = 0.004854187311
evIntercept = -0.004198997334
avgVol = 1.32
TC_THRESHOLD = 0.5
MIN_BET = 25
MAX_BET = 2000

# Derived values
unitSize = (BR * kelly * evPerTC) / avgVol
interceptUnit = (BR * kelly * evIntercept) / avgVol

def get_bet(tc):
    """Calculate bet size for given true count."""

    if tc < 1.0: return 25
    
    elif tc <= 1.5:
        return 100
    elif tc < 3.0: 
        return 300
    elif tc < 4.0:
        return 500
    elif tc < 5.0:
        return 1000
    elif tc < 6.0:
        return 1600
    elif tc < 7.0:
        return 2000
    else:
        return 2000
    
    

    eff = tc - TC_THRESHOLD
    if eff <= 0:
        return MIN_BET
    raw = round((unitSize * eff + interceptUnit) / MIN_BET) * MIN_BET 
    return min(MAX_BET, max(MIN_BET, raw))

def main():
    if len(sys.argv) != 2:
        print("Usage: python rtp_calculator.py <ev_per_tc_csv_file>")
        sys.exit(1)

    csv_file = sys.argv[1]

    # Load CSV data
    rows = []
    with open(csv_file, 'r') as f:
        reader = csv.DictReader(f)
        for r in reader:
            rows.append(r)

    print("=== RTP Calculator for HiLo Strategy ===")
    print(f"Bankroll: ${BR:,}")
    print(f"Kelly Fraction: {kelly}")
    print(f"evPerTC: {evPerTC}")
    print(f"evIntercept: {evIntercept}")
    print(f"avgVolatility: {avgVol}")
    print(f"TC Threshold: {TC_THRESHOLD}")
    print(f"Min Bet: ${MIN_BET}")
    print(f"Max Bet: ${MAX_BET}")
    print(f"unitSize: ${unitSize:.2f}")
    print(f"interceptUnit: ${interceptUnit:.2f}")
    print()

    # Initialize sums
    E_bet = 0.0
    E_profit = 0.0
    total_pct = 0.0

    print("Detailed Calculation:")
    print(f"{'TC':>6} {'p(tc)%':>9} {'EV/dollar':>10} {'Bet':>6} {'m(tc)':>7} {'p*Bet*m':>10} {'p*EV*Bet*m':>12}")
    print("-" * 80)

    for r in rows:
        tc = float(r["TrueCount"])
        hands = int(r["HandsPlayed"])
        wagered = float(r["TotalMoneyWagered"])
        ev = float(r["EVPerDollar"])
        pct = float(r["HandsPlayedPct"])

        p = pct / 100.0
        m = wagered / hands if hands > 0 else 1.0
        bet = get_bet(tc)

        pb_m = p * bet * m
        peb_m = p * ev * bet * m

        E_bet += pb_m
        E_profit += peb_m
        total_pct += pct

        if -10 <= tc <= 15:  # Print key range
            print(f"{tc:>6.1f} {pct:>9.4f} {ev:>10.6f} {bet:>6d} {m:>7.4f} {pb_m:>10.4f} {peb_m:>12.6f}")

    RTP = 1.0 + E_profit / E_bet if E_bet != 0 else 0.0

    print("-" * 80)
    print(f"\nSummary:")
    print(f"Total p(tc) check: {total_pct:.4f}%")
    print(f"E[Bet] = ${E_bet:.4f}")
    print(f"E[Profit] = ${E_profit:.6f}")
    print(f"RTP = {RTP:.6f}  ({(RTP-1)*100:.4f}% player edge)")
    print(f"House Edge = {-(RTP-1)*100:.4f}%")

if __name__ == "__main__":
    main()