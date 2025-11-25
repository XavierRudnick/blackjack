#include "Engine.h"
#include "observers/EventBus.h"
#include <functional>
#include "EngineBuilder.h"



EngineBuilder& EngineBuilder::setDeckSize(int deck_size) {
    number_of_decks_ = deck_size;
    return *this;
}

EngineBuilder& EngineBuilder::setInitialWallet(double wallet) {
    wallet_ = wallet;
    return *this;
}

EngineBuilder& EngineBuilder::enableEvents(bool enable) {
    emitEvents_ = enable;
    return *this;
}

EngineBuilder& EngineBuilder::with3To2Payout() {
    blackjack_payout_multiplier_ = 1.5;
    return *this;
}
EngineBuilder& EngineBuilder::with6To5Payout() {
    blackjack_payout_multiplier_ = 1.2;
    return *this;
}

EngineBuilder& EngineBuilder::withH17Rules() {
    this->dealerHitsSoft17_ = true;
    return *this;
}

EngineBuilder& EngineBuilder::withS17Rules() {
    this->dealerHitsSoft17_ = false;
    return *this;
}

EngineBuilder& EngineBuilder::allowDoubleAfterSplit() {
    this->doubleAfterSplitAllowed_ = true;
    return *this;
}

EngineBuilder& EngineBuilder::noDoubleAfterSplit() {
    this->doubleAfterSplitAllowed_ = false;
    return *this;
}

EngineBuilder& EngineBuilder::allowReSplitAces() {
    this->allowReSplitAces_ = true;
    return *this;
}

EngineBuilder& EngineBuilder::noReSplitAces() {
    this->allowReSplitAces_ = false;
    return *this;
}

EngineBuilder& EngineBuilder::allowSurrender() {
    this->allowSurrender_ = true;
    return *this;
}

EngineBuilder& EngineBuilder::noSurrender() {
    this->allowSurrender_ = false;
    return *this;
}

EngineBuilder& EngineBuilder::allowAutoPlay(){
    this->autoPlay = true;
    return *this;
}

EngineBuilder& EngineBuilder::allowManualPlay(){
    this->autoPlay = false;
    return *this;
}


