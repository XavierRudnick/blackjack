#ifndef PLAYER_H
#define PLAYER_H

#include "CountingStrategy.h"
#include "Hand.h"
#include "action.h"
#include "Card.h"

class Player {
public:
    virtual Action getAction(Hand& user, Hand& dealer, float trueCount) = 0;
    virtual CountingStrategy* getStrategy() = 0;
    virtual void updateDeckStrategySize(int num_cards_left) = 0;
    virtual int getBetSize() = 0;
    virtual void setUnitSize(float kellyFraction) = 0;
    virtual void updateCount(Card card) = 0;
    virtual float getTrueCount() = 0;
    virtual bool shouldAcceptInsurance() = 0;
    virtual ~Player() = default;
};

#endif
