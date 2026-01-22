#include "HiLoStrategy.h"
#include <cmath>

HiLoStrategy::HiLoStrategy(float deck_size){
    num_decks_left = deck_size;
}

int HiLoStrategy::getBetSize() {
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

    // //xav bet spread
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
    // 2-deck 65% pen H17 simulation: TC crossover = 2.5
    constexpr float insuranceThreshold = 2.5f;
    if (true_count >= insuranceThreshold){
        return true;
    }
    return false;
}

Action HiLoStrategy::shouldDeviatefromHard(int playerTotal, Rank dealerUpcard, float trueCount){
    int dealerValue = BasicStrategy::getIndex(dealerUpcard) + INDEX_OFFSET;

    switch (playerTotal) {
        case 16:
            // 2-deck 65% pen H17: 16v10 Stand TC >= 0.9
            if (dealerValue == 10 && trueCount >= 0.9f) {
                return Action::Stand;
            }
            break;
            
        case 15: 
            // 2-deck 65% pen H17: 15v10 Stand TC >= 3.2
            if (dealerValue == 10 && trueCount >= 3.2f) {
                return Action::Stand;
            }
            break;
            
        case 13:
            // 2-deck 65% pen H17: 13v2 Stand TC >= -0.2 (always stand in basic already)
            // 2-deck 65% pen H17: 13v3 Stand TC >= -1.6 (stand more often)
            if (dealerValue == 2 && trueCount >= -0.2f) { 
                return Action::Stand;
            }
            if (dealerValue == 3 && trueCount >= -1.6f) { 
                return Action::Stand;
            }
            break;

        case 12:
            // 2-deck 65% pen H17: 12v3 Stand TC >= 2.2
            if (dealerValue == 3 && trueCount >= 2.2f) {
                return Action::Stand;
            }
            // 2-deck 65% pen H17: 12v2 Stand TC >= 3.8
            if (dealerValue == 2 && trueCount >= 3.8f) {
                return Action::Stand;
            }
            break;
        case 11:
            // 2-deck 65% pen H17: 11v11 Double TC >= -0.1 (always double)
            if (dealerValue == 11 && trueCount >= -0.1f){
                return Action::Double;
            }
            break;
        case 10:
            // 2-deck 65% pen H17: 10v10 Double TC >= 2.8
            if (dealerValue == 10 && trueCount >= 2.8f){
                return Action::Double;
            }
            // 2-deck 65% pen H17: 10v11 Double TC >= 2.6
            if (dealerValue == 11 && trueCount >= 2.6f){
                return Action::Double;
            }
            break;
        case 9:
            // 2-deck 65% pen H17: 9v2 Double TC >= 0.7
            if (dealerValue == 2 && trueCount >= 0.7f){
                return Action::Double;
            }
            // 2-deck 65% pen H17: 9v7 Double TC >= 3.0
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
    switch (playerValue) {
        // 2-deck 65% pen H17: Split 10s v5 TC >= 4.5, Split 10s v6 TC >= 4.3
        case 10: 
            if (dealerValue == 5 && trueCount >= 4.5f) {
                return Action::Split;
            }
            if (dealerValue == 6 && trueCount >= 4.3f) {
                return Action::Split;
            }
            break;
        default: return Action::Skip; break;
    }
    return Action::Skip;
}

Action HiLoStrategy::shouldSurrender(int playerTotal, Rank dealerUpcard, float trueCount){
    int dealerValue = BasicStrategy::getIndex(dealerUpcard) + INDEX_OFFSET;
    switch (playerTotal) {
        case 16:
            // 2-deck 65% pen H17: 16v9 Surrender TC >= 0.4
            if (dealerValue == 9 && trueCount >= 0.4f) {
                return Action::Surrender;
            }
            // 2-deck 65% pen H17: 16v10 Surrender TC >= -2.5 (always surrender)
            if (dealerValue == 10 && trueCount >= -2.5f) {
                return Action::Surrender;
            }
            // 2-deck 65% pen H17: 16v11 Surrender TC >= -1.7 (always surrender)
            if (dealerValue == 11 && trueCount >= -1.7f) {
                return Action::Surrender;
            }
            break;
        case 15:
            // 2-deck 65% pen H17: 15v9 Surrender TC >= 2.2
            if (dealerValue == 9 && trueCount >= 2.2f) {
                return Action::Surrender;
            }
            // 2-deck 65% pen H17: 15v10 Surrender TC >= -0.5 (always surrender)
            if (dealerValue == 10 && trueCount >= -0.5f) {
                return Action::Surrender;
            }
            // 2-deck 65% pen H17: 15v11 Surrender TC >= 0.8
            if (dealerValue == 11 && trueCount >= 0.8f) {
                return Action::Surrender;
            }
            break;
        case 14:
            // 2-deck 65% pen H17: 14v10 Surrender TC >= 3.0
            if (dealerValue == 10 && trueCount >= 3.0f) {
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
    return;
}

std::string HiLoStrategy::getName() {
    return "HiLoStrategy";
}