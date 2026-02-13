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
MIN_HANDS = 500_000

# Derived values
unitSize = (BR * kelly * evPerTC) / avgVol
interceptUnit = (BR * kelly * evIntercept) / avgVol
csv_file = "/home/xav/blackjack/data/ev_per_tc_data/stats_evpertc_accurate/evPerTC/HiLoStrategy/ev_per_tc_HiLoStrategy_2deck_80pen_H17_DAS_NoRAS_NoSurrender_3to2.csv"
rows = []

with open(csv_file, 'r') as f:
    reader = csv.DictReader(f)
    for r in reader:
        rows.append(r)

def filter_rows_by_hands_played(rows, min_hands):
    filtered = []
    for r in rows:
        h = float(r["HandsPlayed"])
        if h >= min_hands:
            filtered.append(r)
    return filtered

def total_average_variance(rows):
    total_variance = 0.0
    for r in rows:
        hands_played = float(r["HandsPlayed"])
        std_error = float(r["StdErrorPerDollar"])
        variance_per_dollar = (std_error * math.sqrt(hands_played)) ** 2
        total_variance += variance_per_dollar

    total_variance /= len(rows)
    return total_variance

filtered = filter_rows_by_hands_played(rows, 500_000)
total_hands = sum(float(r["HandsPlayed"]) for r in filtered)
total_variance = total_average_variance(filtered)
print(total_variance)
def calculate_rtp():
    E_wager = 0.0
    E_profit = 0.0
    for r in filtered:
        true_count = float(r['TrueCount'])
        hands_played = float(r["HandsPlayed"])
        total_payout = float(r["TotalPayout"])
        total_money_wagered = float(r["TotalMoneyWagered"])
        ev_per_tc = float(r["EVPerDollar"])
        std_error = float(r["StdErrorPerDollar"])

        prob_dealt_hand_at_tc = hands_played / total_hands
        wager_multiplier = total_money_wagered / hands_played #how much does split+double+insurance impact game
        variance_per_dollar = (std_error * math.sqrt(hands_played)) ** 2


        effectiveTC = true_count - TC_THRESHOLD

        bet = round(((unitSize * effectiveTC + interceptUnit)) / MIN_BET) * MIN_BET; 
        bet = max(MIN_BET, bet)
        bet = min(MAX_BET, bet)

        E_wager += prob_dealt_hand_at_tc * bet * wager_multiplier
        E_profit += prob_dealt_hand_at_tc * ev_per_tc * bet * wager_multiplier

    print(1 + (E_profit / E_wager))
    return

def dinkelbach_integration():
    lambda_ = 0.028951817395059

if (__name__ == "__main__"): calculate_rtp()