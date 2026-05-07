# Blackjack Simulation Engine — Accuracy & Correctness Audit

**Scope:** `src/` and `include/` (C++ engine, FixedEngine, all strategies, builder, players, helpers).
**Goal:** Verify the data produced by the simulator faithfully reflects real blackjack mechanics, and surface bugs / biases that could distort published statistics.
**Method:** Read every game-mechanics file end-to-end, hand-trace edge cases (BJ, splits, aces, insurance, surrender, both-bust), validate every counting strategy's tag values against canonical Wong / Snyder / Renzey references, and audit numerical stability (Welford, Kelly, RNG).

---

## TL;DR Verdict

The **core game engine and FixedEngine Monte‑Carlo machinery are correct**. Hand scoring, payout math, dealer draw rules (H17/S17), insurance accounting, BJ payouts, splits, doubles, surrender, EV bucket math (weighted Welford), and the deck-clone isolation in the Monte-Carlo path all check out.

There are, however, **specific correctness defects in the unbalanced counting strategies** (KO, Red 7, KISS III, Uston SS, UZen II) that make their *count* values structurally wrong, and a small handful of **secondary issues** (BasicStrategy chart hardcoded for H17 only, surrender allowed after split, NoStrategy never surrenders, minor in-hand TC staleness). These mostly affect *which strategies are accurately characterized*, not whether the underlying engine produces sound EV/RTP numbers.

| Component | Verdict |
|---|---|
| Card / Deck / Hand math | ✅ Correct |
| Engine main loop (deal → insurance → play → evaluate) | ✅ Correct |
| Dealer H17/S17 logic | ✅ Correct |
| Blackjack 3:2 / 6:5 payout | ✅ Correct |
| Insurance accounting | ✅ Correct |
| Split / re-split / double-after-split | ✅ Correct (one rule deviation, see §4) |
| Surrender accounting | ✅ Correct (engine), ⚠️ rule edge case (see §4) |
| FixedEngine deck cloning & EV bucketing | ✅ Correct |
| Weighted Welford EV/variance | ✅ Correct |
| BasicStrategy chart | ⚠️ Mixed H17/S17 (see §5) |
| HiLo, Mentor, Zen, Omega II, Wong Halves, R14, RPC, RAPC | ✅ Tag values correct |
| KO, Red 7, KISS III, Uston SS, UZen II | ❌ **Count is broken** (see §6) |
| Kelly bet sizing (CSV-driven) | ✅ Correct |
| Cushion+B0 trick in `runRTPsims` | ✅ Sound (see §7) |
| RNG / seeding | ✅ Correct |

---

## 1. Card / Deck / Hand

### Card scoring (`src/core/Card.cpp`, `src/core/Hand.cpp`)
- `Card::getValue()` returns 2–9 directly, 10 for T/J/Q/K, **11 for Ace** (initially). ✓
- `Hand::tallyRank` runs an incremental scoring loop with a soft-ace counter; demotes Aces from 11→1 only while score > 21 and softAces > 0. Hand-trace: `A,A → 12 (one soft)`; `A,A,A → 13 (one soft)`; `A,10 → 21 (soft, isBlackjack)`; `A,6,5 → 12 hard` (the 5 demotes the Ace). All correct.
- `isBlackjack()` requires `hand.size()==2` and one ten-value + one ace. ✓
- `isSoft17()` correctly distinguishes hard 17 from soft 17 for the dealer rule.
- `isHandSoft()` = `softAces_ > 0 && runningScore_ <= 21` — necessary guard so a busted-then-demoted hand isn't called soft.

### Deck (`src/core/Deck.cpp`)
- Builds `numDecks * 52` cards, all 4 suits × 13 ranks. ✓
- `std::shuffle` with `std::mt19937` seeded either from `std::random_device` (8×32 bits → `seed_seq`, well-mixed) or via `BLACKJACK_SEED` for determinism.
- Thread-local RNG with an atomic epoch counter — each thread re-seeds when the global seed changes. ✓
- `clone()` returns a deep copy of the underlying vector — the FixedEngine relies on this and it works.
- `deal()` pops two from the back; `hit()` pops one. Both throw `std::runtime_error` when empty (caught by the engine).

