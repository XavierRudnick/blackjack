#ifndef HILOSTRATEGY_H
#define HILOSTRATEGY_H

#include "Card.h"
#include "action.h"
#include "CountingStrategy.h"
#include "BasicStrategy.h"

class HiLoStrategy : public CountingStrategy { //in docs note deck size is counted 100% accuratly in half size increments
    private:
        float true_count = 0;
        float running_count = 0;
        float num_decks_left = 0;
        float initial_decks = 0;
        static const int INDEX_OFFSET = 2; // Since dealer upcards start from 2
    public:
        HiLoStrategy(float deck_size);
        int getBetSize() override;
        void updateCount(Card card) override;       
        void updateDeckSize(int num_cards_left) override;
        
        float getTrueCount() const override;
        float getDecksLeft() const override;
        float getRunningCount() const override;

        bool shouldAcceptInsurance() const override;
        Action shouldDeviatefromHard(int playerTotal, Rank dealerUpcard,float true_count) override;
        Action shouldDeviatefromSplit(Rank playerSplitRank, Rank dealerUpcard,float true_count) override;
        Action shouldSurrender(int playerTotal, Rank dealerUpcard,float true_count) override;

        Action getHardHandAction(int playerTotal, Rank dealerUpcard,float true_count) override;
        Action getSoftHandAction(int playerTotal, Rank dealerUpcard) override;
        Action getSplitAction(Rank playerSplitRank, Rank dealerUpcard,float true_count) override;

        void reset(int deckSize) override;
        std::string getName() override;

        ~HiLoStrategy() override = default;
};

#endif
