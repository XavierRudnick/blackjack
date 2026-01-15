#ifndef GAMECONFIG_H
#define GAMECONFIG_H

#include <vector>
#include <action.h>
#include <set>

struct GameConfig{
        int numDecks = 2;
        double wallet = 1000.0;
        float penetrationThreshold = .75;
        double blackjackPayoutMultiplier = 1.5; 
        bool dealerHitsSoft17 = true;
        bool doubleAfterSplitAllowed = true;
        bool allowReSplitAces = true;
        bool allowSurrender = false;
        bool emitEvents = false;
        bool enabelMontiCarlo = false;
        std::set<std::pair<int, int>> actionValues;
        bool allowSoftHandsInMonteCarlo = false;
        bool requirePairForMonteCarlo = false;  // For split scenarios, only trigger if hand is a splittable pair
        std::vector<Action> monteCarloActions;
};

#endif