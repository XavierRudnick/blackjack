#ifndef COUNTINGSTRATEGY_H
#define COUNTINGSTRATEGY_H

#include "Card.h"

class CountingStrategy {
    public:
        virtual int getBetSize() = 0;
        virtual void updateCount(Card card) = 0;
        virtual void updateDeckSize(int num_cards_left) = 0;

        virtual float getTrueCount() const = 0;
        virtual float getDecksLeft() const = 0;
        virtual float getRunningCount() const = 0;
        
        virtual bool shouldAcceptInsurance() const = 0;
        virtual ~CountingStrategy() = default;
};

#endif