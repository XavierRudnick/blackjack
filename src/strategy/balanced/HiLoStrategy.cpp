#include "HiLoStrategy.h"
#include "Bankroll.h"
#include <cmath>

namespace {
    int getEvenBet() {
        return 1;
    }
}

HiLoStrategy::HiLoStrategy(float deck_size){
    num_decks_left = deck_size;
    initial_decks = deck_size;
}


int HiLoStrategy::getBetSize() {
    float effectiveTC = true_count - PROFITABLE_PLAY_TC_THRESHOLD;
    if (effectiveTC <= 0){
        return MIN_BET;
    }

    float interceptUnit = (Bankroll::getInitialBalance() * kellyFraction * evIntercept) / avgVolatility;
    int bet = std::round((unitSize * effectiveTC + interceptUnit) / (float)MIN_BET) * MIN_BET; // Round to nearest MIN_BET
    bet = std::max(MIN_BET, bet);
    return std::min(getMaxBet(), bet);
}

void HiLoStrategy::setUnitSize(float inputKellyFraction) {
    kellyFraction = inputKellyFraction;
    unitSize = (Bankroll::getInitialBalance() * kellyFraction * evPerTC) / avgVolatility;
    if (unitSize < 1.0f) unitSize = 1.0f;
    return;
}

void HiLoStrategy::updateCount(Card card) {
    Rank rank = card.getRank();
    int score = static_cast<int>(rank) + INDEX_OFFSET;

    constexpr int lowerCard = 6;
    constexpr int upperCard = 10;

    if (score <= lowerCard){
        running_count += 1.0f;
    }
    else if (score >= upperCard){
        running_count -= 1.0f;
    }

    float raw = running_count / num_decks_left;
    true_count = raw;// round when we go back to human level rtp sims
    return;
}

void HiLoStrategy::updateDeckSize(int num_cards_left){
    num_decks_left = static_cast<float>(num_cards_left) / 52.0f;

    // Recompute true count whenever deck size changes so future bets/decisions use latest shoe depth
    if (num_decks_left > 0) {
        float raw = running_count / num_decks_left;
        true_count = raw;
    }

    return;
}

float HiLoStrategy::getTrueCount() const{
    return true_count;
}

float HiLoStrategy::getRunningCount() const{
    return running_count;
}

float HiLoStrategy::getDecksLeft() const{
    return num_decks_left;
}

bool HiLoStrategy::shouldAcceptInsurance() const{
    const bool useSixDeck = initial_decks >= 5.5f;
    // 2-deck 65% pen: TC crossover = 2.5, 6-deck 80% pen: TC crossover = 3.0
    const float insuranceThreshold = useSixDeck ? 3.0f : 2.5f;
    if (true_count >= insuranceThreshold){
        return true;
    }
    return false;
}

Action HiLoStrategy::shouldDeviatefromHard(int playerTotal, Rank dealerUpcard, float trueCount){
    int dealerValue = BasicStrategy::getIndex(dealerUpcard) + INDEX_OFFSET;
    const bool useSixDeck = initial_decks >= 5.5f;

    switch (playerTotal) {
        case 16:
            // 2-deck 65% pen: 16v10 Stand TC >= 0.5, 6-deck 80% pen: TC >= 0.0
            if (dealerValue == 10 && trueCount >= (useSixDeck ? 0.0f : 0.5f)) {
                return Action::Stand;
            }
            break;
            
        case 15: 
            // 2-deck 65% pen: 15v10 Stand TC >= 3.0, 6-deck 80% pen: TC >= 3.5
            if (dealerValue == 10 && trueCount >= (useSixDeck ? 3.5f : 3.0f)) {
                return Action::Stand;
            }
            break;
            
        case 13:
            // 2-deck 65% pen: 13v2 Stand TC >= -0.5, 6-deck 80% pen: TC >= -1.0
            // 2-deck 65% pen: 13v3 Stand TC >= -2.0, 6-deck 80% pen: TC >= -2.5
            if (dealerValue == 2 && trueCount >= (useSixDeck ? -1.0f : -0.5f)) { 
                return Action::Stand;
            }
            if (dealerValue == 3 && trueCount >= (useSixDeck ? -2.5f : -2.0f)) { 
                return Action::Stand;
            }
            break;

        case 12:
            // 2-deck 65% pen: 12v3 Stand TC >= 2.0, 6-deck 80% pen: TC >= 1.5
            if (dealerValue == 3 && trueCount >= (useSixDeck ? 1.5f : 2.0f)) {
                return Action::Stand;
            }
            // 2-deck 65% pen: 12v2 Stand TC >= 3.5, 6-deck 80% pen: TC >= 3.0
            if (dealerValue == 2 && trueCount >= (useSixDeck ? 3.0f : 3.5f)) {
                return Action::Stand;
            }
            break;
        case 11:
            // 2-deck 65% pen: 11v11 Double TC >= -0.5, 6-deck 80% pen: TC >= 0.5
            if (dealerValue == 11 && trueCount >= (useSixDeck ? 0.5f : -0.5f)){
                return Action::Double;
            }
            break;
        case 10:
            // 2-deck 65% pen: 10v10 Double TC >= 2.5, 6-deck 80% pen: TC >= 3.0
            if (dealerValue == 10 && trueCount >= (useSixDeck ? 3.0f : 2.5f)){
                return Action::Double;
            }
            // 2-deck 65% pen: 10v11 Double TC >= 2.5, 6-deck 80% pen: TC >= 3.0
            if (dealerValue == 11 && trueCount >= (useSixDeck ? 3.0f : 2.5f)){
                return Action::Double;
            }
            break;
        case 9:
            // 2-deck 65% pen: 9v2 Double TC >= 0.5, 6-deck 80% pen: TC >= 0.5
            if (dealerValue == 2 && trueCount >= 0.5f){
                return Action::Double;
            }
            // 2-deck 65% pen: 9v7 Double TC >= 3.0, 6-deck 80% pen: TC >= 3.0
            if (dealerValue == 7 && trueCount >= 3.0f){
                return Action::Double;
            }
            break;
        default: return Action::Skip;
    }
    return Action::Skip;
}

