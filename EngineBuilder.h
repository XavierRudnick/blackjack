#ifndef ENGINEBUILDER_H
#define ENGINEBUILDER_H

#include "Engine.h"

class EngineBuilder {
    private:
        int numDecks = 2;
        double wallet = 1000.0;
        bool emitEvents = false;
        double blackjackPayoutMultiplier = 1.5;
        bool dealerHitsSoft17= false;
        bool doubleAfterSplitAllowed = true;
        bool allowReSplitAces_ = true;
        bool allowSurrender_ = false;
        bool autoPlay = true;
        std::optional<Deck> deck;

    public:
        EngineBuilder& setDeckSize(int deck_size);
        EngineBuilder& setDeck(Deck deck);

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
            Engine engine(numDecks, wallet, *deck, std::move(countingStrategy), emitEvents, blackjackPayoutMultiplier, dealerHitsSoft17, doubleAfterSplitAllowed, allowReSplitAces_, allowSurrender_,autoPlay);
            return engine;
        }
};
#endif