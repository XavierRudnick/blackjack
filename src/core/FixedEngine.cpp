#include "FixedEngine.h"
#include "Deck.h"
#include "Engine.h"

#include <fstream>
#include <iomanip>
#include <filesystem>
#include <limits>

FixedEngine::FixedEngine() {}
FixedEngine::FixedEngine(std::vector<Action> monteCarloActions) : monteCarloActions(monteCarloActions) {}

void FixedEngine::calculateEV(Player& player, Deck& deck, Hand& dealer, Hand& user, float trueCount){
    // Play out the hand
    for (Action forcedAction : monteCarloActions) {
        Hand simDealer = dealer;
        Hand simUser = user;
        Deck simDeck = deck.clone();
        std::vector<Hand> hands;
        playForcedHand(player, simDeck, simDealer, simUser,hands, forcedAction);
        float result = evaluateHand(simDeck, simDealer, simUser);

        if (forcedAction == Action::Hit) {
            EVresults[trueCount].hitStats.addResult(result);
        } else if (forcedAction == Action::Stand) {
            EVresults[trueCount].standStats.addResult(result);
        }
        else if (forcedAction == Action::Double){
            EVresults[trueCount].doubleStats.addResult(result * 2);
        }
    }
    
}

void FixedEngine::playForcedHand(Player& player, Deck& deck, Hand& dealer, Hand& user,std::vector<Hand>& hands, Action forcedAction,bool has_split_aces, bool has_split){
    bool game_over = false;
    int i = 0;
    Action action = Action::Skip;
    while(!game_over){
        if (i == 0){
            action = forcedAction;
        }
        else{
            action = player.getAction(user, dealer, player.getTrueCount());
        }

        switch(action)
        {
            case Action::Stand:
                game_over = standHandler(user,hands);
                break;
            case Action::Hit:
                game_over = hitHandler(deck,user,hands);
                break;
            case Action::Double:
                game_over = doubleHandler(deck,user,hands,has_split);
                break;
            case Action::Split: 
                game_over = splitHandler(deck,user,dealer,hands,false,false);
                break;
            case Action::Skip:
                throw std::runtime_error("Skip isnt supposed to happen");
                break;
            case Action::Surrender:
                throw std::runtime_error("Surrender isnt supposed to happen");
                break;
        }   
        ++i; // allow only the first decision to be forced; subsequent ones follow the player strategy
    }
    return;
}

bool FixedEngine::standHandler(Hand& user,std::vector<Hand>& hands){
    hands.emplace_back(user);
    return true;
}

bool FixedEngine::hitHandler(Deck& deck, Hand& user,std::vector<Hand>& hands){
    Card c = deck.hit();
    user.addCard(c);

    if (user.checkOver()) {hands.emplace_back(user); return true;}

    return false;
}

bool FixedEngine::doubleHandler(Deck& deck, Hand& user,std::vector<Hand>& hands, bool has_split){
    if (has_split && !config.doubleAfterSplitAllowed){
        Card card = deck.hit();
        user.addCard(card);
        hands.emplace_back(user);
        return false;
    }
    else{
        user.doubleBet();
        Card card = deck.hit();
        user.addCard(card);

        hands.emplace_back(user);
    }

    return true;
}

bool FixedEngine::splitHandler(Deck& deck, Hand& user,Hand& dealer, std::vector<Hand>& hands,bool has_split, bool has_split_aces){
         // Check if we're splitting Aces
    bool splitting_aces = (user.peekFrontCard() == Rank::Ace);

    if (!user.checkCanSplit()){
        return false;
    }

    if (splitting_aces && has_split_aces && !config.allowReSplitAces){
        // Resplitting aces not allowed: just add the hand and stop.
        hands.emplace_back(user);
        return true;
    }
    
    Hand user2 = Hand(user.getLastCard(),user.getBetSize());
    user.popLastCard();

    Card card1 = deck.hit();
    user.addCard(card1);

    Card card2 = deck.hit();
    user2.addCard(card2);

    if (splitting_aces) {
        // One-card only after splitting aces; allow resplit only when the new hand is still two aces.
        if (user.isAces() && config.allowReSplitAces) {
            splitHandler(deck, user, dealer, hands, true, true);
        } else {
            hands.emplace_back(user);
        }

        if (user2.isAces() && config.allowReSplitAces) {
            splitHandler(deck, user2, dealer, hands, true, true);
        } else {
            hands.emplace_back(user2);
        }
        return true;
    }

    playForcedHand(deck, dealer, user, hands, false, true);
    playForcedHand(deck, dealer, user2, hands, false, true);
    return true;
}

