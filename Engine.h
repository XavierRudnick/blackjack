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

class Engine{
private:
    int numDecks;
    double totalMoneyBet = 0;
    int PENETRATION_THRESHOLD; 
    double wallet;
    double blackjackPayoutMultiplier = 1.5;
    bool dealerHitsSoft17 = false;
    bool doubleAfterSplitAllowed = true;
    bool allowReSplitAces = true;
    bool allowSurrender = false;
    bool emitEvents = false;
    bool autoPlay = true;

    std::optional<Deck> deck;
    std::unique_ptr<CountingStrategy> countingStrategy;

    bool eventsEnabled() const;
    void publish(EventType type, const std::string& message);
    void publishWalletSnapshot();
    std::string describeAction(Action action, Hand& hand, const std::string& label);
    std::string optimalAction(Action action, Hand& hand, const std::string& label);
    std::string describeHand(const std::string& label, Hand& hand, bool hideHoleCard = false);

    EventBus* eventBus = nullptr;

public:
    Engine(int deck_size,int money, std::unique_ptr<CountingStrategy> strategy, bool enableEvents = false, double blackJackMultiplier = 1.5, bool dealerHitsSoft17 = false, bool doubleAfterSplitAllowed = true, bool allowReSplitAces = true, bool allowSurrender = false, bool autoPlay = true);
    std::pair<double, double> runner();

private:
    std::vector<int> getPlayerScores(std::vector<Hand>& hands);
    bool didHandsBust(std::vector<int> scores);
    void evaluateHands(Hand& dealer, std::vector<Hand>& hands);
    std::vector<Hand> user_play(Hand& dealer, Hand& user);
    Action getAction(Hand dealer, Hand user);
    void user_play_hand(Hand& dealer, Hand& user, std::vector<Hand>& hands, bool is_split_aces = false, bool has_split = false);
    void user_play_hand_manual(Hand& dealer, Hand& user, std::vector<Hand>& hands, bool is_split_aces = false, bool has_split = false);
    void print_state(Hand& dealer, Hand& user);
    void print_hand(Hand& user, const std::string& label = "Player");
    void peek_dealer(Hand& dealer);
    Hand draw_cards(int betSize = 0);
    void dealer_draw(Hand& dealer);
    bool insuranceHandler(Hand& dealer,Hand& user);
    bool dealerRobberyHandler(Hand& dealer,Hand& user);
    bool standHandler(Hand& user, std::vector<Hand>& hands, std::string handLabel);
    bool hitHandler(Hand& user, std::vector<Hand>& hands, std::string handLabel);
    bool doubleHandler(Hand& user, std::vector<Hand>& hands, std::string handLabel,bool has_split);
    bool splitHandler(Hand& user,Hand& dealer, std::vector<Hand>& hands, std::string handLabel,bool has_split, bool is_split_aces);
    bool surrenderHandler(Hand& user, std::vector<Hand>& hands, std::string handLabel);
   
};

#endif