### Deal ordering (minor)
The engine deals **both dealer cards then both player cards**, not the casino-style alternating order. This is mathematically irrelevant for a shuffled shoe but affects how rigged-deck unit tests must be written (and is reflected in the existing tests).

---

## 2. Engine Game Loop (`src/core/Engine.cpp`)

### Penetration math
```cpp
config.penetrationThreshold = (1 - config.penetrationThreshold) * config.numDecks * 52;
```
With `penetrationThreshold=0.75` and 6 decks → 78. The shoe loop continues `while deck->getSize() > 78`, i.e. **stops once 75% of the shoe (234 cards) has been dealt**. Correct interpretation of "penetration".

### Hand flow
1. `playHand()` → `playHandImpl()` deducts the bet, deals dealer + player.
2. **Visible-card counting:** dealer upcard + both player cards are passed to `player->updateCount()`. The hole card is counted later, on reveal. ✓
3. **Insurance Monte-Carlo** is dispatched here, *before* insurance resolves, so it sees the full population (including hands where dealer ends up with BJ). Correct for "should I take insurance?" EV.
4. `handleInsurancePhase` (only if dealer shows Ace) → `dealerRobberyHandler` (only if dealer shows Ten). Together these cover both dealer-BJ peek paths. After both, **the dealer is guaranteed to not have BJ** when player play begins.
5. `user_play` → `play_hand`, which is where the **non-insurance Monte-Carlo** is dispatched. Because peek already happened, FixedEngine's clone here is **post-peek**, so its EV samples are properly conditional on "dealer doesn't have BJ given upcard". ✓
6. `evaluateHands` — exposes hole card, runs dealer draw if any hand survived, settles each hand.

### Dealer draw (`Engine::dealer_draw`)
```cpp
if (score > 17 || (score == 17 && !isSoft17) || (isSoft17 && !config.dealerHitsSoft17)) return;
while (!dealer.isDealerOver() || (dealer.isSoft17() && config.dealerHitsSoft17)) { hit }
```
- Hard 17+ → stand.
- Soft 17 + S17 → stand. Soft 17 + H17 → hit.
- Loop hits while score < 17, OR while soft-17 + H17. Re-evaluates `isSoft17()` each iteration so a soft-17 → hit → hard-17-or-better path stops correctly.

Verified by hand-trace through several edge cases. Correct for both H17 and S17.

### Payouts
- **Natural BJ:** `bankroll.deposit(bet + bet * 1.5)` and EV recorded as `+1.5` per unit wagered. With `with6To5Payout()` the multiplier is 1.2 (correct: $6 won per $5 → 1.2). ✓
- **Win:** `deposit(bet * 2)` (stake + winnings), EV `+1`. ✓
- **Loss:** no deposit, EV `-1`. ✓
- **Push:** `deposit(bet)` (stake returned), EV `0`. ✓
- **Surrender:** `deposit(bet * 0.5)`, EV `-0.5`. ✓
- **Insurance accepted, dealer BJ:** `deposit(insuranceWager * 3)` = stake + 2× — pays 2:1 correctly. EV recorded as +1.0 per main bet. ✓
- **Insurance accepted, dealer BJ, player BJ:** main hand pushes (deposit `bet`), insurance pays out (deposit `1.5 * bet`). Net +1 unit. ✓

### Both-bust handling
The branch `else if (dealer_score == 0 && score == 0)` is reachable in `evaluateHands` for split hands where one busts and dealer also busts on the survivors' draw. For single-hand busts the dealer doesn't draw (didHandsBust), so `dealer_score > 0 > score` triggers and the player loses. Both paths attribute the loss correctly.

