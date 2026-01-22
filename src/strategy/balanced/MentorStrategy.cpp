#include "MentorStrategy.h"
#include <cmath>

MentorStrategy::MentorStrategy(float deck_size){
    num_decks_left = deck_size;
}

int MentorStrategy::getBetSize() {
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

void MentorStrategy::updateCount(Card card) {
    Rank rank = card.getRank();
    int score = static_cast<int>(rank) + INDEX_OFFSET;

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
        running_count -= -1;
        break;
    case 10:
        running_count -= 2;
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

void MentorStrategy::updateDeckSize(int num_cards_left){
    num_decks_left = static_cast<float>(num_cards_left) / 52.0f;
    return;
}

float MentorStrategy::getTrueCount() const{
    return true_count;
}

float MentorStrategy::getRunningCount() const{
    return running_count;
}

float MentorStrategy::getDecksLeft() const{
    return num_decks_left;
}

bool MentorStrategy::shouldAcceptInsurance() const{
    // 2-deck 65% pen H17 simulation: TC crossover = 47.5 (impractical, never take insurance)
    constexpr float insuranceThreshold = 47.5f;
    if (true_count >= insuranceThreshold){
        return true;
    }
    return false;
}

Action MentorStrategy::shouldDeviatefromHard(int playerTotal, Rank dealerUpcard, float trueCount){
    int dealerValue = BasicStrategy::getIndex(dealerUpcard) + INDEX_OFFSET;

    switch (playerTotal) {
        case 16:
            // 2-deck 65% pen H17: 16v10 Stand TC >= 47.3 (impractical - use basic strategy)
            break;
            
        case 15: 
            // 2-deck 65% pen H17: 15v10 Stand TC >= 47.5 (impractical - use basic strategy)
            break;

        case 13:
            // 2-deck 65% pen H17: 13v2 Stand TC >= 1.2
            // 2-deck 65% pen H17: 13v3 Stand TC >= 0.6
            if (dealerValue == 2 && trueCount >= 1.2f) { 
                return Action::Stand;
            }
            if (dealerValue == 3 && trueCount >= 0.6f) { 
                return Action::Stand;
            }
            break;

        case 12:
            // 2-deck 65% pen H17: 12v3 Stand TC >= 47.3 (impractical - use basic strategy)
            // 2-deck 65% pen H17: 12v2 Stand TC >= 48.8 (impractical - use basic strategy)
            break;

        case 11:
            // 2-deck 65% pen H17: 11v11 Double TC >= 18.4
            if (dealerValue == 11 && trueCount >= 18.4f){
                return Action::Double;
            }
            break;

        case 10:
            // 2-deck 65% pen H17: 10v10 Double TC >= 47.5 (impractical)
            // 2-deck 65% pen H17: 10v11 Double TC >= 47.5 (impractical)
            break;

        case 9:
            // 2-deck 65% pen H17: 9v2 Double TC >= 1.4
            if (dealerValue == 2 && trueCount >= 1.4f){
                return Action::Double;
            }
            // 2-deck 65% pen H17: 9v7 Double TC >= 47.5 (impractical)
            break;

        default: return Action::Skip;
    }
    return Action::Skip;
}

Action MentorStrategy::shouldDeviatefromSplit(Rank /*playerRank*/, Rank /*dealerUpcard*/, float /*trueCount*/){
    // 2-deck 65% pen H17: Split 10s v5 TC >= 49.8, Split 10s v6 TC >= 49.8 (impractical)
    // Not implementing - TCs too high to ever occur
    return Action::Skip;
}

Action MentorStrategy::shouldSurrender(int playerTotal, Rank dealerUpcard, float trueCount){
    int dealerValue = BasicStrategy::getIndex(dealerUpcard) + INDEX_OFFSET;
    switch (playerTotal) {
        case 16:
            // 2-deck 65% pen H17: 16v9 Surrender TC >= 35.4 (impractical)
            // 2-deck 65% pen H17: 16v10 Surrender TC >= -1.4 (always surrender)
            if (dealerValue == 10 && trueCount >= -1.4f) {
                return Action::Surrender;
            }
            // 2-deck 65% pen H17: 16v11 Surrender TC >= -0.2 (always surrender)
            if (dealerValue == 11 && trueCount >= -0.2f) {
                return Action::Surrender;
            }
            break;
        case 15:
            // 2-deck 65% pen H17: 15v9 Surrender TC >= 47.4 (impractical)
            // 2-deck 65% pen H17: 15v10 Surrender TC >= 18.3
            if (dealerValue == 10 && trueCount >= 18.3f) {
                return Action::Surrender;
            }
            // 2-deck 65% pen H17: 15v11 Surrender TC >= 39.0
            if (dealerValue == 11 && trueCount >= 39.0f) {
                return Action::Surrender;
            }
            break;
        case 14:
            // 2-deck 65% pen H17: 14v10 Surrender TC >= 47.5 (impractical)
            break;
        default: return Action::Skip; break;
    }
    return Action::Skip;
}

Action MentorStrategy::getHardHandAction(int playerTotal, Rank dealerUpcard, float trueCount) {
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

Action MentorStrategy::getSoftHandAction(int playerTotal, Rank dealerUpcard) {
    constexpr int lowerBound = 13;
    int dealerIdx = BasicStrategy::getIndex(dealerUpcard);
    int playerIdx = playerTotal - lowerBound;  // Soft 13 maps to index 0 Since chart starts at A,2
    
    Action action = BasicStrategy::softTotalTable[playerIdx][dealerIdx];

    return action;
}

Action MentorStrategy::getSplitAction(Rank playerSplitRank, Rank dealerUpcard, float trueCount) {
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

void MentorStrategy::reset(int deckSize){
    running_count = 0;
    true_count = 0;
    num_decks_left = deckSize;
}

std::string MentorStrategy::getName() {
    return "MentorStrategy";
}