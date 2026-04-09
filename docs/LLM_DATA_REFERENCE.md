# Blackjack simulator: data artifacts for LLM / downstream analysis

This document describes **what numeric and categorical data this repository produces**, **where it is written**, and **what kinds of mathematics or inference are naturally supported**. It is intended for LLM context ingestion and for humans wiring analytics pipelines.

---

## How data is generated (high level)

1. **Full-shoe play** (`Engine`): normal blackjack with a counting strategy; optional accumulation of **EV per true count** over many shoes.
2. **Forced-action Monte Carlo** (`FixedEngine`): at configured decision cells, the engine replays the same deal multiple times, forcing each candidate action in turn, and records **EV and variance** per (hand shape, dealer upcard, true count bin).
3. **RTP batch runs** (`runAllRTPSimulations` in `src/main.cpp`): parallel full-game simulations; each strategy contributes one **aggregate RTP row** plus one **`ev_per_tc_*.csv`** file.

Configuration (decks, penetration, H17/S17, DAS, RAS, surrender, 3:2 vs 6:5, Kelly fraction) is embedded in **directory layout and filenames**, not always as explicit CSV columns.

---

## Core in-memory statistics (`ActionStats`)

Defined in `include/core/ActionStats.h`. Used both in live `Engine` accounting and in `FixedEngine` cells.

| Field / method | Meaning |
|----------------|---------|
| `handsPlayed` | Count of included outcomes |
| `splitsPlayed` | Split events (used in some EV paths) |
| `totalPayout` | Sum of net payouts (dollars) |
| `totalMoneyWagered` | Sum of wagers used as weights |
| `getEV()` | Weighted mean of `net / wagered` → **EV per dollar wagered** |
| `getVariance()` | Weighted population variance of that per-dollar return |
| `getStdDev()` | `sqrt(variance)` |
| `getStdError()` | `stdDev / sqrt(totalMoneyWagered)` (standard error of the mean EV) |

**Interpretation:** For CSV exports from `FixedEngine`, many forced outcomes use **unit wager** (`addResult(net)`), so EV is effectively **mean units won/lost per trial** for that forced line of play; variance matches that scale. For `Engine`’s live `EVperTC` tracking, wagers reflect actual bet sizes (spread), so EV is **per dollar actually wagered** at that true count.

---

## Decision grid (`DecisionPoint`)

Each histogram cell holds separate `ActionStats` for: **Hit, Stand, Double, Split, Surrender, Insurance Accept, Insurance Decline**. A scenario only *populates* the actions it compares; other columns may be zero / unused.

---

## True count discretization (Monte Carlo / `FixedEngine`)

From `include/core/FixedEngine.h`:

- True count is binned in **steps of 0.5** from **-60 to +60** (241 bins).
- Index formula: `tc_index = clamp(round2(tc), ...) + EV_TC_OFFSET` with `EV_TC_OFFSET = 120`.
- CSV column `TrueCount` repeats the **bin center** as a float: `(index - 120) * 0.5`.

Player totals in the grid: **hard and soft** bands with `EV_P_MIN = 4`, `EV_P_RANGE = 18` → player totals **4–21**. Dealer upcard dimension: **2–11** (Ace = 11) via `EV_D_RANGE = 10`.

---

## Primary CSV: scenario Monte Carlo (`saveScenarioResults`)

**Producer:** `FixedEngine::saveScenarioResults` → `writeCsvRowsForTable` in `src/core/FixedEngine.cpp`.

**Typical path (current `main.cpp`):**  
`stats/<StrategyName>_<ScenarioName>_<NumDecks>_<H17|S17>.csv`

Example: `stats/HiLoStrategy_Hit_vs_Stand_6_H17.csv`

### Header (exact column names)

`UserValue`, `DealerValue`, `IsSoft`, `TrueCount`,  
`Hit EV`, `Hit Variance`, `Stand EV`, `Stand Variance`, `Double EV`, `Double Variance`, `Split EV`, `Split Variance`, `Surrender EV`, `Surrender Variance`, `Insurance Accept EV`, `Insurance Accept Variance`, `Insurance Decline EV`, `Insurance Decline Variance`, `Hands Played`

### Row semantics

- One row per **(UserValue, DealerValue, IsSoft, TrueCount)** combination that received samples.
- `IsSoft`: `0` = hard, `1` = soft.
- `DealerValue`: 11 = Ace up.
- `Hands Played`: max hands count across the populated action stats in that cell (see `maxHandsPlayed` in `FixedEngine.cpp`).
- Only actions **in the scenario** are exercised; irrelevant EV columns may be unused for that file.

