#ifndef FIXEDENGINE_H
#define FIXEDENGINE_H

#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <map>
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

class FixedEngine{

public:    
    FixedEngine();
    // Legacy constructor for backward compatibility
    FixedEngine(std::vector<Action> monteCarloActions, std::map<std::pair<int, int>, std::map<float, DecisionPoint>> EVresults, const GameConfig& gameConfig = GameConfig());
    
    // Legacy single-action calculateEV (for backward compatibility with tests)
    void calculateEV(Player& player, Deck& deck, Hand& dealer, Hand& user, float trueCount, std::pair<int,int> cardValues);
    
    // New multi-scenario calculateEV - evaluates all matching scenarios
    void calculateEVForScenario(Player& player, Deck& deck, Hand& dealer, Hand& user, float trueCount, 
                                 std::pair<int,int> cardValues, const MonteCarloScenario& scenario);
    
    void savetoCSVResults(const std::string& filename = "fixed_engine_results.csv") const;
    void saveScenarioResults(const std::string& scenarioName, const std::string& baseFilename) const;
    const std::map<std::pair<int, int>, std::map<float, DecisionPoint>>& getResults() const;
    const std::map<std::pair<int, int>, std::map<float, DecisionPoint>>& getScenarioResults(const std::string& scenarioName) const;
    std::vector<std::string> getScenarioNames() const;
    
    void merge(const FixedEngine& other);
    
private:
    // Legacy single-action support
    std::vector<Action> monteCarloActions;
    std::map<std::pair<int, int>, std::map<float, DecisionPoint>> EVresults;
    
    // Multi-scenario support: results keyed by scenario name
    std::map<std::string, std::map<std::pair<int, int>, std::map<float, DecisionPoint>>> scenarioResults;
    
    GameConfig config;
    
    void evaluateHand(Deck& deck, Hand& dealer, std::vector<Hand>& hands, float trueCount, Action forcedAction,std::pair<int,int> cardValues, int baseBet);
    
    // New evaluateHand for scenario-specific results
    void evaluateHandForScenario(Deck& deck, Hand& dealer, std::vector<Hand>& hands, float trueCount, 
                                  Action forcedAction, std::pair<int,int> cardValues, int baseBet,
                                  const std::string& scenarioName);
    
    void dealer_draw(Deck& deck, Hand& dealer);

    bool standHandler(Hand& user, std::vector<Hand>& hands);
    bool hitHandler(Deck& deck, Hand& user, std::vector<Hand>& hands);
    bool doubleHandler(Deck& deck, Hand& user, std::vector<Hand>& hands, bool has_split);
    bool splitHandler(Player& player, Deck& deck, Hand& user, Hand& dealer, std::vector<Hand>& hands, bool has_split, bool is_split_aces, float trueCount);
    bool surrenderHandler(Hand& user, std::vector<Hand>& hands);
    bool InsuranceHandler(Player& player, Deck& deck, Hand& user, Hand& dealer, std::vector<Hand>& hands, float trueCount);
    void playForcedHand(Player& player, Deck& deck, Hand& dealer, Hand& user, std::vector<Hand>& hands, Action forcedAction,bool has_split_aces, bool has_split,float trueCount);
 

};

#endif // FIXEDENGINE_H
