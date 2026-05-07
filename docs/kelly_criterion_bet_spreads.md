# Kelly Criterion & Bet Spreads in This Repo

## The Core Formula

The repo uses a linear Kelly-based bet sizing formula derived from the standard Kelly Criterion adapted for blackjack:

```
f* = EV / variance          (pure Kelly fraction of bankroll)

unitSize = (Bankroll × kellyFraction × evPerTC) / avgVolatility

bet(TC) = unitSize × (TC - TC_THRESHOLD) + interceptUnit

where:
  interceptUnit = (Bankroll × kellyFraction × evIntercept) / avgVolatility
```

- `evPerTC` is the EV slope: how much player edge increases per 1 true count increment
- `evIntercept` is the baseline EV at TC = 0 (typically negative — house edge)
- `avgVolatility` is blackjack variance, hardcoded to **1.32** everywhere
- `TC_THRESHOLD` is the true count above which bets start rising (0.5 for HiLo, 1.0 for others)
- Bet is rounded to the nearest `MIN_BET` ($25), then clamped to `[MIN_BET, MAX_BET]`

---

## C++ Side

### `GameConfig` — global Kelly knob
**`include/core/GameConfig.h:12`**
```cpp
float kellyFraction = 0.5f;  // default: half Kelly
```
This is the single configuration value that propagates Kelly fraction into every strategy at engine startup.

### `EngineBuilder` — sets it
**`include/core/EngineBuilder.h`** / **`src/core/EngineBuilder.cpp:30`**
```cpp
EngineBuilder& EngineBuilder::setKellyRisk(float kellyFraction = 1.0f) {
    gameConfig.kellyFraction = kellyFraction;
    return *this;
}
```

### `Engine` — pushes it into the strategy
**`src/core/Engine.cpp:31`**
```cpp
player->setUnitSize(config.kellyFraction);
```
Called during engine construction. Every strategy's `setUnitSize()` receives the Kelly fraction and recomputes `unitSize` from the live bankroll.

### `CountingStrategy` — the interface
**`include/strategy/CountingStrategy.h`**
```cpp
virtual void setUnitSize(float kellyFraction) = 0;

static constexpr int MIN_BET = 25;
static constexpr int MAX_BET = 2000;
```

---

## Per-Strategy Kelly Parameters

All balanced strategies store three compile-time constants derived from Monte Carlo simulation output (averaged across 75% penetration across 2/4/6/8 deck games). Each has its own EV slope because they count differently and have different accuracy.

| Strategy | `evPerTC` | `evIntercept` | `TC_THRESHOLD` | Notes |
|---|---|---|---|---|
| **HiLo** | 0.004854187311 | -0.004198997334 | 0.5 | Highest slope — most responsive system |
| **ZenCount** | 0.002746310273 | -0.004014258728 | 1.0 | Level 2 balanced count |
| **MentorStrategy** | 0.002745999501 | -0.004014569844 | 1.0 | Nearly identical to Zen |
| **OmegaII** | 0.002660834324 | -0.004005554836 | 1.0 | Level 2 with Ace side-count option |
| **R14** | 0.001690236360 | -0.003897738281 | 1.0 | Lowest slope — simpler system |

All strategies share `avgVolatility = 1.32f`.

### `setUnitSize` — identical across all strategies
**`src/strategy/balanced/MentorStrategy.cpp:26`** (same pattern in ZenCount, OmegaII, R14):
```cpp
void MentorStrategy::setUnitSize(float inputKellyFraction) {
    kellyFraction = inputKellyFraction;
    unitSize = (Bankroll::getInitialBalance() * kellyFraction * evPerTC) / avgVolatility;
    if (unitSize < 1.0f) unitSize = 1.0f;
}
```

### `getBetSize` — linear Kelly bet (Mentor, ZenCount, OmegaII, R14)
**`src/strategy/balanced/MentorStrategy.cpp:14`**:
```cpp
int MentorStrategy::getBetSize() {
    float effectiveTC = true_count - PROFITABLE_PLAY_TC_THRESHOLD;
    if (effectiveTC <= 0) return MIN_BET;

    float interceptUnit = (Bankroll::getInitialBalance() * kellyFraction * evIntercept) / avgVolatility;
    int bet = std::round((unitSize * effectiveTC + interceptUnit) / (float)MIN_BET) * MIN_BET;
    bet = std::max(MIN_BET, bet);
    return std::min(getMaxBet(), bet);
}
```

### HiLo is the odd one out — bucketed spread (not linear Kelly)
**`src/strategy/balanced/HiLoStrategy.cpp:17`**

HiLo's `getBetSize()` is currently a hardcoded step ladder, **not** the Kelly formula. The linear Kelly implementation is commented out directly above it:
```cpp
// float effectiveTC = true_count - PROFITABLE_PLAY_TC_THRESHOLD;
// if (effectiveTC <= 0) return MIN_BET;
// float interceptUnit = (Bankroll::getInitialBalance() * kellyFraction * evIntercept) / avgVolatility;
// int bet = std::round((unitSize * effectiveTC + interceptUnit) / (float)MIN_BET) * MIN_BET;
// bet = std::max(MIN_BET, bet);
// return std::min(getMaxBet(), bet);

// Active: bucketed spread
if (count <= 0) return MIN_BET;   // $25
if (count == 1) return 100;
if (count == 2) return 200;
if (count == 3) return 300;
if (count == 4) return 400;
if (count == 5) return 600;
return 1000;
```
`setUnitSize` still exists on HiLo and computes `unitSize` correctly — it just isn't used by the active bet sizing path.

