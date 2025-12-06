#include "BotPlayer.h"

BotPlayer::BotPlayer(bool allowSurrender) : allowSurrender(allowSurrender) {}

Action BotPlayer::getAction(Hand& user, Hand& dealer, float trueCount) {
    Rank dealer_card = dealer.peek_front_card();

    if(user.check_can_double() && allowSurrender){
        Action action = BasicStrategy::shouldSurrender(user.getScore(), dealer_card, trueCount);
        if (action == Action::Surrender) {
            return action;
        }
    }

    if(user.check_can_split()){
        return BasicStrategy::getSplitAction(user.peek_front_card(), dealer_card, trueCount);
    }

    int playerTotal = user.getScore();

    if(user.isHandSoft()){
        Action action = BasicStrategy::getSoftHandAction(playerTotal, dealer_card);

        if (action == Action::Double){
            if (user.check_can_double()){
                return action;
            }
            else if (user.check_should_stand()){
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
            if (user.check_can_double()){
                return action;
            }
            else if(user.check_should_stand()){
                return Action::Stand;
            }
            else{
                return Action::Hit;
            }
        }
        return action;
    }
}
