#ifndef PLAYER_H
#define PLAYER_H

#include "Hand.h"
#include "action.h"

class Player {
public:
    virtual ~Player() = default;
    // The player needs the user's hand, dealer's upcard, and the true count (for strategy)
    virtual Action getAction(Hand& user, Hand& dealer, float trueCount) = 0;
};

#endif