---

## `main.cpp` — Kelly Fraction Sweep

**`src/main.cpp:879`**
```cpp
float kellyFractions[] = {0.125f, 0.25f, 0.5f, 0.75f};
```
The outer simulation loop iterates over all four fractions for every combination of deck count, penetration, and rule set. Each produces a separate CSV output file tagged with the Kelly fraction used.

---

## Python Side

### `ai_analysis_scripts/bet_spread_calculator.py`

The standalone bet spread calculator. Implements the canonical Kelly formula for analysis/reference:

**Lines 157–173:**
```python
def calculate_kelly_bet(ev: float, kelly_fraction: float,
                        bankroll: float, variance: float = 1.32) -> float:
    if ev <= 0:
        return 0
    kelly_fraction_of_bankroll = ev / variance
    adjusted_fraction = kelly_fraction_of_bankroll * kelly_fraction
    return bankroll * adjusted_fraction
```

Three named risk profiles:

| Profile | Kelly Fraction | Max Spread | Min TC to raise |
|---|---|---|---|
| LOW | 0.25 (1/4 Kelly) | 1–8× | TC ≥ 2 |
| MED | 0.50 (1/2 Kelly) | 1–12× | TC ≥ 1 |
| HIGH | 0.75 (3/4 Kelly) | 1–20× | TC ≥ 1 |

EV models keyed by system × deck count × penetration (e.g. HiLo/6deck/75pen → slope 0.006513, base -0.004345). Fallback generic model: slope 0.005, base -0.005.

### `ai_analysis_scripts/rtp_calculator.py`

RTP calculator for HiLo. Kelly constants hardcoded at the top:

**Lines 17–28:**
```python
BR = 50_000
kelly = 0.75
evPerTC = 0.004854187311
evIntercept = -0.004198997334
avgVol = 1.32
TC_THRESHOLD = 0.5
MIN_BET = 25
MAX_BET = 2000

unitSize = (BR * kelly * evPerTC) / avgVol        # ≈ $138.10
interceptUnit = (BR * kelly * evIntercept) / avgVol  # ≈ -$119.68
```

Note: `get_bet()` in this file is also switched to a hardcoded bucketed spread matching HiLo's C++ side — the Kelly formula derivation at lines 52–56 is dead code (unreachable after the early returns):
```python
def get_bet(tc):
    if tc < 1.0: return 25
    elif tc <= 1.5: return 100
    elif tc < 3.0: return 300
    # ...
    # Lines below are unreachable:
    eff = tc - TC_THRESHOLD
    raw = round((unitSize * eff + interceptUnit) / MIN_BET) * MIN_BET
```

### `ai_analysis_scripts/calculate_optimal_rtp.py`

Same Kelly constants as `rtp_calculator.py` (BR=50k, kelly=0.75, evPerTC=0.004854187311, avgVol=1.32). Used to compute optimal theoretical RTP for HiLo.

### `ai_analysis_scripts/ror_analysis.py`

Bankroll-management companion. Given a bet spread and Kelly-derived unit size, provides:
- Infinite horizon ruin probability: `exp(-2 × μ × bankroll / σ²)`
- Finite trip ruin (anytime barrier hit)
- Session-end ruin probability

### `ai_analysis_scripts/verify_bet_spread_ruin.py`

Validates specific hardcoded bet spreads against one-session ruin risk using the RoR formulas above.

### `ai_analysis_scripts/bruteforce_bet_spread_sessions.py`

Searches over TC-ramp bet spreads to find spreads that optimize profit vs ruin tradeoff. Evaluates each candidate on: expected profit/hand, standard deviation, approximate RoR (normal, trip, infinite), and Monte Carlo validation.

---

## Summary: Kelly Fraction Usage Across the Codebase

| Location | Kelly Fraction | Status |
|---|---|---|
| `GameConfig` default | 0.5 | Active — default for all simulations |
| `main.cpp` sweep | 0.125, 0.25, 0.5, 0.75 | Active — all four tested per sim run |
| `rtp_calculator.py` | 0.75 | Active — hardcoded for HiLo RTP analysis |
| `calculate_optimal_rtp.py` | 0.75 | Active — optimal RTP analysis |
| `bet_spread_calculator.py` LOW | 0.25 | Reference tool |
| `bet_spread_calculator.py` MED | 0.50 | Reference tool |
| `bet_spread_calculator.py` HIGH | 0.75 | Reference tool |
| `HiLoStrategy` Kelly path | varies | Inactive (commented out) |
| `HiLoStrategy` bucketed path | N/A | Active — ignores Kelly |
| `MentorStrategy`, `ZenCount`, `OmegaII`, `R14` | varies | Active — full linear Kelly |

## Key Divergence: HiLo vs Other Strategies

The most notable discrepancy is that HiLo — the primary strategy used for RTP analysis — has its Kelly-based linear bet sizing commented out and replaced with a hardcoded bucketed spread. All other strategies (`MentorStrategy`, `ZenCountStrategy`, `OmegaIIStrategy`, `R14Strategy`) use the continuous Kelly formula. The Python `rtp_calculator.py` mirrors this: the Kelly formula is defined but unreachable, while the bucketed spread matches the C++ HiLo behavior.
