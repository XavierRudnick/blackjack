#ifndef ENGINE_H
#define ENGINE_H

#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include "Deck.h"
#include "Hand.h"
#include "BasicStrategy.h"
#include "HiLoStrategy.h"
#include "NoStrategy.h"
#include "observers/EventBus.h"

template <typename Strategy>
struct Engine{
    std::optional<Deck<Strategy>> deck;
    int number_of_decks;
    double wallet;
    double blackjack_payout_multiplier = 1.5;
    bool dealerHitsSoft17 = false;
    bool doubleAfterSplitAllowed = true;
    bool allowReSplitAces = true;
    bool allowSurrender = false;
    int money_bet = 0;
    bool emitEvents = false;
    bool autoPlay = true;
    EventBus* eventBus = nullptr;

    Engine(int deck_size,int money, Strategy strategy, bool enableEvents = false, double blackJackMultiplier = 1.5, bool dealerHitsSoft17 = false, bool doubleAfterSplitAllowed = true, bool allowReSplitAces = true, bool allowSurrender = false, bool autoPlay = true)
        : number_of_decks(deck_size), wallet(money), emitEvents(enableEvents), blackjack_payout_multiplier(blackJackMultiplier), dealerHitsSoft17(dealerHitsSoft17), doubleAfterSplitAllowed(doubleAfterSplitAllowed), allowReSplitAces(allowReSplitAces), allowSurrender(allowSurrender), autoPlay(autoPlay) {
        deck.emplace(number_of_decks, std::move(strategy));
        eventBus = EventBus::getInstance();
    }

    std::pair<int, int> runner(){
        
        while (deck->getSize() > number_of_decks * 13){ // reset when 3/8 left so 62.5% penetration

            deck->getStrategy().updateDeckSize(deck->getSize()); 
            int betSize = deck->getBetSize();
            std::vector<Hand> hands;

            Hand dealer = draw_cards();
            Hand user = draw_cards(betSize);
            peek_dealer(dealer);
            if (insuranceHandler(dealer,user)){
                print_state(dealer, user);
                hands = {user};
            }
            else if (dealerRobberyHandler(dealer,user)){
                print_state(dealer, user);
                hands = {user};
            }
            else{
                hands = user_play(dealer,user);
                evaluateHands(dealer,hands);
            }

            //std::cout << "wallet total : " << wallet << std::endl;
            //std::cout << "true count : " << deck->getStrategy().getCount() << std::endl;
        }    
        return {wallet, money_bet};
    }

    std::vector<int> getPlayerScores(std::vector<Hand>& hands){
        std::vector<int> scores;
        for (Hand hand: hands){
            scores.emplace_back(hand.getFinalScore());
        }
        return scores;
    }

    bool didHandsBust(std::vector<int> scores){
        for (int i : scores){
            if (i > 0) {
                return false;
            }
        }
        return true;
    }

    void evaluateHands(Hand& dealer, std::vector<Hand>& hands){
        std::vector<int> scores = getPlayerScores(hands);
        if (!didHandsBust(scores)){
            dealer_draw(dealer);
        }

        int dealer_score = dealer.getFinalScore();
        std::ostringstream roundSummary;
        roundSummary << "Dealer score: " << dealer_score << ". ";

        for (int i = 0; i < hands.size(); ++i){
            Hand& hand = hands[i];
            int score = hand.getFinalScore();
            std::string outcome = "Push";

            if (hands.size() == 1 && hand.isBlackjack()){
                wallet += static_cast<double>(hand.getBetSize()) * blackjack_payout_multiplier;
                outcome = "Natural Blackjack win";
            } 
            else if (dealer_score > score){
                wallet -= static_cast<double>(hand.getBetSize());
                outcome = "Dealer win";
            }
            else if (dealer_score < score){
                wallet += static_cast<double>(hand.getBetSize());
                outcome = "Player win";
            }
            else if (dealer_score == 0 && score ==0){
                wallet -= static_cast<double>(hand.getBetSize());
                outcome = "Player bust";
            }

            money_bet += hand.getBetSize();
            roundSummary << "Hand " << (i + 1) << ": " << outcome
                         << " (score " << score << ", bet " << hand.getBetSize() << "); ";
        }

        publish(EventType::RoundEnded, roundSummary.str());
        publishWalletSnapshot();
    }

