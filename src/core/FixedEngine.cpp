#include "FixedEngine.h"
#include "Deck.h"
#include "Engine.h"
#include "ActionStats.h"
#include "MonteCarloScenario.h"

#include <fstream>
#include <iomanip>
#include <filesystem>
#include <limits>
#include <cmath>

FixedEngine::FixedEngine() {}
FixedEngine::FixedEngine(std::vector<Action> monteCarloActions,std::map<std::pair<int, int>, std::map<float, DecisionPoint>> EVresults, const GameConfig& gameConfig) : monteCarloActions(monteCarloActions), EVresults(EVresults), config(gameConfig) {}

void FixedEngine::calculateEV(Player& player, Deck& deck, Hand& dealer, Hand& user, float trueCount,std::pair<int,int> cardValues) {
    for (Action forcedAction : monteCarloActions) {
        Hand simDealer = dealer;
        Hand simUser = user;
        Deck simDeck = deck.clone();
        std::vector<Hand> hands;

        playForcedHand(player, simDeck, simDealer, simUser, hands, forcedAction, false, false,trueCount);
        Hand evalDealer = simDealer;
        evaluateHand(simDeck, evalDealer, hands, trueCount, forcedAction,cardValues, simUser.getBetSize());

    }
}

void FixedEngine::calculateEVForScenario(Player& player, Deck& deck, Hand& dealer, Hand& user, float trueCount, 
                                          std::pair<int,int> cardValues, const MonteCarloScenario& scenario) {
    for (Action forcedAction : scenario.actions) {
        Hand simDealer = dealer;
        Hand simUser = user;
        Deck simDeck = deck.clone();
        std::vector<Hand> hands;

        playForcedHand(player, simDeck, simDealer, simUser, hands, forcedAction, false, false, trueCount);
        Hand evalDealer = simDealer;
        evaluateHandForScenario(simDeck, evalDealer, hands, trueCount, forcedAction, cardValues, simUser.getBetSize(), scenario.name);
    }
}

