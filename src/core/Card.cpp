#include "Card.h"

Card::Card(Rank rank, Suit suit){
    rank_ = rank;
    suit_ = suit;
}

Rank Card::getRank() const {
    return rank_;
}

int Card::getValue() const {
    switch(rank_){
        case Rank::Two: return 2;
        case Rank::Three: return 3;
        case Rank::Four: return 4;
        case Rank::Five: return 5;
        case Rank::Six: return 6;
        case Rank::Seven: return 7;
        case Rank::Eight: return 8;
        case Rank::Nine: return 9;
        case Rank::Ten:
        case Rank::Jack:
        case Rank::Queen:
        case Rank::King:
            return 10;
        case Rank::Ace:
            return 11; // Ace is initially worth 11, can be adjusted in Hand class
        default:
            return 0;
    }
}

Suit Card::getSuit() const{
    return suit_;
}
bool Card::isWorthTen(){
    return (rank_ == Rank::Jack || rank_ == Rank::Queen || rank_ == Rank::King || rank_ == Rank::Ten);
}

bool Card::isAce(){
    return (rank_ == Rank::Ace);
}