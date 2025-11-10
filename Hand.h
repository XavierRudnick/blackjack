#ifndef HAND_H
#define HAND_H

#include <vector>
#include "Card.h"

class Hand{
    private:
        std::vector<Card> hand;
        int bet_size_;
    public:
        Hand(std::pair<Card,Card> cards, int bet_size);
        Hand(Card card, int bet_size);
        
        int getBetSize();
        void doubleBet();
        Card get_second_card();
        void pop_second_card();
        void show_cards();
        void dealer_show_cards();
        void peek_dealer();
        bool check_can_split();
        void addCard(Card card);
        bool check_over();
        int getScoreHard();
        int getFinalScore();
        int getDealerScore();
        int getFinalDealerScore();
        bool OfferInsurance();
        bool dealerHiddenTen();
        bool isDealerOver();
        int getScoreSoft();
        bool isSoftHand();
};

#endif
