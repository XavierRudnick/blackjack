#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "Player.h"
#include <iostream>

class HumanPlayer : public Player {
private:
    bool allowSurrender;
public:
    HumanPlayer(bool allowSurrender = false);
    Action getAction(Hand& user, Hand& dealer, float trueCount) override;
    Action getOptimalAction(Hand& user, Hand& dealer, float trueCount);
};

#endif
