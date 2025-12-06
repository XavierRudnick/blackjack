#include "HumanPlayer.h"
#include "BasicStrategy.h"
#include <limits>

HumanPlayer::HumanPlayer(bool allowSurrender) : allowSurrender(allowSurrender) {}

Action HumanPlayer::getAction(Hand& user, Hand& dealer, float trueCount) {
    // Calculate optimal action for hint
    Action optimalAction = Action::Stand; // Default
    Rank dealer_card = dealer.peek_front_card();

    // Logic copied from BotPlayer for hint
    bool found = false;
    if(user.check_can_double() && allowSurrender){
        Action action = BasicStrategy::shouldSurrender(user.getScore(), dealer_card, trueCount);
        if (action == Action::Surrender) {
            optimalAction = action;
            found = true;
        }
    }

    if(!found && user.check_can_split()){
        optimalAction = BasicStrategy::getSplitAction(user.peek_front_card(), dealer_card, trueCount);
        found = true;
    }

    if (!found) {
        int playerTotal = user.getScore();
        if(user.isHandSoft()){
            Action action = BasicStrategy::getSoftHandAction(playerTotal, dealer_card);
            if (action == Action::Double){
                if (user.check_can_double()){
                    optimalAction = action;
                } else if (user.check_should_stand()){
                    optimalAction = Action::Stand;
                } else{
                    optimalAction = Action::Hit;
                }
            } else {
                optimalAction = action;
            }
        } else {
            Action action = BasicStrategy::getHardHandAction(playerTotal, dealer_card, trueCount);
            if (action == Action::Double){
                if (user.check_can_double()){
                    optimalAction = action;
                } else if(user.check_should_stand()){
                    optimalAction = Action::Stand;
                } else{
                    optimalAction = Action::Hit;
                }
            } else {
                optimalAction = action;
            }
        }
    }

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
