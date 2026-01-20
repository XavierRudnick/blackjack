#ifndef MONTECARLOSCENARIO_H
#define MONTECARLOSCENARIO_H

#include <vector>
#include <set>
#include <string>
#include "action.h"

/**
 * Defines a single Monte Carlo scenario for tracking EV across different actions.
 * Multiple scenarios can be tracked simultaneously in one simulation run.
 */
struct MonteCarloScenario {
    std::string name;                           // e.g., "Hit_vs_Stand", "InsuranceAccept_vs_Decline"
    std::vector<Action> actions;                // Actions to compare (e.g., {Hit, Stand})
    std::set<std::pair<int, int>> cardValues;   // Player/Dealer value pairs to track
    bool allowSoftHands = false;                // Whether soft hands are allowed (true for insurance scenarios)
    bool requirePair = false;                   // Whether a splittable pair is required (for split scenarios)
    bool isInsuranceScenario = false;           // Insurance scenarios are handled before regular play
    
    // Check if this scenario applies to the given hand state
    bool appliesTo(int playerScore, int dealerUpcard, bool isSoftHand, bool canSplit) const {
        // Check if this card value pair is tracked
        if (cardValues.find({playerScore, dealerUpcard}) == cardValues.end()) {
            return false;
        }
        
        // Check soft hand restriction
        if (!allowSoftHands && isSoftHand) {
            return false;
        }
        
        // Check pair requirement
        if (requirePair && !canSplit) {
            return false;
        }
        
        return true;
    }
};

#endif // MONTECARLOSCENARIO_H