### Scenario names emitted by `createAllScenarios()` (`src/main.cpp`)

| `ScenarioName` | Actions compared | Notes |
|----------------|------------------|--------|
| `InsuranceAccept_vs_Decline` | Insurance accept vs decline | Many player totals vs dealer Ace; soft allowed |
| `Hit_vs_Stand` | Hit vs Stand | Hard hands only; specific totals vs dealer upcards |
| `Split_vs_Stand_Pair10s` | Split vs Stand | Pair of 10-class cards; `requirePair` |
| `Hit_vs_Double` | Hit vs Double | Hard hands; specific cells |
| `Surrender_vs_Hit` | Surrender vs Hit | Hard hands; totals where surrender is relevant |

Exact active `(player, dealer)` cells are defined in `createAllScenarios()`.

### Legacy / merged table

`FixedEngine::savetoCSVResults` writes the **same column layout** over the **legacy single table** `EVresults_` (used when not splitting by scenario). Unified sims use **per-scenario** files instead.

---

## RTP aggregate CSV

**Producer:** `runAllRTPSimulations` → `stats/rtp_results/rtp_results_<N>deck_<PP>pen_<H17|S17>_<DAS>_<RAS>_<Surrender>_<3to2|6to5>_kelly<KK>.csv`

### Columns

`Strategy`, `Decks`, `Penetration`, `DealerRule`, `DAS`, `RAS`, `Surrender`, `BlackjackPayout`, `Iterations`, `RTP`, `HouseEdge%`, `AvgWallet`, `AvgMoneyBet`, `NetPer1000`, `Duration_s`

**Semantics:**

- One row per strategy for that batch configuration.
- `RTP` / `HouseEdge%` derived from average ending wallet vs average money bet per iteration (initial wallet 50 000 in `runRTPsimsWithResults`).
- `NetPer1000`: net gain/loss scaled to **per $1000 wagered** (see `runRTPsimsWithResults`).

---

## EV per true count CSV (full-game, not forced actions)

**Producer:** `runRTPsimsWithResults` after each strategy’s RTP run.

**Path:** `stats/evPerTC/<StrategyName>/ev_per_tc_<StrategyName>_<N>deck_<PP>pen_<H17|S17>_<DAS>_<RAS>_<Surrender>_<3to2|6to5>.csv`

### Columns

`TrueCount`, `HandsPlayed`, `TotalMoneyWagered`, `TotalPayout`, `EVPerDollar`, `StdErrorPerDollar`

**Semantics:**

- Buckets keyed by **float true count** as reported by the strategy at hand resolution (same keys as accumulated during play).
- `EVPerDollar` = weighted mean net per dollar wagered in that bucket.
- `StdErrorPerDollar` = uncertainty on that mean.

**Use cases:** linear or nonlinear **EV vs TC** fits, spread-weighted RTP estimation, comparing strategies or rule sets, sample-size weighting (used in `ai_analysis_scripts/ev_truecount_fit.py`).

---

## Bundled / archived data (`data/`)

The repo may ship large precomputed trees, e.g.:

- `data/ev_per_tc_data/evPerTC/...`
- `data/rtp_data/.../evPerTC/...` and `.../rtp_results/...`
- `data/deviation_data/...` (scenario CSVs; naming may differ slightly from current `main.cpp`, e.g. `_H17` suffix without deck count)

**Same schemas** as above apply to the CSV contents; **filenames** encode deck count, penetration, rules.

---

## Python-derived outputs (post-processing)

### `ai_analysis_scripts/analyze_deviations.py`

**Input:** folders of scenario CSVs with the **scenario column layout** above.

**Note on filenames:** The script’s `parse_filename` regex expects names ending in `_<DecisionType>.csv` (e.g. `HiLoStrategy_Hit_vs_Stand.csv`). **Current C++ writes** `_<Strategy>_<Scenario>_<Decks>_<H17|S17>.csv`. If analysis fails to detect strategy/decision, align filenames or update the regex. Historical `data/deviation_data/` may already match older conventions.

**Outputs** (from `--output` base name):

| File | Content |
|------|---------|
| `*_full.csv` | Per-hand deviation analysis: crossover TC, CIs, EV diffs at TC 0, ±5, confidence labels, etc. |
| `*_actionable.csv` | Filtered subset (crossover exists, \|TC\| ≤ 15, enough hands) plus human-readable `Deviation` column |
| `*_summary.csv` | Aggregated counts per (game config, strategy) |

Optional `--profiles` → `tc_profiles/` CSVs per hand: `True Count`, `EV Difference`, `EV Diff CI Low/High`, `Hands`.

**Math enabled:** piecewise linear **crossover** estimation, **95% CI** on EV differences, weighted aggregation across rows with same TC.

