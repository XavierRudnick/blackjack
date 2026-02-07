#include "R14Strategy.h"
#include "Bankroll.h"
#include <cmath>

R14Strategy::R14Strategy(float deck_size){
    num_decks_left = deck_size;
    initial_decks = deck_size;
}

int R14Strategy::getEvenBet() const {
    return 1;
}

int R14Strategy::getBetSize() {
    float effectiveTC = true_count - PROFITABLE_PLAY_TC_THRESHOLD;
    if (effectiveTC <= 0){
        return MIN_BET;
    }

    float interceptUnit = (Bankroll::getInitialBalance() * kellyFraction * evIntercept) / avgVolatility;
    int bet = std::round((unitSize * effectiveTC + interceptUnit) / (float)MIN_BET) * MIN_BET; // Round to nearest MIN_BET
    bet = std::max(MIN_BET, bet);
    return std::min(getMaxBet(), bet);
} 

void R14Strategy::setUnitSize(float inputKellyFraction) {
    kellyFraction = inputKellyFraction;
    unitSize = (Bankroll::getInitialBalance() * kellyFraction * evPerTC) / avgVolatility;
    if (unitSize < 1.0f) unitSize = 1.0f;
    return;
}

void R14Strategy::updateCount(Card card) {
    int score = card.getValue();

    switch (score)
    {
    case 2:
        running_count += 2;
        break;
    case 3:
        running_count += 2;
        break;
    case 4:
        running_count += 3;
        break;
    case 5:
        running_count += 4;
        break;
    case 6: 
        running_count += 2;
        break;
    case 7:
        running_count += 1;
        break;
    case 8:
        running_count += 0;
        break;
    case 9:         
        running_count -= 2;
        break;
    case 10:
        running_count -= 3;
        break;
    case 11: //Ace
        running_count += 0;
        break;
    
    default:
        break;
    }

    float raw = running_count / num_decks_left;
    true_count = raw;
    return;
}

void R14Strategy::updateDeckSize(int num_cards_left){
    num_decks_left = static_cast<float>(num_cards_left) / 52.0f;

    if (num_decks_left > 0) {
        float raw = running_count / num_decks_left;
        true_count = raw;
    }
    return;
}

float R14Strategy::getTrueCount() const{
    return true_count;
}

float R14Strategy::getRunningCount() const{
    return running_count;
}

float R14Strategy::getDecksLeft() const{
    return num_decks_left;
}

bool R14Strategy::shouldAcceptInsurance() const{
    const bool useSixDeck = initial_decks >= 5.5f;
    // 2-deck 65% pen: TC crossover = 8.0, 6-deck 80% pen: TC crossover = 9.0
    const float insuranceThreshold = useSixDeck ? 9.0f : 8.0f;
    if (true_count >= insuranceThreshold){
        return true;
    }
    return false;
}

Action R14Strategy::shouldDeviatefromHard(int playerTotal, Rank dealerUpcard, float trueCount){
    int dealerValue = BasicStrategy::getIndex(dealerUpcard) + INDEX_OFFSET;
    const bool useSixDeck = initial_decks >= 5.5f;

    switch (playerTotal) {
        case 16:
            // 2-deck 65% pen: 16v10 Stand TC >= 0.5, 6-deck 80% pen: TC >= 0.5
            if (dealerValue == 10 && trueCount >= 0.5f) {
                return Action::Stand;
            }
            break;
            
        case 15: 
            // 2-deck 65% pen: 15v10 Stand TC >= 6.5, 6-deck 80% pen: TC >= 8.5
            if (dealerValue == 10 && trueCount >= (useSixDeck ? 8.5f : 6.5f)) {
                return Action::Stand;
            }
            break;

        case 13:
            // 2-deck 65% pen: 13v2 Stand TC >= -1.0, 6-deck 80% pen: TC >= -2.0
            // 2-deck 65% pen: 13v3 Stand TC >= -4.0, 6-deck 80% pen: TC >= -5.0
            if (dealerValue == 2 && trueCount >= (useSixDeck ? -2.0f : -1.0f)) { 
                return Action::Stand;
            }
            if (dealerValue == 3 && trueCount >= (useSixDeck ? -5.0f : -4.0f)) { 
                return Action::Stand;
            }
            break;

        case 12:
            // 2-deck 65% pen: 12v3 Stand TC >= 5.0, 6-deck 80% pen: TC >= 4.0
            if (dealerValue == 3 && trueCount >= (useSixDeck ? 4.0f : 5.0f)) {
                return Action::Stand;
            }
            // 2-deck 65% pen: 12v2 Stand TC >= 9.5, 6-deck 80% pen: TC >= 8.5
            if (dealerValue == 2 && trueCount >= (useSixDeck ? 8.5f : 9.5f)) {
                return Action::Stand;
            }
            break;

        case 11:
            // 2-deck 65% pen: 11v11 Double TC >= 0.5, 6-deck 80% pen: TC >= 2.5
            if (dealerValue == 11 && trueCount >= (useSixDeck ? 2.5f : 0.5f)) {
                return Action::Double;
            }
            break;

        case 10:
            // 2-deck 65% pen: 10v10 Double TC >= 7.5, 6-deck 80% pen: TC >= 11.0
            if (dealerValue == 10 && trueCount >= (useSixDeck ? 11.0f : 7.5f)) {
                return Action::Double;
            }
            // 2-deck 65% pen: 10v11 Double TC >= 9.0, 6-deck 80% pen: TC >= 10.0
            if (dealerValue == 11 && trueCount >= (useSixDeck ? 10.0f : 9.0f)) {
                return Action::Double;
            }
            break;

        case 9:
            // 2-deck 65% pen: 9v2 Double TC >= 2.0, 6-deck 80% pen: TC >= 2.0
            if (dealerValue == 2 && trueCount >= 2.0f){
                return Action::Double;
            }
            // 2-deck 65% pen: 9v7 Double TC >= 8.5, 6-deck 80% pen: TC >= 9.0
            if (dealerValue == 7 && trueCount >= (useSixDeck ? 9.0f : 8.5f)) {
                return Action::Double;
            }
            break;

        default: return Action::Skip;
    }
    return Action::Skip;
}

