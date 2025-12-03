#include "Deck.h"
#include <algorithm>

Deck::Deck(int deck_size){
    deck.reserve(deck_size * NUM_CARDS_IN_DECK);  // Pre-allocate memory to avoid reallocations
    for(int i = 0; i < deck_size; i++){
        for(int rank = 0; rank < NUM_RANK; rank++){
            for(int suit = 0; suit < NUM_SUIT; suit++){
                deck.emplace_back(Card(static_cast<Rank>(rank),static_cast<Suit>(suit)));  // use emplace back to construct in place, o(1)
            }
        }
    }

    rand.seed(std::random_device{}());
    std::shuffle(deck.begin(), deck.end(), rand);
}

std::pair<Card,Card> Deck::deal(){
    Card first = deck.back();
    deck.pop_back();
    Card second = deck.back();
    deck.pop_back();

    return {first,second};
}

Card Deck::hit(){
    Card val = deck.back();
    deck.pop_back();

    return val;
}

int Deck::getSize(){
    return deck.size();
}