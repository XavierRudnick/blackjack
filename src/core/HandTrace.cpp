#include "HandTrace.h"

uint8_t computeHandOutcomeCode(const HandTrace& t) {
    if (t.surrendered) return 0;  // surrender counts as a loss

    if (t.split_count > 0) {
        if (t.net_profit > 0.01)  return 6;  // split_net_win
        if (t.net_profit < -0.01) return 7;  // split_net_loss
        return 1;                             // push
    }

    if (t.double_count > 0) {
        if (t.net_profit > 0.01)  return 4;  // double_win
        if (t.net_profit < -0.01) return 5;  // double_loss
        return 1;                             // push
    }

    if (t.net_profit > 1.1) return 3;   // blackjack_win
    if (t.net_profit > 0.01) return 2;  // win
    if (t.net_profit < -0.01) return 0; // loss
    return 1;                            // push
}
