#include "BotPlayer.h"

BotPlayer::BotPlayer(bool allowSurrender) : allowSurrender(allowSurrender) {}

Action BotPlayer::getAction(Hand& user, Hand& dealer, float trueCount) {
    Rank dealer_card = dealer.peekFrontCard();

    if(user.checkCanDouble() && allowSurrender){
        Action action = BasicStrategy::shouldSurrender(user.getScore(), dealer_card, trueCount);
        if (action == Action::Surrender) {
            return action;
        }
    }

    if(user.checkCanSplit()){
        return BasicStrategy::getSplitAction(user.peekFrontCard(), dealer_card, trueCount);
    }

    int playerTotal = user.getScore();

    if(user.isHandSoft()){
        Action action = BasicStrategy::getSoftHandAction(playerTotal, dealer_card);

        if (action == Action::Double){
            if (user.checkCanDouble()){
                return action;
            }
            else if (user.checkShouldStand()){
                return Action::Stand;
            }
            else{
                return Action::Hit;
            }
        }
        return action;
    }
    else{
        Action action = BasicStrategy::getHardHandAction(playerTotal, dealer_card, trueCount);
        if (action == Action::Double){
            if (user.checkCanDouble()){
                return action;
            }
            else if(user.checkShouldStand()){
                return Action::Stand;
            }
            else{
                return Action::Hit;
            }
        }
        return action;
    }
}
