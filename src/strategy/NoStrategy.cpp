#include "NoStrategy.h"
#include <cmath>

NoStrategy::NoStrategy(float deck_size){
    num_decks_left = deck_size;
    return;
}

int NoStrategy::getBetSize() {
    constexpr int defaultBetSize = 5;
    return defaultBetSize;
}

void NoStrategy::updateCount(Card card) {
    return;
}

void NoStrategy::updateDeckSize(int num_cards_left){
    return;
}

float NoStrategy::getTrueCount() const{
    return 0;
}

float NoStrategy::getDecksLeft() const{
    return num_decks_left;
}

float NoStrategy::getRunningCount() const{
    return 0;
}

bool NoStrategy::shouldAcceptInsurance() const{
    return false;
}

Action NoStrategy::getHardHandAction(int playerTotal, Rank dealerUpcard, float trueCount) {
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

    return BasicStrategy::hardTotalTable[playerIdx][dealerIdx];
    
    
}

Action NoStrategy::getSoftHandAction(int playerTotal, Rank dealerUpcard) {
    constexpr int lowerBound = 13;
    int dealerIdx = BasicStrategy::getIndex(dealerUpcard);
    int playerIdx = playerTotal - lowerBound;  // Soft 13 maps to index 0 Since chart starts at A,2
    
    Action action = BasicStrategy::softTotalTable[playerIdx][dealerIdx];

    return action;
}

Action NoStrategy::getSplitAction(Rank playerSplitRank, Rank dealerUpcard, float trueCount) {
    int dealerIdx = BasicStrategy::getIndex(dealerUpcard);
    int pairIdx = BasicStrategy::getIndex(playerSplitRank);

    return BasicStrategy::splitTable[pairIdx][dealerIdx];
    
}