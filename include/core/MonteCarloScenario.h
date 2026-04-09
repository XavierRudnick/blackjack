#ifndef MONTECARLOSCENARIO_H
#define MONTECARLOSCENARIO_H

#include <vector>
#include <string>
#include "action.h"

struct MonteCarloScenario {
    std::string name;
    std::vector<Action> actions;
    bool cardValueActive[18][10] = {};
    bool allowSoftHands = false;
    bool requirePair = false;
    bool isInsuranceScenario = false;

    void addCardValue(int playerScore, int dealerUpcard) {
        if (playerScore < 4 || playerScore > 21 || dealerUpcard < 2 || dealerUpcard > 11) {
            return;
        }
        cardValueActive[playerScore - 4][dealerUpcard - 2] = true;
    }

    int activeCellCount() const {
        int n = 0;
        for (int p = 0; p < 18; ++p) {
            for (int d = 0; d < 10; ++d) {
                if (cardValueActive[p][d]) {
                    ++n;
                }
            }
        }
        return n;
    }

    bool appliesTo(int playerScore, int dealerUpcard, bool isSoftHand, bool canSplit) const {
        if (playerScore < 4 || playerScore > 21 || dealerUpcard < 2 || dealerUpcard > 11) {
            return false;
        }
        if (!cardValueActive[playerScore - 4][dealerUpcard - 2]) {
            return false;
        }
        if (!allowSoftHands && isSoftHand) {
            return false;
        }
        if (requirePair && !canSplit) {
            return false;
        }
        return true;
    }
};

#endif
