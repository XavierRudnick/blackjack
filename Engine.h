#ifndef ENGINE_H
#define ENGINE_H

#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include <memory>

#include "Deck.h"
#include "Hand.h"
#include "CountingStrategy.h"
#include "action.h"
#include "BasicStrategy.h"
#include "observers/EventBus.h"
#include "Player.h"
#include "Bankroll.h"
#include "GameReporter.h"

class Engine{
private:

    Bankroll bankroll;
    static constexpr double SURRENDERMULTIPLIER = .5;
    static constexpr double INSURANCEBETCOST = .5;

public:
    struct GameConfig {
        int numDecks = 2;
        double wallet = 1000.0;
        float penetrationThreshold = .75;
        double blackjackPayoutMultiplier = 1.5; 
        bool dealerHitsSoft17 = false;
        bool doubleAfterSplitAllowed = true;
        bool allowReSplitAces = true;
        bool allowSurrender = false;
        bool emitEvents = false;
    };

private:
    GameConfig config;

    std::optional<Deck> deck;
    std::unique_ptr<CountingStrategy> countingStrategy;
    std::unique_ptr<Player> player;
    std::unique_ptr<GameReporter> reporter;

    //hand evaluation logic
    std::vector<int> getPlayerScores(std::vector<Hand>& hands);
    bool didHandsBust(std::vector<int> scores);
    bool didPlayerGetBlackjack(std::vector<Hand>& hands);
    void evaluateHands(Hand& dealer, std::vector<Hand>& hands);
    
    //hand play logic
    std::vector<Hand> user_play(Hand& dealer, Hand& user);
    void play_hand(Player& player, Hand& dealer, Hand& user, std::vector<Hand>& hands, bool is_split_aces = false, bool has_split = false);

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
    bool splitHandler(Player& player, Hand& user,Hand& dealer, std::vector<Hand>& hands, std::string handLabel,bool has_split, bool is_split_aces);
    bool surrenderHandler(Hand& user, std::vector<Hand>& hands, std::string handLabel);
   
public:
    Engine(const GameConfig& gameConfig,Deck deck, std::unique_ptr<CountingStrategy> strategy, std::unique_ptr<Player> player);
    std::pair<double, double> runner();

};

#endif
