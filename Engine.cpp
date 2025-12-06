#include "Engine.h"
#include "Deck.h"

Engine::Engine(const GameConfig& gameConfig, Deck deck, std::unique_ptr<CountingStrategy> strategy, std::unique_ptr<Player> player)

    : config(gameConfig), deck(std::move(deck)), countingStrategy(std::move(strategy)), player(std::move(player)) {
    
    eventBus = EventBus::getInstance();
    config.penetrationThreshold = (1-config.penetrationThreshold) * config.numDecks * Deck::NUM_CARDS_IN_DECK;
}

std::pair<double, double> Engine::runner(){  
    while (deck->getSize() > config.penetrationThreshold ){
        playHand();
    }    
    return {config.wallet, totalMoneyBet};
}

void Engine::playHand(){
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
    if (handleInsurancePhase(dealer,user)){
        return;
    }
    else if (dealerRobberyHandler(dealer,user)){
        return;
    }
    else{
        hands = user_play(dealer,user);
        evaluateHands(dealer,hands);
    }
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

bool Engine::didPlayerGetBlackjack(std::vector<Hand>& hands){
    if (hands.size() == 1 && hands[0].isBlackjack()){
        return true;
    }
    return false;
}

void Engine::evaluateHands(Hand& dealer, std::vector<Hand>& hands){
    countingStrategy->updateCount(dealer.getCards()[1]); // Reveal hole card

    std::vector<int> scores = getPlayerScores(hands);

    if(didPlayerGetBlackjack(hands) && !dealer.isBlackjack()){
        std::ostringstream roundSummary;
        roundSummary << "Natural Blackjack win! " << ". ";
        config.wallet += hands[0].getBetSize() * config.blackjackPayoutMultiplier;

        std::string outcome = "Natural Blackjack win";
        totalMoneyBet += hands[0].getBetSize();
        roundSummary << "Hand " << (1) << ": " << outcome
                        << " (score " << 21 << ", bet " << hands[0].getBetSize() << "); ";
        publish(EventType::RoundEnded, roundSummary.str());
        publishWalletSnapshot();
        return;
    }

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
            config.wallet += hand.getBetSize() * config.blackjackPayoutMultiplier;
            outcome = "Natural Blackjack win";
        } 
        else if (dealer_score > score){
            config.wallet -= hand.getBetSize();
            outcome = "Dealer win";
        }
        else if (dealer_score < score){
            config.wallet += hand.getBetSize();
            outcome = "Player win";
        }
        else if (dealer_score == 0 && score ==0){
            config.wallet -= hand.getBetSize();
            outcome = "Player bust";
        }

        totalMoneyBet += hand.getBetSize();
        roundSummary << "Hand " << (i + 1) << ": " << outcome
                        << " (score " << score << ", bet " << hand.getBetSize() << "); ";
    }

    publish(EventType::RoundEnded, roundSummary.str());
    publishWalletSnapshot();
}

std::vector<Hand> Engine::user_play(Hand& dealer, Hand& user){
    std::vector<Hand> hands;
    hands.reserve(4);
    
    play_hand(*player, dealer, user, hands, false);
    return hands;
}

