#ifndef ENGINE_H
#define ENGINE_H

#include <optional>
#include <vector>
#include "Deck.h"
#include "Hand.h"
#include "BasicStrategy.h"
#include "HiLoStrategy.h"
#include "NoStrategy.h"

template <typename Strategy>
struct Engine{
    std::optional<Deck<Strategy>> deck;
    const uint8_t number_of_decks;
    int wallet;

    Engine(uint8_t deck_size,int money, Strategy strategy) : number_of_decks(deck_size), wallet(money){
        deck.emplace(number_of_decks, std::move(strategy));
    }

    int runner(){
        //std::cout << "starting a " << static_cast<int>(number_of_decks) << " deck game!" << std::endl;
        while (deck->getSize() > number_of_decks * 13){
            //std::cout << "========================================" << std::endl;

            deck->getStrategy().updateDeckSize(deck->getSize()); 
            int betSize = deck->getBetSize();
            std::vector<Hand> hands;

            Hand dealer = draw_cards();
            Hand user = draw_cards(betSize);
            peek_dealer(dealer);

            if (insuranceHandler(dealer,user,betSize)){
                print_state(dealer, user);
                hands = {user};
            }
            else if (dealerRobberyHandler(dealer,user,betSize)){
                print_state(dealer, user);
                hands = {user};
            }
            else{
                hands = user_play(dealer,user);
                dealer_draw(dealer);
                evaluateHands(dealer,hands);
            }

            //std::cout << "wallet total : " << wallet << std::endl;
            //std::cout << "true count : " << deck->getStrategy().getCount() << std::endl;
        }    
        return wallet;
    }

    void evaluateHands(Hand dealer, std::vector<Hand> hands){
        int dealer_score = dealer.getFinalDealerScore();
        //implement blackjack which checks if size is 2 and total is 21 to pay, unless dealer got natural blackjack too, if not natural u win
        //also implemt dealer gets 10 card and hidden ace and insta flips so u lose
        for (Hand hand : hands){
            int score = hand.getFinalScore();

            if (hand.isBlackjack()){
                wallet += hand.getBetSize() * 1.5;
            } 

            if (dealer_score > score){
                wallet -= hand.getBetSize();
            }
            else if (dealer_score < score){
                wallet += hand.getBetSize();
            }
            if (dealer_score == 0 && score ==0){
                wallet -= hand.getBetSize();
            }
        }

        return;

    }

    std::vector<Hand> user_play(Hand dealer, Hand& user){
        std::vector<Hand> hands;
        hands.reserve(4); //unnessesary but fuck it I do what I want
        user_play_hand(dealer, user, hands);
        return hands;
    }

    Action getAction(Hand dealer, Hand user){
        BasicStrategy strat;
        Rank dealer_card = dealer.peek_front_card();
        if(user.check_can_split()){
            return strat.getSplitAction(user.peek_front_card(),dealer_card,deck->getStrategy().getCount());
        }

        if (user.doesHandHaveAce()) {
            int hardScore = user.getScoreHard();
            int softScore = user.getScoreSoft();
            
            // If hard score busts, use soft score as hard total
            if(hardScore > 21){
                return strat.getHardHandAction(softScore,dealer_card,deck->getStrategy().getCount());
            }
            // If hard score is valid, treat as soft hand
            else{
                return strat.getSoftHandAction(hardScore,dealer_card);
            }
        }

        else {
            return strat.getHardHandAction(user.getScoreHard(),dealer_card,deck->getStrategy().getCount());
        }

    }


