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
        playForcedHand(player, simDeck, simDealer, simUser, forcedAction);
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

Hand FixedEngine::playForcedHand(Player& player, Deck& deck, Hand& dealer, Hand& user, Action forcedAction){
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
                game_over = standHandler(user);
                break;
            case Action::Hit:
                game_over = hitHandler(player,deck,user);
                break;
            case Action::Double:
                game_over = doubleHandler(player,deck,user);
                break;
            case Action::Split: 
                throw std::runtime_error("split isnt supposed to happen");
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
    return user;
}

bool FixedEngine::standHandler(Hand& user){
    return true;
}

bool FixedEngine::hitHandler(Player& player, Deck& deck, Hand& user){
    Card c = deck.hit();
    user.addCard(c);

    if (user.checkOver()) {return true;}

    return false;
}

bool FixedEngine::doubleHandler(Player& player, Deck& deck, Hand& user){
    user.doubleBet();
    Card card = deck.hit();
    user.addCard(card);

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