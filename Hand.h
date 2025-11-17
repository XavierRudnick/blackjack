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
        Card getLastCard();
        void popLastCard();
        void show_cards();
        void dealer_show_cards();
        void peek_dealer();
        bool check_can_split();
        bool check_can_double();
        void addCard(Card card);
        bool check_over();
        int getScore();
        int getFinalScore();
        bool OfferInsurance();
        bool dealerHiddenTen();
        bool isDealerOver();
        Rank peek_front_card();
        bool dealerShowsTen();
        bool dealerHiddenAce();
        bool isBlackjack();
        bool isHandSoft();
};

#endif
