#include "WongHalvesStrategy.h"
#include <cmath>

WongHalvesStrategy::WongHalvesStrategy(float deck_size){
    num_decks_left = deck_size;
}

int WongHalvesStrategy::getBetSize() {
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

void WongHalvesStrategy::updateCount(Card card) {
    Rank rank = card.getRank();
    int score = static_cast<int>(rank) + INDEX_OFFSET;

    switch (score)
    {
    case 2:
        running_count += 0.5;
        break;
    case 3:
        running_count += 1;
        break;
    case 4:
        running_count += 1;
        break;
    case 5:
        running_count += 1.5;
        break;
    case 6: 
        running_count += 1;
        break;
    case 7:
        running_count += .5;
        break;
    case 8:
        running_count += 0;
        break;
    case 9:         
        running_count -= .5;
        break;
    case 10:
        running_count -= 1;
        break;
    case 11: //Ace
        running_count -= 1;
        break;
    
    default:
        break;
    }

    float raw = running_count / num_decks_left;
    true_count = raw;
    return;
}

void WongHalvesStrategy::updateDeckSize(int num_cards_left){
    num_decks_left = static_cast<float>(num_cards_left) / 52.0f;

    if (num_decks_left > 0) {
        float raw = running_count / num_decks_left;
        true_count = raw;
    }
    return;
}

float WongHalvesStrategy::getTrueCount() const{
    return true_count;
}

float WongHalvesStrategy::getRunningCount() const{
    return running_count;
}

float WongHalvesStrategy::getDecksLeft() const{
    return num_decks_left;
}

bool WongHalvesStrategy::shouldAcceptInsurance() const{
    // 2-deck 65% pen H17 simulation: TC crossover = 21.3 (very high, rarely triggered)
    constexpr float insuranceThreshold = 21.3f;
    if (true_count >= insuranceThreshold){
        return true;
    }
    return false;
}

Action WongHalvesStrategy::shouldDeviatefromHard(int playerTotal, Rank dealerUpcard, float trueCount){
    int dealerValue = BasicStrategy::getIndex(dealerUpcard) + INDEX_OFFSET;

    switch (playerTotal) {
        case 16:
            // 2-deck 65% pen H17: 16v10 Stand TC >= 13.0 (high for Wong Halves)
            if (dealerValue == 10 && trueCount >= 13.0f) {
                return Action::Stand;
            }
            break;
            
        case 15: 
            // 2-deck 65% pen H17: 15v10 Stand TC >= 21.1 (rarely triggered)
            if (dealerValue == 10 && trueCount >= 21.1f) {
                return Action::Stand;
            }
            break;

        case 13:
            // 2-deck 65% pen H17: 13v2 Stand TC >= 0.8
            // 2-deck 65% pen H17: 13v3 Stand TC >= 0.3
            if (dealerValue == 2 && trueCount >= 0.8f) { 
                return Action::Stand;
            }
            if (dealerValue == 3 && trueCount >= 0.3f) { 
                return Action::Stand;
            }
            break;

        case 12:
            // 2-deck 65% pen H17: 12v3 Stand TC >= 19.5
            if (dealerValue == 3 && trueCount >= 19.5f) {
                return Action::Stand;
            }
            // 2-deck 65% pen H17: 12v2 Stand TC >= 21.5
            if (dealerValue == 2 && trueCount >= 21.5f) {
                return Action::Stand;
            }
            break;

        case 11:
            // 2-deck 65% pen H17: 11v11 Double TC >= 11.0
            if (dealerValue == 11 && trueCount >= 11.0f){
                return Action::Double;
            }
            break;

        case 10:
            // 2-deck 65% pen H17: 10v10 Double TC >= 21.3
            if (dealerValue == 10 && trueCount >= 21.3f){
                return Action::Double;
            }
            // 2-deck 65% pen H17: 10v11 Double TC >= 21.1
            if (dealerValue == 11 && trueCount >= 21.1f){
                return Action::Double;
            }
            break;

        case 9:
            // 2-deck 65% pen H17: 9v2 Double TC >= 3.9
            if (dealerValue == 2 && trueCount >= 3.9f){
                return Action::Double;
            }
            // 2-deck 65% pen H17: 9v7 Double TC >= 19.9
            if (dealerValue == 7 && trueCount >= 19.9f){
                return Action::Double;
            }
            break;

        default: return Action::Skip;
    }
    return Action::Skip;
}

Action WongHalvesStrategy::shouldDeviatefromSplit(Rank playerRank, Rank dealerUpcard, float trueCount){
    int dealerValue = BasicStrategy::getIndex(dealerUpcard) + INDEX_OFFSET;
    int playerValue = BasicStrategy::getIndex(playerRank) + INDEX_OFFSET;
    switch (playerValue) {
        // 2-deck 65% pen H17: Split 10s v5 TC >= 23.4, Split 10s v6 TC >= 23.2
        case 10: 
            if (dealerValue == 5 && trueCount >= 23.4f) {
                return Action::Split;
            }
            if (dealerValue == 6 && trueCount >= 23.2f) {
                return Action::Split;
            }
            break;
        default: return Action::Skip; break;
    }
    return Action::Skip;
}

Action WongHalvesStrategy::shouldSurrender(int playerTotal, Rank dealerUpcard, float trueCount){
    int dealerValue = BasicStrategy::getIndex(dealerUpcard) + INDEX_OFFSET;
    switch (playerTotal) {
        case 16:
            // 2-deck 65% pen H17: 16v9 Surrender TC >= 12.7
            if (dealerValue == 9 && trueCount >= 12.7f) {
                return Action::Surrender;
            }
            // 2-deck 65% pen H17: 16v10 Surrender TC >= -0.8 (always surrender)
            if (dealerValue == 10 && trueCount >= -0.8f) {
                return Action::Surrender;
            }
            // 2-deck 65% pen H17: 16v11 Surrender TC >= -0.1 (always surrender)
            if (dealerValue == 11 && trueCount >= -0.1f) {
                return Action::Surrender;
            }
            break;
        case 15:
            // 2-deck 65% pen H17: 15v9 Surrender TC >= 19.3
            if (dealerValue == 9 && trueCount >= 19.3f) {
                return Action::Surrender;
            }
            // 2-deck 65% pen H17: 15v10 Surrender TC >= 11.2
            if (dealerValue == 10 && trueCount >= 11.2f) {
                return Action::Surrender;
            }
            // 2-deck 65% pen H17: 15v11 Surrender TC >= 12.9
            if (dealerValue == 11 && trueCount >= 12.9f) {
                return Action::Surrender;
            }
            break;
        case 14:
            // 2-deck 65% pen H17: 14v10 Surrender TC >= 20.0
            if (dealerValue == 10 && trueCount >= 20.0f) {
                return Action::Surrender;
            }
            break;
        default: return Action::Skip; break;
    }
    return Action::Skip;
}

Action WongHalvesStrategy::getHardHandAction(int playerTotal, Rank dealerUpcard, float trueCount) {
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

Action WongHalvesStrategy::getSoftHandAction(int playerTotal, Rank dealerUpcard) {
    constexpr int lowerBound = 13;
    int dealerIdx = BasicStrategy::getIndex(dealerUpcard);
    int playerIdx = playerTotal - lowerBound;  // Soft 13 maps to index 0 Since chart starts at A,2
    
    Action action = BasicStrategy::softTotalTable[playerIdx][dealerIdx];

    return action;
}

Action WongHalvesStrategy::getSplitAction(Rank playerSplitRank, Rank dealerUpcard, float trueCount) {
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

void WongHalvesStrategy::reset(int deckSize){
    running_count = 0;
    true_count = 0;
    num_decks_left = deckSize;
}

std::string WongHalvesStrategy::getName() {
    return "WongHalvesStrategy";
}