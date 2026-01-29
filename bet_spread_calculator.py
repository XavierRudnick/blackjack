#!/usr/bin/env python3
"""
Blackjack Bet Spread Calculator

Calculates optimal bet spreads based on True Count using:
- EV per True Count derived from Monte Carlo simulations
- Kelly Criterion with adjustable risk levels
- Bankroll and session management considerations

Three risk tiers:
- LOW:  1/4 Kelly - Conservative, minimizes risk of ruin, good for limited bankrolls
- MED:  1/2 Kelly - Balanced approach, standard for most counters
- HIGH: 3/4 Kelly - Aggressive, higher variance, maximum long-term growth
"""

import argparse
import math
from dataclasses import dataclass
from typing import Dict, List, Tuple, Optional
import os

# Try to import optional dependencies
try:
    import pandas as pd
    HAS_PANDAS = True
except ImportError:
    HAS_PANDAS = False

try:
    import numpy as np
    HAS_NUMPY = True
except ImportError:
    HAS_NUMPY = False


# =============================================================================
# EV PER TRUE COUNT MODELS (derived from Monte Carlo simulations)
# =============================================================================

# Linear approximation: EV = slope * TC + base_edge
# These are derived from weighted linear regression on simulation data
EV_MODELS = {
    # HiLo Strategy - most common counting system
    'HiLo': {
        '2deck': {
            '65pen': {'slope': 0.006699, 'base': -0.000346},  # 0.67% per TC
            '70pen': {'slope': 0.006707, 'base': 0.000010},
            '75pen': {'slope': 0.006698, 'base': 0.000408},
            '80pen': {'slope': 0.006636, 'base': 0.000943},
        },
        '6deck': {
            '65pen': {'slope': 0.006328, 'base': -0.004812},  # 0.63% per TC
            '70pen': {'slope': 0.006420, 'base': -0.004593},
            '75pen': {'slope': 0.006513, 'base': -0.004345},
            '80pen': {'slope': 0.006552, 'base': -0.004110},
        },
        '8deck': {
            '65pen': {'slope': 0.006209, 'base': -0.005389},  # 0.62% per TC
            '70pen': {'slope': 0.006298, 'base': -0.005250},
            '75pen': {'slope': 0.006398, 'base': -0.005044},
            '80pen': {'slope': 0.006484, 'base': -0.004823},
        },
    },
    # ZenCount - Level 2 count, slightly more accurate
    'ZenCount': {
        '2deck': {
            '75pen': {'slope': 0.003856, 'base': 0.001053},
        },
        '6deck': {
            '75pen': {'slope': 0.003742, 'base': -0.004057},
        },
        '8deck': {
            '75pen': {'slope': 0.003703, 'base': -0.004762},
        },
    },
    # Generic fallback (conservative estimate)
    'Generic': {
        'default': {'slope': 0.005, 'base': -0.005},  # ~0.5% per TC, 0.5% house edge
    }
}

# Blackjack variance per hand (standard deviation squared)
# SD ≈ 1.1-1.15, so variance ≈ 1.21-1.32
BLACKJACK_VARIANCE = 1.32  # Conservative estimate


@dataclass
class BetSpreadConfig:
    """Configuration for bet spread calculation"""
    bankroll: float          # Total bankroll in dollars
    min_bet: float           # Table minimum bet
    max_bet: float           # Table maximum bet (or personal max)
    counting_system: str     # e.g., 'HiLo', 'ZenCount'
    deck_count: str          # e.g., '2deck', '6deck', '8deck'
    penetration: str         # e.g., '75pen', '80pen'
    session_bankroll: Optional[float] = None  # Amount brought to session


@dataclass 
class RiskProfile:
    """Risk profile parameters"""
    name: str
    kelly_fraction: float
    max_bet_units: int       # Cap on maximum bet in units
    min_tc_to_raise: float   # Don't raise bets until this TC
    description: str


# Risk profiles
RISK_PROFILES = {
    'low': RiskProfile(
        name='Low Risk (Conservative)',
        kelly_fraction=0.25,      # 1/4 Kelly
        max_bet_units=8,          # Max 8x min bet
        min_tc_to_raise=2.0,      # Only raise at TC >= 2
        description='Minimizes risk of ruin. Best for limited bankrolls or new counters.'
    ),
    'med': RiskProfile(
        name='Medium Risk (Balanced)',
        kelly_fraction=0.50,      # 1/2 Kelly
        max_bet_units=12,         # Max 12x min bet
        min_tc_to_raise=1.0,      # Raise at TC >= 1
        description='Standard approach. Good balance of growth and protection.'
    ),
    'high': RiskProfile(
        name='High Risk (Aggressive)',
        kelly_fraction=0.75,      # 3/4 Kelly
        max_bet_units=20,         # Max 20x min bet  
        min_tc_to_raise=1.0,      # Raise at TC >= 1
        description='Maximum growth rate. Requires large bankroll and high variance tolerance.'
    ),
}


