#include "BotPlayer.h"

BotPlayer::BotPlayer(bool allowSurrender, std::unique_ptr<CountingStrategy> strat)
    : allowSurrender(allowSurrender), strategy(std::move(strat)) {}

CountingStrategy* BotPlayer::getStrategy() {
    return strategy.get();
}

void BotPlayer::updateDeckStrategySize(int num_cards_left) {
    strategy->updateDeckSize(num_cards_left);
    return;
}

int BotPlayer::getBetSize() {
    return strategy->getBetSize();
}

void BotPlayer::updateCount(Card card) {
    strategy->updateCount(card);
    return;
}

float BotPlayer::getTrueCount() {
    return strategy->getTrueCount();
}

bool BotPlayer::shouldAcceptInsurance() {
    return strategy->shouldAcceptInsurance();
}

void BotPlayer::resetCount(int deckSize) {
    strategy->reset(deckSize);
    return;
}

std::string BotPlayer::getStrategyName() {
    return strategy->getName();
}

Action BotPlayer::getAction(Hand& user, Hand& dealer, float trueCount) {
    Rank dealer_card = dealer.peekFrontCard();

    if(user.checkCanDouble() && allowSurrender){
        Action action = strategy->shouldSurrender(user.getScore(), dealer_card, trueCount);
        if (action == Action::Surrender) {
            return action;
        }
    }

    if(user.checkCanSplit()){
        return strategy->getSplitAction(user.peekFrontCard(), dealer_card, trueCount);
    }

    int playerTotal = user.getScore();

    if(user.isHandSoft()){
        Action action = strategy->getSoftHandAction(playerTotal, dealer_card);

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
        Action action = strategy->getHardHandAction(playerTotal, dealer_card, trueCount);
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
