#include "RPCStrategy.h"
#include "Bankroll.h"
#include <cmath>

RPCStrategy::RPCStrategy(float deck_size){
    num_decks_left = deck_size;
    initial_decks = deck_size;
}

int RPCStrategy::getEvenBet() const {
    return 1;
}

int RPCStrategy::getBetSize() {
    float effectiveTC = true_count - PROFITABLE_PLAY_TC_THRESHOLD;
    if (effectiveTC <= 0){
        return MIN_BET;
    }

    int bet = std::round((unitSize * effectiveTC) / (float)MIN_BET) * MIN_BET; // Round to nearest MIN_BET
    bet = std::max(MIN_BET, bet);
    return std::min(getMaxBet(), bet);
} 

void RPCStrategy::setUnitSize(float inputKellyFraction) {
    kellyFraction = inputKellyFraction;
    unitSize = (Bankroll::getInitialBalance() * kellyFraction * evPerTC) / avgVolatility;
    if (unitSize < 1.0f) unitSize = 1.0f;
    return;
}

void RPCStrategy::updateCount(Card card) {
    int score = card.getValue();

    switch (score)
    {
    case 2:
        running_count += 1;
        break;
    case 3:
        running_count += 2;
        break;
    case 4:
        running_count += 2;
        break;
    case 5:
        running_count += 2;
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
        running_count += 0;
        break;
    case 10:
        running_count -= 2;
        break;
    case 11: //Ace
        running_count -= 2;
        break;
    
    default:
        break;
    }

    float raw = running_count / num_decks_left;
    true_count = raw;
    return;
}

void RPCStrategy::updateDeckSize(int num_cards_left){
    num_decks_left = static_cast<float>(num_cards_left) / 52.0f;

    if (num_decks_left > 0) {
        float raw = running_count / num_decks_left;
        true_count = raw;
    }
    return;
}

float RPCStrategy::getTrueCount() const{
    return true_count;
}

float RPCStrategy::getRunningCount() const{
    return running_count;
}

float RPCStrategy::getDecksLeft() const{
    return num_decks_left;
}

bool RPCStrategy::shouldAcceptInsurance() const{
    const bool useSixDeck = initial_decks >= 5.5f;
    // 2-deck 65% pen: TC crossover = 4.5, 6-deck 80% pen: TC crossover = 6.0
    const float insuranceThreshold = useSixDeck ? 6.0f : 4.5f;
    if (true_count >= insuranceThreshold){
        return true;
    }
    return false;
}

Action RPCStrategy::shouldDeviatefromHard(int playerTotal, Rank dealerUpcard, float trueCount){
    int dealerValue = BasicStrategy::getIndex(dealerUpcard) + INDEX_OFFSET;
    const bool useSixDeck = initial_decks >= 5.5f;

    switch (playerTotal) {
        case 16:
            // 2-deck 65% pen: 16v10 Stand TC >= 1.5, 6-deck 80% pen: TC >= 0.5
            if (dealerValue == 10 && trueCount >= (useSixDeck ? 0.5f : 1.5f)) {
                return Action::Stand;
            }
            break;
            
        case 15: 
            // 2-deck 65% pen: 15v10 Stand TC >= 5.5, 6-deck 80% pen: TC >= 6.5
            if (dealerValue == 10 && trueCount >= (useSixDeck ? 6.5f : 5.5f)) {
                return Action::Stand;
            }
            break;

        case 13:
            // 2-deck 65% pen: 13v2 Stand TC >= -1.0, 6-deck 80% pen: TC >= -1.5
            // 2-deck 65% pen: 13v3 Stand TC >= -3.0, 6-deck 80% pen: TC >= -4.0
            if (dealerValue == 2 && trueCount >= (useSixDeck ? -1.5f : -1.0f)) { 
                return Action::Stand;
            }
            if (dealerValue == 3 && trueCount >= (useSixDeck ? -4.0f : -3.0f)) { 
                return Action::Stand;
            }
            break;

        case 12:
            // 2-deck 65% pen: 12v3 Stand TC >= 3.5, 6-deck 80% pen: TC >= 2.5
            if (dealerValue == 3 && trueCount >= (useSixDeck ? 2.5f : 3.5f)) {
                return Action::Stand;
            }
            // 2-deck 65% pen: 12v2 Stand TC >= 6.0, 6-deck 80% pen: TC >= 5.5
            if (dealerValue == 2 && trueCount >= (useSixDeck ? 5.5f : 6.0f)) {
                return Action::Stand;
            }
            break;

        case 11:
            // 2-deck 65% pen: 11v11 Double TC >= -0.5, 6-deck 80% pen: TC >= 1.5
            if (dealerValue == 11 && trueCount >= (useSixDeck ? 1.5f : -0.5f)){
                return Action::Double;
            }
            break;

        case 10:
            // 2-deck 65% pen: 10v10 Double TC >= 5.0, 6-deck 80% pen: TC >= 5.5
            if (dealerValue == 10 && trueCount >= (useSixDeck ? 5.5f : 5.0f)){
                return Action::Double;
            }
            // 2-deck 65% pen: 10v11 Double TC >= 4.5, 6-deck 80% pen: TC >= 6.0
            if (dealerValue == 11 && trueCount >= (useSixDeck ? 6.0f : 4.5f)){
                return Action::Double;
            }
            break;

        case 9:
            // 2-deck 65% pen: 9v2 Double TC >= 1.0, 6-deck 80% pen: TC >= 1.5
            if (dealerValue == 2 && trueCount >= (useSixDeck ? 1.5f : 1.0f)){
                return Action::Double;
            }
            // 2-deck 65% pen: 9v7 Double TC >= 6.0, 6-deck 80% pen: TC >= 6.5
            if (dealerValue == 7 && trueCount >= (useSixDeck ? 6.5f : 6.0f)){
                return Action::Double;
            }
            break;

        default: return Action::Skip;
    }
    return Action::Skip;
}

