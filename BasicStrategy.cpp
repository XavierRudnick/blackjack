#include "BasicStrategy.h"

//implement illustrias 18 for deviations from uncounted optimal table  

// Hard total strategy table (player total 5-20 vs dealer 2-9, 10, A)
// Rows: player totals 5-20
// Columns: dealer upcard 2, 3, 4, 5, 6, 7, 8, 9, 10/J/Q/K, A
const Action BasicStrategy::hardTotalTable[16][10] = {
    // Dealer:  2               3           4              5            6           7            8            9           10           A
    /* 5  */ {Action::Hit,   Action::Hit,  Action::Hit,    Action::Hit, Action::Hit, Action::Hit, Action::Hit, Action::Hit, Action::Hit, Action::Hit},
    /* 6  */ {Action::Hit,   Action::Hit,  Action::Hit,    Action::Hit, Action::Hit, Action::Hit, Action::Hit, Action::Hit, Action::Hit, Action::Hit},
    /* 7  */ {Action::Hit,   Action::Hit,  Action::Hit,    Action::Hit, Action::Hit, Action::Hit, Action::Hit, Action::Hit, Action::Hit, Action::Hit},
    /* 8  */ {Action::Hit,   Action::Hit,  Action::Hit,    Action::Hit, Action::Hit, Action::Hit, Action::Hit, Action::Hit, Action::Hit, Action::Hit},
    /* 9  */ {Action::Hit,   Action::Double, Action::Double, Action::Double, Action::Double, Action::Hit, Action::Hit, Action::Hit, Action::Hit, Action::Hit},
    /* 10 */ {Action::Double, Action::Double, Action::Double, Action::Double, Action::Double, Action::Double, Action::Double, Action::Double, Action::Hit, Action::Hit},
    /* 11 */ {Action::Double, Action::Double, Action::Double, Action::Double, Action::Double, Action::Double, Action::Double, Action::Double, Action::Double, Action::Hit},
    /* 12 */ {Action::Hit,   Action::Hit,   Action::Stand, Action::Stand, Action::Stand, Action::Hit,   Action::Hit,   Action::Hit,   Action::Hit,   Action::Hit},
    /* 13 */ {Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Hit,   Action::Hit,   Action::Hit,   Action::Hit,   Action::Hit},
    /* 14 */ {Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Hit,   Action::Hit,   Action::Hit,   Action::Hit,   Action::Hit},
    /* 15 */ {Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Hit,   Action::Hit,   Action::Hit,   Action::Hit,   Action::Hit},
    /* 16 */ {Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Hit,   Action::Hit,   Action::Hit,   Action::Hit,   Action::Hit},
    /* 17 */ {Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand},
    /* 18 */ {Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand},
    /* 19 */ {Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand},
    /* 20 */ {Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand}
};

// Soft total strategy table (player soft total 13-21 vs dealer 2-9, 10, A)
// Soft 13 = A-2, Soft 14 = A-3, ..., Soft 21 = A-10
// Rows: soft totals 13-21
// Columns: dealer upcard 2, 3, 4, 5, 6, 7, 8, 9, 10/J/Q/K, A
const Action BasicStrategy::softTotalTable[9][10] = {
    // Dealer:          2             3             4            5                6           7              8           9           10             A
    /* A-2 (13) */ {Action::Hit, Action::Hit, Action::Hit, Action::Double, Action::Double, Action::Hit, Action::Hit, Action::Hit, Action::Hit, Action::Hit},
    /* A-3 (14) */ {Action::Hit, Action::Hit, Action::Hit, Action::Double, Action::Double, Action::Hit, Action::Hit, Action::Hit, Action::Hit, Action::Hit},
    /* A-4 (15) */ {Action::Hit, Action::Hit, Action::Double, Action::Double, Action::Double, Action::Hit, Action::Hit, Action::Hit, Action::Hit, Action::Hit},
    /* A-5 (16) */ {Action::Hit, Action::Hit, Action::Double, Action::Double, Action::Double, Action::Hit, Action::Hit, Action::Hit, Action::Hit, Action::Hit},
    /* A-6 (17) */ {Action::Hit, Action::Double, Action::Double, Action::Double, Action::Double, Action::Hit, Action::Hit, Action::Hit, Action::Hit, Action::Hit},
    /* A-7 (18) */ {Action::Stand, Action::Double, Action::Double, Action::Double, Action::Double, Action::Stand, Action::Stand, Action::Hit, Action::Hit, Action::Hit},
    /* A-8 (19) */ {Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand},
    /* A-9 (20) */ {Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand},
    /* A-10(21) */ {Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand}
};

