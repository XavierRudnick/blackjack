#include "Hand.h"
#include <iostream>

Hand::Hand(std::pair<Card,Card> cards, int bet_size){
    hand.emplace_back(cards.first);//adds variable to end of vec
    hand.emplace_back(cards.second);
    bet_size_ = bet_size;
}

Hand::Hand(Card card, int bet_size){
    hand.emplace_back(card);
    bet_size_ = bet_size;
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
    return;
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
    int score = 0;
    int soft_aces = 0;

    for (const Card& val : hand){
        Rank rank = val.getRank();

        if (rank == Rank::Ace){
            score += 11;
            soft_aces += 1;
            
        }
        else if (rank == Rank::Jack || rank == Rank::Queen || rank == Rank::King){
            score += 10;
        }
        else{
            score += static_cast<int>(rank) + INDEX_OFFSET;
        }
    }

    while(score > 21 && soft_aces > 0){
        score -= 10;
        soft_aces -= 1;
    }

    return score;
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
    int score = 0;
    int soft_aces = 0;

   for (const Card& val : hand){
        Rank rank = val.getRank();

        if (rank == Rank::Ace){
            score += 11;
            soft_aces += 1;
            
        }
        else if (rank == Rank::Jack || rank == Rank::Queen || rank == Rank::King){
            score += 10;
        }
        else{
            score += static_cast<int>(rank) + INDEX_OFFSET;
        }
    }

    while (score > 21 && soft_aces > 0) {
        score -= 10;
        soft_aces -= 1;
    }

    return (score <= 21) && (soft_aces > 0);
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