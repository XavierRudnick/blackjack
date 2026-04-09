#ifndef FIXEDENGINE_H
#define FIXEDENGINE_H

#include <algorithm>
#include <array>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "Deck.h"
#include "Hand.h"
#include "CountingStrategy.h"
#include "action.h"
#include "BasicStrategy.h"
#include "observers/EventBus.h"
#include "Player.h"
#include "Bankroll.h"
#include "GameReporter.h"
#include "GameConfig.h"
#include "ActionStats.h"
#include "MonteCarloScenario.h"

// True count buckets: -60 .. +60 in 0.5 steps (241 bins). Index = (int)(tc*2) + EV_TC_OFFSET.
inline constexpr int EV_TC_HALF_MIN = -120; // corresponds to true count -60
inline constexpr int EV_TC_HALF_MAX = 120;  // corresponds to true count +60
inline constexpr int EV_TC_OFFSET = -EV_TC_HALF_MIN; // 120, so TC 0 -> index 120
inline constexpr int EV_TC_STEPS = EV_TC_HALF_MAX - EV_TC_HALF_MIN + 1; // 241
inline constexpr int EV_D_RANGE = 10;
inline constexpr int EV_P_MIN = 4;
inline constexpr int EV_P_RANGE = 18;
inline constexpr std::size_t EV_TABLE_SLOTS = 2u * EV_P_RANGE * EV_D_RANGE * EV_TC_STEPS;

using EVTable = std::array<DecisionPoint, EV_TABLE_SLOTS>;

inline int evIdx(int playerTotal, int dealerUpcard, bool isSoft, float trueCount) {
    int p = std::clamp(playerTotal - EV_P_MIN, 0, EV_P_RANGE - 1);
    int d = std::clamp(dealerUpcard - 2, 0, EV_D_RANGE - 1);
    int tc = std::clamp(static_cast<int>(trueCount * 2.0f) + EV_TC_OFFSET, 0, EV_TC_STEPS - 1);
    int s = isSoft ? 1 : 0;
    return ((s * EV_P_RANGE + p) * EV_D_RANGE + d) * EV_TC_STEPS + tc;
}

class FixedEngine{

public:
    FixedEngine();
    FixedEngine(std::vector<Action> monteCarloActions, const GameConfig& gameConfig = GameConfig());
    FixedEngine(std::vector<Action> monteCarloActions, const EVTable& EVresults, const GameConfig& gameConfig = GameConfig());

    void calculateEV(Player& player, Deck& deck, Hand& dealer, Hand& user, float trueCount,
                       std::pair<int, int> cardValues, bool isSoft);

    void calculateEVForScenario(Player& player, Deck& deck, Hand& dealer, Hand& user, float trueCount,
                                std::pair<int, int> cardValues, bool isSoft, const MonteCarloScenario& scenario);

    void savetoCSVResults(const std::string& filename = "fixed_engine_results.csv") const;
    void saveScenarioResults(const std::string& scenarioName, const std::string& baseFilename) const;
    const EVTable& getResults() const;
    const EVTable& getScenarioResults(const std::string& scenarioName) const;
    std::vector<std::string> getScenarioNames() const;

    void merge(const FixedEngine& other);

    bool hasAnyLegacyResults() const;

private:
    std::vector<Action> monteCarloActions;
    std::unique_ptr<EVTable> EVresults_;

    std::unordered_map<std::string, EVTable> scenarioResults;

    GameConfig config;

    std::vector<Hand> handsBuffer_;

    void evaluateHand(Deck& deck, Hand& dealer, std::vector<Hand>& hands, float trueCount, Action forcedAction,
                      std::pair<int, int> cardValues, bool isSoft, int baseBet);

    void evaluateHandForScenario(Deck& deck, Hand& dealer, std::vector<Hand>& hands, float trueCount,
                                 Action forcedAction, std::pair<int, int> cardValues, bool isSoft, int baseBet,
                                 const std::string& scenarioName);

    void evaluateHandCore(Deck& deck, Hand& dealer, std::vector<Hand>& hands, Action forcedAction, int baseBet,
                          DecisionPoint& decisionPoint);

    void dealer_draw(Deck& deck, Hand& dealer);

    bool standHandler(Hand& user, std::vector<Hand>& hands);
    bool hitHandler(Deck& deck, Hand& user, std::vector<Hand>& hands);
    bool doubleHandler(Deck& deck, Hand& user, std::vector<Hand>& hands, bool has_split);
    bool splitHandler(Player& player, Deck& deck, Hand& user, Hand& dealer, std::vector<Hand>& hands, bool has_split,
                      bool is_split_aces, float trueCount);
    bool surrenderHandler(Hand& user, std::vector<Hand>& hands);
    bool InsuranceHandler(Player& player, Deck& deck, Hand& user, Hand& dealer, std::vector<Hand>& hands,
                          float trueCount);
    void playForcedHand(Player& player, Deck& deck, Hand& dealer, Hand& user, std::vector<Hand>& hands,
                        Action forcedAction, bool has_split_aces, bool has_split, float trueCount);
};

#endif // FIXEDENGINE_H