void Engine::play_hand(Player& player, Hand& dealer, Hand& user, std::vector<Hand>& hands, bool has_split_aces, bool has_split){ 
    bool game_over = false;
    const std::string handLabel = has_split_aces ? "Player (split aces)" : "Player";
    print_hand(user, handLabel); 

    while(!game_over){
        Action action = player.getAction(user, dealer, countingStrategy->getTrueCount());
        
        switch(action)
        {
            case Action::Stand:
                game_over = standHandler(user, hands, handLabel);
                break;
            case Action::Hit:
                game_over = hitHandler(user, hands, handLabel);
                break;
            case Action::Double:
                game_over = doubleHandler(user, hands, handLabel, has_split);
                break;
            case Action::Split:
                game_over = splitHandler(player, user, dealer, hands, handLabel, has_split_aces, true);
                break;
            case Action::Surrender:
                game_over = surrenderHandler(user, hands, handLabel);
                break;
            case Action::Skip: 
                break;
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
    // Fix: Check for Hard 17 or > 17. If Soft 17, check rule.
    bool isSoft17 = dealer.isSoft17();
    int score = dealer.getScore();
    
    if (score > 17 || (score == 17 && !isSoft17) || (isSoft17 && !config.dealerHitsSoft17)) {
        return;
    }
    while (!dealer.isDealerOver() || (dealer.isSoft17() && config.dealerHitsSoft17)) {
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


bool Engine::handleInsurancePhase(Hand& dealer, Hand& user) {
    if (!canOfferInsurance(dealer)) {
        return false;
    }

    print_hand(user);
    bool accepted = askInsurance();

    if (eventsEnabled()) {
        std::ostringstream oss;
        oss << "Insurance offered. Strategy " << (accepted ? "accepts" : "declines")
            << ". " << describeHand("Dealer", dealer, true);
        publish(EventType::ActionTaken, oss.str());
    }

    return resolveInsurance(accepted, dealer, user);
}

bool Engine::canOfferInsurance(Hand& dealer) {
    return dealer.OfferInsurance();
}

bool Engine::askInsurance() {
    return countingStrategy->shouldAcceptInsurance();
}

bool Engine::resolveInsurance(bool accepted, Hand& dealer, Hand& user) {
    if (accepted) {
        return handleInsuranceAccepted(dealer, user);
    } else {
        return handleInsuranceDeclined(dealer, user);
    }
}

bool Engine::handleInsuranceAccepted(Hand& dealer, Hand& user) {
    bool dealerHasBlackjack = dealer.dealerHiddenTen();
    bool playerHasBlackjack = user.isBlackjack();

    if (dealerHasBlackjack) {
        countingStrategy->updateCount(dealer.getCards()[1]); // Reveal hole card
        
        if (playerHasBlackjack) {
            config.wallet += user.getBetSize(); 
            totalMoneyBet += user.getBetSize() * config.blackjackPayoutMultiplier; // Tracking?
            
            if (eventsEnabled()) {
                publish(EventType::RoundEnded, "Insurance wins: dealer blackjack vs player blackjack");
            }
        } else {
            totalMoneyBet += user.getBetSize() * config.blackjackPayoutMultiplier; // Tracking?

            if (eventsEnabled()) {
                publish(EventType::RoundEnded, "Insurance wins: dealer blackjack");
            }
        }
        publishWalletSnapshot();
        return true; 
    } else {
        if (eventsEnabled()) {
            publish(EventType::ActionTaken, "Insurance accepted automatically: dealer lacked blackjack");
        }
        config.wallet -= static_cast<double>(user.getBetSize()) * INSURANCEBETCOST;
        totalMoneyBet += user.getBetSize() * INSURANCEBETCOST;
        return false; // Round continues
    }
}

bool Engine::handleInsuranceDeclined(Hand& dealer, Hand& user) {
    bool dealerHasBlackjack = dealer.dealerHiddenTen();
    bool playerHasBlackjack = user.isBlackjack();

    if (dealerHasBlackjack) {
        countingStrategy->updateCount(dealer.getCards()[1]); // Reveal hole card
        if (playerHasBlackjack) {
            totalMoneyBet += user.getBetSize();
            if (eventsEnabled()) {
                publish(EventType::RoundEnded, "Dealer blackjack pushes player blackjack (no insurance)");
                publishWalletSnapshot();
            }
        } else {
            config.wallet -= user.getBetSize();
            totalMoneyBet += user.getBetSize();
            if (eventsEnabled()) {
                publish(EventType::RoundEnded, "Dealer blackjack; player loses without insurance");
                publishWalletSnapshot();
            }
        }
        return true; 
    } else {
        if (eventsEnabled()) {
            publish(EventType::ActionTaken, "Insurance declined; dealer lacks blackjack");
        }
        return false;
    }
}

bool Engine::dealerRobberyHandler(Hand& dealer,Hand& user){
    if (dealer.dealerShowsTen() && dealer.dealerHiddenAce()){
        print_hand(user);
        countingStrategy->updateCount(dealer.getCards()[1]); // Reveal hole card
        if (!user.isBlackjack()){
            config.wallet -= user.getBetSize();
        }
        totalMoneyBet += user.getBetSize();
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
    if (has_split && !config.doubleAfterSplitAllowed){
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

bool Engine::splitHandler(Player& player, Hand& user, Hand& dealer, std::vector<Hand>& hands, std::string handLabel, bool has_split_aces,bool has_split){
     // Check if we're splitting Aces
    bool splitting_aces = (user.peek_front_card() == Rank::Ace);

    if (splitting_aces && has_split_aces && !config.allowReSplitAces){
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
            play_hand(player, dealer,user,hands,true, true);
        }
        else{
            hands.emplace_back(user);
        }
        
        if (user2.isAces()){
            play_hand(player, dealer,user2,hands,true, true);
        }
        else{
            hands.emplace_back(user2);
        }
        
        return true;
    }
    else{
        play_hand(player, dealer,user,hands,false, true);
        play_hand(player, dealer,user2,hands,false, true);
    }
    return true;
}

bool Engine::surrenderHandler(Hand& user, std::vector<Hand>& hands, std::string handLabel){
    config.wallet -= static_cast<double>(user.getBetSize()) * SURRENDERMULTIPLIER;
    totalMoneyBet += user.getBetSize();

    if (eventsEnabled()){
        publish(EventType::ActionTaken, describeAction(Action::Surrender, user, handLabel));
        publishWalletSnapshot();
    }
    return true;
}

bool Engine::eventsEnabled() const {
    return config.emitEvents && eventBus;
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
    oss << "Wallet: " << config.wallet << " | True Count: " << countingStrategy->getTrueCount() << " | Running Count: " << countingStrategy->getRunningCount() << " | Decks Left: " << countingStrategy->getDecksLeft();
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