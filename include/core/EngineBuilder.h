#ifndef ENGINEBUILDER_H
#define ENGINEBUILDER_H

#include "Engine.h"

class EngineBuilder {
    private:
        Engine::GameConfig gameConfig;
        std::optional<Deck> deck;
        EventBus* eventBus = nullptr;

    public:
        EngineBuilder& setDeckSize(int deck_size);
        EngineBuilder& setDeck(Deck deck);

        EngineBuilder& withEventBus(EventBus* bus);

        EngineBuilder& setInitialWallet(double wallet);
        EngineBuilder& enableEvents(bool enable);

        EngineBuilder& setPenetrationThreshold(float threshold);

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

        Engine build(std::unique_ptr<Player> player);
};
#endif