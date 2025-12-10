#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "Player.h"
#include "CountingStrategy.h"

#include <iostream>
#include <limits>
#include <memory>
class HumanPlayer : public Player {
private:
    bool allowSurrender;
    std::unique_ptr<CountingStrategy> strategy;
public:
    HumanPlayer(bool allowSurrender = false, std::unique_ptr<CountingStrategy> strat = nullptr);
    ~HumanPlayer() override = default;
    Action getAction(Hand& user, Hand& dealer, float trueCount) override;
    Action getOptimalAction(Hand& user, Hand& dealer, float trueCount);

    CountingStrategy* getStrategy() override;
    
    void updateDeckStrategySize(int num_cards_left) override;
    int getBetSize() override ;
    void updateCount(Card card) override;
    float getTrueCount() override;
    bool shouldAcceptInsurance() override;
};

#endif
