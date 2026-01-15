#ifndef DECK_H
#define DECK_H

#include <random>
#include <vector>
#include "Card.h"

class Deck{
    private:
        std::vector<Card> deck;

        static std::mt19937& getGlobalRng() {
            static std::mt19937 rand(std::random_device{}());
            return rand;
        }

    public:
        static const int NUM_RANK = 13;
        static const int NUM_SUIT = 4;
        static const int NUM_CARDS_IN_DECK = 52;

        Deck(int deck_size);
        static Deck createTestDeck(std::vector<Card> stackedCards);
        std::pair<Card,Card> deal();
        Card hit();
        int getSize();
        Deck clone() const;
        void reset();

        void shuffle();
};

#endif
