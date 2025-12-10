#ifndef KOSTRATEGY_H
#define KOSTRATEGY_H

#include "Card.h"
#include "Deck.h"
#include "CountingStrategy.h"

class KoStrategy : public CountingStrategy { //in docs note deck size is counted 100% accuratly in half size increments
    private:
        float num_decks_left = 0;
        float true_count = 0; //initial running count for KO system
        static const int INDEX_OFFSET = 2; // Since dealer upcards start from 2
    public:
        KoStrategy(float deck_size);
        int getBetSize() override;
        void updateCount(Card card) override;       
        void updateDeckSize(int num_cards_left) override;
        float getTrueCount() const override;
        float getDecksLeft() const override;
        float getRunningCount() const override;
        bool shouldAcceptInsurance() const override;

        ~KoStrategy() override = default;
};

#endif
