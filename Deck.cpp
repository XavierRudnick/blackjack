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

Deck Deck::createTestDeck(std::vector<Card> stackedCards) {
    Deck d(0);
    d.deck = stackedCards;
    return d;
} 

std::pair<Card,Card> Deck::deal(){
    if (deck.size() < 2) {
                throw std::runtime_error("Not enough cards in deck to deal 39");
            }
            
    Card first = deck.back();
    deck.pop_back();
    Card second = deck.back();
    deck.pop_back();

    return {first,second};
}

Card Deck::hit(){
     if (deck.empty()) {
                throw std::runtime_error("Deck is empty - cannot hit 62");
            }

    Card val = deck.back();
    deck.pop_back();

    return val;
}

int Deck::getSize(){
    return deck.size();
}