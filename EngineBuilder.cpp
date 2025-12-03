#include "Engine.h"
#include "observers/EventBus.h"
#include <functional>
#include "EngineBuilder.h"



EngineBuilder& EngineBuilder::setDeckSize(int deck_size) {
    number_of_decks = deck_size;
    return *this;
}

EngineBuilder& EngineBuilder::setInitialWallet(double money) {
    wallet = money;
    return *this;
}

EngineBuilder& EngineBuilder::enableEvents(bool enable) {
    emitEvents = enable;
    return *this;
}

EngineBuilder& EngineBuilder::with3To2Payout() {
    blackjack_payout_multiplier = 1.5;
    return *this;
}
EngineBuilder& EngineBuilder::with6To5Payout() {
    blackjack_payout_multiplier = 1.2;
    return *this;
}

EngineBuilder& EngineBuilder::withH17Rules() {
    dealerHitsSoft17 = true;
    return *this;
}

EngineBuilder& EngineBuilder::withS17Rules() {
    dealerHitsSoft17 = false;
    return *this;
}

EngineBuilder& EngineBuilder::allowDoubleAfterSplit() {
    doubleAfterSplitAllowed = true;
    return *this;
}

EngineBuilder& EngineBuilder::noDoubleAfterSplit() {
    doubleAfterSplitAllowed = false;
    return *this;
}

EngineBuilder& EngineBuilder::allowReSplitAces() {
    allowReSplitAces_ = true;
    return *this;
}

EngineBuilder& EngineBuilder::noReSplitAces() {
    allowReSplitAces_ = false;
    return *this;
}

EngineBuilder& EngineBuilder::allowSurrender() {
    allowSurrender_ = true;
    return *this;
}

EngineBuilder& EngineBuilder::noSurrender() {
    allowSurrender_ = false;
    return *this;
}

EngineBuilder& EngineBuilder::allowAutoPlay(){
    autoPlay = true;
    return *this;
}

EngineBuilder& EngineBuilder::allowManualPlay(){
    autoPlay = false;
    return *this;
}


