#include "KISSIIIStrategy.h"
#include <cmath>

KISSIIIStrategy::KISSIIIStrategy(float deck_size){
    num_decks_left = deck_size;
    true_count = deck_size * -2; //initial running count for KO system
    deckStartSize = deck_size;
}

int KISSIIIStrategy::getBetSize() {
    if (deckStartSize == 2){
        if (true_count < 0){
            return 25;
        }
        else if (true_count < 4) {
            return 100;
        } 
        else if (true_count < 8) { 
            return 300;
        } 
        else if (true_count < 12) {
            return 500;
        } 
        else if (true_count < 16) {
            return 1000;
        } 
        else if (true_count < 20) {
            return 1600;
        } 
        else if (true_count < 24) {
            return 2000;
        } 
        else {
            return 2000;
        }
    }
    else if (deckStartSize == 6){
         if (true_count < -4){
            return 25;
        }
        else if (true_count < 0) {
            return 100;
        } 
        else if (true_count < 4) { 
            return 300;
        } 
        else if (true_count < 8) {
            return 500;
        } 
        else if (true_count < 12) {
            return 1000;
        } 
        else if (true_count < 16) {
            return 1600;
        } 
        else if (true_count < 20) {
            return 2000;
        } 
        else {
            return 2000;
        }
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

void KISSIIIStrategy::updateCount(Card card) {
    Rank rank = card.getRank();
    Suit suit = card.getSuit();
    int score = static_cast<int>(rank) + INDEX_OFFSET;

    constexpr int lowerCard = 7;
    constexpr int upperCard = 10;

    if (rank == Rank::Two){ //red seven rule
        if (suit == Suit::Spades || suit == Suit::Clubs){
            true_count += 1;
        }     
    }
    else if (score <= lowerCard){
        true_count += 1;
    }
    else if (score >= upperCard){
        true_count -= 1;
    }
    
    return;
}

void KISSIIIStrategy::updateDeckSize(int num_cards_left){
    float decks_left_unrounded = num_cards_left / Deck::NUM_CARDS_IN_DECK; 
    num_decks_left = std::round(decks_left_unrounded * 2.0) / 2.0;//convert to only count float .5 segments
    return;
}

float KISSIIIStrategy::getTrueCount() const{
    return true_count;
}

float KISSIIIStrategy::getRunningCount() const{
    return true_count;
}

float KISSIIIStrategy::getDecksLeft() const{
    return num_decks_left;
}

bool KISSIIIStrategy::shouldAcceptInsurance() const{
    constexpr int insuranceThreshold = 5; //mathmatical point where insurance is profitable accoding to gemini
    if (true_count >= insuranceThreshold){
        return true;
    }
    return false;
}

Action KISSIIIStrategy::getHardHandAction(int playerTotal, Rank dealerUpcard, float trueCount) {
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

Action KISSIIIStrategy::getSoftHandAction(int playerTotal, Rank dealerUpcard) {
    constexpr int lowerBound = 13;
    int dealerIdx = BasicStrategy::getIndex(dealerUpcard);
    int playerIdx = playerTotal - lowerBound;  // Soft 13 maps to index 0 Since chart starts at A,2
    
    Action action = BasicStrategy::softTotalTable[playerIdx][dealerIdx];

    return action;
}

Action KISSIIIStrategy::getSplitAction(Rank playerSplitRank, Rank dealerUpcard, float trueCount) {
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


Action KISSIIIStrategy::shouldDeviatefromHard(int playerTotal, Rank dealerUpcard, float trueCount){
    int dealerValue = BasicStrategy::getIndex(dealerUpcard) + INDEX_OFFSET;

    switch (playerTotal) {
        case 16:
            if (dealerValue == 10 && trueCount >= 0) {
                return Action::Stand;
            }
            break;
        case 15:
            if (dealerValue == 10 && trueCount >= 4) {
                return Action::Stand;
            }
            break;
        case 12:
            if (dealerValue == 3 && trueCount >= 2) {
                return Action::Stand;
            }
            if (dealerValue == 2 && trueCount >= 3) {
                return Action::Stand;
            }
            break;
    }
    return Action::Skip;
}

Action KISSIIIStrategy::shouldDeviatefromSplit(Rank playerRank, Rank dealerUpcard, float trueCount){
    return Action::Skip;
}

Action KISSIIIStrategy::shouldSurrender(int playerTotal, Rank dealerUpcard, float trueCount){
    return Action::Skip;
}