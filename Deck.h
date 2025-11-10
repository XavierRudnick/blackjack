#ifndef DECK_H
#define DECK_H

#include <vector>
#include <random>
#include <algorithm>
#include "Card.h"

template <typename Strategy>
class Deck{
    private:
        const uint8_t NUM_RANK = 13;
        const uint8_t NUM_SUIT = 4;
        const uint8_t NUM_CARDS_IN_DECK = 52;
        std::vector<Card> deck;
        std::mt19937 rand;
        Strategy strategy;

    public:
        Deck(uint8_t deck_size, Strategy strategy_input) : strategy(std::move(strategy_input)){
            deck.reserve(deck_size * NUM_CARDS_IN_DECK);  // Pre-allocate memory to avoid reallocations

            for(int i = 0; i < deck_size; i++){
                for(int rank = static_cast<int>(Rank::Two); rank < NUM_RANK; rank++){
                    for(int suit = static_cast<int>(Suit::Spades); suit < NUM_SUIT; suit++){
                        deck.emplace_back(Card(static_cast<Rank>(rank),static_cast<Suit>(suit)));  // use emplace back to construct in place, o(1)
                    }
                }
            }

            rand.seed(std::random_device{}());
            std::shuffle(deck.begin(), deck.end(), rand);
        }

        std::pair<Card,Card> deal(){
            Card first = deck.back();
            deck.pop_back();
            Card second = deck.back();
            deck.pop_back();
            strategy.updateCount(first);
            strategy.updateCount(second);
            return {first,second};
        }

        Card dealOne(){
            Card first = deck.back();
            deck.pop_back();
            strategy.updateCount(first);
            return first;
        }

        Card hit(){
            Card val = deck.back();
            deck.pop_back();
            strategy.updateCount(val);
            return val;
        }

        int getSize(){
            return deck.size();
        }

        Strategy getStrategy(){
            return strategy;
        }

        int getBetSize(){
            return strategy.getBetSize();
        }
};

#endif
