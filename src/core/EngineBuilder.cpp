#include "Engine.h"
#include "observers/EventBus.h"
#include <functional>
#include "EngineBuilder.h"
#include <stdexcept>



EngineBuilder& EngineBuilder::setDeckSize(int deck_size) {
    gameConfig.numDecks = deck_size;
    return *this;
}

EngineBuilder& EngineBuilder::setDeck(Deck inputDeck) {
    deck.emplace(inputDeck);
    return *this;
}

EngineBuilder& EngineBuilder::withEventBus(EventBus* bus) {
    eventBus = bus;
    return *this;
}

EngineBuilder& EngineBuilder::setInitialWallet(double money = 1000) {
    gameConfig.wallet = money;
    return *this;
}

EngineBuilder& EngineBuilder::setPenetrationThreshold(float threshold = 0.75){
    gameConfig.penetrationThreshold = threshold;
    return *this;
}
EngineBuilder& EngineBuilder::enableEvents(bool enable = false) {
    gameConfig.emitEvents = enable;
    return *this;
}

EngineBuilder& EngineBuilder::with3To2Payout(bool enable = true) {
    if (!enable) {
        return with6To5Payout();
    }
    gameConfig.blackjackPayoutMultiplier = 1.5;
    return *this;
}
EngineBuilder& EngineBuilder::with6To5Payout() {
    gameConfig.blackjackPayoutMultiplier = 1.2;
    return *this;
}

EngineBuilder& EngineBuilder::withH17Rules(bool enable = true) {
    if (!enable) {
        return withS17Rules();
    }
    gameConfig.dealerHitsSoft17 = true;
    return *this;
}

EngineBuilder& EngineBuilder::withS17Rules() {
    gameConfig.dealerHitsSoft17 = false;
    return *this;
}

EngineBuilder& EngineBuilder::allowDoubleAfterSplit(bool enable = true) {
    if (!enable) {
        return noDoubleAfterSplit();
    }
    gameConfig.doubleAfterSplitAllowed = true;
    return *this;
}

EngineBuilder& EngineBuilder::noDoubleAfterSplit() {
    gameConfig.doubleAfterSplitAllowed = false;
    return *this;
}

EngineBuilder& EngineBuilder::allowReSplitAces(bool enable = true) {
    if (!enable) {
        return noReSplitAces();
    }
    gameConfig.allowReSplitAces = true;
    return *this;
}

EngineBuilder& EngineBuilder::noReSplitAces() {
    gameConfig.allowReSplitAces = false;
    return *this;
}

EngineBuilder& EngineBuilder::allowSurrender(bool enable = true) {
    if (!enable) {
        return noSurrender();
    }
    gameConfig.allowSurrender = true;
    return *this;
}

EngineBuilder& EngineBuilder::noSurrender() {
    gameConfig.allowSurrender = false;
    return *this;
}

EngineBuilder& EngineBuilder::enableMontiCarlo(bool enable = false) {
    if (!enable) {
        return noMontiCarlo();
    }
    gameConfig.enabelMontiCarlo = true;
    return *this;
}

EngineBuilder& EngineBuilder::noMontiCarlo() {
    gameConfig.enabelMontiCarlo = false;
    return *this;
}

EngineBuilder& EngineBuilder::setActionValues(std::set<std::pair<int, int>> values) {
    gameConfig.actionValues = values;
    return *this;
}

EngineBuilder& EngineBuilder::setActions(std::vector<Action> actions) {
    gameConfig.monteCarloActions = actions;
    return *this;
}

EngineBuilder& EngineBuilder::allowSoftHandsInMonteCarlo(bool enable = false) {
    gameConfig.allowSoftHandsInMonteCarlo = enable;
    return *this;
}

EngineBuilder& EngineBuilder::requirePairForMonteCarlo(bool enable = false) {
    gameConfig.requirePairForMonteCarlo = enable;
    return *this;
}

EngineBuilder& EngineBuilder::setEVActions(std::map<std::pair<int, int>, std::map<float, DecisionPoint>> values) {
    EVresults = values;
    return *this;
}   

Engine EngineBuilder::build(Player* player) {
    Engine engine(gameConfig, *deck, player, eventBus, EVresults);
    return engine;
}
