#ifndef BOTPLAYER_H
#define BOTPLAYER_H

#include <memory>

#include "Player.h"
#include "BasicStrategy.h"
#include "CountingStrategy.h"

class BotPlayer : public Player {
private:
    bool allowSurrender;
    std::unique_ptr<CountingStrategy> strategy;
public:
    BotPlayer(bool allowSurrender = false, std::unique_ptr<CountingStrategy> strat = nullptr);
    ~BotPlayer() override = default;
    Action getAction(Hand& user, Hand& dealer, float trueCount) override;
    CountingStrategy* getStrategy() override;
    
    void updateDeckStrategySize(int num_cards_left) override;
    int getBetSize() override ;
    void updateCount(Card card) override;
    float getTrueCount() override;
    bool shouldAcceptInsurance() override;


};

#endif