def get_ev_at_tc(tc: float, config: BetSpreadConfig) -> float:
    """
    Calculate expected value at a given true count.
    
    Returns EV as a decimal (e.g., 0.01 = 1% edge)
    """
    system = config.counting_system
    deck = config.deck_count
    pen = config.penetration
    
    # Try to find exact match
    if system in EV_MODELS:
        if deck in EV_MODELS[system]:
            if pen in EV_MODELS[system][deck]:
                model = EV_MODELS[system][deck][pen]
                return model['slope'] * tc + model['base']
    
    # Fallback to generic model
    model = EV_MODELS['Generic']['default']
    return model['slope'] * tc + model['base']


def calculate_kelly_bet(ev: float, kelly_fraction: float, 
                        bankroll: float, variance: float = BLACKJACK_VARIANCE) -> float:
    """
    Calculate optimal bet using Kelly Criterion.
    
    Kelly formula: f* = edge / variance
    Where f* is the fraction of bankroll to bet.
    
    We use fractional Kelly for risk management.
    """
    if ev <= 0:
        return 0
    
    kelly_fraction_of_bankroll = ev / variance
    adjusted_fraction = kelly_fraction_of_bankroll * kelly_fraction
    
    return bankroll * adjusted_fraction


def calculate_bet_spread(config: BetSpreadConfig, 
                         risk_profile: RiskProfile,
                         tc_range: Tuple[int, int] = (-5, 12)) -> Dict[float, float]:
    """
    Calculate bet spread for all true counts.
    
    Returns dict mapping TC -> bet amount
    """
    bets = {}
    
    for tc in range(tc_range[0], tc_range[1] + 1):
        # Get EV at this true count
        ev = get_ev_at_tc(tc, config)
        
        # Calculate Kelly bet
        kelly_bet = calculate_kelly_bet(ev, risk_profile.kelly_fraction, config.bankroll)
        
        # Apply constraints
        if tc < risk_profile.min_tc_to_raise or ev <= 0:
            # Bet minimum when count is low/negative
            bet = config.min_bet
        else:
            # Round to nearest unit
            bet = max(config.min_bet, kelly_bet)
            
            # Cap at max units
            max_allowed = config.min_bet * risk_profile.max_bet_units
            bet = min(bet, max_allowed)
            
            # Cap at table max
            bet = min(bet, config.max_bet)
        
        # Round to practical bet amount
        bet = round_bet(bet, config.min_bet)
        
        bets[tc] = bet
    
    return bets


def round_bet(bet: float, min_bet: float) -> float:
    """Round bet to practical chip amounts"""
    # Round to nearest unit (min_bet)
    units = round(bet / min_bet)
    return max(1, units) * min_bet


def calculate_bet_units(bets: Dict[float, float], min_bet: float) -> Dict[float, int]:
    """Convert bet amounts to units (multiples of min bet)"""
    return {tc: int(round(bet / min_bet)) for tc, bet in bets.items()}


def estimate_hourly_ev(config: BetSpreadConfig, bets: Dict[float, float],
                       hands_per_hour: int = 80) -> Tuple[float, float]:
    """
    Estimate hourly EV and standard deviation.
    
    Uses approximate TC distribution for the given game.
    Returns (hourly_ev, hourly_std)
    """
    # Approximate TC distribution (rough percentages at each TC)
    # This varies by penetration but this is a reasonable approximation
    tc_distribution = {
        -5: 0.01, -4: 0.02, -3: 0.04, -2: 0.08, -1: 0.12,
        0: 0.30,
        1: 0.15, 2: 0.10, 3: 0.07, 4: 0.05, 5: 0.03,
        6: 0.015, 7: 0.01, 8: 0.005, 9: 0.003, 10: 0.002,
        11: 0.001, 12: 0.001
    }
    
    total_ev = 0
    total_variance = 0
    
    for tc, freq in tc_distribution.items():
        if tc not in bets:
            continue
        
        bet = bets[tc]
        ev = get_ev_at_tc(tc, config)
        
        # EV contribution
        hand_ev = bet * ev * freq * hands_per_hour
        total_ev += hand_ev
        
        # Variance contribution (bet^2 * variance * frequency)
        hand_variance = (bet ** 2) * BLACKJACK_VARIANCE * freq * hands_per_hour
        total_variance += hand_variance
    
    hourly_std = math.sqrt(total_variance)
    
    return total_ev, hourly_std