    void user_play_hand(Hand dealer, Hand& user, std::vector<Hand>& hands){ 
    
        bool game_over = false;
        print_hand(user); 

        while(!game_over){
            //std::cout << std::endl;
            Action action = getAction(dealer, user);
            switch(action)
            {
                case Action::Stand:
                {
                    print_hand(user);
                    game_over = true;
                    hands.emplace_back(user);
                    break;
                }
                case Action::Hit:
                {
                    if (deck->getSize() < 1) {
                        std::cout << "HOW DID YOU REACH THIS YOU ARE COOKED!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                        game_over = true;
                        hands.emplace_back(user);
                        break;
                    }
                    user.addCard(deck->hit());
                    if (user.check_over()) {game_over = true; hands.emplace_back(user);}
                    print_hand(user);
                    break;
                }
                case Action::Double:
                {
                    if (deck->getSize() < 1) {
                        // Can't double, just stand instead
                        std::cout << "HOW DID YOU REACH THIS YOU ARE COOKED!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                        game_over = true;
                        hands.emplace_back(user);
                        break;
                    }
                    user.doubleBet();
                    user.addCard(deck->hit());
                    print_hand(user);
                    game_over = true;
                    hands.emplace_back(user);
                    break;
                }
                case Action::Split:
                {
                    if (deck->getSize() < 2) {
                        // Can't split, just stand instead
                        std::cout << "HOW DID YOU REACH THIS YOU ARE COOKED!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                        game_over = true;
                        hands.emplace_back(user);
                        break;
                    }
                    Hand user2 = Hand(user.get_second_card(),user.getBetSize());
                    user.pop_second_card();
                    user.addCard(deck->hit());
                    user2.addCard(deck->hit());

                    user_play_hand(dealer,user2,hands);
                    print_hand(user);
                    break;
                }
                case Action::Skip: 
                    //std::cout << "HOW DID YOU REACH THIS YOU ARE COOKED!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                    break;

            }
        }

    }
    void user_play_hand_manual(Hand& user, std::vector<Hand>& hands){ //need to implement strategy for here
        int choice;
        bool game_over = false;
        print_hand(user);

        while(!game_over){
            //std::cin >> choice;
            //std::cout << std::endl;
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

                        user_play_hand_manual(user2,hands);
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
        //std::cout << "Your card" << std::endl;
        user.show_cards();
       // std::cout << "0: Stand, 1: Hit, 2: Double, 3: Stand" << std::endl;
        //std::cout << std::endl;
    }

    void peek_dealer(Hand dealer){
        //std::cout << "Dealer card" << std::endl;
        dealer.peek_dealer();
        //std::cout << std::endl;
    }

    Hand draw_cards(int betSize = 0){
        if (deck->getSize() < 2) {
            throw std::runtime_error("Not enough cards to draw initial hand 258");
        }
        Hand hand = Hand(deck->deal(), betSize);
        return hand;
    }

    void dealer_draw(Hand& dealer){
        dealer.dealer_show_cards();
        if (dealer.isDealerOver()){
            return;
        }
        while (!dealer.isDealerOver()){
            if (deck->getSize() < 1) {
                throw std::runtime_error("Not enough cards to draw initial hand 271");
            }
            dealer.addCard(deck->hit());
            dealer.dealer_show_cards();
        }
        return;
    }

    bool insuranceHandler(Hand dealer,Hand user, int betSize){

        if (dealer.OfferInsurance()){
            if (deck->getStrategy().shouldAcceptInsurance()){
                if (dealer.dealerHiddenTen() && user.isBlackjack()){
                    wallet += betSize;
                    return true;
                }
                else if (dealer.dealerHiddenTen() && !user.isBlackjack()){
                    return true;
                }
                else{
                    return false;
                }
            }
            else{
                if(dealer.dealerHiddenTen() && user.isBlackjack()){
                    return true;
                }
                else if (dealer.dealerHiddenTen() && !user.isBlackjack()) {
                    wallet -= betSize;
                    return true;
                }
                else{
                    return false;
                }
            }
        }
        return false;
    }

    bool dealerRobberyHandler(Hand dealer,Hand user, int betSize){
        if (dealer.dealerShowsTen() && dealer.dealerHiddenAce()){
            if (!user.isBlackjack()){
                wallet -= betSize;
            }
            return true;
        }
        return false;
        
    }

};

#endif
