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

int Card::getRankInt(){
    switch (rank_) {
        case Rank::Two:   return 0;
        case Rank::Three: return 1;
        case Rank::Four:  return 2;
        case Rank::Five:  return 3;
        case Rank::Six:   return 4;
        case Rank::Seven: return 5;
        case Rank::Eight: return 6;
        case Rank::Nine:  return 7;
        case Rank::Ten:   return 8;
        case Rank::Jack:  return 8;
        case Rank::Queen: return 8;
        case Rank::King:  return 8;
        case Rank::Ace:   return 9;
        default:      return -1;
    }
}