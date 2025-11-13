#ifndef CARD_H
#define CARD_H

#include "rank.h"
#include "suit.h"

class Card{
    private:
        Rank rank_ = Rank::Ace;
        Suit suit_ = Suit::Spades;
    public:
        Card(Rank rank, Suit suit);
        Rank getRank();
        Suit getSuit();
        int getRankInt();
};

#endif
