#ifndef ENGINEBUILDER_H
#define ENGINEBUILDER_H

#include "Engine.h"

class EngineBuilder {
    private:
        int number_of_decks = 2;
        double wallet = 1000.0;
        bool emitEvents = false;
        double blackjack_payout_multiplier = 1.5;
        bool dealerHitsSoft17= false;
        bool doubleAfterSplitAllowed = true;
        bool allowReSplitAces_ = true;
        bool allowSurrender_ = false;
        bool autoPlay = true;

    public:
        EngineBuilder& setDeckSize(int deck_size);
        EngineBuilder& setInitialWallet(double wallet);
        EngineBuilder& enableEvents(bool enable);

        EngineBuilder& with3To2Payout();
        EngineBuilder& with6To5Payout();

        EngineBuilder& withH17Rules();
        EngineBuilder& withS17Rules();

        EngineBuilder& allowReSplitAces();
        EngineBuilder& noReSplitAces();

        EngineBuilder& allowDoubleAfterSplit();
        EngineBuilder& noDoubleAfterSplit();

        EngineBuilder& allowSurrender();
        EngineBuilder& noSurrender();

        EngineBuilder& allowAutoPlay();
        EngineBuilder& allowManualPlay();

        Engine build(std::unique_ptr<CountingStrategy> countingStrategy) {
            Engine engine(number_of_decks, wallet, std::move(countingStrategy), emitEvents, blackjack_payout_multiplier, dealerHitsSoft17, doubleAfterSplitAllowed, allowReSplitAces_, allowSurrender_,autoPlay);
            engine.eventBus = EventBus::getInstance();
            return engine;
        }
};
#endif