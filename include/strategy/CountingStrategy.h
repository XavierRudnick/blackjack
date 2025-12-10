#ifndef COUNTINGSTRATEGY_H
#define COUNTINGSTRATEGY_H

#include "action.h"
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

        virtual Action shouldDeviatefromHard(int playerTotal, Rank dealerUpcard,float true_count);
        virtual Action shouldDeviatefromSplit(Rank playerSplitRank, Rank dealerUpcard,float true_count);
        virtual Action shouldSurrender(int playerTotal, Rank dealerUpcard,float true_count);

        virtual Action getHardHandAction(int playerTotal, Rank dealerUpcard,float true_count)= 0 ;
        virtual Action getSoftHandAction(int playerTotal, Rank dealerUpcard)= 0 ;
        virtual Action getSplitAction(Rank playerSplitRank, Rank dealerUpcard,float true_count)= 0 ;
};

#endif