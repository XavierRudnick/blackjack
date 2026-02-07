#include "Deck.h"
#include <algorithm>
#include <atomic>

namespace {
    std::atomic<bool> gDeterministicSeedEnabled{false};
    std::atomic<std::uint32_t> gDeterministicSeed{0u};
    std::atomic<std::uint64_t> gRngEpoch{1u};
}

std::mt19937& Deck::getGlobalRng() {
    static thread_local std::mt19937 rng;
    static thread_local std::uint64_t localEpoch = 0u;

    const std::uint64_t globalEpoch = gRngEpoch.load(std::memory_order_acquire);
    if (localEpoch != globalEpoch) {
        if (gDeterministicSeedEnabled.load(std::memory_order_acquire)) {
            rng.seed(gDeterministicSeed.load(std::memory_order_relaxed));
        } else {
            std::random_device rd;
            std::seed_seq seedData{rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd()};
            rng.seed(seedData);
        }
        localEpoch = globalEpoch;
    }

    return rng;
}

Deck::Deck(int deck_size){
    deck.reserve(deck_size * NUM_CARDS_IN_DECK);  // Pre-allocate memory to avoid reallocations
    
    for(int i = 0; i < deck_size; i++){
        for(int rank = 0; rank < NUM_RANK; rank++){
            for(int suit = 0; suit < NUM_SUIT; suit++){
                deck.emplace_back(Card(static_cast<Rank>(rank),static_cast<Suit>(suit)));  // use emplace back to construct in place, o(1)
            }
        }
    }

    shuffle();
}

void Deck::shuffle() {
    std::shuffle(deck.begin(), deck.end(), getGlobalRng());
}

Deck Deck::createTestDeck(std::vector<Card> stackedCards) {
    Deck riggedDeck(0);
    riggedDeck.deck = stackedCards;
    return riggedDeck;
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

Deck Deck::clone() const{
    Deck copy(0);
    copy.deck = this->deck;
    return copy;
}

void Deck::reset(){
    std::shuffle(deck.begin(), deck.end(), getGlobalRng());
}

void Deck::setSeed(std::uint32_t seed) {
    gDeterministicSeed.store(seed, std::memory_order_release);
    gDeterministicSeedEnabled.store(true, std::memory_order_release);
    gRngEpoch.fetch_add(1u, std::memory_order_acq_rel);
}

void Deck::clearSeed() {
    gDeterministicSeedEnabled.store(false, std::memory_order_release);
    gRngEpoch.fetch_add(1u, std::memory_order_acq_rel);
}
