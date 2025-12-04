#include "Engine.h"

Engine::Engine(int deck_size, int money, std::unique_ptr<CountingStrategy> strategy, 
               bool enableEvents, double blackJackMultiplier, 
               bool dealerHitsSoft17, bool doubleAfterSplitAllowed, 
               bool allowReSplitAces, bool allowSurrender, 
               bool autoPlay)

    : number_of_decks(deck_size), countingStrategy(std::move(strategy)), wallet(money), 
      emitEvents(enableEvents), blackjack_payout_multiplier(blackJackMultiplier), 
      dealerHitsSoft17(dealerHitsSoft17), doubleAfterSplitAllowed(doubleAfterSplitAllowed), 
      allowReSplitAces(allowReSplitAces), allowSurrender(allowSurrender), autoPlay(autoPlay) {
    
    deck.emplace(number_of_decks);
    eventBus = EventBus::getInstance();
    PENETRATION_THRESHOLD = number_of_decks * 13;
}

std::pair<int, int> Engine::runner(){  
    while (deck->getSize() > PENETRATION_THRESHOLD){

        countingStrategy->updateDeckSize(deck->getSize());
        std::vector<Hand> hands;

        int bet = countingStrategy->getBetSize();
        Hand dealer = draw_cards();
        Hand user = draw_cards(bet);

        // Count visible cards
        countingStrategy->updateCount(dealer.getCards()[0]);

        for (Card card : user.getCards()) {
            countingStrategy->updateCount(card);
        }

        peek_dealer(dealer);
        if (insuranceHandler(dealer,user)){
            continue;
        }
        else if (dealerRobberyHandler(dealer,user)){
            continue;
        }
        else{
            hands = user_play(dealer,user);
            evaluateHands(dealer,hands);
        }
    }    
    return {wallet, money_bet};
}

std::vector<int> Engine::getPlayerScores(std::vector<Hand>& hands){
    std::vector<int> scores;
    for (Hand hand: hands){
        scores.emplace_back(hand.getFinalScore());
    }
    return scores;
}

//if all hands bust dealer does not draw
bool Engine::didHandsBust(std::vector<int> scores){
    for (int score : scores){
        if (score > 0) {
            return false;
        }
    }
    return true;
}

void Engine::evaluateHands(Hand& dealer, std::vector<Hand>& hands){
    countingStrategy->updateCount(dealer.getCards()[1]); // Reveal hole card

    std::vector<int> scores = getPlayerScores(hands);
    if (!didHandsBust(scores)){
        dealer_draw(dealer);
    }

    int dealer_score = dealer.getFinalScore();
    std::ostringstream roundSummary;
    roundSummary << "Dealer score: " << dealer_score << ". ";

    for (int i = 0; i < hands.size(); i++){
        Hand& hand = hands[i];
        int score = hand.getFinalScore();
        std::string outcome = "Push";

        if (hands.size() == 1 && hand.isBlackjack()){
            wallet += hand.getBetSize() * blackjack_payout_multiplier;
            outcome = "Natural Blackjack win";
        } 
        else if (dealer_score > score){
            wallet -= hand.getBetSize();
            outcome = "Dealer win";
        }
        else if (dealer_score < score){
            wallet += hand.getBetSize();
            outcome = "Player win";
        }
        else if (dealer_score == 0 && score ==0){
            wallet -= hand.getBetSize();
            outcome = "Player bust";
        }

        money_bet += hand.getBetSize();
        roundSummary << "Hand " << (i + 1) << ": " << outcome
                        << " (score " << score << ", bet " << hand.getBetSize() << "); ";
    }

    publish(EventType::RoundEnded, roundSummary.str());
    publishWalletSnapshot();
}

std::vector<Hand> Engine::user_play(Hand& dealer, Hand& user){
    std::vector<Hand> hands;
    hands.reserve(4);
    if (autoPlay){
        user_play_hand(dealer, user, hands, false);
    }
    else{
        user_play_hand_manual(dealer, user, hands, false);
    }
    
    return hands;
}

