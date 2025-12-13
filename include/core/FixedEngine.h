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

class FixedEngine{

public:
    struct ActionStats {
        double totalPayout = 0.0; 
        int handsPlayed = 0;

        void addResult(double payout) {
            totalPayout += payout;
            handsPlayed++;
        }

        double getEV() const {
            if (handsPlayed == 0) return 0.0;
            return totalPayout / handsPlayed;
        }
    };

    struct DecisionPoint {
        ActionStats hitStats;
        ActionStats standStats;
        ActionStats doubleStats;
    };

    std::map<float, DecisionPoint> EVresults;
    FixedEngine();
    FixedEngine(std::vector<Action> monteCarloActions);
    void calculateEV(Player& player,Deck& deck,Hand& dealer, Hand& user, float trueCount);
    void printResults();
    void savetoCSVResults(const std::string& filename = "fixed_engine_results.csv") const;
    const std::map<float, DecisionPoint>& getResults() const { return EVresults; }
    void merge(const FixedEngine& other);
private:
    std::vector<Action> monteCarloActions;

    float evaluateHand(Deck& deck, Hand& dealer, Hand& user);
    void dealer_draw(Deck& deck, Hand& dealer);

    // bool handleInsurancePhase(Hand& dealer, Hand& user);
    // bool canOfferInsurance(Hand& dealer);
    // bool askInsurance();
    // bool resolveInsurance(bool accepted, Hand& dealer, Hand& user);
    // bool handleInsuranceAccepted(Hand& dealer, Hand& user);
    // bool handleInsuranceDeclined(Hand& dealer, Hand& user);
    // bool dealerRobberyHandler(Hand& dealer,Hand& user);

    bool standHandler(Hand& user);
    bool hitHandler(Player& player, Deck& deck, Hand& user);
    bool doubleHandler(Player& player, Deck& deck, Hand& user);
    bool splitHandler(Hand& user,Hand& dealer, std::vector<Hand>& hands,bool has_split, bool is_split_aces);
    bool surrenderHandler(Hand& user, std::vector<Hand>& hands);

    Hand playForcedHand(Player& player, Deck& deck, Hand& dealer, Hand& user, Action forcedAction);
 

};

#endif // FIXEDENGINE_H
