#ifndef BASICSTRATEGY_H
#define BASICSTRATEGY_H

#include "rank.h"
#include "action.h"
#include <cstdint>

class BasicStrategy {
    public:
        static const int INDEX_OFFSET = 2; // Since dealer upcards start from 2
        static const Action hardTotalTable[16][10];
        static const Action softTotalTable[9][10];
        static const Action splitTable[10][10];
        static int getIndex(Rank dealerUpcard);
        

};

#endif
