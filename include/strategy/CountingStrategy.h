#ifndef COUNTINGSTRATEGY_H
#define COUNTINGSTRATEGY_H

#include <string>
#include <sstream>

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
        virtual float getBettingTrueCount() const { return getTrueCount(); }
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

        // Standard EV-per-TC CSV path under data/ev_per_tc_data/evPerTC/<Strategy>/.
        // Mirrors the filename pattern used by runRTPsimsWithResults / collectStrategyDeviationEvRtpDataset.
        // Strategies with non-standard CSV layouts can override.
        virtual std::string defaultEvCsvPath(int numDecks, float penetration, bool h17,
                                             bool das, bool ras, bool surrender, bool bj3to2) {
            std::ostringstream ss;
            ss << "data/ev_per_tc_data/evPerTC/" << getName()
               << "/ev_per_tc_" << getName() << "_"
               << numDecks << "deck_"
               << static_cast<int>(penetration * 100.0f + 0.5f) << "pen_"
               << (h17 ? "H17" : "S17") << "_"
               << (das ? "DAS" : "NoDAS") << "_"
               << (ras ? "RAS" : "NoRAS") << "_"
               << (surrender ? "Surrender" : "NoSurrender") << "_"
               << (bj3to2 ? "3to2" : "6to5") << ".csv";
            return ss.str();
        }

        // Minimum and maximum bet constants (centralized defaults)
        static constexpr int MIN_BET = 100;
        static constexpr int MAX_BET = 5000;
        // Runtime getter to allow per-strategy overrides when needed
        virtual int getMaxBet() const { return MAX_BET; }

        virtual ~CountingStrategy() = default;

    protected:
        // Default unit sizing stored in base for convenience
        float unitSize = 25.0f;

};

#endif
