#ifndef NOSTRATEGY_H
#define NOSTRATEGY_H

#include "CountingStrategy.h"
#include "Card.h"

class NoStrategy : public CountingStrategy { //in docs note deck size is counted 100% accuratly in half size increments
    private:
        float num_decks_left = 0;
    public:
        NoStrategy(float deck_size);
        int getBetSize() override;
        void updateCount(Card card) override;
        void updateDeckSize(int num_cards_left) override;
        float getTrueCount() override;
        float getDecksLeft() override;
        float getRunningCount() override;
        bool shouldAcceptInsurance() override;
};

#endif
