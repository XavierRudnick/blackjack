#ifndef BOTPLAYER_H
#define BOTPLAYER_H

#include "Player.h"
#include "BasicStrategy.h"

class BotPlayer : public Player {
private:
    bool allowSurrender;
public:
    BotPlayer(bool allowSurrender = false);
    Action getAction(Hand& user, Hand& dealer, float trueCount) override;
};

#endif
