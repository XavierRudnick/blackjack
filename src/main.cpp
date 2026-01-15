#include <cstdlib>
#include <iostream>
#include <set>
#include "Engine.h"
#include "HiLoStrategy.h"
#include "ZenCountStrategy.h"
#include "RAPCStrategy.h"
#include "CountingStrategy.h"
#include "NoStrategy.h"
#include "observers/ConsoleObserver.h"
#include "observers/EventBus.h"
#include "EngineBuilder.h"
#include "HumanPlayer.h"
#include "BotPlayer.h"
#include <chrono> 
#include <array>
#include <sstream>
#include <functional>
#include "LoggingCountingStrategy.h"
#include "FixedEngine.h"
#include "MentorStrategy.h"
#include "OmegaIIStrategy.h"
#include "R14Strategy.h"
#include "WongHalvesStrategy.h"
#include "RPCStrategy.h"

void playManualGame(int numDecksUsed){
    ConsoleObserver consoleObserver;
    EventBus& bus = EventBus::getInstance();
    bus.detachAll();
    bus.registerObserver(&consoleObserver, {EventType::CardsDealt, EventType::ActionTaken, EventType::RoundEnded, EventType::GameStats});


    Deck deck(numDecksUsed);

    auto hilo = std::make_unique<HiLoStrategy>(numDecksUsed);
    HumanPlayer human(false, std::move(hilo)); 

    Engine hiLoEngine = EngineBuilder()
                        .withEventBus(&bus)
                        .setDeckSize(numDecksUsed)
                        .setDeck(deck)
                        .setPenetrationThreshold(.75)
                        .setInitialWallet(1000)
                        .enableEvents(true)
                        .with3To2Payout(true)
                        .withH17Rules(true)
                        .allowDoubleAfterSplit(true)
                        .build(&human);

    hiLoEngine.runner();
}

void runRTPsims(int numDecksUsed, int iterations, float deckPenetration,std::unique_ptr<CountingStrategy> strategy){

    std::pair<double, double> gameStats = {0, 0};
    ConsoleObserver consoleObserver;
    EventBus& bus = EventBus::getInstance();
    //bus.detachAll();
    //bus.registerObserver(&consoleObserver, {EventType::CardsDealt, EventType::ActionTaken, EventType::RoundEnded, EventType::GameStats});

    Deck deck(numDecksUsed);
    BotPlayer robot(false, std::move(strategy)); 

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; i++){
        deck.reset();
        robot.resetCount(numDecksUsed);

        std::pair<double, double> profit = {1000, 0};

        Engine hiLoEngine = EngineBuilder()
                                    .withEventBus(&bus)
                                    .setDeckSize(numDecksUsed)
                                    .setDeck(deck)
                                    .setPenetrationThreshold(deckPenetration)
                                    .setInitialWallet(1000)
                                    .enableEvents(false)
                                    .with3To2Payout(true)
                                    .withH17Rules(false)
                                    .allowDoubleAfterSplit(true)
                                    .allowReSplitAces(true)
                                    .build(&robot);
        profit = hiLoEngine.runner();

        if (i % 100000 == 0 && i != 0){
            std::cout  << "Completed " << i << " / " << iterations << " iterations." <<std::endl;
        }

        gameStats.first += profit.first;
        gameStats.second += profit.second;
    } 

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Simulation time for " << numDecksUsed << " decks and " << iterations << " iterations: " << duration.count() / 1000000.0 << " seconds." << std::endl;

    double average = gameStats.first / iterations;
    double avgMoneyBet = gameStats.second / iterations;
    double diff = average-1000;
    double normal =  1000.0 / avgMoneyBet;
    double money_lost_per = diff * normal;
    double rtp = (1000+money_lost_per) /1000;

    std::cout << "Average after " << iterations << " rounds: " << average << std::endl;
    std::cout << "Average money bet: " << avgMoneyBet << std::endl;
    std::cout << "Difference: " << diff << std::endl;
    std::cout << "Money gained/lost per 1000$ " << money_lost_per << "$" << std::endl;
    std::cout << "RTP " << rtp << std::endl;
}