Action R14Strategy::shouldDeviatefromSplit(Rank playerRank, Rank dealerUpcard, float trueCount){
    int dealerValue = BasicStrategy::getIndex(dealerUpcard) + INDEX_OFFSET;
    int playerValue = BasicStrategy::getIndex(playerRank) + INDEX_OFFSET;
    const bool useSixDeck = initial_decks >= 5.5f;
    switch (playerValue) {
        // 2-deck 65% pen: Split 10s v5 TC >= 11.5, 6-deck 80% pen: TC >= 12.5
        // 2-deck 65% pen: Split 10s v6 TC >= 10.0, 6-deck 80% pen: TC >= 10.5
        case 10:
            if (dealerValue == 5 && trueCount >= (useSixDeck ? 12.5f : 11.5f)) {
                return Action::Split;
            }
            if (dealerValue == 6 && trueCount >= (useSixDeck ? 10.5f : 10.0f)) {
                return Action::Split;
            }
            break;
        default: return Action::Skip; break;
    }
    return Action::Skip;
}

Action R14Strategy::shouldSurrender(int playerTotal, Rank dealerUpcard, float trueCount){
    int dealerValue = BasicStrategy::getIndex(dealerUpcard) + INDEX_OFFSET;
    const bool useSixDeck = initial_decks >= 5.5f;
    switch (playerTotal) {
        case 16:
            // 2-deck 65% pen: 16v9 Surrender TC >= -0.5, 6-deck 80% pen: TC >= -2.0
            if (dealerValue == 9 && trueCount >= (useSixDeck ? -2.0f : -0.5f)) {
                return Action::Surrender;
            }
            // 2-deck 65% pen: 16v10 Surrender TC >= -7.0, 6-deck 80% pen: TC >= -8.0
            if (dealerValue == 10 && trueCount >= (useSixDeck ? -8.0f : -7.0f)) {
                return Action::Surrender;
            }
            // 2-deck 65% pen: 16v11 Surrender TC >= -3.0, 6-deck 80% pen: TC >= -4.5
            if (dealerValue == 11 && trueCount >= (useSixDeck ? -4.5f : -3.0f)) {
                return Action::Surrender;
            }
            break;
        case 15:
            // 2-deck 65% pen: 15v9 Surrender TC >= 5.0, 6-deck 80% pen: TC >= 5.0
            if (dealerValue == 9 && trueCount >= 5.0f) {
                return Action::Surrender;
            }
            // 2-deck 65% pen: 15v10 Surrender TC >= -2.0, 6-deck 80% pen: TC >= -1.5
            if (dealerValue == 10 && trueCount >= (useSixDeck ? -1.5f : -2.0f)) {
                return Action::Surrender;
            }
            // 2-deck 65% pen: 15v11 Surrender TC >= 3.5, 6-deck 80% pen: TC >= 4.0
            if (dealerValue == 11 && trueCount >= (useSixDeck ? 4.0f : 3.5f)) {
                return Action::Surrender;
            }
            break;
        case 14:
            // 2-deck 65% pen: 14v10 Surrender TC >= 6.0, 6-deck 80% pen: TC >= 6.5
            if (dealerValue == 10 && trueCount >= (useSixDeck ? 6.5f : 6.0f)) {
                return Action::Surrender;
            }
            break;
        default: return Action::Skip; break;
    }
    return Action::Skip;
}

Action R14Strategy::getHardHandAction(int playerTotal, Rank dealerUpcard, float trueCount) {
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

Action R14Strategy::getSoftHandAction(int playerTotal, Rank dealerUpcard) {
    constexpr int lowerBound = 13;
    int dealerIdx = BasicStrategy::getIndex(dealerUpcard);
    int playerIdx = playerTotal - lowerBound;  // Soft 13 maps to index 0 Since chart starts at A,2
    
    Action action = BasicStrategy::softTotalTable[playerIdx][dealerIdx];

    return action;
}

Action R14Strategy::getSplitAction(Rank playerSplitRank, Rank dealerUpcard, float trueCount) {
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

void R14Strategy::reset(int deckSize){
    running_count = 0;
    true_count = 0;
    num_decks_left = deckSize;
    initial_decks = deckSize;
}

std::string R14Strategy::getName() {
    return "R14Strategy";
}