Action HiLoStrategy::shouldDeviatefromSplit(Rank playerRank, Rank dealerUpcard, float trueCount){
    int dealerValue = BasicStrategy::getIndex(dealerUpcard) + INDEX_OFFSET;
    int playerValue = BasicStrategy::getIndex(playerRank) + INDEX_OFFSET;
    const bool useSixDeck = initial_decks >= 5.5f;
    switch (playerValue) {
        // 2-deck 65% pen: Split 10s v5 TC >= 4.0, 6-deck 80% pen: TC >= 4.5
        // 2-deck 65% pen: Split 10s v6 TC >= 4.0, 6-deck 80% pen: TC >= 4.0
        case 10: 
            if (dealerValue == 5 && trueCount >= (useSixDeck ? 4.5f : 4.0f)) {
                return Action::Split;
            }
            if (dealerValue == 6 && trueCount >= 4.0f) {
                return Action::Split;
            }
            break;
        default: return Action::Skip; break;
    }
    return Action::Skip;
}

Action HiLoStrategy::shouldSurrender(int playerTotal, Rank dealerUpcard, float trueCount){
    int dealerValue = BasicStrategy::getIndex(dealerUpcard) + INDEX_OFFSET;
    const bool useSixDeck = initial_decks >= 5.5f;
    switch (playerTotal) {
        case 16:
            // 2-deck 65% pen: 16v9 Surrender TC >= 0.0, 6-deck 80% pen: TC >= -0.5
            if (dealerValue == 9 && trueCount >= (useSixDeck ? -0.5f : 0.0f)) {
                return Action::Surrender;
            }
            // 2-deck 65% pen: 16v10 Surrender TC >= -3.0, 6-deck 80% pen: TC >= -3.5
            if (dealerValue == 10 && trueCount >= (useSixDeck ? -3.5f : -3.0f)) {
                return Action::Surrender;
            }
            // 2-deck 65% pen: 16v11 Surrender TC >= -2.0, 6-deck 80% pen: TC >= -2.0
            if (dealerValue == 11 && trueCount >= -2.0f) {
                return Action::Surrender;
            }
            break;
        case 15:
            // 2-deck 65% pen: 15v9 Surrender TC >= 2.0, 6-deck 80% pen: TC >= 2.0
            if (dealerValue == 9 && trueCount >= 2.0f) {
                return Action::Surrender;
            }
            // 2-deck 65% pen: 15v10 Surrender TC >= -0.5, 6-deck 80% pen: TC >= -0.5
            if (dealerValue == 10 && trueCount >= -0.5f) {
                return Action::Surrender;
            }
            // 2-deck 65% pen: 15v11 Surrender TC >= 0.5, 6-deck 80% pen: TC >= 1.0
            if (dealerValue == 11 && trueCount >= (useSixDeck ? 1.0f : 0.5f)) {
                return Action::Surrender;
            }
            break;
        case 14:
            // 2-deck 65% pen: 14v10 Surrender TC >= 2.5, 6-deck 80% pen: TC >= 2.5
            if (dealerValue == 10 && trueCount >= 2.5f) {
                return Action::Surrender;
            }
            break;
        default: return Action::Skip; break;
    }
    return Action::Skip;
}

Action HiLoStrategy::getHardHandAction(int playerTotal, Rank dealerUpcard, float trueCount) {
    constexpr int lowerBound = 5;
    constexpr int upperBound = 20;
    if (playerTotal < lowerBound) {
        return Action::Hit;
    }

    if (playerTotal > upperBound) {
        return Action::Stand;
    }
    
    int dealerIdx = BasicStrategy::getIndex(dealerUpcard);
    
    int playerIdx = playerTotal - lowerBound;  // Player total 5 maps to index 0 Since chart starts at 5
    Action deviation = shouldDeviatefromHard(playerTotal, dealerUpcard, trueCount);
    if (deviation != Action::Skip) {
        return deviation;
    }
    else{
        return BasicStrategy::hardTotalTable[playerIdx][dealerIdx];
    }
    
}

Action HiLoStrategy::getSoftHandAction(int playerTotal, Rank dealerUpcard) {
    constexpr int lowerBound = 13;
    int dealerIdx = BasicStrategy::getIndex(dealerUpcard);
    int playerIdx = playerTotal - lowerBound;  // Soft 13 maps to index 0 Since chart starts at A,2
    
    Action action = BasicStrategy::softTotalTable[playerIdx][dealerIdx];

    return action;
}

Action HiLoStrategy::getSplitAction(Rank playerSplitRank, Rank dealerUpcard, float trueCount) {
    int dealerIdx = BasicStrategy::getIndex(dealerUpcard);
    int pairIdx = BasicStrategy::getIndex(playerSplitRank);
    Action deviation = shouldDeviatefromSplit(playerSplitRank, dealerUpcard, trueCount);
    if (deviation != Action::Skip) {
        return deviation;
    }
    else{
        return BasicStrategy::splitTable[pairIdx][dealerIdx];
    }
}

void HiLoStrategy::reset(int deckSize){
    running_count = 0;
    true_count = 0;
    num_decks_left = deckSize;
    initial_decks = deckSize;
    return;
}

std::string HiLoStrategy::getName() {
    return "HiLoStrategy";
}
