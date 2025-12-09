#ifndef PLAYER_H
#define PLAYER_H

#include "Hand.h"
#include "action.h"

class Player {
public:
    virtual Action getAction(Hand& user, Hand& dealer, float trueCount) = 0;
    virtual ~Player() = default;
};

#endif
