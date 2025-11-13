#ifndef BASICSTRATEGY_H
#define BASICSTRATEGY_H

#include "rank.h"
#include "action.h"
#include <cstdint>

class BasicStrategy {
    private:
        static const Action hardTotalTable[16][10];
        static const Action softTotalTable[9][10];
        static const Action splitTable[10][10];
        
        static int getIndex(Rank dealerUpcard);
        static Action shouldDeviatefromHard(int playerTotal, Rank dealerUpcard,float true_count);
        static Action shouldDeviatefromSplit(Rank playerSplitRank, Rank dealerUpcard,float true_count);

    public:
        static Action getHardHandAction(int playerTotal, Rank dealerUpcard,float true_count);
        static Action getSoftHandAction(int playerTotal, Rank dealerUpcard);
        static Action getSplitAction(Rank playerSplitRank, Rank dealerUpcard,float true_count);

};

#endif