def print_bet_spread_table(config: BetSpreadConfig, 
                           all_bets: Dict[str, Dict[float, float]]):
    """Print formatted bet spread table for all risk levels"""
    
    print("\n" + "=" * 80)
    print("BLACKJACK BET SPREAD CALCULATOR")
    print("=" * 80)
    
    print(f"\nGame Configuration:")
    print(f"  Counting System: {config.counting_system}")
    print(f"  Deck Count:      {config.deck_count}")
    print(f"  Penetration:     {config.penetration}")
    print(f"  Bankroll:        ${config.bankroll:,.0f}")
    print(f"  Min Bet:         ${config.min_bet:.0f}")
    print(f"  Max Bet:         ${config.max_bet:.0f}")
    
    # Get EV model info
    ev_at_0 = get_ev_at_tc(0, config)
    ev_at_1 = get_ev_at_tc(1, config)
    slope = ev_at_1 - ev_at_0
    
    print(f"\nEV Model:")
    print(f"  Base House Edge (TC=0): {ev_at_0*100:.3f}%")
    print(f"  EV per True Count:      {slope*100:.3f}%")
    print(f"  Breakeven TC:           ~{-ev_at_0/slope:.1f}")
    
    # Print table header
    print("\n" + "-" * 80)
    print(f"{'TC':>4} | {'EV%':>7} | {'LOW RISK':>12} | {'MED RISK':>12} | {'HIGH RISK':>12}")
    print(f"{'':>4} | {'':>7} | {'Bet':>6} {'Units':>5} | {'Bet':>6} {'Units':>5} | {'Bet':>6} {'Units':>5}")
    print("-" * 80)
    
    for tc in range(-5, 13):
        ev = get_ev_at_tc(tc, config)
        ev_pct = f"{ev*100:+.2f}%"
        
        row = f"{tc:>4} | {ev_pct:>7} |"
        
        for risk_level in ['low', 'med', 'high']:
            bet = all_bets[risk_level][tc]
            units = int(round(bet / config.min_bet))
            row += f" ${bet:>5.0f} {units:>4}x |"
        
        # Highlight positive EV rows
        if ev > 0:
            print(f"\033[92m{row}\033[0m")  # Green for positive EV
        else:
            print(row)
    
    print("-" * 80)
    
    # Print risk profile descriptions and hourly stats
    print("\n" + "=" * 80)
    print("RISK PROFILE ANALYSIS")
    print("=" * 80)
    
    for risk_level in ['low', 'med', 'high']:
        profile = RISK_PROFILES[risk_level]
        bets = all_bets[risk_level]
        
        hourly_ev, hourly_std = estimate_hourly_ev(config, bets)
        
        # Risk of ruin approximation (simplified)
        # RoR ≈ ((1-edge)/(1+edge))^(bankroll/unit)
        avg_bet = sum(bets.values()) / len(bets)
        
        print(f"\n{profile.name}:")
        print(f"  {profile.description}")
        print(f"  Kelly Fraction:    {profile.kelly_fraction:.0%}")
        print(f"  Max Spread:        1-{profile.max_bet_units}")
        print(f"  Est. Hourly EV:    ${hourly_ev:+.2f}")
        print(f"  Est. Hourly Std:   ${hourly_std:.2f}")
        print(f"  1-Std Range:       ${hourly_ev-hourly_std:+.2f} to ${hourly_ev+hourly_std:+.2f}")