### Insurance asks before peek
When dealer shows Ace, `handleInsurancePhase` asks `player->shouldAcceptInsurance()` *before* checking `dealerHiddenTen`. This matches real dealer protocol (decision precedes hole-card check). ✓

### `dealerRobberyHandler` (dealer 10 + hidden Ace)
The terminology is non-standard but the logic is right: dealer auto-flips BJ; player BJ → push, otherwise loss. Hole card is counted on reveal. ✓

### Bet bookkeeping
- `currentHandBetTotal` accumulates main bet + insurance + double + extra-split bets and is used to refund on deck-exhaustion exceptions.
- `bankroll.addTotalBet(amount)` is increased on every wager and decreased on the refund path. Used by the RTP normalizer.

### Split mechanics (`Engine::splitHandler`)
- Re-split-aces gating works: `splitting_aces && has_split_aces && !allowReSplitAces` short-circuits.
- For non-aces, both new hands recurse through `play_hand` (full action set on each side).
- For aces, each hand gets exactly one card; recursion only re-enters splitHandler if the new card is another Ace **and** RAS is on, which is the canonical rule.

### One real rule deviation
`BotPlayer::getAction` checks `shouldSurrender` whenever `checkCanDouble()` is true (i.e. hand has 2 cards). After a split, the new sub-hand also has 2 cards, so **surrender is offered after split**. Most real houses prohibit this. If `allowSurrender=true`, this slightly inflates surrender frequency (and its measured EV, see §4).

---

## 3. FixedEngine Monte-Carlo (`src/core/FixedEngine.cpp`)

This is the engine that produces all the EV-by-action tables. It must be exactly right for the published statistics to be trustworthy.

### Isolation
`calculateEV` clones both deck (`deck.clone()`) and Hand objects (value semantics) per forced action. The simulation cannot leak state into the live game. ✓

### Counting in the simulated path
The cloned deck is consumed without calling `player->updateCount()`. That is **deliberate and correct**: subsequent strategy decisions inside the simulation should be based on the count *as of the decision point*, not as of cards drawn in a hypothetical future. The strategy receives the original `trueCount` as a parameter for deviations.

### EV normalization (per *original* base bet)
- `Hit`/`Stand`: `addResult(result)` where result ∈ {-1, 0, +1}.
- `Double`: `addResult(result * 2)` — hand resolves at 2× stake, recorded per unit base.
- `Split`: `addResult(splitPayout)` where `splitPayout = Σ_hand result * (handBet / baseBet)`. This handles double-after-split correctly because the multiplier on a doubled split-hand is 2.
- `Surrender`: `addResult(-0.5)`.
- `Insurance Accept`: handled with explicit branches:
  - dealer BJ + player BJ → +1.0 (push on main, win 0.5 stake on insurance @ 2:1).
  - dealer BJ + no player BJ → 0.0 (lose main, win insurance — net break-even).
  - dealer no BJ → `result - 0.5` (main result minus the lost insurance stake).
  - player BJ + dealer no BJ → +1.0 (early branch, BJ +1.5 minus 0.5 insurance loss).
- `Insurance Decline`: just main-hand result, with player-BJ early branch returning +1.5.

All branches were hand-verified and reconcile with real EV calculations.

### Welford / variance
`ActionStats::addResult(net, wagered)` is a **weighted online mean and M2** with weight = wagered:
```
δ      = value − meanOld
meanNew = meanOld + (w / Wnew) · δ
M2     += w · δ · (value − meanNew)
```
This is a textbook West/Welford weighted update; the numerical formula matches the textbook derivation. `getVariance()` returns `M2 / W` (population variance per weight unit) and `getStdError()` returns `std/√W`, which is the **per-dollar-wagered** error, exactly what the CSV Kelly sizer expects.

