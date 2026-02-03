#ifndef COUNTINGSTRATEGY_H
#define COUNTINGSTRATEGY_H

#include "action.h"
#include "Card.h"

class CountingStrategy {
    public:
        virtual int getBetSize() = 0;
        virtual void updateCount(Card card) = 0;
        virtual void updateDeckSize(int num_cards_left) = 0;
        // Set the Kelly fraction (must be handled by each strategy implementation)
        virtual void setUnitSize(float kellyFraction) = 0;

        virtual float getTrueCount() const = 0;
        virtual float getDecksLeft() const = 0;
        virtual float getRunningCount() const = 0;
        
        virtual bool shouldAcceptInsurance() const = 0;

        // Accessors for bet sizing defaults
        virtual float getUnitSize() const { return unitSize; }
        virtual int getMinBet() const { return MIN_BET; }
        virtual Action shouldDeviatefromHard(int playerTotal, Rank dealerUpcard,float true_count) = 0;
        virtual Action shouldDeviatefromSplit(Rank playerSplitRank, Rank dealerUpcard,float true_count) = 0;
        virtual Action shouldSurrender(int playerTotal, Rank dealerUpcard,float true_count) = 0;

        virtual Action getHardHandAction(int playerTotal, Rank dealerUpcard,float true_count)= 0 ;
        virtual Action getSoftHandAction(int playerTotal, Rank dealerUpcard)= 0 ;
        virtual Action getSplitAction(Rank playerSplitRank, Rank dealerUpcard,float true_count)= 0 ;

        virtual void reset(int deckSize) = 0;
        virtual std::string getName() = 0;

        // Minimum and maximum bet constants (centralized defaults)
        static constexpr int MIN_BET = 25;
        static constexpr int MAX_BET = 2000;
        // Runtime getter to allow per-strategy overrides when needed
        virtual int getMaxBet() const { return MAX_BET; }

        virtual ~CountingStrategy() = default;

    protected:
        // Default unit sizing stored in base for convenience
        float unitSize = 25.0f;

};

#endif