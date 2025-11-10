#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <optional>

using namespace std;

class BaseStrategy {
    public:
        int getBetSize() const {
            return 1;
        }
        void updateCount(Card card) {
            return;
        }
        void updateCount(Hand hand) {
            return;
        }
        
};

class HiLoStrategy { //in docs note deck size is counted 100% accuratly in half size increments
    private:
        int true_count = 0;
        float num_decks_left;
    public:
        HiLoStrategy(float deck_size){
            num_decks_left = deck_size;
        }

        int getBetSize() const {
            switch(true_count){
                case <= 1.5:
                    return 1;
                case < 3:
                    return 2;
                case < 4:
                    return 3;
                case < 5:
                    return 5;
                case >= 5;
                    return (true_count * 2) - 2
            }
        }

        void updateCount(Card card) {
            Rank rank = card.getRank();
            int score = static_cast<int>(rank) + 2;
            int running_count = 0;

            if (score <= 6){
                running_count += 1;
            }
            else if (score >= 10){
                running_count -= 1;
            }

            true_count += running_count / deck_size_;
            return;
        }

        void updateCount(Hand hand) {
            for (Card card : hand){
                updateCount(card);
            }
        }

        void updateDeckSize(int num_cards_left){
            float decks_left_unrounded = num_cards_left / 52; 
            decks_left_unrounded *= 2.0;
            float decks_left_rounded = round(decks_left_unrounded);
            decks_left_rounded = decks_left_rounded / 2;
            num_decks_left = decks_left_rounded;
            return; //this allows us to round to nearest .5
        }

        int getCount(){
            return true_count;
        }
};