Action Engine::getAction(Hand dealer, Hand user){
    BasicStrategy basicStrategy;
    Rank dealer_card = dealer.peek_front_card();

    if(user.check_can_double() && allowSurrender){
        Action action = basicStrategy.shouldSurrender(user.getScore(), dealer_card, countingStrategy->getTrueCount());
        if (action == Action::Surrender) {
            return action;
        }
    }
    

    if(user.check_can_split()){
        return basicStrategy.getSplitAction(user.peek_front_card(),dealer_card,countingStrategy->getTrueCount());
    }

    int playerTotal = user.getScore();

    if(user.isHandSoft()){
        Action action = basicStrategy.getSoftHandAction(playerTotal,dealer_card);

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
            Action action = basicStrategy.getHardHandAction(playerTotal,dealer_card,countingStrategy->getTrueCount());
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


void Engine::user_play_hand(Hand& dealer, Hand& user, std::vector<Hand>& hands, bool has_split_aces, bool has_split){ 

    bool game_over = false;
    const std::string handLabel = has_split_aces ? "Player (split aces)" : "Player";
    print_hand(user, handLabel); 

    while(!game_over){
        Action action = getAction(dealer, user);
        switch(action)
        {
            case Action::Stand:
            {
                game_over = standHandler(user, hands, handLabel);
                break;
            }
            case Action::Hit:
            {
                game_over = hitHandler(user, hands, handLabel);
                break;
            }
            case Action::Double:
            {
                game_over = doubleHandler(user, hands, handLabel, has_split);
                break;
            }
            case Action::Split:
            {
                game_over = splitHandler(user, dealer, hands, handLabel, has_split_aces, true);
                break;
            }
            case Action::Surrender://probably rework this, dont like maniputlating wallet here :/
            {
                game_over = surrenderHandler(user, hands, handLabel);
                break;
            }
            case Action::Skip: 
            {
                //should never reach here
                std::cout << "HOW DID YOU REACH THIS SKIP!!!!!!!!!!!!!!!!!!!!!" << std::endl;
                if (eventsEnabled()){
                    publish(EventType::ActionTaken, handLabel + " attempted to skip turn");
                }
                break;
            }

        }
    }

}
void Engine::user_play_hand_manual(Hand& dealer, Hand& user, std::vector<Hand>& hands, bool has_split_aces, bool has_split){ //need to implement strategy for here
    int choice;
    bool game_over = false;
    const std::string handLabel = has_split_aces ? "Player (split aces)" : "Player";
    print_hand(user, handLabel); 

    while(!game_over){
        std::cin >> choice;
        Action action = Action::Skip;
        Action optimal = getAction(dealer, user);
        switch(choice)
        {
            case 0:
                game_over = standHandler(user, hands, handLabel);
                if (eventsEnabled()){
                    publish(EventType::ActionTaken, optimalAction(optimal, user,handLabel));
                }          
                break;
            case 1:
            {
                game_over = hitHandler(user, hands, handLabel);
                if (eventsEnabled()){
                    publish(EventType::ActionTaken, optimalAction(optimal, user,handLabel));
                }
                break;
            }
            case 2:
            {
                game_over = doubleHandler(user, hands, handLabel, has_split);
                if (eventsEnabled()){
                    publish(EventType::ActionTaken, optimalAction(optimal, user,handLabel));
                }
                break;
            }
                
            case 3:
            {
                game_over = splitHandler(user, dealer, hands, handLabel, has_split_aces, true);
                if (eventsEnabled()){
                    publish(EventType::ActionTaken, optimalAction(optimal, user,handLabel));
                }
                break;
            }

        }
    }
}

void Engine::print_state(Hand& dealer, Hand& user){
    if (eventsEnabled()){
        publish(EventType::CardsDealt, describeHand("Dealer", dealer));
    }
}

void Engine::print_hand(Hand& user, const std::string& label){
    if (eventsEnabled()){
        publish(EventType::CardsDealt, describeHand(label, user));
    }
}

void Engine::peek_dealer(Hand& dealer){
    if (eventsEnabled()){
        publish(EventType::CardsDealt, describeHand("Dealer (showing)", dealer, true));
    }
}

Hand Engine::draw_cards(int betSize){
    Hand hand = Hand(deck->deal(), betSize);
    return hand;
}

void Engine::dealer_draw(Hand& dealer){
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
        
        Card c = deck->hit();
        countingStrategy->updateCount(c);
        dealer.addCard(c);
        if (eventsEnabled()){
            publish(EventType::CardsDealt, describeHand("Dealer", dealer));
        }
    }
    return;
}


bool Engine::insuranceHandler(Hand& dealer,Hand& user){
    if (dealer.OfferInsurance()){
        print_hand(user);
        bool acceptInsurance = countingStrategy->shouldAcceptInsurance();

        if (eventsEnabled()){
            std::ostringstream oss;
            oss << "Insurance offered. Strategy " << (acceptInsurance ? "accepts" : "declines")
                << ". " << describeHand("Dealer", dealer, true);
            publish(EventType::ActionTaken, oss.str());
        }

        if (acceptInsurance){
            if (dealer.dealerHiddenTen() && user.isBlackjack()){
                countingStrategy->updateCount(dealer.getCards()[1]); // Reveal hole card
                wallet += static_cast<double>(user.getBetSize()); //this is correct
                money_bet += user.getBetSize() * 1.5;
                if (eventsEnabled()){
                    publish(EventType::RoundEnded, "Insurance wins: dealer blackjack vs player blackjack");
                }
                publishWalletSnapshot();
                return true;
            }
            else if (dealer.dealerHiddenTen() && !user.isBlackjack()){
                countingStrategy->updateCount(dealer.getCards()[1]); // Reveal hole card
                money_bet += user.getBetSize() * 1.5;
                if (eventsEnabled()){
                    publish(EventType::RoundEnded, "Insurance wins: dealer blackjack");
                }
                publishWalletSnapshot();
                return true;
            }
            else{
                if (eventsEnabled()){
                    publish(EventType::ActionTaken, "Insurance accepted automatically: dealer lacked blackjack");
                }
                wallet -= static_cast<double>(user.getBetSize()) * 0.5; //this is correct
                money_bet += user.getBetSize() * 0.5;
                return false;
            }
        }
        else{
            if(dealer.dealerHiddenTen() && user.isBlackjack()){
                countingStrategy->updateCount(dealer.getCards()[1]); // Reveal hole card
                money_bet += user.getBetSize();
                if (eventsEnabled()){
                    publish(EventType::RoundEnded, "Dealer blackjack pushes player blackjack (no insurance)");
                    publishWalletSnapshot();
                }
                return true;
            }
            else if (dealer.dealerHiddenTen() && !user.isBlackjack()) {
                countingStrategy->updateCount(dealer.getCards()[1]); // Reveal hole card
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

bool Engine::dealerRobberyHandler(Hand& dealer,Hand& user){
    if (dealer.dealerShowsTen() && dealer.dealerHiddenAce()){
        print_hand(user);
        countingStrategy->updateCount(dealer.getCards()[1]); // Reveal hole card
        if (!user.isBlackjack()){
            wallet -= user.getBetSize();
        }
        money_bet += user.getBetSize();
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

bool Engine::standHandler(Hand& user, std::vector<Hand>& hands, std::string handLabel){
    hands.emplace_back(user);
    if (eventsEnabled()){
        publish(EventType::ActionTaken, describeAction(Action::Stand, user, handLabel));
    }
    return true;
}

bool Engine::hitHandler(Hand& user, std::vector<Hand>& hands, std::string handLabel){
    Card c = deck->hit();
    countingStrategy->updateCount(c);
    user.addCard(c);

    if (eventsEnabled()){
        publish(EventType::ActionTaken, describeAction(Action::Hit, user, handLabel));
    }

    if (user.check_over()) {hands.emplace_back(user); return true;}

    return false;
}

bool Engine::doubleHandler(Hand& user, std::vector<Hand>& hands, std::string handLabel,bool has_split){
    if (has_split && !doubleAfterSplitAllowed){
        //hands.emplace_back(user);
        if (eventsEnabled()){
            publish(EventType::ActionTaken, handLabel + " cannot double after split; hits instead");
        }

        Card c = deck->hit();
        countingStrategy->updateCount(c);
        user.addCard(c);

        hands.emplace_back(user);

        if (eventsEnabled()){
            publish(EventType::ActionTaken, describeAction(Action::Double, user, handLabel));
        }
        return true;
    }

    user.doubleBet();
    Card c = deck->hit();
    countingStrategy->updateCount(c);
    user.addCard(c);
    hands.emplace_back(user);

    if (eventsEnabled()){
        publish(EventType::ActionTaken, describeAction(Action::Double, user, handLabel));
    }
    return true;
}

bool Engine::splitHandler(Hand& user, Hand& dealer, std::vector<Hand>& hands, std::string handLabel, bool has_split_aces,bool has_split){
     // Check if we're splitting Aces
    bool splitting_aces = (user.peek_front_card() == Rank::Ace);

    if (splitting_aces && has_split_aces && !allowReSplitAces){
         hitHandler(user, hands, handLabel);
    }
    
    Hand user2 = Hand(user.getLastCard(),user.getBetSize());
    user.popLastCard();

    Card c1 = deck->hit();
    countingStrategy->updateCount(c1);
    user.addCard(c1);

    Card c2 = deck->hit();
    countingStrategy->updateCount(c2);
    user2.addCard(c2);

    if (eventsEnabled()){
        std::ostringstream oss;
        oss << handLabel << " splits into -> "
            << describeHand(handLabel + " (hand 1)", user) << " | "
            << describeHand(handLabel + " (hand 2)", user2);
        publish(EventType::ActionTaken, oss.str());
    }

    if (splitting_aces) {
        if (user.isAces()){
            if (autoPlay){
                user_play_hand(dealer,user,hands,true, true);
            }
            else{
                user_play_hand_manual(dealer,user,hands,true, true);
            }
        }
        else{
            hands.emplace_back(user);
        }
        
        if (user2.isAces()){
            if (autoPlay){
                user_play_hand(dealer,user2,hands,true, true);
            }
            else{
                user_play_hand_manual(dealer,user2,hands,true, true);
            }
        }
        else{
            hands.emplace_back(user2);
        }
        
        return true;
    }
    else{
        if (autoPlay){
            user_play_hand(dealer,user,hands,false, true);
            user_play_hand(dealer,user2,hands,false, true);
        }
        else{
            user_play_hand_manual(dealer,user,hands,false, true);
            user_play_hand_manual(dealer,user2,hands,false, true);
        }
    }
    return true;
}

bool Engine::surrenderHandler(Hand& user, std::vector<Hand>& hands, std::string handLabel){
    wallet -= static_cast<double>(user.getBetSize()) * 0.5;
    money_bet += user.getBetSize();

    if (eventsEnabled()){
        publish(EventType::ActionTaken, describeAction(Action::Surrender, user, handLabel));
        publishWalletSnapshot();
    }
    return true;
}

bool Engine::eventsEnabled() const {
    return emitEvents && eventBus;
}

void Engine::publish(EventType type, const std::string& message){
    if (!eventsEnabled()) {
        return;
    }
    eventBus->notifyObservers(type, message);
}

void Engine::publishWalletSnapshot(){
    if (!eventsEnabled()) {
        return;
    }
    std::ostringstream oss;
    oss << "Wallet: " << wallet << " | True Count: " << countingStrategy->getTrueCount() << " | Running Count: " << countingStrategy->getRunningCount() << " | Decks Left: " << countingStrategy->getDecksLeft();
    publish(EventType::GameStats, oss.str());
    publish(EventType::GameStats, "============================================================================");
}

std::string Engine::describeHand(const std::string& label, Hand& hand, bool hideHoleCard){
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

std::string Engine::describeAction(Action action, Hand& hand, const std::string& label){
    std::ostringstream oss;
    oss << label << " chose " << action << ". " << describeHand(label, hand);
    return oss.str();
}

std::string Engine::optimalAction(Action action, Hand& hand, const std::string& label){
    std::ostringstream oss;
    oss << label << " optimal action : " << action << ". ";
    return oss.str();
}