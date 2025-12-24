#ifndef ENGINE_H
#define ENGINE_H

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
#include "FixedEngine.h"
#include "GameConfig.h"

class Engine{

public:

    Engine(
        const GameConfig& gameConfig, //pass by reference to avoid copy
        Deck deck,
        std::unique_ptr<Player> player,
        EventBus* eventBus // not owned can be nullptr
    );

    std::pair<double, double> runner();
    FixedEngine runnerMonte();

private:
    Bankroll bankroll;
    static constexpr double SURRENDERMULTIPLIER = .5;
    static constexpr double INSURANCEBETCOST = .5;
    GameConfig config;

    std::optional<Deck> deck;
    std::unique_ptr<Player> player;
    std::unique_ptr<GameReporter> reporter;

    FixedEngine fixedEngine;

    //hand evaluation logic
    std::vector<int> getPlayerScores(std::vector<Hand>& hands);
    bool didHandsBust(std::vector<int> scores);
    bool didPlayerGetNaturalBlackjack(std::vector<Hand>& hands);
    void NaturalBlackJackHandler(Hand& dealer, Hand& user);
    void evaluateHands(Hand& dealer, std::vector<Hand>& hands);
    
    //hand play logic
    std::vector<Hand> user_play(Hand& dealer, Hand& user);
    void play_hand(Hand& dealer, Hand& user, std::vector<Hand>& hands, bool is_split_aces = false, bool has_split = false);

    //card drawing logic
    Hand draw_cards(int betSize = 0);
    void dealer_draw(Hand& dealer);

    //game logic
    void playHand();

    bool handleInsurancePhase(Hand& dealer, Hand& user);
    bool canOfferInsurance(Hand& dealer);
    bool askInsurance();
    bool resolveInsurance(bool accepted, Hand& dealer, Hand& user);
    bool handleInsuranceAccepted(Hand& dealer, Hand& user);
    bool handleInsuranceDeclined(Hand& dealer, Hand& user);
    bool dealerRobberyHandler(Hand& dealer,Hand& user);

    bool standHandler(Hand& user, std::vector<Hand>& hands, std::string handLabel);
    bool hitHandler(Hand& user, std::vector<Hand>& hands, std::string handLabel);
    bool doubleHandler(Hand& user, std::vector<Hand>& hands, std::string handLabel,bool has_split);
    bool splitHandler(Hand& user,Hand& dealer, std::vector<Hand>& hands, std::string handLabel,bool has_split, bool is_split_aces);
    bool surrenderHandler(Hand& user, std::vector<Hand>& hands, std::string handLabel);

    void playForcedHand(Hand& dealer, Hand& user, std::vector<Hand>& hands, bool is_split_aces, bool has_split, Action forcedAction);
    void calculateEV(Hand& dealer, Hand& user, std::vector<Hand>& hands, bool is_split_aces, bool has_split, Action forcedAction, float trueCount);

   

};

#endif
