#ifndef HILOSTRATEGY_H
#define HILOSTRATEGY_H

#include "Card.h"

class HiLoStrategy { //in docs note deck size is counted 100% accuratly in half size increments
    private:
        int true_count = 0;
        float num_decks_left;
    public:
        HiLoStrategy(float deck_size);
        int getBetSize() const;
        void updateCount(Card card);
        void updateDeckSize(int num_cards_left);
        int getCount();
};

#endif
