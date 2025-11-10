#ifndef ENGINE_H
#define ENGINE_H

#include <optional>
#include <vector>
#include "Deck.h"
#include "Hand.h"

template <typename Strategy>
struct Engine{
    std::optional<Deck<Strategy>> deck;
    const uint8_t number_of_decks;

    Engine(uint8_t deck_size, Strategy strategy) : number_of_decks(deck_size){
        deck.emplace(deck_size, std::move(strategy));
        runner();
    }

    void runner(){
        std::cout << "starting a " << static_cast<int>(number_of_decks) << " deck game!" << std::endl;
        int total = 100;
        while (deck->getSize() > number_of_decks * 13){
            std::cout << "========================================" << std::endl;
            deck->getStrategy().updateDeckSize(deck->getSize()); 
            int betSize = deck->getBetSize();
            Hand dealer = draw_cards();
            Hand user = draw_cards(betSize);

            //implement insurance here

            peek_dealer(dealer);
            std::vector<Hand> hands = user_play(user);
            dealer_draw(dealer);
            
            int dealer_score = dealer.getDealerScore();

            for (Hand hand : hands){
                int score = hand.getFinalScore();
                if (dealer_score > score){
                    total -= hand.getBetSize();
                }
                else if (dealer_score < score){
                    total += hand.getBetSize();
                }
            }

            std::cout << "total : " << total << std::endl;
            std::cout << "true count : " << deck->getStrategy().getCount() << std::endl;
        }    
    }

    std::vector<Hand> user_play(Hand& user){
        std::vector<Hand> hands;
        hands.reserve(4); //unnessesary but fuck it I do what I want
        user_play_hand(user, hands);
        return hands;
    }

    void user_play_hand(Hand& user, std::vector<Hand>& hands){ //need to implement strategy for here
        int choice;
        bool game_over = false;
        print_hand(user);

        while(!game_over){
            std::cin >> choice;
            std::cout << std::endl;
            switch(choice)
            {
                case 0:
                    print_hand(user);
                    game_over = true;
                    hands.emplace_back(user);
                    break;
                case 1:
                    user.addCard(deck->hit());
                    if (user.check_over()) {game_over = true; hands.emplace_back(user);}
                    print_hand(user);
                    break;
                case 2:
                    user.doubleBet();
                    user.addCard(deck->hit());
                    print_hand(user);
                    game_over = true;
                    hands.emplace_back(user);
                    break;
                case 3:
                    if (user.check_can_split()){
                        Hand user2 = Hand(user.get_second_card(),user.getBetSize());
                        user.pop_second_card();
                        user.addCard(deck->hit());
                        user2.addCard(deck->hit());

                        user_play_hand(user2,hands);
                        print_hand(user);
                    }
                    break;
            }
        }
    }

    void print_initial(Hand dealer, Hand user){
        peek_dealer(dealer);
        print_hand(user);
    }

    void print_state(Hand dealer, Hand user){
        dealer.dealer_show_cards();
        print_hand(user);
    }

    void print_hand(Hand user){
        std::cout << "Your card" << std::endl;
        user.show_cards();
        std::cout << "0: Stand, 1: Hit, 2: Double, 3: Stand" << std::endl;
        std::cout << std::endl;
    }

    void peek_dealer(Hand dealer){
        std::cout << "Dealer card" << std::endl;
        dealer.peek_dealer();
        std::cout << std::endl;
    }

    Hand draw_cards(int betSize = 0){
        Hand hand = Hand(deck->deal(), betSize);
        return hand;
    }

    void dealer_draw(Hand& dealer){
        dealer.dealer_show_cards();
        if (dealer.isDealerOver()){
            return;
        }
        while (!dealer.isDealerOver()){
            dealer.addCard(deck->hit());
            dealer.dealer_show_cards();
        }
        return;
    }
};

#endif
