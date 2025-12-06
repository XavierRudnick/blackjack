#include "Engine.h"
#include "observers/EventBus.h"
#include <functional>
#include "EngineBuilder.h"
#include <stdexcept>



EngineBuilder& EngineBuilder::setDeckSize(int deck_size) {
    gameConfig.numDecks = deck_size;
    return *this;
}

EngineBuilder& EngineBuilder::setDeck(Deck d) {
    deck.emplace(d);
    return *this;
}

EngineBuilder& EngineBuilder::withEventBus(EventBus* bus) {
    eventBus = bus;
    return *this;
}

EngineBuilder& EngineBuilder::setInitialWallet(double money) {
    gameConfig.wallet = money;
    return *this;
}

EngineBuilder& EngineBuilder::setPenetrationThreshold(float threshold){
    gameConfig.penetrationThreshold = threshold;
    return *this;
}
EngineBuilder& EngineBuilder::enableEvents(bool enable) {
    gameConfig.emitEvents = enable;
    return *this;
}

EngineBuilder& EngineBuilder::with3To2Payout() {
    gameConfig.blackjackPayoutMultiplier = 1.5;
    return *this;
}
EngineBuilder& EngineBuilder::with6To5Payout() {
    gameConfig.blackjackPayoutMultiplier = 1.2;
    return *this;
}

EngineBuilder& EngineBuilder::withH17Rules() {
    gameConfig.dealerHitsSoft17 = true;
    return *this;
}

EngineBuilder& EngineBuilder::withS17Rules() {
    gameConfig.dealerHitsSoft17 = false;
    return *this;
}

EngineBuilder& EngineBuilder::allowDoubleAfterSplit() {
    gameConfig.doubleAfterSplitAllowed = true;
    return *this;
}

EngineBuilder& EngineBuilder::noDoubleAfterSplit() {
    gameConfig.doubleAfterSplitAllowed = false;
    return *this;
}

EngineBuilder& EngineBuilder::allowReSplitAces() {
    gameConfig.allowReSplitAces = true;
    return *this;
}

EngineBuilder& EngineBuilder::noReSplitAces() {
    gameConfig.allowReSplitAces = false;
    return *this;
}

EngineBuilder& EngineBuilder::allowSurrender() {
    gameConfig.allowSurrender = true;
    return *this;
}

EngineBuilder& EngineBuilder::noSurrender() {
    gameConfig.allowSurrender = false;
    return *this;
}

Engine EngineBuilder::build(std::unique_ptr<CountingStrategy> countingStrategy, std::unique_ptr<Player> player) {
    Engine engine(gameConfig, *deck, std::move(countingStrategy), std::move(player), eventBus);
    return engine;
}
