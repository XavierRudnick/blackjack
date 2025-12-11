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

void HiLoStrategy::updateCount(Card card) {
    Rank rank = card.getRank();
    int score = static_cast<int>(rank) + INDEX_OFFSET;

    if (score <= 6){
        running_count += 1;
    }
    else if (score >= 10){
        running_count -= 1;
    }

    float raw = running_count / num_decks_left; 
    true_count = std::round(raw * 2.0) / 2.0;//convert to only count int .5 segments
    return;
}

void HiLoStrategy::updateDeckSize(int num_cards_left){
    float decks_left_unrounded = num_cards_left / 52.0; 
    num_decks_left = std::round(decks_left_unrounded * 2.0) / 2.0;//convert to only count float .5 segments

    // Recompute true count whenever deck size changes so future bets/decisions use latest shoe depth
    if (num_decks_left > 0) {
        float raw = running_count / num_decks_left;
        true_count = std::round(raw * 2.0) / 2.0; // keep 0.5 increments
    }
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
    if (true_count >= 3){//mathmatical point where insurance is profitable accoding to gemini
        return true;
    }
    return false;
}
