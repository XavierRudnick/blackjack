#ifndef DECK_H
#define DECK_H

#include <random>
#include <vector>
#include "Card.h"

class Deck{
    private:
        const int NUM_RANK = 13;
        const int NUM_SUIT = 4;
        const int NUM_CARDS_IN_DECK = 52;
        std::vector<Card> deck;
        std::mt19937 rand;

    public:
        Deck(int deck_size);
        std::pair<Card,Card> deal();
        Card hit();
        int getSize();
};

#endif
