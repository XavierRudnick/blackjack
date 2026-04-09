#include "Hand.h"
#include <iostream>

void Hand::tallyRank(Rank r) {
    if (r == Rank::Ace) {
        runningScore_ += 11;
        softAces_++;
    } else if (r == Rank::Jack || r == Rank::Queen || r == Rank::King) {
        runningScore_ += 10;
    } else {
        runningScore_ += static_cast<int>(r) + INDEX_OFFSET;
    }
    while (runningScore_ > 21 && softAces_ > 0) {
        runningScore_ -= 10;
        softAces_--;
    }
}

void Hand::recomputeFromHand() {
    runningScore_ = 0;
    softAces_ = 0;
    for (const Card& c : hand) {
        tallyRank(c.getRank());
    }
}

Hand::Hand(std::pair<Card,Card> cards, int bet_size){
    hand.emplace_back(cards.first);
    hand.emplace_back(cards.second);
    bet_size_ = bet_size;
    recomputeFromHand();
}

Hand::Hand(Card card, int bet_size){
    hand.emplace_back(card);
    bet_size_ = bet_size;
    recomputeFromHand();
}

int Hand::getBetSize(){
    return bet_size_;
}

void Hand::doubleBet(){
    bet_size_ *= 2;
}

Card Hand::getLastCard(){
    return hand.back();
}

void Hand::popLastCard(){
    hand.pop_back();
    recomputeFromHand();
}

Rank Hand::peekFrontCard(){
    return hand.front().getRank();
}

bool Hand::OfferInsurance(){
    return hand.front().isAce();
}

bool Hand::dealerHiddenTen(){
    return hand.back().isWorthTen();
}

bool Hand::dealerShowsTen(){
    return hand.front().isWorthTen();
}

bool Hand::dealerHiddenAce(){
    return hand.back().isAce();
}

void Hand::addCard(Card card){
    hand.emplace_back(card);
    tallyRank(card.getRank());
}

bool Hand::checkOver(){
    if (getScore() > 21){
        return true;
    }
    return false;
}

bool Hand::isSoft17() {
    int score = getScore();
    if (score == 17 && isHandSoft()) {
        return true;
    }
    return false;
}

bool Hand::isDealerOver(){
    return getScore() >= 17;
}

int Hand::getScore(){
    return runningScore_;
}

int Hand::getFinalScore(){
    int score = getScore();

    if (score > 21){
        return 0;
    }
    else{
        return score;
    } 
}

bool Hand::isHandSoft() {
    return softAces_ > 0 && runningScore_ <= 21;
}

bool Hand::checkCanSplit(){
    if (hand.size() == 2 && hand.front().getRank() == hand.back().getRank()){
        return true;
    }
    return false;
} 

bool Hand::checkCanDouble(){
    if (hand.size() == 2){
        return true;
    }
    return false;
} 

bool Hand::checkShouldStand(){
    if (getScore() == 18 || getScore() == 19){
        return true;
    }
    return false;
}

const std::vector<Card>& Hand::getCards() const{
    return hand;
}

bool Hand::isBlackjack(){
    if (hand.size() == 2){
        bool frontTen = hand.front().isWorthTen();
        bool backTen = hand.back().isWorthTen();
        bool frontAce = hand.front().isAce();
        bool backAce = hand.back().isAce();

        if (frontTen && backAce){
            return true;
        }
        else if (frontAce && backTen){
            return true;
        }
    }
    return false;
}

bool Hand::isAces(){
    if (hand.size() == 2){
        bool frontAce = hand.front().isAce();
        bool backAce = hand.back().isAce();

        if (frontAce && backAce){
            return true;
        }
    }
    return false;
}
