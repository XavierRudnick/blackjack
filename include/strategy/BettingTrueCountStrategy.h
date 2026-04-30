#ifndef BETTINGTRUECOUNTSTRATEGY_H
#define BETTINGTRUECOUNTSTRATEGY_H

#include "CountingStrategy.h"

class BettingTrueCountStrategy {
public:
    virtual float getBettingTrueCount() const = 0;
    virtual ~BettingTrueCountStrategy() = default;
};

inline float getBettingTrueCountFor(const CountingStrategy& strategy) {
    if (const auto* bettingStrategy = dynamic_cast<const BettingTrueCountStrategy*>(&strategy)) {
        return bettingStrategy->getBettingTrueCount();
    }
    return strategy.getTrueCount();
}

#endif
