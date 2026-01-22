#include "OmegaIIStrategy.h"
#include <cmath>

OmegaIIStrategy::OmegaIIStrategy(float deck_size){
    num_decks_left = deck_size;
}

int OmegaIIStrategy::getBetSize() {
    if (true_count < 1){
        return 25;
    }
    else if (true_count <= 1.5) {
        return 100;
    } 
    else if (true_count < 3.0) { 
        return 300;
    } 
    else if (true_count < 4.0) {
        return 500;
    } 
    else if (true_count < 5.0) {
        return 1000;
    } 
    else if (true_count < 6.0) {
        return 1600;
    } 
    else if (true_count < 7.0) {
        return 2000;
    } 
    else {
        return 2000;
    }

    //xav bet spread
    // if (true_count < 1){
    //     return 5;
    // }
    // else if (true_count <= 1.5) {
    //     return 20;
    // } 
    // else if (true_count < 3.0) { 
    //     return 30;
    // } 
    // else if (true_count < 4.0) {
    //     return 50;
    // } 
    // else if (true_count < 5.0) {
    //     return 100;
    // } 
    // else if (true_count < 6.0) {
    //     return 160;
    // } 
    // else if (true_count < 7.0) {
    //     return 200;
    // } 
    // else {
    //     return 200;
    // }

}

void OmegaIIStrategy::updateCount(Card card) {
    Rank rank = card.getRank();
    int score = static_cast<int>(rank) + INDEX_OFFSET;

    switch (score)
    {
    case 2:
        running_count += 1;
        break;
    case 3:
        running_count += 1;
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
        running_count -= 1;
        break;
    case 10:
        running_count -= 2;
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

void OmegaIIStrategy::updateDeckSize(int num_cards_left){
    num_decks_left = static_cast<float>(num_cards_left) / 52.0f;

        // Recompute true count whenever deck size changes so future bets/decisions use latest shoe depth
    if (num_decks_left > 0) {
        float raw = running_count / num_decks_left;
        true_count = raw;
    }

    return;
}

float OmegaIIStrategy::getTrueCount() const{
    return true_count;
}

float OmegaIIStrategy::getRunningCount() const{
    return running_count;
}

float OmegaIIStrategy::getDecksLeft() const{
    return num_decks_left;
}

bool OmegaIIStrategy::shouldAcceptInsurance() const{
    // 2-deck 65% pen H17 simulation: TC crossover = 42.9 (impractical, never take insurance)
    constexpr float insuranceThreshold = 42.9f;
    if (true_count >= insuranceThreshold){
        return true;
    }
    return false;
}

Action OmegaIIStrategy::shouldDeviatefromHard(int playerTotal, Rank dealerUpcard, float trueCount){
    int dealerValue = BasicStrategy::getIndex(dealerUpcard) + INDEX_OFFSET;

    switch (playerTotal) {
        case 16:
            // 2-deck 65% pen H17: 16v10 Stand TC >= 35.9 (impractical)
            break;
            
        case 15: 
            // 2-deck 65% pen H17: 15v10 Stand TC >= 40.4 (impractical)
            break;

        case 13:
            // 2-deck 65% pen H17: 13v2 Stand TC >= 1.0
            // 2-deck 65% pen H17: 13v3 Stand TC >= -0.2 (always stand)
            if (dealerValue == 2 && trueCount >= 1.0f) { 
                return Action::Stand;
            }
            if (dealerValue == 3 && trueCount >= -0.2f) { 
                return Action::Stand;
            }
            break;

        case 12:
            // 2-deck 65% pen H17: 12v3 Stand TC >= 38.8 (impractical)
            // 2-deck 65% pen H17: 12v2 Stand TC >= 44.3 (impractical)
            break;

        case 11:
            // 2-deck 65% pen H17: 11v11 Double TC >= 16.8
            if (dealerValue == 11 && trueCount >= 16.8f){
                return Action::Double;
            }
            break;

        case 10:
            // 2-deck 65% pen H17: 10v10 Double TC >= 42.8 (impractical)
            // 2-deck 65% pen H17: 10v11 Double TC >= 40.3 (impractical)
            break;

        case 9:
            // 2-deck 65% pen H17: 9v2 Double TC >= 1.5
            if (dealerValue == 2 && trueCount >= 1.5f){
                return Action::Double;
            }
            // 2-deck 65% pen H17: 9v7 Double TC >= 39.9 (impractical)
            break;

        default: return Action::Skip;
    }
    return Action::Skip;
}

Action OmegaIIStrategy::shouldDeviatefromSplit(Rank /*playerRank*/, Rank /*dealerUpcard*/, float /*trueCount*/){
    // 2-deck 65% pen H17: Split 10s v5 TC >= 44.4, Split 10s v6 TC >= 44.4 (impractical)
    // Not implementing - TCs too high to ever occur
    return Action::Skip;
}

Action OmegaIIStrategy::shouldSurrender(int playerTotal, Rank dealerUpcard, float trueCount){
    int dealerValue = BasicStrategy::getIndex(dealerUpcard) + INDEX_OFFSET;
    switch (playerTotal) {
        case 16:
            // 2-deck 65% pen H17: 16v9 Surrender TC >= 27.3 (impractical)
            // 2-deck 65% pen H17: 16v10 Surrender TC >= -0.5 (always surrender)
            if (dealerValue == 10 && trueCount >= -0.5f) {
                return Action::Surrender;
            }
            // 2-deck 65% pen H17: 16v11 Surrender TC >= -0.1 (always surrender)
            if (dealerValue == 11 && trueCount >= -0.1f) {
                return Action::Surrender;
            }
            break;
        case 15:
            // 2-deck 65% pen H17: 15v9 Surrender TC >= 38.8 (impractical)
            // 2-deck 65% pen H17: 15v10 Surrender TC >= 16.8
            if (dealerValue == 10 && trueCount >= 16.8f) {
                return Action::Surrender;
            }
            // 2-deck 65% pen H17: 15v11 Surrender TC >= 34.4 (impractical)
            break;
        case 14:
            // 2-deck 65% pen H17: 14v10 Surrender TC >= 39.9 (impractical)
            break;
        default: return Action::Skip; break;
    }
    return Action::Skip;
}

Action OmegaIIStrategy::getHardHandAction(int playerTotal, Rank dealerUpcard, float trueCount) {
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

Action OmegaIIStrategy::getSoftHandAction(int playerTotal, Rank dealerUpcard) {
    constexpr int lowerBound = 13;
    int dealerIdx = BasicStrategy::getIndex(dealerUpcard);
    int playerIdx = playerTotal - lowerBound;  // Soft 13 maps to index 0 Since chart starts at A,2
    
    Action action = BasicStrategy::softTotalTable[playerIdx][dealerIdx];

    return action;
}

Action OmegaIIStrategy::getSplitAction(Rank playerSplitRank, Rank dealerUpcard, float trueCount) {
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

void OmegaIIStrategy::reset(int deckSize){
    running_count = 0;
    true_count = 0;
    num_decks_left = deckSize;
}

std::string OmegaIIStrategy::getName() {
    return "OmegaIIStrategy";
}