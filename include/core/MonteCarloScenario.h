#ifndef MONTECARLOSCENARIO_H
#define MONTECARLOSCENARIO_H

#include <vector>
#include <set>
#include <string>
#include "action.h"

struct MonteCarloScenario {
    std::string name;                       
    std::vector<Action> actions;            
    std::set<std::pair<int, int>> cardValues;  
    bool allowSoftHands = false;            
    bool requirePair = false;                 
    bool isInsuranceScenario = false;          
    

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

#endif