    std::vector<Hand> user_play(Hand& dealer, Hand& user){
        std::vector<Hand> hands;
        hands.reserve(4); //unnessesary but fuck it I do what I want
        if (autoPlay){
            user_play_hand(dealer, user, hands, false);
        }
        else{
            user_play_hand_manual(dealer, user, hands, false);
        }
        
        return hands;
    }

    Action getAction(Hand dealer, Hand user){
        BasicStrategy strat;
        Rank dealer_card = dealer.peek_front_card();

        if(user.check_can_double() &&allowSurrender){
            Action action = strat.shouldSurrender(user.getScore(), dealer_card, deck->getStrategy().getCount());
            if (action == Action::Surrender) {
                return action;
            }
        }

        if(user.check_can_split()){
            return strat.getSplitAction(user.peek_front_card(),dealer_card,deck->getStrategy().getCount());
        }

        int playerTotal = user.getScore();

        if(user.isHandSoft()){
            if (playerTotal < 13 || playerTotal > 21) {
                throw std::runtime_error("why arent you splitting aces, are too many returning?? 99");
            }
            Action action = strat.getSoftHandAction(playerTotal,dealer_card);

            if (action == Action::Double){
                if (user.check_can_double()){
                    return action;
                }
                else if (user.check_should_stand()){
                    return Action::Stand;
                }
                else{
                    return Action::Hit;
                }
            }

            return action;
        }
        else{
             Action action = strat.getHardHandAction(playerTotal,dealer_card,deck->getStrategy().getCount());
             if (action == Action::Double){//I think this is correct if DAS is allowed or nah. specifically for soft vals, maybe investigate
                if (user.check_can_double()){
                    return action;
                }
                else if(user.check_should_stand()){
                    return Action::Stand;
                }
                else{
                    return Action::Hit;
                }
            }
            return action;
        }

    }


