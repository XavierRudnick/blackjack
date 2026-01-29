#ifndef R14STRATEGY_H
#define R14STRATEGY_H

#include "Card.h"
#include "Deck.h"
#include "action.h"
#include "CountingStrategy.h"
#include "BasicStrategy.h"

class R14Strategy : public CountingStrategy { //in docs note deck size is counted 100% accuratly in half size increments
    private:
        float true_count = 0;
        float running_count = 0;
        float num_decks_left = 0;
        float initial_decks = 0;
        float unitSize = 25;
        float kellyFraction = 0.5f;
        
        static constexpr int MIN_BET = 25;
        static const int INDEX_OFFSET = 2; // Since dealer upcards start from 2
        static constexpr float evPerTC = .000225f; // 0.0225% per TC from 2deck 75pen data
        static constexpr float avgVolatility = 1.32f;
        static constexpr float PROFITABLE_PLAY_TC_THRESHOLD = 39.0f; // R14 profitable at TC >= 39.0
        int getEvenBet() const;
    public:
        R14Strategy(float deck_size);
        int getBetSize() override;
        void setUnitSize(float kellyFraction) override;
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

        ~R14Strategy() override = default;
};

#endif