// Pair strategy table (pair rank vs dealer upcard)
// Rows: pairs 2-2, 3-3, 4-4, 5-5, 6-6, 7-7, 8-8, 9-9, 10-10, A-A
// Columns: dealer upcard 2, 3, 4, 5, 6, 7, 8, 9, 10/J/Q/K, A
const Action BasicStrategy::splitTable[10][10] = {
    // Dealer:          2           3                4           5                 6             7             8             9           10          A
    /* 2-2  */ {Action::Split, Action::Split, Action::Split, Action::Split, Action::Split, Action::Split, Action::Hit, Action::Hit, Action::Hit, Action::Hit},
    /* 3-3  */ {Action::Split, Action::Split, Action::Split, Action::Split, Action::Split, Action::Split, Action::Hit, Action::Hit, Action::Hit, Action::Hit},
    /* 4-4  */ {Action::Hit, Action::Hit, Action::Hit, Action::Split, Action::Split, Action::Hit, Action::Hit, Action::Hit, Action::Hit, Action::Hit},
    /* 5-5  */ {Action::Double, Action::Double, Action::Double, Action::Double, Action::Double, Action::Double, Action::Double, Action::Double, Action::Hit, Action::Hit},
    /* 6-6  */ {Action::Split, Action::Split, Action::Split, Action::Split, Action::Split, Action::Hit, Action::Hit, Action::Hit, Action::Hit, Action::Hit},
    /* 7-7  */ {Action::Split, Action::Split, Action::Split, Action::Split, Action::Split, Action::Split, Action::Hit, Action::Hit, Action::Hit, Action::Hit},
    /* 8-8  */ {Action::Split, Action::Split, Action::Split, Action::Split, Action::Split, Action::Split, Action::Split, Action::Split, Action::Split, Action::Split},
    /* 9-9  */ {Action::Split, Action::Split, Action::Split, Action::Split, Action::Split, Action::Stand, Action::Split, Action::Split, Action::Stand, Action::Stand},
    /* 10-10*/ {Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand, Action::Stand},
    /* A-A  */ {Action::Split, Action::Split, Action::Split, Action::Split, Action::Split, Action::Split, Action::Split, Action::Split, Action::Split, Action::Split}
};

int BasicStrategy::getIndex(Rank upcard) {
    switch (upcard) {
        case Rank::Two:   return 0;
        case Rank::Three: return 1;
        case Rank::Four:  return 2;
        case Rank::Five:  return 3;
        case Rank::Six:   return 4;
        case Rank::Seven: return 5;
        case Rank::Eight: return 6;
        case Rank::Nine:  return 7;
        case Rank::Ten:   return 8;
        case Rank::Jack:  return 8;
        case Rank::Queen: return 8;
        case Rank::King:  return 8;
        case Rank::Ace:   return 9;
        default:      return -1;
    }
}

