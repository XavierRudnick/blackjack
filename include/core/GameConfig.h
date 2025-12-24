#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <vector>
#include <action.h>

struct GameConfig{
        int numDecks = 2;
        double wallet = 1000.0;
        float penetrationThreshold = .75;
        double blackjackPayoutMultiplier = 1.5; 
        bool dealerHitsSoft17 = false;
        bool doubleAfterSplitAllowed = true;
        bool allowReSplitAces = true;
        bool allowSurrender = false;
        bool emitEvents = false;
        bool enabelMontiCarlo = false;
        int userHandValue = 0;
        int dealerUpcardValue = 0;
        std::vector<Action> monteCarloActions;
};

#endif