#include <iostream>
#include "BasicStrategy.h"
#include "rank.h"

// Helper function to convert Action enum to string
std::string actionToString(Action action) {
    switch (action) {
        case Action::Stand:  return "Stand";
        case Action::Hit:    return "Hit";
        case Action::Double: return "Double";
        case Action::Split:  return "Split";
        default:             return "Unknown";
    }
}

// Example usage of the BasicStrategy lookup tables
int main() {
    std::cout << "=== Basic Strategy Lookup Table Examples ===" << std::endl << std::endl;
    
    // Example 1: Hard 16 vs Dealer 10
    std::cout << "Example 1: Player has hard 16, Dealer shows 10" << std::endl;
    Action action1 = BasicStrategy::getHardHandAction(16, Rank::Ten);
    std::cout << "Optimal action: " << actionToString(action1) << std::endl << std::endl;
    
    // Example 2: Hard 11 vs Dealer 6
    std::cout << "Example 2: Player has hard 11, Dealer shows 6" << std::endl;
    Action action2 = BasicStrategy::getHardHandAction(11, Rank::Six);
    std::cout << "Optimal action: " << actionToString(action2) << std::endl << std::endl;
    
    // Example 3: Soft 18 (A-7) vs Dealer 9
    std::cout << "Example 3: Player has soft 18 (A-7), Dealer shows 9" << std::endl;
    Action action3 = BasicStrategy::getSoftHandAction(18, Rank::Nine);
    std::cout << "Optimal action: " << actionToString(action3) << std::endl << std::endl;
    
    // Example 4: Soft 17 (A-6) vs Dealer 5
    std::cout << "Example 4: Player has soft 17 (A-6), Dealer shows 5" << std::endl;
    Action action4 = BasicStrategy::getSoftHandAction(17, Rank::Five);
    std::cout << "Optimal action: " << actionToString(action4) << std::endl << std::endl;
    
    // Example 5: Pair of 8s vs Dealer Ace
    std::cout << "Example 5: Player has pair of 8s, Dealer shows Ace" << std::endl;
    Action action5 = BasicStrategy::getPairAction(Rank::Eight, Rank::Ace);
    std::cout << "Optimal action: " << actionToString(action5) << std::endl << std::endl;
    
    // Example 6: Pair of 5s vs Dealer 7
    std::cout << "Example 6: Player has pair of 5s, Dealer shows 7" << std::endl;
    Action action6 = BasicStrategy::getPairAction(Rank::Five, Rank::Seven);
    std::cout << "Optimal action: " << actionToString(action6) << std::endl << std::endl;
    
    // Example 7: Pair of Aces vs Dealer 10
    std::cout << "Example 7: Player has pair of Aces, Dealer shows 10" << std::endl;
    Action action7 = BasicStrategy::getPairAction(Rank::Ace, Rank::Ten);
    std::cout << "Optimal action: " << actionToString(action7) << std::endl << std::endl;
    
    // Example 8: Hard 12 vs Dealer 4 (when doubling not allowed)
    std::cout << "Example 8: Player has hard 12, Dealer shows 4, cannot double" << std::endl;
    Action action8 = BasicStrategy::getHardHandAction(12, Rank::Four, false);
    std::cout << "Optimal action: " << actionToString(action8) << std::endl << std::endl;
    
    return 0;
}
