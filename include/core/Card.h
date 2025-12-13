#ifndef CARD_H
#define CARD_H

#include "rank.h"
#include "suit.h"

class Card{
    private:
        Rank rank_;
        Suit suit_;
    public:
        Card(Rank rank, Suit suit);
        Rank getRank();
        Suit getSuit();
        bool isWorthTen();
        bool isAce();
        int getValue();
};

#endif
