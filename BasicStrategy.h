#ifndef BASICSTRATEGY_H
#define BASICSTRATEGY_H

#include "rank.h"
#include <cstdint>

enum class Action : uint8_t {
    Stand,
    Hit,
    Double,
    Split
};

class BasicStrategy {
public:
    static Action getHardHandAction(int playerTotal, Rank dealerUpcard);
    static Action getSoftHandAction(int playerTotal, Rank dealerUpcard);
    static Action getSplitAction(Rank playerSplitRank, Rank dealerUpcard);
    
private:
    // Convert dealer's upcard rank to index (0-9: 2-10/Face, Ace)
    static int getDealerIndex(Rank dealerUpcard);
    
    // Lookup tables based on basic strategy
    static const Action hardTotalTable[16][10];
    static const Action softTotalTable[9][10];
    static const Action splitTable[10][10];
};

#endif