void runMontesims(int numDecksUsed, int iterations, float deckPenetration, 
    const std::vector<Action>& monteCarloActions, 
    std::unique_ptr<CountingStrategy> strategy, 
    const std::set<std::pair<int,int>>& dealerVplayerValues, 
    bool blackJackPayout3to2, bool dealerHits17, bool allowDoubleAfterSplit, bool allowReSplitAces,
    bool allowSoftHandsInMonteCarlo, bool requirePairForMonteCarlo,
    const std::string& actionSetName) {

    EventBus& bus = EventBus::getInstance();
    Deck deck(numDecksUsed);
    
    std::string strategyName = strategy->getName();
    std::cout << "Running " << actionSetName << " simulations for strategy " << strategyName << std::endl;
    
    BotPlayer robot(false, std::move(strategy)); 
    FixedEngine fixedEngineTotal;

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; i++){
        deck.reset();
        robot.resetCount(numDecksUsed);

        Engine engine = EngineBuilder()
                            .withEventBus(&bus)
                            .setDeckSize(numDecksUsed)
                            .setDeck(deck)
                            .setPenetrationThreshold(deckPenetration)
                            .setInitialWallet(1000)
                            .enableEvents(false)
                            .with3To2Payout(blackJackPayout3to2)
                            .withH17Rules(dealerHits17)
                            .allowDoubleAfterSplit(allowDoubleAfterSplit)
                            .allowReSplitAces(allowReSplitAces)
                            .enableMontiCarlo(true)
                            .setActionValues(dealerVplayerValues)
                            .allowSoftHandsInMonteCarlo(allowSoftHandsInMonteCarlo)
                            .requirePairForMonteCarlo(requirePairForMonteCarlo)
                            .setActions(monteCarloActions)
                            .build(&robot);

        FixedEngine fixedEngine = engine.runnerMonte();
        fixedEngineTotal.merge(fixedEngine);

        if (i % 500000 == 0 && i != 0){
            std::cout  << "  Completed " << i << " / " << iterations << " iterations." << std::endl;
        }
    }
    
    std::ostringstream filename;
    filename << "stats/" << strategyName << "_" << actionSetName << ".csv";
    fixedEngineTotal.savetoCSVResults(filename.str());
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
    std::cout << "  Saved results to " << filename.str() << " (" << duration.count() << "s)" << std::endl;
}

