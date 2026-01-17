#include "Engine.h"
#include "Deck.h"

#include <algorithm>

Engine::Engine(
    const GameConfig& gameConfig,
    Deck deck,
    Player* player,
    EventBus* eventBus,
    std::map<std::pair<int, int>, std::map<float, DecisionPoint>>& EVresults
)
    : bankroll(gameConfig.wallet), 
    config(gameConfig), 
    deck(std::move(deck)), 
    player(player),
    reporter(eventBus, gameConfig.emitEvents),
    fixedEngine(config.monteCarloActions,EVresults,gameConfig)
{
    config.penetrationThreshold = (1-config.penetrationThreshold) * config.numDecks * Deck::NUM_CARDS_IN_DECK;
}

static bool isInsuranceMonteCarloActionSet(const GameConfig& config) {
    return std::find(config.monteCarloActions.begin(), config.monteCarloActions.end(), Action::InsuranceAccept) != config.monteCarloActions.end() ||
           std::find(config.monteCarloActions.begin(), config.monteCarloActions.end(), Action::InsuranceDecline) != config.monteCarloActions.end();
}

std::pair<double, double> Engine::runner(){  
    while (deck->getSize() > config.penetrationThreshold ){
        playHand();
    }  
    return {bankroll.getBalance(), bankroll.getTotalMoneyBet()};
}

FixedEngine Engine::runnerMonte(){  
    while (deck->getSize() > config.penetrationThreshold ){
        playHand();
    }  
    return {fixedEngine};
}

void Engine::playHand(){
    player->updateDeckStrategySize(deck->getSize());
    std::vector<Hand> hands;

    int bet = player->getBetSize();
    bankroll.withdraw(bet);
    bankroll.addTotalBet(bet);

    Hand dealer = draw_cards();
    Hand user = draw_cards(bet);

    // Count visible cards
    player->updateCount(dealer.getCards()[0]);

    for (const Card& card : user.getCards()) {
        player->updateCount(card);
    }

    // Monte Carlo for insurance decisions must run BEFORE the insurance phase.
    if (config.enabelMontiCarlo && isInsuranceMonteCarloActionSet(config) && dealer.getCards().front().getRank() == Rank::Ace) {
        const std::pair<int, int> cardValues{user.getScore(), dealer.getCards().front().getValue()};
        if (config.actionValues.count(cardValues)) {
            fixedEngine.calculateEV(*player, *deck, dealer, user, player->getTrueCount(), cardValues);
        }
    }

    reporter.reportHand(dealer, "Dealer (showing)", true);
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

bool Engine::didPlayerGetNaturalBlackjack(std::vector<Hand>& hands){
    if (hands.size() == 1 && hands[0].isBlackjack()){
        return true;
    }
    return false;
}

void Engine::NaturalBlackJackHandler(Hand& dealer, Hand& user){
    std::ostringstream roundSummary;
    roundSummary << "Natural Blackjack win! " << ". ";
    bankroll.deposit(user.getBetSize() + user.getBetSize() * config.blackjackPayoutMultiplier);

    std::string outcome = "Natural Blackjack win";
    roundSummary << "Hand " << (1) << ": " << outcome << " (score " << 21 << ", bet " << user.getBetSize() << "); ";

    reporter.reportRoundResult(roundSummary.str());
    reporter.reportStats(bankroll, *player->getStrategy());
    return;
}   

void Engine::evaluateHands(Hand& dealer, std::vector<Hand>& hands){
    player->updateCount(dealer.getCards()[1]); // Reveal hole card

    std::vector<int> scores = getPlayerScores(hands);

    if(didPlayerGetNaturalBlackjack(hands) && !dealer.isBlackjack()){
        NaturalBlackJackHandler(dealer, hands[0]);
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

        if (dealer_score > score){
            outcome = "Dealer win";
        }
        else if (dealer_score < score){
            bankroll.deposit(hand.getBetSize() * 2);
            outcome = "Player win";
        }
        else if (dealer_score == 0 && score ==0){
            outcome = "Player bust";
        }
        else {
            bankroll.deposit(hand.getBetSize());
        }

        roundSummary << "Hand " << (i + 1) << ": " << outcome << " (score " << score << ", bet " << hand.getBetSize() << "); ";
    }

    reporter.reportRoundResult(roundSummary.str());
    reporter.reportStats(bankroll, *player->getStrategy());
}

std::vector<Hand> Engine::user_play(Hand& dealer, Hand& user){
    std::vector<Hand> hands;
    
    play_hand(dealer, user, hands, false);
    return hands;
}

void Engine::play_hand(Hand& dealer, Hand& user, std::vector<Hand>& hands, bool has_split_aces, bool has_split){ 
    bool game_over = false;
    const std::string handLabel = has_split_aces ? "Player (split aces)" : "Player";
    reporter.reportHand(user, handLabel); 

    const std::pair<int,int> cardValues{user.getScore(), dealer.getCards().front().getValue()};

    // Check if we should run monte carlo for this hand.
    // Insurance MC is handled earlier in playHand() (before insurance resolution).
    bool shouldRunMonteCarlo = config.enabelMontiCarlo && config.actionValues.count(cardValues) && !isInsuranceMonteCarloActionSet(config);
    
    // If requirePairForMonteCarlo is set, only run if hand is a splittable pair
    if (shouldRunMonteCarlo && config.requirePairForMonteCarlo && !user.checkCanSplit()) {
        shouldRunMonteCarlo = false;
    }
    
    if (shouldRunMonteCarlo) {
        const bool isSoftHand = user.isHandSoft();
        if (config.allowSoftHandsInMonteCarlo || !isSoftHand) {
            fixedEngine.calculateEV(*player, *deck, dealer, user, player->getTrueCount(), cardValues);
        }
    }

    while(!game_over){
        Action action = player->getAction(user, dealer, player->getTrueCount());
        
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
                game_over = splitHandler(user, dealer, hands, handLabel, has_split_aces, true);
                break;
            case Action::Surrender:
                game_over = surrenderHandler(user, hands, handLabel);
                break;
            case Action::Skip: 
                break;
        }
    }
}

