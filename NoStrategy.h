#ifndef NOSTRATEGY_H
#define NOSTRATEGY_H

#include "Card.h"

class NoStrategy { //in docs note deck size is counted 100% accuratly in half size increments
    public:
        float num_decks_left;
        NoStrategy(float deck_size);
        int getBetSize() const;
        void updateCount(Card card);
        void updateDeckSize(int num_cards_left);
        float getCount();
        float getDecksLeft();
        float getRunningCount();
        bool shouldAcceptInsurance();
};

#endif
