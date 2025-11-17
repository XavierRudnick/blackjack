#include "NoStrategy.h"
#include <cmath>

NoStrategy::NoStrategy(float deck_size){
    return;
}

int NoStrategy::getBetSize() const {
    return 5;
}

void NoStrategy::updateCount(Card card) {
    return;
}

void NoStrategy::updateDeckSize(int num_cards_left){
    return;
}

float NoStrategy::getCount(){
    return 0;
}

bool NoStrategy::shouldAcceptInsurance(){
    return false;
}