def generate_simple_formula(config: BetSpreadConfig) -> str:
    """Generate a simple formula the user can memorize"""
    
    ev_at_0 = get_ev_at_tc(0, config)
    slope = get_ev_at_tc(1, config) - ev_at_0
    breakeven = -ev_at_0 / slope
    
    formula = f"""
================================================================================
SIMPLE BET SPREAD FORMULA
================================================================================

For {config.counting_system} | {config.deck_count} | {config.penetration}

📊 QUICK REFERENCE:
   • Breakeven TC: {breakeven:.1f} (EV becomes positive)
   • EV per TC: ~{slope*100:.2f}%

💰 BET SIZING FORMULA (in units):

   LOW RISK:    Units = max(1, TC - 1)           cap at 8
   MEDIUM RISK: Units = max(1, (TC - 1) × 1.5)   cap at 12  
   HIGH RISK:   Units = max(1, (TC - 1) × 2)     cap at 20

📋 MEMORIZABLE SPREAD:

   TC ≤ 1:  Bet 1 unit (minimum)
   TC = 2:  Bet 1-2 units
   TC = 3:  Bet 2-4 units
   TC = 4:  Bet 3-6 units
   TC = 5:  Bet 4-8 units
   TC = 6+: Bet 6-12+ units (based on risk tolerance)

⚠️  BANKROLL REQUIREMENTS (to avoid ruin):
   LOW RISK:    ~200 units recommended
   MEDIUM RISK: ~400 units recommended  
   HIGH RISK:   ~800 units recommended

   Your bankroll: ${config.bankroll:,.0f}
   Your min bet:  ${config.min_bet:.0f}
   Your units:    {config.bankroll / config.min_bet:.0f} units
"""
    return formula


def export_to_csv(config: BetSpreadConfig, 
                  all_bets: Dict[str, Dict[float, float]],
                  filename: str = 'my_bet_spread.csv'):
    """Export bet spread to CSV file"""
    
    rows = []
    for tc in range(-5, 13):
        ev = get_ev_at_tc(tc, config)
        row = {
            'TrueCount': tc,
            'EV_Percent': ev * 100,
            'Low_Bet': all_bets['low'][tc],
            'Low_Units': int(round(all_bets['low'][tc] / config.min_bet)),
            'Med_Bet': all_bets['med'][tc],
            'Med_Units': int(round(all_bets['med'][tc] / config.min_bet)),
            'High_Bet': all_bets['high'][tc],
            'High_Units': int(round(all_bets['high'][tc] / config.min_bet)),
        }
        rows.append(row)
    
    if HAS_PANDAS:
        df = pd.DataFrame(rows)
        df.to_csv(filename, index=False)
    else:
        # Manual CSV writing
        with open(filename, 'w') as f:
            headers = rows[0].keys()
            f.write(','.join(headers) + '\n')
            for row in rows:
                f.write(','.join(str(v) for v in row.values()) + '\n')
    
    print(f"\nBet spread exported to: {filename}")


def main():
    parser = argparse.ArgumentParser(
        description='Calculate optimal blackjack bet spreads based on true count'
    )
    parser.add_argument('--bankroll', '-b', type=float, default=10000,
                        help='Total bankroll in dollars (default: 10000)')
    parser.add_argument('--min-bet', '-m', type=float, default=25,
                        help='Table minimum bet (default: 25)')
    parser.add_argument('--max-bet', '-M', type=float, default=500,
                        help='Table/personal maximum bet (default: 500)')
    parser.add_argument('--system', '-s', type=str, default='HiLo',
                        choices=['HiLo', 'ZenCount', 'Generic'],
                        help='Counting system (default: HiLo)')
    parser.add_argument('--decks', '-d', type=str, default='6deck',
                        choices=['2deck', '6deck', '8deck'],
                        help='Number of decks (default: 6deck)')
    parser.add_argument('--penetration', '-p', type=str, default='75pen',
                        choices=['65pen', '70pen', '75pen', '80pen'],
                        help='Deck penetration (default: 75pen)')
    parser.add_argument('--export', '-e', type=str,
                        help='Export to CSV file')
    parser.add_argument('--formula-only', '-f', action='store_true',
                        help='Only show simple formula')
    
    args = parser.parse_args()
    
    # Build configuration
    config = BetSpreadConfig(
        bankroll=args.bankroll,
        min_bet=args.min_bet,
        max_bet=args.max_bet,
        counting_system=args.system,
        deck_count=args.decks,
        penetration=args.penetration
    )
    
    # Calculate bet spreads for all risk levels
    all_bets = {}
    for risk_level, profile in RISK_PROFILES.items():
        all_bets[risk_level] = calculate_bet_spread(config, profile)
    
    if args.formula_only:
        print(generate_simple_formula(config))
    else:
        print_bet_spread_table(config, all_bets)
        print(generate_simple_formula(config))
    
    if args.export:
        export_to_csv(config, all_bets, args.export)


if __name__ == '__main__':
    main()
