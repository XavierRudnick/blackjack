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
