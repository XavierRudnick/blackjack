#include "Card.h"

Card::Card(Rank rank, Suit suit){
    rank_ = rank;
    suit_ = suit;
}

Rank Card::getRank(){
    return rank_;
}

Suit Card::getSuit(){
    return suit_;
}
bool Card::isWorthTen(){
    return (rank_ == Rank::Jack || rank_ == Rank::Queen || rank_ == Rank::King || rank_ == Rank::Ten);
}

bool Card::isAce(){
    return (rank_ == Rank::Ace);
}