Action RPCStrategy::shouldDeviatefromSplit(Rank playerRank, Rank dealerUpcard, float trueCount){
    int dealerValue = BasicStrategy::getIndex(dealerUpcard) + INDEX_OFFSET;
    int playerValue = BasicStrategy::getIndex(playerRank) + INDEX_OFFSET;
    const bool useSixDeck = initial_decks >= 5.5f;
    switch (playerValue) {
        // 2-deck 65% pen: Split 10s v5 TC >= 8.5, 6-deck 80% pen: TC >= 8.5
        // 2-deck 65% pen: Split 10s v6 TC >= 8.5, 6-deck 80% pen: TC >= 8.5
        case 10: 
            if (dealerValue == 5 && trueCount >= (useSixDeck ? 8.5f : 8.5f)) {
                return Action::Split;
            }
            if (dealerValue == 6 && trueCount >= (useSixDeck ? 8.5f : 8.5f)) {
                return Action::Split;
            }
            break;
        default: return Action::Skip; break;
    }
    return Action::Skip;
}

Action RPCStrategy::shouldSurrender(int playerTotal, Rank dealerUpcard, float trueCount){
    int dealerValue = BasicStrategy::getIndex(dealerUpcard) + INDEX_OFFSET;
    const bool useSixDeck = initial_decks >= 5.5f;
    switch (playerTotal) {
        case 16:
            // 2-deck 65% pen: 16v9 Surrender TC >= 1.0, 6-deck 80% pen: TC >= -1.0
            if (dealerValue == 9 && trueCount >= (useSixDeck ? -1.0f : 1.0f)) {
                return Action::Surrender;
            }
            // 2-deck 65% pen: 16v10 Surrender TC >= -5.5, 6-deck 80% pen: TC >= -6.5
            if (dealerValue == 10 && trueCount >= (useSixDeck ? -6.5f : -5.5f)) {
                return Action::Surrender;
            }
            // 2-deck 65% pen: 16v11 Surrender TC >= -3.5, 6-deck 80% pen: TC >= -4.0
            if (dealerValue == 11 && trueCount >= (useSixDeck ? -4.0f : -3.5f)) {
                return Action::Surrender;
            }
            break;
        case 15:
            // 2-deck 65% pen: 15v9 Surrender TC >= 4.0, 6-deck 80% pen: TC >= 4.0
            if (dealerValue == 9 && trueCount >= 4.0f) {
                return Action::Surrender;
            }
            // 2-deck 65% pen: 15v10 Surrender TC >= -0.5, 6-deck 80% pen: TC >= -1.0
            if (dealerValue == 10 && trueCount >= (useSixDeck ? -1.0f : -0.5f)) {
                return Action::Surrender;
            }
            // 2-deck 65% pen: 15v11 Surrender TC >= 1.5, 6-deck 80% pen: TC >= 2.5
            if (dealerValue == 11 && trueCount >= (useSixDeck ? 2.5f : 1.5f)) {
                return Action::Surrender;
            }
            break;
        case 14:
            // 2-deck 65% pen: 14v10 Surrender TC >= 4.5, 6-deck 80% pen: TC >= 4.5
            if (dealerValue == 10 && trueCount >= 4.5f) {
                return Action::Surrender;
            }
            break;
        default: return Action::Skip; break;
    }
    return Action::Skip;
}

Action RPCStrategy::getHardHandAction(int playerTotal, Rank dealerUpcard, float trueCount) {
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

Action RPCStrategy::getSoftHandAction(int playerTotal, Rank dealerUpcard) {
    constexpr int lowerBound = 13;
    int dealerIdx = BasicStrategy::getIndex(dealerUpcard);
    int playerIdx = playerTotal - lowerBound;  // Soft 13 maps to index 0 Since chart starts at A,2
    
    Action action = BasicStrategy::softTotalTable[playerIdx][dealerIdx];

    return action;
}

Action RPCStrategy::getSplitAction(Rank playerSplitRank, Rank dealerUpcard, float trueCount) {
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

void RPCStrategy::reset(int deckSize){
    running_count = 0;
    true_count = 0;
    num_decks_left = deckSize;
    initial_decks = deckSize;
}

std::string RPCStrategy::getName() {
    return "RPCStrategy";
}