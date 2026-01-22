#include "R14Strategy.h"
#include <cmath>

R14Strategy::R14Strategy(float deck_size){
    num_decks_left = deck_size;
}

int R14Strategy::getBetSize() {
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

void R14Strategy::updateCount(Card card) {
    Rank rank = card.getRank();
    int score = static_cast<int>(rank) + INDEX_OFFSET;

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
    // 2-deck 65% pen H17 simulation: TC crossover = 53.9 (impractical, never take insurance)
    constexpr float insuranceThreshold = 53.9f;
    if (true_count >= insuranceThreshold){
        return true;
    }
    return false;
}

Action R14Strategy::shouldDeviatefromHard(int playerTotal, Rank dealerUpcard, float trueCount){
    int dealerValue = BasicStrategy::getIndex(dealerUpcard) + INDEX_OFFSET;

    switch (playerTotal) {
        case 16:
            // 2-deck 65% pen H17: 16v10 Stand TC >= -10.3 (always stand) - NOTE: negative means always stand in basic
            // This is unusual - the simulation says stand is worse until very negative TC
            // For practical purposes, follow basic strategy (hit) and don't stand based on count
            break;
            
        case 15: 
            // 2-deck 65% pen H17: 15v10 Stand TC >= 49.9 (impractical)
            break;

        case 13:
            // 2-deck 65% pen H17: 13v2 Stand TC >= 1.5
            // 2-deck 65% pen H17: 13v3 Stand TC >= 0.2
            if (dealerValue == 2 && trueCount >= 1.5f) { 
                return Action::Stand;
            }
            if (dealerValue == 3 && trueCount >= 0.2f) { 
                return Action::Stand;
            }
            break;

        case 12:
            // 2-deck 65% pen H17: 12v3 Stand TC >= 48.9 (impractical)
            // 2-deck 65% pen H17: 12v2 Stand TC >= 54.0 (impractical)
            break;

        case 11:
            // 2-deck 65% pen H17: 11v11 Double TC >= 25.3 (impractical)
            break;

        case 10:
            // 2-deck 65% pen H17: 10v10 Double TC >= 50.0 (impractical)
            // 2-deck 65% pen H17: 10v11 Double TC >= 50.0 (impractical)
            break;

        case 9:
            // 2-deck 65% pen H17: 9v2 Double TC >= 2.4
            if (dealerValue == 2 && trueCount >= 2.4f){
                return Action::Double;
            }
            // 2-deck 65% pen H17: 9v7 Double TC >= 48.9 (impractical)
            break;

        default: return Action::Skip;
    }
    return Action::Skip;
}

Action R14Strategy::shouldDeviatefromSplit(Rank /*playerRank*/, Rank /*dealerUpcard*/, float /*trueCount*/){
    // 2-deck 65% pen H17: Split 10s v5 TC >= 54.0, Split 10s v6 TC >= 54.0 (impractical)
    // Not implementing - TCs too high to ever occur
    return Action::Skip;
}

Action R14Strategy::shouldSurrender(int playerTotal, Rank dealerUpcard, float trueCount){
    int dealerValue = BasicStrategy::getIndex(dealerUpcard) + INDEX_OFFSET;
    switch (playerTotal) {
        case 16:
            // 2-deck 65% pen H17: 16v9 Surrender TC >= 38.7 (impractical)
            // 2-deck 65% pen H17: 16v10 Surrender TC >= -1.0 (always surrender)
            if (dealerValue == 10 && trueCount >= -1.0f) {
                return Action::Surrender;
            }
            // 2-deck 65% pen H17: 16v11 Surrender TC >= -0.3 (always surrender)
            if (dealerValue == 11 && trueCount >= -0.3f) {
                return Action::Surrender;
            }
            break;
        case 15:
            // 2-deck 65% pen H17: 15v9 Surrender TC >= 48.8 (impractical)
            // 2-deck 65% pen H17: 15v10 Surrender TC >= 25.4 (impractical)
            // 2-deck 65% pen H17: 15v11 Surrender TC >= 38.9 (impractical)
            break;
        case 14:
            // 2-deck 65% pen H17: 14v10 Surrender TC >= 49.0 (impractical)
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
}

std::string R14Strategy::getName() {
    return "R14Strategy";
}