#ifndef DECK_H
#define DECK_H

#include <cstdint>
#include <random>
#include <vector>
#include "Card.h"

class Deck{
    private:
        std::vector<Card> deck;

        static std::mt19937& getGlobalRng();

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

        static void setSeed(std::uint32_t seed);
        static void clearSeed();

        const std::vector<Card>& getDeckCards() const;
        void restoreDeckCards(const std::vector<Card>& deckCards);

        void shuffle();
};

#endif