void FixedEngine::playForcedHand(Player& player, Deck& deck, Hand& dealer, Hand& user,std::vector<Hand>& hands, Action forcedAction,bool has_split_aces, bool has_split,float trueCount){
    bool game_over = false;
    int i = 0;
    Action action = Action::Skip;
    while(!game_over){
        if (i == 0 && forcedAction != Action::Skip){
            action = forcedAction;
        }
        else{
            action = player.getAction(user, dealer, trueCount);
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
                game_over = splitHandler(player, deck, user, dealer, hands, has_split, has_split_aces, trueCount);
                break;
            case Action::InsuranceAccept:
                game_over = InsuranceHandler(player, deck, user, dealer, hands, trueCount);
                break;
            case Action::InsuranceDecline:
                game_over = InsuranceHandler(player, deck, user, dealer, hands, trueCount);
                break;
            case Action::Surrender:
                game_over = surrenderHandler(user, hands);
                break;
            case Action::Skip:
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

bool FixedEngine::surrenderHandler(Hand& user,std::vector<Hand>& hands){
    hands.emplace_back(user);
    return true;
}

bool FixedEngine::InsuranceHandler(Player& /*player*/, Deck& /*deck*/, Hand& user, Hand& dealer, std::vector<Hand>& hands, float /*trueCount*/){
    bool dealerHasBlackjack = dealer.dealerHiddenTen();

    if (dealerHasBlackjack) {  
        hands.emplace_back(user);
        return true; 
    } else {
        return false; // Round continues
    }
}


bool FixedEngine::doubleHandler(Deck& deck, Hand& user,std::vector<Hand>& hands, bool has_split){
    if (has_split && !config.doubleAfterSplitAllowed){
        Card card = deck.hit();
        user.addCard(card);
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

bool FixedEngine::splitHandler(Player& player, Deck& deck, Hand& user,Hand& dealer, std::vector<Hand>& hands,bool /*has_split*/, bool has_split_aces, float trueCount){
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
            splitHandler(player, deck, user, dealer, hands, true, true, trueCount);
        } else {
            hands.emplace_back(user);
        }

        if (user2.isAces() && config.allowReSplitAces) {
            splitHandler(player, deck, user2, dealer, hands, true, true, trueCount);
        } else {
            hands.emplace_back(user2);
        }
        return true;
    }

    // Play out both split hands using normal strategy decisions
    playForcedHand(player, deck, dealer, user, hands, Action::Skip, has_split_aces || splitting_aces, true, trueCount);
    playForcedHand(player, deck, dealer, user2, hands, Action::Skip, has_split_aces || splitting_aces, true, trueCount);
    return true;
    
}

void FixedEngine::evaluateHand(Deck& deck, Hand& dealer, std::vector<Hand>& hands, float trueCount, Action forcedAction, std::pair<int,int> cardValues, int baseBet) {
    float bucketedTrueCount = std::round(trueCount * 2.0f) / 2.0f;
    DecisionPoint& decisionPoint = EVresults[cardValues][bucketedTrueCount];

    if (forcedAction == Action::Split) {
        //decisionPoint.splitStats.timesSplit();
        double splitPayout = 0.0;

        for (Hand& hand : hands) {
            int userScore = hand.getFinalScore();

            if (userScore != 0){
                dealer_draw(deck, dealer);
            }
            
            int dealerScore = dealer.getFinalScore();
            float result = 1.0f;

            if (dealerScore > userScore){
                result *= -1.0f;
            }
            else if (dealerScore < userScore){
                result *= 1.0f;
            }
            else if (dealerScore == 0 && userScore == 0){
                result *= -1.0f;
            }
            else {
                result *= 0.0f;
            }

            // Normalize by base bet while still accounting for doubles after split
            double betMultiplier = 1.0;
            if (baseBet > 0) {
                betMultiplier = static_cast<double>(hand.getBetSize()) / static_cast<double>(baseBet);
            }
            splitPayout += result * betMultiplier;
        }

        decisionPoint.splitStats.addResult(splitPayout);
        return;
    }

    for (Hand& hand : hands) {
        int userScore = hand.getFinalScore();

        if(hand.isBlackjack() && !dealer.isBlackjack() && hands.size() == 1){
            if (forcedAction == Action::InsuranceAccept){
                decisionPoint.insuranceAcceptStats.addResult(1.0f);
                return;
            }
            else if (forcedAction == Action::InsuranceDecline){
                decisionPoint.insuranceDeclineStats.addResult(1.5f);
                return;
            }
            decisionPoint.standStats.addResult(1.5f);
            return;
        }

        if (userScore != 0){
            dealer_draw(deck,dealer);
        }
        
        int dealerScore = dealer.getFinalScore();
        float result = 1.0f;

        if (dealerScore > userScore){
            result *= -1.0f;
        }
        else if (dealerScore < userScore){
            result *= 1.0f;
        }
        else if (dealerScore == 0 && userScore == 0){
            result *= -1.0f;
        }
        else {
            result *= 0.0f;
        }

        if (forcedAction == Action::Hit) {
            decisionPoint.hitStats.addResult(result);
        } else if (forcedAction == Action::Stand) {
            decisionPoint.standStats.addResult(result);
        }
        else if (forcedAction == Action::Double){
            decisionPoint.doubleStats.addResult(result * 2.0f);
        }
        else if (forcedAction == Action::Surrender){
            decisionPoint.surrenderStats.addResult(-0.5f);
        }
        else if (forcedAction == Action::InsuranceAccept){
            if (dealer.isBlackjack() && hand.isBlackjack() && hands.size() == 1){
                decisionPoint.insuranceAcceptStats.addResult(1.0f);
            }
            else if (dealer.isBlackjack() && !hand.isBlackjack()){
                decisionPoint.insuranceAcceptStats.addResult(0.0f);
            }
            else{
            decisionPoint.insuranceAcceptStats.addResult(result - 0.5f);
            }
        }
        else if (forcedAction == Action::InsuranceDecline){
            decisionPoint.insuranceDeclineStats.addResult(result);
        }
    }

}

void FixedEngine::evaluateHandForScenario(Deck& deck, Hand& dealer, std::vector<Hand>& hands, float trueCount, 
                                           Action forcedAction, std::pair<int,int> cardValues, int baseBet,
                                           const std::string& scenarioName) {
    float bucketedTrueCount = std::round(trueCount * 2.0f) / 2.0f;
    DecisionPoint& decisionPoint = scenarioResults[scenarioName][cardValues][bucketedTrueCount];

    if (forcedAction == Action::Split) {
        //decisionPoint.splitStats.timesSplit();
        double splitPayout = 0.0;

        for (Hand& hand : hands) {
            int userScore = hand.getFinalScore();

            if (userScore != 0){
                dealer_draw(deck, dealer);
            }
            
            int dealerScore = dealer.getFinalScore();
            float result = 1.0f;

            if (dealerScore > userScore){
                result *= -1.0f;
            }
            else if (dealerScore < userScore){
                result *= 1.0f;
            }
            else if (dealerScore == 0 && userScore == 0){
                result *= -1.0f;
            }
            else {
                result *= 0.0f;
            }

            double betMultiplier = 1.0;
            if (baseBet > 0) {
                betMultiplier = static_cast<double>(hand.getBetSize()) / static_cast<double>(baseBet);
            }
            splitPayout += result * betMultiplier;
        }

        decisionPoint.splitStats.addResult(splitPayout);
        return;
    }
    
    for (Hand& hand : hands) {
        int userScore = hand.getFinalScore();

        if(hand.isBlackjack() && !dealer.isBlackjack() && hands.size() == 1){
            if (forcedAction == Action::InsuranceAccept){
                decisionPoint.insuranceAcceptStats.addResult(1.0f);
                return;
            }
            else if (forcedAction == Action::InsuranceDecline){
                decisionPoint.insuranceDeclineStats.addResult(1.5f);
                return;
            }
            decisionPoint.standStats.addResult(1.5f);
            return;
        }

        if (userScore != 0){
            dealer_draw(deck, dealer);
        }
        
        int dealerScore = dealer.getFinalScore();
        float result = 1.0f;

        if (dealerScore > userScore){
            result *= -1.0f;
        }
        else if (dealerScore < userScore){
            result *= 1.0f;
        }
        else if (dealerScore == 0 && userScore == 0){
            result *= -1.0f;
        }
        else {
            result *= 0.0f;
        }

        if (forcedAction == Action::Hit) {
            decisionPoint.hitStats.addResult(result);
        } else if (forcedAction == Action::Stand) {
            decisionPoint.standStats.addResult(result);
        }
        else if (forcedAction == Action::Double){
            decisionPoint.doubleStats.addResult(result * 2.0f);
        }
        else if (forcedAction == Action::Surrender){
            decisionPoint.surrenderStats.addResult(-0.5f);
        }
        else if (forcedAction == Action::InsuranceAccept){
            if (dealer.isBlackjack() && hand.isBlackjack() && hands.size() == 1){
                decisionPoint.insuranceAcceptStats.addResult(1.0f);
            }
            else if (dealer.isBlackjack() && !hand.isBlackjack()){
                decisionPoint.insuranceAcceptStats.addResult(0.0f);
            }
            else{
                decisionPoint.insuranceAcceptStats.addResult(result - 0.5f);
            }
        }
        else if (forcedAction == Action::InsuranceDecline){
            decisionPoint.insuranceDeclineStats.addResult(result);
        }
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

    out << "UserValue,DealerValue,TrueCount,"
        << "Hit EV,Hit Variance,Stand EV,Stand Variance,Double EV,Double Variance,Split EV,Split Variance,Surrender EV,Surrender Variance,Insurance Accept EV,Insurance Accept Variance,Insurance Decline EV,Insurance Decline Variance,Hands Played" << '\n';

    for (const auto& [cardValues, tcMap] : EVresults) {
        for (const auto& [trueCount, decisionPoint] : tcMap) {
            // Find first non-zero hands played count
            int handsPlayed = decisionPoint.hitStats.handsPlayed;
            if (handsPlayed == 0) handsPlayed = decisionPoint.standStats.handsPlayed;
            if (handsPlayed == 0) handsPlayed = decisionPoint.doubleStats.handsPlayed;
            if (handsPlayed == 0) handsPlayed = decisionPoint.splitStats.handsPlayed;
            if (handsPlayed == 0) handsPlayed = decisionPoint.surrenderStats.handsPlayed;
            if (handsPlayed == 0) handsPlayed = decisionPoint.insuranceAcceptStats.handsPlayed;
            if (handsPlayed == 0) handsPlayed = decisionPoint.insuranceDeclineStats.handsPlayed;

            out << cardValues.first << ','
                << cardValues.second << ','
                << std::setprecision(std::numeric_limits<float>::max_digits10) << std::defaultfloat
                << trueCount << ','
                << std::fixed << std::setprecision(6)
                << decisionPoint.hitStats.getEV() << ','
                << decisionPoint.hitStats.getVariance() << ','
                << decisionPoint.standStats.getEV() << ','
                << decisionPoint.standStats.getVariance() << ','
                << decisionPoint.doubleStats.getEV() << ','
                << decisionPoint.doubleStats.getVariance() << ','
                << decisionPoint.splitStats.getEV() << ','
                << decisionPoint.splitStats.getVariance() << ','
                << decisionPoint.surrenderStats.getEV() << ','
                << decisionPoint.surrenderStats.getVariance() << ','
                << decisionPoint.insuranceAcceptStats.getEV() << ','
                << decisionPoint.insuranceAcceptStats.getVariance() << ','
                << decisionPoint.insuranceDeclineStats.getEV() << ','
                << decisionPoint.insuranceDeclineStats.getVariance() << ','
                << handsPlayed
                << '\n';
        }
    }
}

void FixedEngine::merge(const FixedEngine& other){
    // Properly merge ActionStats using Welford's parallel algorithm
    auto accumulate = [](ActionStats& dst, const ActionStats& src) {
        // Always accumulate splitsPlayed for split tracking
        dst.splitsPlayed += src.splitsPlayed;
        
        if (src.handsPlayed == 0) return;
        
        int totalCount = dst.handsPlayed + src.handsPlayed;
        double delta = src.mean - dst.mean;
        
        // Combine totals
        dst.totalPayout += src.totalPayout;
        
        // Combine means and M2 using parallel Welford's algorithm
        dst.mean = (dst.handsPlayed * dst.mean + src.handsPlayed * src.mean) / totalCount;
        dst.M2 = dst.M2 + src.M2 + delta * delta * dst.handsPlayed * src.handsPlayed / totalCount;
        
        dst.handsPlayed = totalCount;
    };

    // Merge legacy EVresults
    for (const auto& [cardValues, tcMapOther] : other.EVresults) {
        auto& currentTcMap = EVresults[cardValues];
        for (const auto& [trueCount, decisionPoint] : tcMapOther) {
            DecisionPoint& currentPoint = currentTcMap[trueCount];

            accumulate(currentPoint.hitStats, decisionPoint.hitStats);
            accumulate(currentPoint.standStats, decisionPoint.standStats);
            accumulate(currentPoint.doubleStats, decisionPoint.doubleStats);
            accumulate(currentPoint.splitStats, decisionPoint.splitStats);
            accumulate(currentPoint.surrenderStats, decisionPoint.surrenderStats);
            accumulate(currentPoint.insuranceAcceptStats, decisionPoint.insuranceAcceptStats);
            accumulate(currentPoint.insuranceDeclineStats, decisionPoint.insuranceDeclineStats);
        }
    }
    
    // Merge multi-scenario results
    for (const auto& [scenarioName, resultsMapOther] : other.scenarioResults) {
        auto& currentScenarioMap = scenarioResults[scenarioName];
        for (const auto& [cardValues, tcMapOther] : resultsMapOther) {
            auto& currentTcMap = currentScenarioMap[cardValues];
            for (const auto& [trueCount, decisionPoint] : tcMapOther) {
                DecisionPoint& currentPoint = currentTcMap[trueCount];

                accumulate(currentPoint.hitStats, decisionPoint.hitStats);
                accumulate(currentPoint.standStats, decisionPoint.standStats);
                accumulate(currentPoint.doubleStats, decisionPoint.doubleStats);
                accumulate(currentPoint.splitStats, decisionPoint.splitStats);
                accumulate(currentPoint.surrenderStats, decisionPoint.surrenderStats);
                accumulate(currentPoint.insuranceAcceptStats, decisionPoint.insuranceAcceptStats);
                accumulate(currentPoint.insuranceDeclineStats, decisionPoint.insuranceDeclineStats);
            }
        }
    }
}

const std::map<std::pair<int, int>, std::map<float, DecisionPoint>>& FixedEngine::getResults() const 
{ 
    return EVresults; 
}

const std::map<std::pair<int, int>, std::map<float, DecisionPoint>>& FixedEngine::getScenarioResults(const std::string& scenarioName) const {
    static const std::map<std::pair<int, int>, std::map<float, DecisionPoint>> empty;
    auto it = scenarioResults.find(scenarioName);
    if (it == scenarioResults.end()) {
        return empty;
    }
    return it->second;
}

std::vector<std::string> FixedEngine::getScenarioNames() const {
    std::vector<std::string> names;
    names.reserve(scenarioResults.size());
    for (const auto& [name, _] : scenarioResults) {
        names.push_back(name);
    }
    return names;
}

void FixedEngine::saveScenarioResults(const std::string& scenarioName, const std::string& baseFilename) const {
    auto it = scenarioResults.find(scenarioName);
    if (it == scenarioResults.end()) {
        std::cerr << "No results found for scenario: " << scenarioName << std::endl;
        return;
    }
    
    std::filesystem::path outPath(baseFilename);
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

    out << "UserValue,DealerValue,TrueCount,"
        << "Hit EV,Hit Variance,Stand EV,Stand Variance,Double EV,Double Variance,Split EV,Split Variance,Surrender EV,Surrender Variance,Insurance Accept EV,Insurance Accept Variance,Insurance Decline EV,Insurance Decline Variance,Hands Played" << '\n';

    const auto& resultsMap = it->second;
    for (const auto& [cardValues, tcMap] : resultsMap) {
        for (const auto& [trueCount, decisionPoint] : tcMap) {
            int handsPlayed = decisionPoint.hitStats.handsPlayed;
            if (handsPlayed == 0) handsPlayed = decisionPoint.standStats.handsPlayed;
            if (handsPlayed == 0) handsPlayed = decisionPoint.doubleStats.handsPlayed;
            if (handsPlayed == 0) handsPlayed = decisionPoint.splitStats.handsPlayed;
            if (handsPlayed == 0) handsPlayed = decisionPoint.surrenderStats.handsPlayed;
            if (handsPlayed == 0) handsPlayed = decisionPoint.insuranceAcceptStats.handsPlayed;
            if (handsPlayed == 0) handsPlayed = decisionPoint.insuranceDeclineStats.handsPlayed;

            out << cardValues.first << ','
                << cardValues.second << ','
                << std::setprecision(std::numeric_limits<float>::max_digits10) << std::defaultfloat
                << trueCount << ','
                << std::fixed << std::setprecision(6)
                << decisionPoint.hitStats.getEV() << ','
                << decisionPoint.hitStats.getVariance() << ','
                << decisionPoint.standStats.getEV() << ','
                << decisionPoint.standStats.getVariance() << ','
                << decisionPoint.doubleStats.getEV() << ','
                << decisionPoint.doubleStats.getVariance() << ','
                << decisionPoint.splitStats.getEV() << ','
                << decisionPoint.splitStats.getVariance() << ','
                << decisionPoint.surrenderStats.getEV() << ','
                << decisionPoint.surrenderStats.getVariance() << ','
                << decisionPoint.insuranceAcceptStats.getEV() << ','
                << decisionPoint.insuranceAcceptStats.getVariance() << ','
                << decisionPoint.insuranceDeclineStats.getEV() << ','
                << decisionPoint.insuranceDeclineStats.getVariance() << ','
                << handsPlayed
                << '\n';
        }
    }
}