float FixedEngine::evaluateHand(Deck& deck, Hand& dealer, Hand& user){
    int score = user.getFinalScore();

    if(user.isBlackjack() && !dealer.isBlackjack()){
        return 1.5;
    }

    if (score != 0){
        dealer_draw(deck, dealer);
    }

    int dealer_score = dealer.getFinalScore();

    if (dealer_score > score){
        return -1;
    }
    else if (dealer_score < score){
        return 1;
    }
    else if (dealer_score == 0 && score == 0){
        return -1;
    }
    else {
        return 0;
    }

}

void FixedEngine::dealer_draw(Deck& deck,Hand& dealer){
    // Fix: Check for Hard 17 or > 17. If Soft 17, check rule.
    bool isSoft17 = dealer.isSoft17();
    int score = dealer.getScore();
    
    if (score > 17 || (score == 17 && !isSoft17)) {
        return;
    }
    while (!dealer.isDealerOver()){
        if (deck.getSize() < 1) {
            throw std::runtime_error("Not enough cards to draw initial hand 271");
        }
        
        Card c = deck.hit();
        dealer.addCard(c);
    }
    return;
}

void FixedEngine::printResults(){
        std::cout << "TrueCount, Hit EV, Stand EV, Double EV, Hands Played" << std::endl;
        for (const auto& [trueCount, decisionPoint] : EVresults) {
            std::cout << trueCount << ", "
                      << decisionPoint.hitStats.getEV() << ", "
                      << decisionPoint.standStats.getEV() << ", "
                      << decisionPoint.doubleStats.getEV() << ", "
                      << (decisionPoint.hitStats.handsPlayed)
                      << std::endl;
        }
    }

void FixedEngine::savetoCSVResults(const std::string& filename) const {
    std::filesystem::path outPath(filename);
    if (outPath.has_parent_path()) {
        std::error_code ec;
        std::filesystem::create_directories(outPath.parent_path(), ec);
        if (ec) {
            std::cerr << "Failed to create directory " << outPath.parent_path().string()
                      << ": " << ec.message() << std::endl;
            return;
        }
    }

    std::ofstream out(outPath, std::ios::out | std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "Failed to open " << outPath.string() << " for writing results." << std::endl;
        return;
    }

    out << "TrueCount,Hit EV,Stand EV,Double EV,Hands Played" << '\n';

    for (const auto& [trueCount, decisionPoint] : EVresults) {
        out << std::setprecision(std::numeric_limits<float>::max_digits10) << std::defaultfloat
            << trueCount << ','
            << std::fixed << std::setprecision(6)
            << decisionPoint.hitStats.getEV() << ','
            << decisionPoint.standStats.getEV() << ','
            << decisionPoint.doubleStats.getEV() << ','
            << decisionPoint.hitStats.handsPlayed
            << '\n';
    }
}

void FixedEngine::merge(const FixedEngine& other){
    for (const auto& [trueCount, decisionPoint] : other.EVresults) {
        DecisionPoint& currentPoint = EVresults[trueCount];

        currentPoint.hitStats.totalPayout += decisionPoint.hitStats.totalPayout;
        currentPoint.hitStats.handsPlayed += decisionPoint.hitStats.handsPlayed;

        currentPoint.standStats.totalPayout += decisionPoint.standStats.totalPayout;
        currentPoint.standStats.handsPlayed += decisionPoint.standStats.handsPlayed;

        currentPoint.doubleStats.totalPayout += decisionPoint.doubleStats.totalPayout;
        currentPoint.doubleStats.handsPlayed += decisionPoint.doubleStats.handsPlayed;
    }
}

const std::map<float, FixedEngine::DecisionPoint>& FixedEngine::getResults() const 
{ 
    return EVresults; 
}