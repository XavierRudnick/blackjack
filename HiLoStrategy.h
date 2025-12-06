#ifndef HILOSTRATEGY_H
#define HILOSTRATEGY_H

#include "Card.h"
#include "CountingStrategy.h"

class HiLoStrategy : public CountingStrategy { //in docs note deck size is counted 100% accuratly in half size increments
    private:
        float true_count = 0;
        float running_count = 0;
        float num_decks_left = 0;
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

        ~HiLoStrategy() override = default;
};

#endif
