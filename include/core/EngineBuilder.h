#ifndef ENGINEBUILDER_H
#define ENGINEBUILDER_H

#include "Engine.h"
#include "GameConfig.h"
class EngineBuilder {
    private:
        GameConfig gameConfig;
        std::optional<Deck> deck;
        EventBus* eventBus = nullptr;

    public:
        EngineBuilder& setDeckSize(int deck_size);
        EngineBuilder& setDeck(Deck deck);

        EngineBuilder& withEventBus(EventBus* bus);

        EngineBuilder& setInitialWallet(double wallet);
        EngineBuilder& enableEvents(bool enable);

        EngineBuilder& setPenetrationThreshold(float threshold);

        EngineBuilder& with3To2Payout(bool enable);
        EngineBuilder& with6To5Payout();

        EngineBuilder& withH17Rules(bool enable);
        EngineBuilder& withS17Rules();

        EngineBuilder& allowReSplitAces(bool enable);
        EngineBuilder& noReSplitAces();

        EngineBuilder& allowDoubleAfterSplit(bool enable);
        EngineBuilder& noDoubleAfterSplit();

        EngineBuilder& allowSurrender(bool enable);
        EngineBuilder& noSurrender();

        EngineBuilder& enableMontiCarlo(bool enable);
        EngineBuilder& noMontiCarlo();

        EngineBuilder& setUserHandValue(int value);
        EngineBuilder& setDealerUpcardValue(int value);

        EngineBuilder& setActions(std::vector<Action> actions);

        Engine build(Player* player);
};
#endif