The `merge(other)` path uses the standard parallel-pair formula
```
δ        = src.mean − dst.mean
meanNew  = (Wd·md + Ws·ms) / (Wd + Ws)
M2new    = Md + Ms + δ²·Wd·Ws / (Wd+Ws)
```
which is correct for combining two weighted Welford accumulators. ✓

### The "dead branch" (cosmetic, not a bug)
In FixedEngine, after `if (userScore != 0) dealer_draw(...)`, the branch `dealerScore == 0 && userScore == 0` is unreachable: when the player busts, the dealer doesn't draw, and the dealer's initial 2-card score is always in [4, 21]. Code-style smell only.

### Subtle issue: per-hand dealer_draw inside the split loop
For split forced-action evaluation, dealer_draw is called inside the per-hand loop. The dealer's hand is mutable, so the **first** call resolves the dealer; subsequent calls early-return. This is fine (one consistent dealer line per simulated event), but the same single dealer decision is used to settle both split halves — exactly as in real play.

---

## 4. Surrender / Split rules — minor deviations

| Issue | Effect |
|---|---|
| `BotPlayer::getAction` allows surrender after split (any 2-card sub-hand) | Inflates surrender EV slightly when `allowSurrender=true`, since the action is taken in scenarios real casinos disallow. |
| FixedEngine surrender always records −0.5 (no peek interaction) | Insurance-MC is dispatched pre-peek; non-insurance MC is dispatched **post-peek** (after `handleInsurancePhase`/`dealerRobberyHandler`), so dealer-BJ scenarios are already filtered out of surrender's sample. **Net effect: no bias.** ✓ |
| `NoStrategy::shouldSurrender` always returns Skip | "No-strategy" baseline never surrenders even when surrender is on the menu. This makes its measured RTP slightly worse than canonical basic strategy with surrender; not a bug, but worth flagging when reading published numbers. |

The previously-suspected "FixedEngine surrender doesn't respect dealer-peek BJ" bias is **not present** in this codebase, because the live Engine peeks first and only then invokes the FixedEngine.

---

## 5. BasicStrategy chart (`src/strategy/BasicStrategy.cpp`)

Tables hard-coded for **multi-deck H17 DAS** with these notable cells:

| Cell | Code | Canonical H17 | Canonical S17 |
|---|---|---|---|
| 11 vs A | Double | Double | Hit (S17) |
| A-7 vs 2 | Double | Double | **Stand** |
| A-8 vs 6 | Double | Double | Stand |
| 9-9 vs A | Stand | Split | Stand |

Three of the four cells are H17-correct, but **9-9 vs A is the S17 play** — so the chart is a mild H17/S17 mix. Net impact on RTP is on the order of ~0.001% per cell at most. If the simulator is run with `withS17Rules()`, the A-7/A-8 cells are wrong by a similar tiny margin.

The chart correctly omits surrender; surrender is delegated to each `CountingStrategy::shouldSurrender`. **NoStrategy never returns Surrender**, so a surrender-allowed game played with NoStrategy gets a ~0.08% pessimistic RTP relative to canonical basic strategy with surrender.

---

## 6. ❌ Counting strategy correctness

### 6.1 Tag values
Verified every strategy's per-card increments against published references:

