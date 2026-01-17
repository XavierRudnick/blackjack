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

class FixedEngine{

public:    
    FixedEngine();
    FixedEngine(std::vector<Action> monteCarloActions,std::map<std::pair<int, int>, std::map<float, DecisionPoint>> EVresults, const GameConfig& gameConfig = GameConfig());
    void calculateEV(Player& player,Deck& deck,Hand& dealer, Hand& user, float trueCount, std::pair<int,int> cardValues);
    void savetoCSVResults(const std::string& filename = "fixed_engine_results.csv") const;
    const std::map<std::pair<int, int>, std::map<float, DecisionPoint>>& getResults() const;
    void merge(const FixedEngine& other);
private:
    std::vector<Action> monteCarloActions;
    std::map<std::pair<int, int>, std::map<float, DecisionPoint>> EVresults;
    GameConfig config;
    void evaluateHand(Deck& deck, Hand& dealer, std::vector<Hand>& hands, float trueCount, Action forcedAction,std::pair<int,int> cardValues, int baseBet);
    void dealer_draw(Deck& deck, Hand& dealer);

    // bool handleInsurancePhase(Hand& dealer, Hand& user);
    // bool canOfferInsurance(Hand& dealer);
    // bool askInsurance();
    // bool resolveInsurance(bool accepted, Hand& dealer, Hand& user);
    // bool handleInsuranceAccepted(Hand& dealer, Hand& user);
    // bool handleInsuranceDeclined(Hand& dealer, Hand& user);
    // bool dealerRobberyHandler(Hand& dealer,Hand& user);

    bool standHandler(Hand& user, std::vector<Hand>& hands);
    bool hitHandler(Deck& deck, Hand& user, std::vector<Hand>& hands);
    bool doubleHandler(Deck& deck, Hand& user, std::vector<Hand>& hands, bool has_split);
    bool splitHandler(Player& player, Deck& deck, Hand& user, Hand& dealer, std::vector<Hand>& hands, bool has_split, bool is_split_aces, float trueCount);
    bool surrenderHandler(Hand& user, std::vector<Hand>& hands);
    bool InsuranceHandler(Player& player, Deck& deck, Hand& user, Hand& dealer, std::vector<Hand>& hands, float trueCount);
    void playForcedHand(Player& player, Deck& deck, Hand& dealer, Hand& user, std::vector<Hand>& hands, Action forcedAction,bool has_split_aces, bool has_split,float trueCount);
 

};

#endif // FIXEDENGINE_H
