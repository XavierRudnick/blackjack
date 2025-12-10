#include "KoStrategy.h"
#include <cmath>

KoStrategy::KoStrategy(float deck_size){
    num_decks_left = deck_size;
    true_count = deck_size * -4; //initial running count for KO system
}

int KoStrategy::getBetSize() {
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

void KoStrategy::updateCount(Card card) {
    Rank rank = card.getRank();
    int score = static_cast<int>(rank) + INDEX_OFFSET;

    constexpr int lowerCard = 7;
    constexpr int upperCard = 10;

    if (score <= lowerCard){
        true_count += 1;
    }
    else if (score >= upperCard){
        true_count -= 1;
    }
    
    return;
}

void KoStrategy::updateDeckSize(int num_cards_left){
    float decks_left_unrounded = num_cards_left / Deck::NUM_CARDS_IN_DECK; 
    num_decks_left = std::round(decks_left_unrounded * 2.0) / 2.0;//convert to only count float .5 segments
    return;
}

float KoStrategy::getTrueCount() const{
    return true_count;
}

float KoStrategy::getRunningCount() const{
    return true_count;
}

float KoStrategy::getDecksLeft() const{
    return num_decks_left;
}

bool KoStrategy::shouldAcceptInsurance() const{
    constexpr int insuranceThreshold = 5; //mathmatical point where insurance is profitable accoding to gemini
    if (true_count >= insuranceThreshold){
        return true;
    }
    return false;
}

