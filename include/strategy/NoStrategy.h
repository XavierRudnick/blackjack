#ifndef NOSTRATEGY_H
#define NOSTRATEGY_H

#include "CountingStrategy.h"
#include "Card.h"
#include "action.h"
#include "BasicStrategy.h"

class NoStrategy : public CountingStrategy { //in docs note deck size is counted 100% accuratly in half size increments
    private:
        float num_decks_left = 0;
    public:
        NoStrategy(float deck_size);
        int getBetSize() override;
        void updateCount(Card card) override;
        void updateDeckSize(int num_cards_left) override;
        float getTrueCount() const override;
        float getDecksLeft() const override;
        float getRunningCount() const override;
        bool shouldAcceptInsurance() const override;

        Action getHardHandAction(int playerTotal, Rank dealerUpcard,float true_count) override;
        Action getSoftHandAction(int playerTotal, Rank dealerUpcard) override;
        Action getSplitAction(Rank playerSplitRank, Rank dealerUpcard,float true_count) override;

         ~NoStrategy() override = default;


};

#endif
