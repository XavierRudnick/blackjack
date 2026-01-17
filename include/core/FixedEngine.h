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

class FixedEngine{

public:
    struct ActionStats {
        int handsPlayed = 0;  // number of hands
        double totalPayout = 0;  // total payout across all hands
        double mean = 0.0;    // running EV
        double M2 = 0.0;      // running squared deviation

        void addResult(double x) {
            handsPlayed++;
            totalPayout += x;

            double delta = x - mean;
            mean += delta / handsPlayed;

            double delta2 = x - mean;
            M2 += delta * delta2;
        }

        double getEV() const {
            return mean;
        }

        double getVariance() const {
            return (handsPlayed > 1) ? (M2 / handsPlayed) : 0.0;   // population variance
        }

        double getStdDev() const {
            return std::sqrt(getVariance());
        }

        double getStdError() const {
            return getStdDev() / std::sqrt(handsPlayed);
        }
    };


    struct DecisionPoint {
        ActionStats hitStats;
        ActionStats standStats;
        ActionStats doubleStats;
        ActionStats splitStats;
        ActionStats surrenderStats;
        ActionStats insuranceAcceptStats;
        ActionStats insuranceDeclineStats;
    };
    
    FixedEngine();
    FixedEngine(std::vector<Action> monteCarloActions, const GameConfig& gameConfig = GameConfig());
    void calculateEV(Player& player,Deck& deck,Hand& dealer, Hand& user, float trueCount, std::pair<int,int> cardValues);
    void savetoCSVResults(const std::string& filename = "fixed_engine_results.csv") const;
    const std::map<std::pair<int, int>, std::map<float, DecisionPoint>>& getResults() const;
    void merge(const FixedEngine& other);
private:
    std::vector<Action> monteCarloActions;
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