    void user_play_hand(Hand& dealer, Hand& user, std::vector<Hand>& hands, bool is_split_aces = false, bool has_split = false){ 
    
        bool game_over = false;
        const std::string handLabel = is_split_aces ? "Player (split aces)" : "Player";
        print_hand(user, handLabel); 

        // If this is a split Ace hand, only deal one card and stand
        if (is_split_aces) {//rsa handler
            user.addCard(deck->hit());

            if (allowReSplitAces && user.getLastCard().getRank() == Rank::Ace) {
                if (eventsEnabled()){
                    publish(EventType::ActionTaken, handLabel + "split aces again");
                }

                bool splitting_aces = true;   
                Hand user2 = Hand(user.getLastCard(),user.getBetSize());
                user.popLastCard();
                user.addCard(deck->hit());
                user2.addCard(deck->hit());

                if (eventsEnabled()){
                    std::ostringstream oss;
                    oss << handLabel << " splits into -> "
                        << describeHand(handLabel + " (hand 1)", user) << " | "
                        << describeHand(handLabel + " (hand 2)", user2);
                    publish(EventType::ActionTaken, oss.str());
                }

                user_play_hand(dealer,user2,hands,splitting_aces, true);
                
                // If splitting aces, this hand also gets only one card
                if (splitting_aces) {
                    game_over = true;
                    hands.emplace_back(user);
                }
                return;
            } 
            else {
                if (eventsEnabled()){
                    publish(EventType::ActionTaken, handLabel + " forced to stand after split aces");
                }
                game_over = true;
                hands.emplace_back(user);
            }
            return;
        }

        while(!game_over){
            Action action = getAction(dealer, user);
            switch(action)
            {
                case Action::Stand:
                {
                    //print_hand(user, handLabel);
                    game_over = true;
                    hands.emplace_back(user);
                    if (eventsEnabled()){
                        publish(EventType::ActionTaken, describeAction(action, user, handLabel));
                    }
                    break;
                }
                case Action::Hit:
                {
                    if (deck->getSize() < 1) {
                        std::cout << "HOW DID YOU REACH THIS YOU ARE COOKED!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                        game_over = true;
                        hands.emplace_back(user);
                        if (eventsEnabled()){
                            publish(EventType::ActionTaken, handLabel + " attempted to hit with insufficient cards");
                        }
                        break;
                    }
                    user.addCard(deck->hit());
                    if (user.check_over()) {game_over = true; hands.emplace_back(user);}

                    if (eventsEnabled()){
                        publish(EventType::ActionTaken, describeAction(action, user, handLabel));
                    }
                    break;
                }
                case Action::Double:
                {
                    if (deck->getSize() < 1) {
                        std::cout << "HOW DID YOU REACH THIS YOU ARE COOKED!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                        game_over = true;
                        hands.emplace_back(user);
                        if (eventsEnabled()){
                            publish(EventType::ActionTaken, handLabel + " attempted to double with insufficient cards");
                        }
                        break;
                    }
                    if (has_split && !doubleAfterSplitAllowed){
                        //print_hand(user, handLabel);
                        game_over = true;
                        hands.emplace_back(user);
                        if (eventsEnabled()){
                            publish(EventType::ActionTaken, handLabel + " cannot double after split; hits instead");
                        }

                        user.addCard(deck->hit());
                        if (user.check_over()) {game_over = true; hands.emplace_back(user);}
                        //print_hand(user, handLabel);
                        if (eventsEnabled()){
                            publish(EventType::ActionTaken, describeAction(action, user, handLabel));
                        }
                        break;
                    }
                    user.doubleBet();
                    user.addCard(deck->hit());
                    //print_hand(user, handLabel);
                    game_over = true;
                    hands.emplace_back(user);
                    if (eventsEnabled()){
                        publish(EventType::ActionTaken, describeAction(action, user, handLabel));
                    }
                    break;
                }
                case Action::Split://only called when hand has 2 cards
                {
                    if (deck->getSize() < 2) {
                        std::cout << "HOW DID YOU REACH THIS YOU ARE COOKED!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                        game_over = true;
                        hands.emplace_back(user);
                        if (eventsEnabled()){
                            publish(EventType::ActionTaken, handLabel + " attempted to split with insufficient cards");
                        }
                        break;
                    }
                    
                    // Check if we're splitting Aces
                    bool splitting_aces = (user.peek_front_card() == Rank::Ace);
                    
                    Hand user2 = Hand(user.getLastCard(),user.getBetSize());
                    user.popLastCard();
                    user.addCard(deck->hit());
                    user2.addCard(deck->hit());

                    if (eventsEnabled()){
                        std::ostringstream oss;
                        oss << handLabel << " splits into -> "
                            << describeHand(handLabel + " (hand 1)", user) << " | "
                            << describeHand(handLabel + " (hand 2)", user2);
                        publish(EventType::ActionTaken, oss.str());
                    }

                    user_play_hand(dealer,user2,hands,splitting_aces, true);
                    
                    // If splitting aces, this hand also gets only one card
                    if (splitting_aces) {
                        game_over = true;
                        hands.emplace_back(user);
                    }
                    
                    //print_hand(user, handLabel);
                    break;
                }
                case Action::Surrender://probably rework this, dont like maniputlating wallet here :/
                {
                    wallet -= static_cast<double>(user.getBetSize()) * 0.5;
                    money_bet += user.getBetSize();
                    game_over = true;
                    //hands.emplace_back(user); dont place in hands to be evaluated later
                    if (eventsEnabled()){
                        publish(EventType::ActionTaken, describeAction(action, user, handLabel));
                        publishWalletSnapshot();
                    }
                    break;
                }
                case Action::Skip: 
                    std::cout << "HOW DID YOU REACH THIS YOU ARE COOKED!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                    if (eventsEnabled()){
                        publish(EventType::ActionTaken, handLabel + " attempted to skip turn");
                    }
                    break;

            }
        }

    }
    void user_play_hand_manual(Hand& dealer, Hand& user, std::vector<Hand>& hands, bool is_split_aces = false, bool has_split = false){ //need to implement strategy for here
        int choice;
        bool game_over = false;
        const std::string handLabel = is_split_aces ? "Player (split aces)" : "Player";
        print_hand(user, handLabel); 

        

        while(!game_over){
            std::cin >> choice;
            //std::cout << std::endl;
            Action action = Action::Skip;
            switch(choice)
            {
                case 0:
                    action = Action::Stand;
                    game_over = true;
                    hands.emplace_back(user);
                    if (eventsEnabled()){
                        publish(EventType::ActionTaken, describeAction(action, user, handLabel));
                    }
                    break;
                case 1:
                    action = Action::Hit;
                    if (deck->getSize() < 1) {
                        std::cout << "HOW DID YOU REACH THIS YOU ARE COOKED!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                        game_over = true;
                        hands.emplace_back(user);
                        if (eventsEnabled()){
                            publish(EventType::ActionTaken, handLabel + " attempted to hit with insufficient cards");
                        }
                        break;
                    }
                    user.addCard(deck->hit());
                    if (user.check_over()) {game_over = true; hands.emplace_back(user);}
                    if (eventsEnabled()){
                        publish(EventType::ActionTaken, describeAction(action, user, handLabel));
                    }
                    break;
                case 2:
                {
                    action = Action::Double;
                    if (deck->getSize() < 1) {
                        std::cout << "HOW DID YOU REACH THIS YOU ARE COOKED!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                        game_over = true;
                        hands.emplace_back(user);
                        if (eventsEnabled()){
                            publish(EventType::ActionTaken, handLabel + " attempted to double with insufficient cards");
                        }
                        break;
                    }
                    user.doubleBet();
                    user.addCard(deck->hit());

                    game_over = true;
                    hands.emplace_back(user);
                    if (eventsEnabled()){
                        publish(EventType::ActionTaken, describeAction(action, user, handLabel));
                    }
                    break;
                }
                    
                case 3:
                {
                    action = Action::Split;
                    if (deck->getSize() < 2) {
                        std::cout << "HOW DID YOU REACH THIS YOU ARE COOKED!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                        game_over = true;
                        hands.emplace_back(user);
                        if (eventsEnabled()){
                            publish(EventType::ActionTaken, handLabel + " attempted to split with insufficient cards");
                        }
                        break;
                    }
                    
                    // Check if we're splitting Aces
                    bool splitting_aces = (user.peek_front_card() == Rank::Ace);
                    
                    Hand user2 = Hand(user.getLastCard(),user.getBetSize());
                    user.popLastCard();
                    user.addCard(deck->hit());
                    user2.addCard(deck->hit());

                    if (eventsEnabled()){
                        std::ostringstream oss;
                        oss << handLabel << " splits into -> "
                            << describeHand(handLabel + " (hand 1)", user) << " | "
                            << describeHand(handLabel + " (hand 2)", user2);
                        publish(EventType::ActionTaken, oss.str());
                    }

                    user_play_hand_manual(dealer,user2,hands,splitting_aces, true);
                    
                    // If splitting aces, this hand also gets only one card
                    if (splitting_aces) {
                        game_over = true;
                        hands.emplace_back(user);
                    }
                    
                    //print_hand(user, handLabel);
                    break;
                }

            }
        }
    }

    void print_initial(Hand& dealer, Hand& user){
        peek_dealer(dealer);
        print_hand(user);
    }

    void print_state(Hand& dealer, Hand& user){
        if (eventsEnabled()){
            publish(EventType::CardsDealt, describeHand("Dealer", dealer));
        }
        //print_hand(user);
    }

    void print_hand(Hand& user, const std::string& label = "Player"){
        if (eventsEnabled()){
            publish(EventType::CardsDealt, describeHand(label, user));
        }
    }

    void peek_dealer(Hand& dealer){
        if (eventsEnabled()){
            publish(EventType::CardsDealt, describeHand("Dealer (showing)", dealer, true));
        }
    }

    Hand draw_cards(int betSize = 0){
        if (deck->getSize() < 2) {
            throw std::runtime_error("Not enough cards to draw initial hand 258");
        }
        Hand hand = Hand(deck->deal(), betSize);
        return hand;
    }

    void dealer_draw(Hand& dealer){
        if (eventsEnabled()){
            publish(EventType::CardsDealt, describeHand("Dealer", dealer));
        }
        if (dealer.isDealerOver() || (dealer.isSoft17() && !dealerHitsSoft17)) {
            return;
        }
        while (!dealer.isDealerOver() || (dealer.isSoft17() && dealerHitsSoft17)) {
            if (deck->getSize() < 1) {
                throw std::runtime_error("Not enough cards to draw initial hand 271");
            }
            
            dealer.addCard(deck->hit());
            if (eventsEnabled()){
                publish(EventType::CardsDealt, describeHand("Dealer", dealer));
            }
        }
        return;
    }


    bool insuranceHandler(Hand& dealer,Hand& user){

        if (dealer.OfferInsurance()){
            bool acceptInsurance = deck->getStrategy().shouldAcceptInsurance();
            if (eventsEnabled()){
                std::ostringstream oss;
                oss << "Insurance offered. Strategy " << (acceptInsurance ? "accepts" : "declines")
                    << ". " << describeHand("Dealer", dealer, true);
                publish(EventType::ActionTaken, oss.str());
            }

            if (acceptInsurance){
                if (dealer.dealerHiddenTen() && user.isBlackjack()){
                    wallet += static_cast<double>(user.getBetSize()) * 0.5;
                    money_bet += user.getBetSize();
                    if (eventsEnabled()){
                        publish(EventType::RoundEnded, "Insurance wins: dealer blackjack vs player blackjack");
                    }
                    publishWalletSnapshot();
                    return true;
                }
                else if (dealer.dealerHiddenTen() && !user.isBlackjack()){
                    money_bet += user.getBetSize();
                    if (eventsEnabled()){
                        publish(EventType::RoundEnded, "Insurance wins: dealer blackjack");
                    }
                    publishWalletSnapshot();
                    return true;
                }
                else{
                    if (eventsEnabled()){
                        publish(EventType::ActionTaken, "Insurance declined automatically: dealer lacked blackjack");
                    }
                    return false;
                }
            }
            else{
                if(dealer.dealerHiddenTen() && user.isBlackjack()){
                    money_bet += user.getBetSize();
                    if (eventsEnabled()){
                        publish(EventType::RoundEnded, "Dealer blackjack pushes player blackjack (no insurance)");
                        publishWalletSnapshot();
                    }
                    return true;
                }
                else if (dealer.dealerHiddenTen() && !user.isBlackjack()) {
                    wallet -= user.getBetSize();
                    money_bet += user.getBetSize();
                    if (eventsEnabled()){
                        publish(EventType::RoundEnded, "Dealer blackjack; player loses without insurance");
                        publishWalletSnapshot();
                    }
                    return true;
                }
                else{
                    if (eventsEnabled()){
                        publish(EventType::ActionTaken, "Insurance declined; dealer lacks blackjack");
                    }
                    return false;
                }
            }
        }
        return false;
    }

    bool dealerRobberyHandler(Hand& dealer,Hand& user){
        if (dealer.dealerShowsTen() && dealer.dealerHiddenAce()){
            if (!user.isBlackjack()){
                wallet -= user.getBetSize();
                money_bet += user.getBetSize()  * 1;
            }
            if (eventsEnabled()){
                std::ostringstream oss;
                oss << "Dealer flipped blackjack. " << describeHand("Dealer", dealer);
                publish(EventType::RoundEnded, oss.str());
                publishWalletSnapshot();
            }
            return true;
        }
        return false;
        
    }

