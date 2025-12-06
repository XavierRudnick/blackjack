#include "NoStrategy.h"
#include <cmath>

NoStrategy::NoStrategy(float deck_size){
    num_decks_left = deck_size;
    return;
}

int NoStrategy::getBetSize() {
    return 5;
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
