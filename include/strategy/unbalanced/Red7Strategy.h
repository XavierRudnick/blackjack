#ifndef RED7STRATEGY_H
#define RED7STRATEGY_H

#include "Card.h"
#include "Deck.h"
#include "CountingStrategy.h"
#include "BasicStrategy.h"

class Red7Strategy : public CountingStrategy { //in docs note deck size is counted 100% accuratly in half size increments
    private:
        float num_decks_left = 0;
        float true_count = 0; //initial running count for KO system
        float deckStartSize = 0;
        static const int INDEX_OFFSET = 2; // Since dealer upcards start from 2
    public:
        Red7Strategy(float deck_size);
        int getBetSize() override;
        void updateCount(Card card) override;       
        void updateDeckSize(int num_cards_left) override;
        float getTrueCount() const override;
        float getDecksLeft() const override;
        float getRunningCount() const override;
        bool shouldAcceptInsurance() const override;

        Action shouldDeviatefromHard(int playerTotal, Rank dealerUpcard,float true_count=0) override;
        Action shouldDeviatefromSplit(Rank playerSplitRank, Rank dealerUpcard,float true_count=0) override;
        Action shouldSurrender(int playerTotal, Rank dealerUpcard,float true_count=0) override;

        Action getHardHandAction(int playerTotal, Rank dealerUpcard,float true_count) override;
        Action getSoftHandAction(int playerTotal, Rank dealerUpcard) override;
        Action getSplitAction(Rank playerSplitRank, Rank dealerUpcard,float true_count) override;

        ~Red7Strategy() override = default;
};

#endif