Hand Engine::draw_cards(int betSize){
    Hand hand = Hand(deck->deal(), betSize);
    return hand;
}

void Engine::dealer_draw(Hand& dealer){
    reporter.reportHand(dealer, "Dealer");
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
        player->updateCount(c);
        dealer.addCard(c);
        reporter.reportHand(dealer, "Dealer");
    }
    return;
}


bool Engine::handleInsurancePhase(Hand& dealer, Hand& user) {
    if (!canOfferInsurance(dealer)) {
        return false;
    }

    reporter.reportHand(user, "Player");
    bool accepted = askInsurance();

    std::ostringstream oss;
    oss << "Insurance offered. Strategy " << (accepted ? "accepts" : "declines");
    reporter.reportMessage(EventType::ActionTaken, oss.str());
    reporter.reportHand(dealer, "Dealer", true);

    return resolveInsurance(accepted, dealer, user);
}

bool Engine::canOfferInsurance(Hand& dealer) {
    return dealer.OfferInsurance();
}

bool Engine::askInsurance() {
    return player->shouldAcceptInsurance();
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
        player->updateCount(dealer.getCards()[1]); // Reveal hole card
        
        if (playerHasBlackjack) {
            bankroll.deposit(user.getBetSize() * 2.5);
            reporter.reportInsuranceResult("Insurance wins: dealer blackjack vs player blackjack");
        } else {
            bankroll.deposit(user.getBetSize() * 1.5);
            reporter.reportInsuranceResult("Insurance wins: dealer blackjack");
        }
        reporter.reportStats(bankroll, *player->getStrategy());
        return true; 
    } else {
        reporter.reportMessage(EventType::ActionTaken, "Insurance accepted automatically: dealer lacked blackjack");
        bankroll.withdraw(user.getBetSize() * INSURANCEBETCOST);
        bankroll.addTotalBet(user.getBetSize() * INSURANCEBETCOST);
        return false; // Round continues
    }
}

bool Engine::handleInsuranceDeclined(Hand& dealer, Hand& user) {
    bool dealerHasBlackjack = dealer.dealerHiddenTen();
    bool playerHasBlackjack = user.isBlackjack();

    if (dealerHasBlackjack) {
        player->updateCount(dealer.getCards()[1]); // Reveal hole card
        if (playerHasBlackjack) {

            bankroll.deposit(user.getBetSize());
            reporter.reportRoundResult("Dealer blackjack pushes player blackjack (no insurance)");
            reporter.reportStats(bankroll, *player->getStrategy());
        } else {

            reporter.reportRoundResult("Dealer blackjack; player loses without insurance");
            reporter.reportStats(bankroll, *player->getStrategy());
        }
        return true; 
    } else {
        reporter.reportMessage(EventType::ActionTaken, "Insurance declined; dealer lacks blackjack");
        return false;
    }
}

