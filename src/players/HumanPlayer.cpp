#include "HumanPlayer.h"
#include "BasicStrategy.h"
#include <limits>

HumanPlayer::HumanPlayer(bool allowSurrender, std::unique_ptr<CountingStrategy> strat)
    : allowSurrender(allowSurrender), strategy(std::move(strat)) {}

CountingStrategy* HumanPlayer::getStrategy() {
    return strategy.get();
}

void HumanPlayer::updateDeckStrategySize(int num_cards_left) {
    strategy->updateDeckSize(num_cards_left);
    return;
}

int HumanPlayer::getBetSize() {
    return strategy->getBetSize();
}

void HumanPlayer::updateCount(Card card) {
    strategy->updateCount(card);
    return;
    
}

float HumanPlayer::getTrueCount() {
    return strategy->getTrueCount();
}

bool HumanPlayer::shouldAcceptInsurance() {
    return strategy->shouldAcceptInsurance();
}

Action HumanPlayer::getOptimalAction(Hand& user, Hand& dealer, float trueCount) {
    Action optimalAction = Action::Stand;
    Rank dealer_card = dealer.peekFrontCard();

    bool found = false;
    if(user.checkCanDouble() && allowSurrender){
        Action action = strategy->shouldSurrender(user.getScore(), dealer_card, trueCount);
        if (action == Action::Surrender) {
            optimalAction = action;
            found = true;
        }
    }

    if(!found && user.checkCanSplit()){
        optimalAction = strategy->getSplitAction(user.peekFrontCard(), dealer_card, trueCount);
        found = true;
    }

    if (!found) {
        int playerTotal = user.getScore();
        if(user.isHandSoft()){
            Action action = strategy->getSoftHandAction(playerTotal, dealer_card);
            if (action == Action::Double){
                if (user.checkCanDouble()){
                    optimalAction = action;
                } else if (user.checkShouldStand()){
                    optimalAction = Action::Stand;
                } else{
                    optimalAction = Action::Hit;
                }
            } else {
                optimalAction = action;
            }
        } else {
            Action action = strategy->getHardHandAction(playerTotal, dealer_card, trueCount);
            if (action == Action::Double){
                if (user.checkCanDouble()){
                    optimalAction = action;
                } else if(user.checkShouldStand()){
                    optimalAction = Action::Stand;
                } else{
                    optimalAction = Action::Hit;
                }
            } else {
                optimalAction = action;
            }
        }
    }

    return optimalAction;
}

Action HumanPlayer::getAction(Hand& user, Hand& dealer, float trueCount) {
    Action optimalAction = getOptimalAction(user, dealer, trueCount);

    int choice;
    std::cout << "Your hand score: " << user.getScore() << std::endl;
    std::cout << "Choose action (0: Stand, 1: Hit, 2: Double, 3: Split, 4: Surrender): ";
    
    while (!(std::cin >> choice) || choice < 0 || choice > 4) {
        std::cout << "Invalid input. Try again: ";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    std::cout << "Optimal action was " << optimalAction << std::endl;

    switch (choice) {
        case 0: return Action::Stand;
        case 1: return Action::Hit;
        case 2: return Action::Double;
        case 3: return Action::Split;
        case 4: return Action::Surrender;
        default: return Action::Stand;
    }
}
