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

void Hand::show_cards(){
    for (Card val : hand){
        //std::cout << val.getRank() << " " << val.getSuit() << std::endl;
    }
    //std::cout << "value: " << getScoreHard() << std::endl;
    if (isSoftHand()){
       // std::cout << "soft value: " << getScoreSoft() << std::endl;
    }
}

void Hand::dealer_show_cards(){
   // std::cout << "Dealer card" << std::endl;
    for (Card val : hand){
      //  std::cout << val.getRank() << " " << val.getSuit() << std::endl;
    }
   // std::cout << "value: " << getScoreHard() << std::endl;
    //std::cout << std::endl;
}

void Hand::peek_dealer(){
   // std::cout << hand.front().getRank() << " " << hand.front().getSuit() << std::endl;
}

Rank Hand::peek_front_card(){
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

bool Hand::check_over(){
    if (getScoreHard() > 21 && getScoreSoft() > 21){
        return true;
    }
    return false;
}

int Hand::getScoreHard(){
    int score = 0;
    bool ace_appeared = false;
    for (Card val : hand){

        Rank rank = val.getRank();
        if (rank == Rank::Ace){
            if (!ace_appeared) {ace_appeared = true; score += 11;}
            else {score += 1;}
        }
        else if (rank == Rank::Jack || rank == Rank::Queen || rank == Rank::King){
            score += 10;
        }
        else{
            score += static_cast<int>(rank) + 2;
        }

    }
    return score;
}

int Hand::getFinalScore(){
    int hard = getScoreHard();
    int soft = getScoreSoft();


    if (hard > 21 && soft <= 21){
        return soft;
    }
    else if (soft > 21){
        return 0;
    }
    else{
        return hard;
    } 
}

int Hand::getFinalDealerScore(){//do I use this anywher?
    int hard = getDealerScore();

    if (hard > 21){
        return 0;
    }
    else{
        return hard;
    } 
}

int Hand::getDealerScore(){
    int score = 0;
    for (Card val : hand){
        Rank rank = val.getRank();
        if (rank == Rank::Ace){
            if (score + 11 > 21){
                score += 1;
            }
            else{
                score += 11;
            }
        }
        else if (rank == Rank::Jack || rank == Rank::Queen || rank == Rank::King){
            score += 10;
        }
        else{
            score += static_cast<int>(rank) + 2;
        }

        if (score > 21 && doesHandHaveAce()){
            score -= 10;
        }
    }
    return score;
}

bool Hand::isDealerOver(){//dealer can have soft
    if (getDealerScore() >= 17){
        return true;
    }
    return false;
}

int Hand::getScoreSoft(){
    int score = 0;

    for (Card val : hand){

        Rank rank = val.getRank();
        if (rank == Rank::Ace){
            score += 1;
        }
        else if (rank == Rank::Jack || rank == Rank::Queen || rank == Rank::King){
            score += 10;
        }
        else{
            score += static_cast<int>(rank) + 2;
        }

    }
    return score;
}

bool Hand::isSoftHand(){
    for (Card val : hand){
        Rank rank = val.getRank();
        if (rank == Rank::Ace){
            return true;
        }
    }
    return false;
}

bool Hand::doesHandHaveAce(){
    for (Card val : hand){
        Rank rank = val.getRank();
        if (rank == Rank::Ace){
            return true;
        }
    }
    return false;
}

bool Hand::check_can_split(){
    if (hand.size() == 2 && hand.front().getRank() == hand.back().getRank()){
        return true;
    }
    return false;
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