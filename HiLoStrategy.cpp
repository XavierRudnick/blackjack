#include "HiLoStrategy.h"
#include <cmath>

HiLoStrategy::HiLoStrategy(float deck_size){
    num_decks_left = deck_size;
}

int HiLoStrategy::getBetSize() const {
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
    /*
    if (true_count < 1){
        return 5;
    }
    else if (true_count <= 1.5) {
        return 10;
    } 
    else if (true_count < 3.0) { 
        return 30;
    } 
    else if (true_count < 4.0) {
        return 50;
    } 
    else if (true_count < 5.0) {
        return 100;
    } 
    else if (true_count < 6.0) {
        return 160;
    } 
    else if (true_count < 7.0) {
        return 200;
    } 
    else {
        return 200;
    }

    if (true_count < 1){
        return 10;
    }
    else if (true_count <= 1.5) {
        return 20;
    } 
    else if (true_count < 3.0) { 
        return 40;
    } 
    else if (true_count < 4.0) {
        return 60;
    } 
    else if (true_count < 5.0) {
        return 120;
    } 
    else if (true_count < 6.0) {
        return 180;
    } 
    else if (true_count < 7.0) {
        return 240;
    } 
    else {
        return 240;
    }
    */
}

void HiLoStrategy::updateCount(Card card) {
    Rank rank = card.getRank();
    int score = static_cast<int>(rank) + 2;//add 2 to account for enum starting at 0
    //int running_count = 0;

    if (score <= 6){
        running_count += 1;
    }
    else if (score >= 10){
        running_count -= 1;
    }
    //std::cout << "running count: " << running_count << std::endl;
    //std::cout << "decks left: " << num_decks_left << std::endl;
    true_count = running_count / num_decks_left; 
    return;
}

void HiLoStrategy::updateDeckSize(int num_cards_left){
    float decks_left_unrounded = num_cards_left / 52.0; 
    decks_left_unrounded *= 2.0;
    float decks_left_rounded = round(decks_left_unrounded);
    decks_left_rounded = decks_left_rounded / 2;
    num_decks_left = decks_left_rounded;
    //std::cout << "decks left: " << num_decks_left << std::endl;
    return;
}

float HiLoStrategy::getCount(){
    return true_count;
}

bool HiLoStrategy::shouldAcceptInsurance(){
    if (true_count >= 3){//mathmatical point where insurance is profitable accoding to gemini
        return true;
    }
    return false;
}