int main(){
    int numDecksUsed = 6;
    float deckPenetration = 0.80;
    bool blackJackPayout3to2 = true; 
    bool dealerHits17 = false;
    bool allowDoubleAfterSplit = true;
    bool allowReSplitAces = true;
    int iterations = 10000000;

    // Action sets for comparison
    std::vector<Action> InsuranceChoices = {Action::InsuranceAccept, Action::InsuranceDecline};
    std::vector<Action> HitvStandActions = {Action::Hit, Action::Stand};
    std::vector<Action> HitvDoubleActions = {Action::Hit, Action::Double};
    std::vector<Action> SplitvStandActions = {Action::Split, Action::Stand};
    std::vector<Action> SurrendervHitActions = {Action::Surrender, Action::Hit};  // Surrender vs basic strategy (Hit)

    // Card value sets for each action comparison
    std::set<std::pair<int, int>> InsuranceValues = {
        {21,11}, {20,11}, {19,11}, {18,11}, {17,11}, {16,11}, {15,11},
        {14,11}, {13,11}, {12,11}, {11,11}, {10,11}, {9,11}, {8,11},
        {7,11}, {6,11}, {5,11}, {4,11}, {3,11}, {2,11}
    };

    std::set<std::pair<int, int>> HitvStandValues = {
        {16, 10}, {15, 10}, {12, 3}, {12, 2}, {13, 2}, {13, 3}
    };

    std::set<std::pair<int, int>> SplitvStandValues = {
        {20, 5},  // Pair of 10s vs 5
        {20, 6},  // Pair of 10s vs 6
    };

    std::set<std::pair<int, int>> HitvDoubleValues = {
        {10, 10}, {10, 11}, {11, 11}, {9, 2}, {9, 7}
    };

    std::set<std::pair<int, int>> SurrendervHitValues = {
        {15, 9}, {15, 10}, {14, 10}, {15, 11}, {16, 9}, {16, 10}, {16, 11}
    };

    // Helper lambda to create all strategies
    auto createStrategies = [numDecksUsed]() {
        std::vector<std::unique_ptr<CountingStrategy>> strategies;
        strategies.push_back(std::make_unique<HiLoStrategy>(numDecksUsed));
        strategies.push_back(std::make_unique<NoStrategy>(numDecksUsed));
        strategies.push_back(std::make_unique<MentorStrategy>(numDecksUsed));
        strategies.push_back(std::make_unique<RPCStrategy>(numDecksUsed));
        strategies.push_back(std::make_unique<RAPCStrategy>(numDecksUsed));
        strategies.push_back(std::make_unique<ZenCountStrategy>(numDecksUsed));
        strategies.push_back(std::make_unique<R14Strategy>(numDecksUsed));
        strategies.push_back(std::make_unique<OmegaIIStrategy>(numDecksUsed));
        strategies.push_back(std::make_unique<WongHalvesStrategy>(numDecksUsed));
        return strategies;
    };

    std::cout << "\n=== BLACKJACK MONTE CARLO SIMULATIONS ===" << std::endl;
    std::cout << "Decks: " << numDecksUsed << ", Penetration: " << deckPenetration 
              << ", Iterations per strategy: " << iterations << std::endl << std::endl;

    // Run Insurance simulations (dealer shows Ace)
    std::cout << "\n--- Insurance Accept vs Decline ---" << std::endl;
    for (auto& strategy : createStrategies()) {
        runMontesims(numDecksUsed, iterations, deckPenetration,
            InsuranceChoices, std::move(strategy), InsuranceValues,
            blackJackPayout3to2, dealerHits17, allowDoubleAfterSplit, allowReSplitAces,
            true,   // allowSoftHandsInMonteCarlo (dealer shows Ace)
            false,  // requirePairForMonteCarlo
            "InsuranceAccept_vs_Decline");
    }

    // Run Hit vs Stand simulations
    std::cout << "\n--- Hit vs Stand ---" << std::endl;
    for (auto& strategy : createStrategies()) {
        runMontesims(numDecksUsed, iterations, deckPenetration,
            HitvStandActions, std::move(strategy), HitvStandValues,
            blackJackPayout3to2, dealerHits17, allowDoubleAfterSplit, allowReSplitAces,
            false,  // allowSoftHandsInMonteCarlo
            false,  // requirePairForMonteCarlo
            "Hit_vs_Stand");
    }

    // Run Split vs Stand simulations (requires pair of 10s)
    std::cout << "\n--- Split vs Stand (Pair of 10s) ---" << std::endl;
    for (auto& strategy : createStrategies()) {
        runMontesims(numDecksUsed, iterations, deckPenetration,
            SplitvStandActions, std::move(strategy), SplitvStandValues,
            blackJackPayout3to2, dealerHits17, allowDoubleAfterSplit, allowReSplitAces,
            false,  // allowSoftHandsInMonteCarlo
            true,   // requirePairForMonteCarlo - only trigger when hand is a splittable pair
            "Split_vs_Stand_Pair10s");
    }

    // Run Hit vs Double simulations
    std::cout << "\n--- Hit vs Double ---" << std::endl;
    for (auto& strategy : createStrategies()) {
        runMontesims(numDecksUsed, iterations, deckPenetration,
            HitvDoubleActions, std::move(strategy), HitvDoubleValues,
            blackJackPayout3to2, dealerHits17, allowDoubleAfterSplit, allowReSplitAces,
            false,  // allowSoftHandsInMonteCarlo
            false,  // requirePairForMonteCarlo
            "Hit_vs_Double");
    }

    // Run Surrender vs Hit simulations
    std::cout << "\n--- Surrender vs Hit ---" << std::endl;
    for (auto& strategy : createStrategies()) {
        runMontesims(numDecksUsed, iterations, deckPenetration,
            SurrendervHitActions, std::move(strategy), SurrendervHitValues,
            blackJackPayout3to2, dealerHits17, allowDoubleAfterSplit, allowReSplitAces,
            false,  // allowSoftHandsInMonteCarlo
            false,  // requirePairForMonteCarlo
            "Surrender_vs_Hit");
    }

    std::cout << "\n=== ALL SIMULATIONS COMPLETE ===" << std::endl;
    
    return 0;
}