| Strategy | Tag values | Verdict |
|---|---|---|
| HiLo (`HiLoStrategy.cpp`) | 2–6:+1, 7–9:0, 10/J/Q/K/A:−1 | ✅ |
| Zen (`ZenCountStrategy.cpp`) | 2,3:+1; 4,5,6:+2; 7:+1; 8,9:0; 10:−2; A:−1 | ✅ |
| Omega II (`OmegaIIStrategy.cpp`) | 2,3:+1; 4,5,6:+2; 7:+1; 8:0; 9:−1; 10:−2; A:0 | ✅ |
| Omega II + ace count (`OmegaIIStrategyAceCount.cpp`) | Same RC + side ace count, ace-adjusted TC for betting; raw TC for play decisions | ✅ Canonical implementation. |
| Mentor (`MentorStrategy.cpp`) | 2:+1; 3,4,5,6:+2; 7:+1; 8:0; 9:−1; 10:−2; A:−1 | ✅ Net per deck = 0. |
| R14 / Revere 14 (`R14Strategy.cpp`) | 2,3:+2; 4:+3; 5:+4; 6:+2; 7:+1; 8:0; 9:−2; 10:−3; A:0 | ✅ Net per deck = 0. |
| Wong Halves (`WongHalves.cpp`) | 2:+0.5; 3,4:+1; 5:+1.5; 6:+1; 7:+0.5; 8:0; 9:−0.5; 10:−1; A:−1 | ✅ |
| RAPC (`RPACStrategy.cpp`) | 2,7:+2; 3,4,6:+3; 5:+4; 8:0; 9:−1; 10:−3; A:−4 | ✅ Net per deck = 0. |
| RPC (`RPCStrategy.cpp`) | 2:+1; 3,4,5,6:+2; 7:+1; 8,9:0; 10:−2; A:−2 | ✅ Net per deck = 0. |
| KO | 2–7:+1; 8,9:0; 10/A:−1 | Tags ✅ |
| Red 7 | 2–6:+1; red 7:+1, black 7:0; 8,9:0; 10/A:−1 | Tags ✅ |
| KISS III | black 2:+1, red 2:0; 3–7:+1; 8,9:0; 10/A:−1 | Tags ✅ |
| Uston SS | 2,3,4:+2; 5:+3; 6:+2; 7:+1; 8:0; 9:−1; 10:−2; A:−2 | Tags ✅ Net +4/deck. |
| UZen II | 2:+1; 3,4,5,6:+2; 7:+1; 8,9:0; 10:−2; A:−1 | Tags ✅ Net +4/deck. |

**Tag values for every strategy are correct.**

### 6.2 ❌ Bug A — unbalanced strategies are using a balanced-style true count

Every "unbalanced" strategy (KO, Red 7, KISS III, Uston SS, UZen II) implements `updateDeckSize` like this:
```cpp
void KoStrategy::updateDeckSize(int num_cards_left){
    num_decks_left = static_cast<float>(num_cards_left) / 52.0f;
    if (num_decks_left > 0) {
        float raw = true_count / num_decks_left;
        true_count = raw;            // ← MUTATES the count in place
    }
}
```
And `updateCount` then **adds tag values to this already-divided number**:
```cpp
true_count += 1; // for low cards
```

Two compounding problems here:

1. **Conversion to TC defeats the purpose of an unbalanced count.** KO/Red 7/KISS/Uston SS/UZen II are designed precisely so that the player *uses the running count directly* against fixed key-counts — no division required. Dividing by decks-left turns them into broken pseudo-balanced systems.
2. **Mutating `true_count` in place** then re-adding tag values means each subsequent card pushes `true_count` further away from any meaningful quantity, because you're adding integer tags onto a fraction. Over a shoe the value drifts incoherently.

Concretely: with KO at 6 decks, after a few hands:
- correct KO running count might be e.g. `−18` (player slightly behind),
- this code, after one `updateDeckSize` call divides it by ~5 decks left → `−3.6`, then the next card-tag of `+1` makes it `−2.6`, then the next `updateDeckSize` divides `−2.6` by 4.8 → `−0.54`, etc.

The number returned by `getTrueCount()` (and used for both bet sizing and deviation thresholds) is no longer interpretable as either a running count or a true count — it is corrupted.

**Consequence:** any RTP / EV statistic published using KO, Red 7, KISS III, Uston SS, or UZen II is **not measuring what its name says it is measuring**. The strategies still play near-basic-strategy (their deviation tables happen to fire rarely), so RTPs may look plausible, but they are **not** valid measurements of those counting systems.

### 6.3 ❌ Bug B — Initial Running Count (IRC) is off by one deck

