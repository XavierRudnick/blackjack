#ifndef ENGINEBUILDER_H
#define ENGINEBUILDER_H

#include "Engine.h"

class EngineBuilder {
    private:
        int number_of_decks_ = 2;
        double wallet_ = 1000.0;
        bool emitEvents_ = false;
        double blackjack_payout_multiplier_ = 1.5;
        bool dealerHitsSoft17_ = false;
        bool doubleAfterSplitAllowed_ = true;
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
            Engine engine(number_of_decks_, wallet_, std::move(countingStrategy), emitEvents_, blackjack_payout_multiplier_, dealerHitsSoft17_, doubleAfterSplitAllowed_, allowReSplitAces_, allowSurrender_,autoPlay);
            engine.eventBus = EventBus::getInstance();
            return engine;
        }
};
#endif