# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build Commands

```bash
make                    # builds: blackjack, run_tests, run_fixed_engine_tests (optimized: -O3 -march=native -flto=auto)
make blackjack          # main simulator only
make test               # builds run_tests
make test_fixed_engine  # builds run_fixed_engine_tests
make clean

# Debug build (override CXXFLAGS)
make CXXFLAGS="-std=c++17 -Wall -Wextra -g"

./run_tests
./run_fixed_engine_tests

BLACKJACK_SEED=12345 ./blackjack   # deterministic shuffle for debugging
```

Note: there is no `Makefile`, only `makefile` (lowercase). The makefile auto-discovers strategy sources via `$(wildcard src/strategy/*.cpp ...)`.

## Simulation Configuration

Simulations are configured entirely in `src/main.cpp` — there is no stable CLI. To change what runs, edit `main.cpp` directly (e.g. which strategies, H17/S17, deck count, penetration, iteration count, output paths).

Outputs are written to `stats/` at runtime. Sample/consolidated data lives in `data/`.

## Architecture

### Data Flow
`main.cpp` → `setUpSimsH17()`/`setUpSimsS17()` → `runMontesims()` → `Engine::runnerMonte()` → `FixedEngine::calculateEV()` → CSV output → Python post-processing

### Core Engine
- **`Engine`** (`include/core/Engine.h`): Full blackjack game loop — deals cards, resolves player actions, applies rules.
- **`EngineBuilder`** (`include/core/EngineBuilder.h`): Fluent builder — the only way to construct an `Engine`. All rule config flows through `GameConfig`.
- **`FixedEngine`** (`include/core/FixedEngine.h`): Monte Carlo engine — forces specific actions at configured decision points and accumulates `ActionStats` per true count.
- **`GameConfig`** (`include/core/GameConfig.h`): Struct holding all rule flags (H17/S17, DAS, surrender, penetration, Monte Carlo scenarios). Modify only via `EngineBuilder`.
- **`MonteCarloScenario`** (`include/core/MonteCarloScenario.h`): Describes a comparison (e.g. Hit vs Stand at player 16 vs dealer 10), with filters for soft hands, pairs, insurance.

### Strategy System
All counting strategies inherit from `CountingStrategy` (`include/strategy/CountingStrategy.h`).

- **Balanced**: `HiLo`, `ZenCount`, `OmegaII`, `Mentor`, `R14`, `WongHalves`, `RAPC`, `RPC` → `include/strategy/balanced/`
- **Unbalanced**: `Ko`, `Red7`, `KissIII`, `UZenII`, `UstonSS` → `include/strategy/unbalanced/`
- `NoStrategy`: baseline with no counting
- `LoggingCountingStrategy`: decorator wrapper for debug logging

Required interface: `getBetSize()`, `updateCount()`, `getTrueCount()`, `shouldAcceptInsurance()`, deviation methods.

### Adding a New Counting Strategy
1. Add header to `include/strategy/balanced/` or `unbalanced/`
2. Add source to `src/strategy/balanced/` or `unbalanced/`
3. Add to `createStrategies()` lambda in `src/main.cpp`
4. The makefile picks it up automatically via wildcard.

### Player System
`Player` interface → `BotPlayer` (automated, used in simulations) / `HumanPlayer` (interactive). Players own a `CountingStrategy` via `std::unique_ptr`.

### Event / Observer System
Singleton `EventBus` with observer pattern. `ConsoleObserver` prints game events during debugging. Events are disabled by default in simulation mode (`emitEvents = false` in `GameConfig`).

## Testing

Tests use rigged decks for determinism:
```cpp
std::vector<Card> stack = {Card(Rank::Ten, Suit::Hearts), ...};
Deck riggedDeck = Deck::createTestDeck(stack);
```
- `src/test.cpp` → core engine tests
- `src/testFixedEngine.cpp` → Monte Carlo / FixedEngine tests

Tests build with `NDEBUG` stripped (uses `TEST_CXXFLAGS`).

## Python Analysis Scripts

All post-processing lives in `ai_analysis_scripts/` (README refers to `scripts/` which is outdated).

```bash
python3 -m pip install numpy pandas scipy
python3 ai_analysis_scripts/analyze_deviations.py    # find true-count crossover points
python3 ai_analysis_scripts/generate_deviation_reports.py
python3 ai_analysis_scripts/rtp_calculator.py
python3 ai_analysis_scripts/ror_analysis.py          # risk-of-ruin analysis
```
