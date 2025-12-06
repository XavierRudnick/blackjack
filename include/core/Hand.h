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
        void addCard(Card card);

        bool checkCanSplit();
        bool checkCanDouble();
        bool checkShouldStand();

        bool checkOver();
        bool isDealerOver();

        int getScore();
        int getFinalScore();

        bool OfferInsurance();
        bool dealerHiddenTen();
        bool dealerShowsTen();
        bool dealerHiddenAce();
        Rank peekFrontCard();

        bool isBlackjack();
        bool isHandSoft();
        bool isSoft17();
        bool isAces();
        std::vector<Card> getCards() const;
};

#endif