### `ai_analysis_scripts/consolidate_insurance.py`

**Input:** insurance scenario CSV (same columns as engine).

**Output:** Aggregates over `UserValue` → one row per `(DealerValue, TrueCount[, IsSoft])` with weighted `Insurance Accept/Decline EV`, `Hands Played`, `Best Action`, `EV Diff (Accept-Decline)`.

### `ai_analysis_scripts/hand_lookup.py`

**Input:** `ev_per_tc_*.csv` files under chosen roots.

**Output:** `<Strategy>_hands_lookup.csv` — matrix of **HandsPlayed** by `TrueCount` × configuration column (built from filename tokens).

### `ai_analysis_scripts/ev_truecount_fit.py`

**Input:** `ev_per_tc` CSVs (`TrueCount`, `HandsPlayed`, `EVPerDollar`, …).

**Output:** Model comparison (polynomials, splines, LOWESS, piecewise linear, etc.) and summary CSV/plots as configured.

### `ai_analysis_scripts/rtp_calculator.py`

**Input:** single `ev_per_tc_*.csv`.

**Behavior:** Example RTP math using **hard-coded** Hi-Lo spread parameters; useful as a template, not a generic loader.

### `ai_analysis_scripts/generate_deviation_reports.py` / `generate_stats_deviation_report.py`

**Input:** folders of deviation CSVs.

**Output:** consolidated human-facing reports (`deviation_report*.csv`, comparison CSVs) with columns such as `Game Config`, `Strategy`, `Rules`, `Decision`, `Action A/B`, `Player Value`, `Dealer Upcard`, `Deviation TC`, `Total Hands`, `Notes`.

### Risk / spread tooling (`ror_analysis.py`, `bet_spread_calculator.py`, `bruteforce_bet_spread_sessions.py`, `verify_bet_spread_ruin.py`)

Consume **bucketed EV/variance** and/or `ev_per_tc` data to approximate **session EV**, **variance**, and **risk of ruin** under a bet schedule. Exact CSV schemas are defined in each script’s argparse / loaders.

---

## What mathematics this data supports well

- **EV vs true count:** regression, splines, changepoint detection; always weight or filter by `HandsPlayed` / `Hands Played`.
- **Optimal deviations:** compare two actions’ EV columns across TC; find **zero-crossing** of `EV_A - EV_B` with uncertainty (see `analyze_deviations.py`).
- **Insurance:** treat as binary decision; pool across player holdings at same TC and dealer Ace (`consolidate_insurance.py`).
- **Rule / penetration / deck sensitivity:** join files that share strategy but differ only in filename tokens or `Decks` / `Penetration` columns.
- **Overall RTP:** use `rtp_results_*.csv` or combine `ev_per_tc` with a **betting distribution** over TC (must be supplied or inferred; not in raw CSV alone).
- **Uncertainty:** use `StdErrorPerDollar` (ev_per_tc) or propagate variance from per-cell `* Variance` columns (scenario CSVs) when comparing actions.

---

## What the data does *not* fully specify

- **Composition-dependent** states (exact cards) are **not** exported—only totals, soft flag, and sometimes pair requirements inside the scenario logic.
- **Betting spread** used during `ev_per_tc` collection is whatever the strategy + Kelly config implements in `Engine`—not repeated per row in the CSV.
- **Covariance** between actions at the same dealt position is **not** stored; each forced action is a separate simulation path.

---

## Quick reference: file → schema

| Artifact | Key columns |
|----------|-------------|
| Scenario `FixedEngine` CSV | `UserValue`, `DealerValue`, `IsSoft`, `TrueCount`, paired `* EV`/`* Variance` per action, `Hands Played` |
| `ev_per_tc_*.csv` | `TrueCount`, `HandsPlayed`, `TotalMoneyWagered`, `TotalPayout`, `EVPerDollar`, `StdErrorPerDollar` |
| `rtp_results_*.csv` | `Strategy`, rules columns, `RTP`, `HouseEdge%`, wallet/bet aggregates |

---

## Source pointers

- Scenario CSV writer: `src/core/FixedEngine.cpp` (`writeCsvRowsForTable`, `saveScenarioResults`, `savetoCSVResults`)
- Bucketing constants: `include/core/FixedEngine.h`
- RTP + ev_per_tc writer: `src/main.cpp` (`runRTPsimsWithResults`, `runAllRTPSimulations`)
- Live per-hand TC stats: `src/core/Engine.cpp` (`EVperTC` updates)
- Scenario definitions: `src/main.cpp` (`createAllScenarios`, `runUnifiedMonteSims`)
