# Blackjack Monte Carlo Simulator

A C++17 blackjack engine + Monte Carlo simulator for evaluating card-counting strategies and generating **EV-by-true-count** data (for betting + playing deviations).

**Status:** in active development. Targeting a more polished release (plus a companion website at `learn2count.io`) around **March/April 2026**.

## What it does

- Simulates blackjack shoes with configurable rules (e.g. **H17/S17**, **DAS**, re-split aces, surrender, penetration).
- Runs Monte Carlo experiments to compare forced actions at specific decision points (e.g. *Hit vs Stand*, *Insurance accept vs decline*).
- Aggregates results into CSVs you can post-process into deviation tables / true-count thresholds.
- Includes multiple counting systems (balanced + unbalanced) under `include/strategy/` (e.g. Hi-Lo, Zen Count, Omega II, Wong Halves, RAPC/RPC, KO, Red 7, KISS III).

## Build

Prereqs: a C++17 compiler (Makefile defaults to `g++`).

```bash
make
```

Targets:

```bash
make            # builds: blackjack, run_tests, run_fixed_engine_tests
make test       # builds: run_tests
make test_fixed_engine
make clean
```

## Run

The executable is currently configured from code (see `src/main.cpp`) rather than a stable CLI.

```bash
./blackjack
```

Deterministic shuffles (useful for debugging):

```bash
BLACKJACK_SEED=12345 ./blackjack
```

## Outputs

Depending on which simulation mode you enable in `src/main.cpp`, runs will write CSVs under `stats/`, for example:

- `stats/rtp_results/*.csv` (RTP / house edge summaries)
- `stats/evPerTC/<Strategy>/*.csv` (EV per true count)
- `stats/<Strategy>_<Scenario>_<Decks>_<H17|S17>.csv` (scenario-level deviation comparisons)

There are also sample/consolidated outputs in `data/` (deviation reports, lookup tables, etc.).

## Analysis scripts (Python)

Post-processing lives in `scripts/` (e.g. `scripts/analyze_deviations.py`).

Common deps:

```bash
python3 -m pip install numpy pandas scipy
python3 scripts/analyze_deviations.py
```

## Tests

```bash
./run_tests
./run_fixed_engine_tests
```

Tests use rigged decks (`Deck::createTestDeck`) for deterministic scenarios.

## Disclaimer

This project is for educational/research use. Nothing here is gambling advice.