Action BasicStrategy::shouldDeviatefromHard(int playerTotal, Rank dealerUpcard, float true_count){
    int dealerValue = getIndex(dealerUpcard) +2;
    switch (playerTotal) {
        case 16:
            if (dealerValue == 10 && true_count >= 0) {
                return Action::Stand;
            }
            if (dealerValue == 9 && true_count >= 5) {
                return Action::Stand;
            }
            break;
            
        case 15: 
            if (dealerValue == 10 && true_count >= 4) {
                return Action::Stand;
            }
            break;
            
        case 13:
            if (dealerValue == 2 && true_count <= -1) { 
                return Action::Stand;
            }
            if (dealerValue == 3 && true_count <= -2) { 
                return Action::Stand;
            }
            break;

        case 12:
            if (dealerValue == 3 && true_count >= 2) {
                return Action::Stand;
            }
            if (dealerValue == 2 && true_count >= 3) {
                return Action::Stand;
            }
            if (dealerValue == 4 && true_count <= 0){
                return Action::Hit;
            }
            if (dealerValue == 5 && true_count <= -2){
                return Action::Hit;
            }
            if (dealerValue == 6 && true_count <= -1){
                return Action::Hit;
            }
            break;
        case 11:
            if (dealerValue == 11 && true_count >= 1){
                return Action::Double;
            }
            break;
        case 10:
            if (dealerValue == 10 && true_count >=4){
                return Action::Double;
            }
            if (dealerValue == 11 && true_count >= 4){
                return Action::Double;
            }
            break;
        case 9:
            if (dealerValue == 2  && true_count >= 1){
                return Action::Double;
            }
            if (dealerValue == 7  && true_count >= 3){
                return Action::Double;
            }
            break;
        default: return Action::Skip; break;
    }
    return Action::Skip;
}

Action BasicStrategy::shouldDeviatefromSplit(Rank playerRank, Rank dealerUpcard, float true_count){
    int dealerValue = getIndex(dealerUpcard) +2;
    int playerValue = getIndex(playerRank) +2;
    switch (playerValue) {
        case 9:
            if (dealerValue == 7 && true_count >= 4) {
                return Action::Split;
            }
            if (dealerValue == 11 && true_count >= 5) {
                return Action::Split;
            }
            break;
        
        // Commented out, very obvious counting cards when you split on tens    
        // case 10: 
        //     if (dealerValue == 5 && true_count >= 5) {
        //         return Action::Split;
        //     }
        //     if (dealerValue == 4 && true_count >= 6) {
        //         return Action::Split;
        //     }
        //     if (dealerValue == 6 && true_count >= 4) {
        //         return Action::Split;
        //     }
        //     break;
        default: return Action::Skip; break;
    }
    return Action::Skip;
}

Action BasicStrategy::getHardHandAction(int playerTotal, Rank dealerUpcard, float true_count) {
    int dealerIdx = getIndex(dealerUpcard);
    
    // Bounds check: if playerTotal < 5, default to hit
    if (playerTotal < 5) {
        return Action::Hit;
    }
    // Bounds check: if playerTotal > 20, default to stand
    if (playerTotal > 20) {
        return Action::Stand;
    }
    
    int playerIdx = playerTotal - 5;  // Player total 5 maps to index 0
    if (Action deviation = shouldDeviatefromHard(playerTotal, dealerUpcard, true_count); deviation != Action::Skip) {
        return deviation;
    }
    else{
        return hardTotalTable[playerIdx][dealerIdx];
    }
    
}

Action BasicStrategy::getSoftHandAction(int playerTotal, Rank dealerUpcard) {
    int dealerIdx = getIndex(dealerUpcard);
    int playerIdx = playerTotal - 13;  // Soft 13 maps to index 0
    
    Action action = softTotalTable[playerIdx][dealerIdx];

    return action;
}

Action BasicStrategy::getSplitAction(Rank playerSplitRank, Rank dealerUpcard, float true_count) {
    int dealerIdx = getIndex(dealerUpcard);
    int pairIdx = getIndex(playerSplitRank);
    
    if (Action deviation = shouldDeviatefromSplit(playerSplitRank, dealerUpcard, true_count); deviation != Action::Skip) {
        return deviation;
    }
    else{
        return splitTable[pairIdx][dealerIdx];
    }

}