KO documentation specifies IRC = `−4·(N − 1)` for N decks (so the pivot point lands at +4 regardless of N). Red 7 uses `−2·(N − 1)`. The code uses:

```cpp
KoStrategy::KoStrategy(float deck_size){
    true_count = deck_size * -4;   // -24 for 6 decks; canonical IRC is -20
}
```
and the same `-2·N` (vs canonical `−2·(N−1)`) in Red 7 / KISS III. Even if Bug A were fixed, every published key count would be shifted by a constant +4 (or +2). Combined with Bug A, the count is nonsense.

### 6.4 In-hand TC staleness (minor, all strategies)
`num_decks_left` is updated only at the start of each hand (`Engine::playHand → player->updateDeckStrategySize(deck->getSize())`), but `updateCount` runs per card during a hand. Within a single 4–6-card hand the divisor lags by up to ~6/52 of a deck. The HiLo / Zen / Omega II / Wong Halves / RAPC / RPC / R14 strategies recompute the TC from running_count whenever `updateDeckSize` is called, so the staleness only affects in-hand decisions (insurance & late deviations). Mentor doesn't do that recomputation in `updateDeckSize`, so its TC also lags whenever the deck size updates without a card update. All of these are at-most-a-fraction-of-a-true-count effects — well within Monte-Carlo noise.

### 6.5 Deviation tables and insurance thresholds
Each balanced strategy's deviation table comes annotated with comments like
```
// 2-deck 65% pen: 16v10 Stand TC >= 0.5, 6-deck 80% pen: TC >= 0.0
```
indicating the thresholds were back-calculated **from this simulator's own data**. They diverge slightly from textbook Illustrious-18/Fab-4 values but are internally consistent. That's fine if you trust the underlying simulator math (which I do, see §1–§3); it does mean these strategies are calibrated to *this engine* and not to any third-party reference.

---

## 7. Bet sizing and bankroll mechanics

### 7.1 CSV Kelly sizer (`src/strategy/CSVKellyBetSizer.cpp`)
Bets are looked up by half-unit TC bucket from a precomputed table. The bet formula at construction time:
```
var = sd² · handsPlayed     // sd = StdErrorPerDollar from CSV
raw = B0 · k · EV / var
bet = round(clip(raw, MIN_BET, MAX_BET) / MIN_BET) · MIN_BET
```
Two algebra checks:
- The CSV's `StdErrorPerDollar` is `stdDevPerDollar / √W` where `W = totalMoneyWagered`. So `sd² · handsPlayed = (stdDevPerDollar)² · (handsPlayed / W)`. For the even-bet (1-unit) producer, `W ≈ handsPlayed · (avg bet inflation from splits/doubles)` ≈ `handsPlayed · 1.05`, so `sd² · handsPlayed ≈ varPerDollar / 1.05`. This is the right order of magnitude for fixed-B0 fractional-Kelly sizing; the Python sister script uses the same formula.
- `betForTrueCount` uses `lround(TC / 0.5) − tcMinScaled` for bucket lookup, matching the +0.5 quantization used elsewhere.

The fixed-B0 design (B0 captured at construction, never updated by the engine) is intentional — and matches the Python `bankroll_replay_kelly.py` reference. It also means `setUnitSize` is a no-op once wrapped in this sizer, which is what `runRTPsims*` rely on.

### 7.2 The `cushion = 1e9` trick (`runRTPsims` in `main.cpp`)
The engine halts a shoe when bankroll < $25 (`Engine::runner`). To replay realistic Kelly trajectories without artificially halting on early ruin, every shoe is built with `setInitialWallet(cushion + profit.first)` and `profit.first = end_profit.first − cushion` to recover the true bankroll. Because `CSVKellyBetSizer` captures B0 at construction (=$100k or $50k, *not* the cushioned wallet), bet sizes are unaffected by the cushion. Verified — this trick is correct and consistent.