bool Engine::dealerRobberyHandler(Hand& dealer,Hand& user){
    if (dealer.dealerShowsTen() && dealer.dealerHiddenAce()){
        reporter.reportHand(user, "Player"); 
        player->updateCount(dealer.getCards()[1]); // Reveal hole card
        if (!user.isBlackjack()){
            // Lose. Do nothing.
        } else {

             bankroll.deposit(user.getBetSize());
        }
        reporter.reportDealerFlip(dealer);
        reporter.reportStats(bankroll, *player->getStrategy());
        return true;
    }
    return false;
    
}

bool Engine::standHandler(Hand& user, std::vector<Hand>& hands, std::string handLabel){
    hands.emplace_back(user);
    reporter.reportAction(Action::Stand, user, handLabel);
    return true;
}

bool Engine::hitHandler(Hand& user, std::vector<Hand>& hands, std::string handLabel){
    Card c = deck->hit();
    player->updateCount(c);
    user.addCard(c);

    reporter.reportAction(Action::Hit, user, handLabel);

    if (user.checkOver()) {hands.emplace_back(user); return true;}

    return false;
}

bool Engine::doubleHandler(Hand& user, std::vector<Hand>& hands, std::string handLabel,bool has_split){
    if (has_split && !config.doubleAfterSplitAllowed){
        reporter.reportMessage(EventType::ActionTaken, handLabel + " cannot double after split; hits instead");

        Card c = deck->hit();
        player->updateCount(c);
        user.addCard(c);

        hands.emplace_back(user);

        reporter.reportAction(Action::Double, user, handLabel);
        return true;
    }

    // Deduct additional bet
    bankroll.withdraw(user.getBetSize());
    bankroll.addTotalBet(user.getBetSize());

    user.doubleBet();
    Card card = deck->hit();
    player->updateCount(card);
    user.addCard(card);
    hands.emplace_back(user);

    reporter.reportAction(Action::Double, user, handLabel);
    return true;
}

bool Engine::splitHandler(Hand& user, Hand& dealer, std::vector<Hand>& hands, std::string handLabel, bool has_split_aces,bool has_split){
     // Check if we're splitting Aces
    bool splitting_aces = (user.peekFrontCard() == Rank::Ace);

    if (splitting_aces && has_split_aces && !config.allowReSplitAces){
        // Resplitting aces not allowed: just add the hand and stop.
        hands.emplace_back(user);
        return true;
    }
    
    Hand user2 = Hand(user.getLastCard(),user.getBetSize());
    user.popLastCard();

    // Deduct bet for new hand
    bankroll.withdraw(user2.getBetSize());
    bankroll.addTotalBet(user2.getBetSize());

    Card card1 = deck->hit();
    player->updateCount(card1);
    user.addCard(card1);

    Card card2 = deck->hit();
    player->updateCount(card2);
    user2.addCard(card2);

    reporter.reportSplit(handLabel, user, user2);

    if (splitting_aces) {
        // One-card only after splitting aces; allow resplit only when the new hand is still two aces.
        if (user.isAces() && config.allowReSplitAces) {
            splitHandler(user, dealer, hands, handLabel, true, true);
        } else {
            hands.emplace_back(user);
        }

        if (user2.isAces() && config.allowReSplitAces) {
            splitHandler(user2, dealer, hands, handLabel, true, true);
        } else {
            hands.emplace_back(user2);
        }
        return true;
    }

    play_hand(dealer, user, hands, false, true);
    play_hand(dealer, user2, hands, false, true);
    return true;
}

bool Engine::surrenderHandler(Hand& user, std::vector<Hand>& hands, std::string handLabel){

    bankroll.deposit(static_cast<double>(user.getBetSize()) * SURRENDERMULTIPLIER);
    reporter.reportAction(Action::Surrender, user, handLabel);
    reporter.reportStats(bankroll, *player->getStrategy());
    return true;
}