private:
    bool eventsEnabled() const {
        return emitEvents && eventBus;
    }

    void publish(EventType type, const std::string& message){
        if (!eventsEnabled()) {
            return;
        }
        eventBus->notifyObservers(type, message);
    }

    void publishWalletSnapshot(){
        if (!eventsEnabled()) {
            return;
        }
        std::ostringstream oss;
        oss << "Wallet: " << wallet << " | Count: " << deck->getStrategy().getCount() << " | Decks Left: " << deck->getStrategy().getDecksLeft();
        publish(EventType::GameStats, oss.str());
        publish(EventType::GameStats, "============================================================================");
    }

    std::string describeHand(const std::string& label, Hand& hand, bool hideHoleCard = false){
        std::ostringstream oss;
        oss << label << " hand: ";
        std::vector<Card> cards = hand.getCards();

        if (cards.empty()){
            oss << "<empty>";
        } else {
            for (size_t i = 0; i < cards.size(); ++i){
                if (hideHoleCard && i == 1){
                    oss << "[hidden]";
                }
                else {
                    Card card = cards[i];
                    oss << card.getRank() << " of " << card.getSuit();
                }

                if (i + 1 < cards.size()){
                    oss << ", ";
                }
            }
        }

        if (hideHoleCard){
            oss << " | score: [hidden]";
        }
        else{
            oss << " | score: " << hand.getScore();
        }
        if (hand.getBetSize() > 0){
            oss << " | bet: " << hand.getBetSize();
        }
        if (hand.isBlackjack()){
            oss << " | Blackjack";
        }
        else if (hand.check_over()){
            oss << " | Bust";
        }

        return oss.str();
    }

    std::string describeAction(Action action, Hand& hand, const std::string& label){
        std::ostringstream oss;
        oss << label << " chose " << action << ". " << describeHand(label, hand);
        return oss.str();
    }
};

#endif
