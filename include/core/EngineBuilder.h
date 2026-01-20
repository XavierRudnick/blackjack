#ifndef ENGINEBUILDER_H
#define ENGINEBUILDER_H

#include "Engine.h"
#include "GameConfig.h"
#include "MonteCarloScenario.h"

class EngineBuilder {
    private:
        GameConfig gameConfig;
        std::optional<Deck> deck;
        EventBus* eventBus = nullptr;
        std::map<std::pair<int, int>, std::map<float, DecisionPoint>> EVresults;

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

        // Legacy single-scenario methods (for backward compatibility)
        EngineBuilder& setActionValues(std::set<std::pair<int, int>> values);
        EngineBuilder& setActions(std::vector<Action> actions);
        EngineBuilder& allowSoftHandsInMonteCarlo(bool enable);
        EngineBuilder& requirePairForMonteCarlo(bool enable);
        EngineBuilder& setEVActions(std::map<std::pair<int, int>, std::map<float, DecisionPoint>> values);

        // New multi-scenario method
        EngineBuilder& addMonteCarloScenario(const MonteCarloScenario& scenario);
        EngineBuilder& setMonteCarloScenarios(const std::vector<MonteCarloScenario>& scenarios);

        Engine build(Player* player);
};
#endif