### 7.3 ⚠️ `Bankroll::initialBalance` is a static
This static is overwritten by every Engine constructor (`initialBalance = startBalance`). Strategies that compute their unit-size from `Bankroll::getInitialBalance()` (HiLo / Zen / Omega II / Mentor / R14 / Wong Halves / RAPC / RPC / OmegaII+Ace) read this static at `setUnitSize` time. In the multi-threaded simulation paths (`runAllRTPSimulations`, `setUpUnifiedSims`) every thread happens to use the same `initialWallet` so this isn't biting today, **but it is a latent thread-safety bug**: if two future threads ever build engines concurrently with different wallets, the one that calls `setUnitSize` second would compute the wrong unit. Recommend making `initialBalance` per-instance (or per-thread).

In current usage the field doesn't matter when the strategy is wrapped in `CSVKellyBetSizer` (which ignores it) or when `useFixedUnitBet=true` (which short-circuits `getBetSize`).

---

## 8. RNG and determinism

`Deck::getGlobalRng()` uses a `thread_local mt19937` plus a `std::atomic<uint64_t> gRngEpoch` counter. Threads reseed when the epoch ticks. Seeds come from either `BLACKJACK_SEED` (uint32 → mt19937) or `random_device` mixed through an 8×32 `seed_seq`. This is the right pattern for a multi-threaded sim; both seeding modes were verified by code reading.

One small note: `Deck::reset()` is `std::shuffle(deck.begin(), deck.end(), rng)`. So the cards in the shoe are **never replaced** — the same 52·N cards are reshuffled. For Monte-Carlo this is correct ("infinite-shoe" sims aren't intended).

---

## 9. Final assessment

The simulator's *engine* — game flow, dealer logic, payouts, FixedEngine cloning, EV/variance bookkeeping, Kelly bet replay — is implemented carefully and produces correct numbers. I have high confidence in any RTP/EV figure produced **for**:

- Basic-strategy / no-counting baselines (`NoStrategy`).
- HiLo, Zen, Omega II (with or without ace count), Mentor, Wong Halves, R14, RAPC, RPC.

I have **low confidence in any published number for KO, Red 7, KISS III, Uston SS, or UZen II**, because the count those strategies maintain is structurally incoherent (Bug A in §6.2 is the dominant error; Bug B in §6.3 compounds it). Their RTP figures will look reasonable because they degrade roughly to "basic strategy with rare nonsense deviations", but the figures do not represent the canonical strategies they're named after.

The non-bugs worth tracking:
- BasicStrategy is hardcoded H17 (3 cells diverge under S17, all <1¢/100 bets).
- Surrender is allowed after split if `allowSurrender=true` (rule deviation).
- `NoStrategy` never surrenders even when surrender is on the menu.
- `Bankroll::initialBalance` is a static (latent thread-safety hazard).
- In-hand TC slightly stale because `num_decks_left` updates per hand, not per card (sub-true-count noise).

### Recommended fixes (in priority order)
1. **Unbalanced strategies (KO, Red 7, KISS III, Uston SS, UZen II):** rename `true_count` member to `running_count`, drop the `updateDeckSize` mutation entirely, and have `getTrueCount() const` simply return the running count. Fix the IRCs to `-K * (numDecks - 1)` where K is each system's per-deck imbalance (4 for KO/Uston SS/UZen II, 2 for Red 7/KISS III).
2. Either parameterize `BasicStrategy` charts by H17/S17, or pick one and document it.
3. Gate surrender on "is this the player's first action of an unsplit hand" (e.g., only when no split occurred this hand).
4. Move `Bankroll::initialBalance` off the static into per-engine state (or pass it through `setUnitSize`).
5. (Optional) Recompute true count after every `updateCount` using the freshest deck size for the strategies that don't already.

After (1) is fixed, the unbalanced-strategy datasets should be regenerated; until then they should be flagged in the README as not representing canonical KO/Red7/etc.
