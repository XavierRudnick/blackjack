#include <algorithm>
#include <cstdlib>
#include <memory>
#include <iostream>
#include <set>
#include <fstream>
#include <iomanip>
#include <mutex>
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
#include "OmegaIIStrategyAceCount.h"
#include "R14Strategy.h"
#include "WongHalvesStrategy.h"
#include "RPCStrategy.h"
#include "MonteCarloScenario.h"
#include <thread>
#include <filesystem>

namespace fs = std::filesystem;

// void playManualGame(int numDecksUsed){
//     ConsoleObserver consoleObserver;
//     EventBus& bus = EventBus::getInstance();
//     bus.detachAll();
//     bus.registerObserver(&consoleObserver, {EventType::CardsDealt, EventType::ActionTaken, EventType::RoundEnded, EventType::GameStats});


//     Deck deck(numDecksUsed);

//     auto hilo = std::make_unique<HiLoStrategy>(numDecksUsed);
//     //HumanPlayer human(false, std::move(hilo)); 

//     Engine hiLoEngine = EngineBuilder()
//                         .withEventBus(&bus)
//                         .setDeckSize(numDecksUsed)
//                         .setDeck(deck)
//                         .setPenetrationThreshold(.75)
//                         .setInitialWallet(1000)
//                         .enableEvents(true)
//                         .with3To2Payout(true)
//                         .withH17Rules(true)
//                         .allowDoubleAfterSplit(true)
//                         .build(&human);

//     hiLoEngine.runner();
// }

void runRTPsims(int numDecksUsed, int iterations, float deckPenetration,std::unique_ptr<CountingStrategy> strategy){

    std::pair<double, double> gameStats = {0, 0};
    ConsoleObserver consoleObserver;
    EventBus& bus = EventBus::getInstance();
    //bus.detachAll();
    //bus.registerObserver(&consoleObserver, {EventType::CardsDealt, EventType::ActionTaken, EventType::RoundEnded, EventType::GameStats});
    int losses = 0;
    Deck deck(numDecksUsed);
    BotPlayer robot(false, std::move(strategy)); 

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; i++){
        std::pair<double, double> profit = {50000, 0};
        for (int j = 0; j < 15; j++){

            deck.reset();
            robot.resetCount(numDecksUsed);

            Engine hiLoEngine = EngineBuilder()
                                        .withEventBus(&bus)
                                        .setDeckSize(numDecksUsed)
                                        .setDeck(deck)
                                        .setPenetrationThreshold(deckPenetration)
                                        .setInitialWallet(profit.first)
                                        .setKellyRisk(0.5f)
                                        .enableEvents(false)
                                        .with3To2Payout(true)
                                        .withH17Rules(true)
                                        .allowDoubleAfterSplit(true)
                                        .allowReSplitAces(false)
                                        .build(&robot);
            auto end_profit = hiLoEngine.runner();
            profit.second += end_profit.second;
            profit.first = end_profit.first;

            if (profit.first < 25.0){
                losses++;
                break;        
            }
        }
    
        if (i % 10000000 == 0 && i != 0){
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
    double diff = average-50000;
    double normal =  50000.0 / avgMoneyBet;
    double money_lost_per = diff * normal;
    double rtp = (50000+money_lost_per) /50000;
    std::cout << "lose br rate " << (float)losses/iterations << std::endl;
    std::cout << "Average after " << iterations << " rounds: " << average << std::endl;
    std::cout << "Average money bet: " << avgMoneyBet << std::endl;
    std::cout << "Difference: " << diff << std::endl;
    std::cout << "Money gained/lost per 1000$ " << money_lost_per << "$" << std::endl;
    std::cout << "RTP " << rtp << std::endl;
}

// // Legacy single-action simulation (kept for backward compatibility)
// void runMontesims(int numDecksUsed, int iterations, float deckPenetration, 
//     const std::vector<Action>& monteCarloActions, 
//     std::unique_ptr<CountingStrategy> strategy, 
//     const std::set<std::pair<int,int>>& dealerVplayerValues, 
//     bool blackJackPayout3to2, bool dealerHits17, bool allowDoubleAfterSplit, bool allowReSplitAces,
//     bool allowSoftHandsInMonteCarlo, bool requirePairForMonteCarlo,
//     const std::string& actionSetName) {

//     EventBus& bus = EventBus::getInstance();
//     Deck deck(numDecksUsed);
//     std::map<std::pair<int, int>, std::map<float, DecisionPoint>> EVresults;
//     std::string strategyName = strategy->getName();
//     std::cout << "Running " << actionSetName << " simulations for strategy " << strategyName << std::endl;
    
//     BotPlayer robot(false, std::move(strategy)); 
//     FixedEngine fixedEngineTotal;

//     auto start_time = std::chrono::high_resolution_clock::now();

//     for (int i = 0; i < iterations; i++){
//         deck.reset();
//         robot.resetCount(numDecksUsed);

//         Engine engine = EngineBuilder()
//                             .withEventBus(&bus)
//                             .setDeckSize(numDecksUsed)
//                             .setDeck(deck)
//                             .setPenetrationThreshold(deckPenetration)
//                             .setInitialWallet(1000)
//                             .enableEvents(false)
//                             .with3To2Payout(blackJackPayout3to2)
//                             .withH17Rules(dealerHits17)
//                             .allowDoubleAfterSplit(allowDoubleAfterSplit)
//                             .allowReSplitAces(allowReSplitAces)
//                             .enableMontiCarlo(true)
//                             .setActionValues(dealerVplayerValues)
//                             .allowSoftHandsInMonteCarlo(allowSoftHandsInMonteCarlo)
//                             .requirePairForMonteCarlo(requirePairForMonteCarlo)
//                             .setActions(monteCarloActions)
//                             .setEVActions(EVresults)
//                             .build(&robot);

//         FixedEngine fixedEngine = engine.runnerMonte();
//         fixedEngineTotal.merge(fixedEngine);

//         if (i % 500000 == 0 && i != 0){
//             std::cout  << "  Completed " << i << " / " << iterations << " iterations." << std::endl;
//         }
//     }
//     std::string H17Str = dealerHits17 ? "H17" : "S17";
//     std::ostringstream filename;
//     filename << "stats/" << strategyName << "_" << actionSetName << "_" << H17Str << ".csv";
//     fixedEngineTotal.savetoCSVResults(filename.str());
    
//     auto end_time = std::chrono::high_resolution_clock::now();
//     auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
//     std::cout << "  Saved results to " << filename.str() << " (" << duration.count() << "s)" << std::endl;
// }

// Unified multi-scenario simulation - tracks ALL action comparisons in a single simulation pass
void runUnifiedMonteSims(int numDecksUsed, long long iterations, float deckPenetration,
    std::unique_ptr<CountingStrategy> strategy,
    const std::vector<MonteCarloScenario>& scenarios,
    bool blackJackPayout3to2, bool dealerHits17, bool allowDoubleAfterSplit, bool allowReSplitAces,
    const std::string& statsOutputDirectory = "stats",
    bool useFixedUnitBetForHands = false) {

    EventBus& bus = EventBus::getInstance();
    Deck deck(numDecksUsed);
    std::map<std::pair<int, int>, std::map<float, DecisionPoint>> EVresults;
    std::string strategyName = strategy->getName();
    std::string H17Str = dealerHits17 ? "H17" : "S17";
    
    fs::create_directories(statsOutputDirectory);

    std::cout << "Running unified simulation for strategy " << strategyName << " (" << H17Str << ")" << std::endl;
    std::cout << "  Tracking " << scenarios.size() << " scenario(s) simultaneously" << std::endl;
    if (useFixedUnitBetForHands) {
        std::cout << "  Using fixed 1-unit bets for Monte Carlo hands (EV normalization)" << std::endl;
    }
    
    BotPlayer robot(false, std::move(strategy)); 
    FixedEngine fixedEngineTotal;

    auto start_time = std::chrono::high_resolution_clock::now();

    for (long long i = 0; i < iterations; ++i) {
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
                            .setMonteCarloScenarios(scenarios)
                            .setEVActions(EVresults)
                            .useFixedUnitBet(useFixedUnitBetForHands)
                            .build(&robot);

        FixedEngine fixedEngine = engine.runnerMonte();
        fixedEngineTotal.merge(fixedEngine);

        if (i % 50000000 == 0 && i != 0) {
            std::cout << "  Completed " << i << " / " << iterations << " iterations. Time: "
                      << std::chrono::duration_cast<std::chrono::seconds>(
                             std::chrono::high_resolution_clock::now() - start_time)
                             .count()
                      << "s. Strategy: " << strategyName << std::endl;
        }
    }
    
    // Save results for each scenario to separate CSV files
    for (const auto& scenario : scenarios) {
        std::ostringstream filename;
        filename << statsOutputDirectory << "/" << strategyName << "_" << scenario.name << "_" << numDecksUsed << "_" << H17Str << ".csv";
        fixedEngineTotal.saveScenarioResults(scenario.name, filename.str());
        std::cout << "  Saved " << scenario.name << " to " << filename.str() << std::endl;
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
    std::cout << "  Unified simulation completed in " << duration.count() << "s" << std::endl;
}

// Helper function to create all Monte Carlo scenarios
std::vector<MonteCarloScenario> createAllScenarios() {
    std::vector<MonteCarloScenario> scenarios;
    
    //1. Insurance Accept vs Decline - allows soft hands (dealer shows Ace)
    MonteCarloScenario insuranceScenario;
    insuranceScenario.name = "InsuranceAccept_vs_Decline";
    insuranceScenario.actions = {Action::InsuranceAccept, Action::InsuranceDecline};
    for (int ps = 21; ps >= 2; --ps) {
        insuranceScenario.addCardValue(ps, 11);
    }
    insuranceScenario.allowSoftHands = true;  // Insurance allows soft hands
    insuranceScenario.requirePair = false;
    insuranceScenario.isInsuranceScenario = true;
    scenarios.push_back(insuranceScenario);
    
    // 2. Hit vs Stand - hard hands only
    MonteCarloScenario hitVsStandScenario;
    hitVsStandScenario.name = "Hit_vs_Stand";
    hitVsStandScenario.actions = {Action::Hit, Action::Stand};
    hitVsStandScenario.addCardValue(16, 10);
    hitVsStandScenario.addCardValue(15, 10);
    hitVsStandScenario.addCardValue(12, 3);
    hitVsStandScenario.addCardValue(12, 2);
    hitVsStandScenario.addCardValue(13, 2);
    hitVsStandScenario.addCardValue(13, 3);
    hitVsStandScenario.allowSoftHands = false;  // Hard hands only
    hitVsStandScenario.requirePair = false;
    hitVsStandScenario.isInsuranceScenario = false;
    scenarios.push_back(hitVsStandScenario);
    
    // 3. Split vs Stand (Pair of 10s) - requires pair
    MonteCarloScenario splitVsStandScenario;
    splitVsStandScenario.name = "Split_vs_Stand_Pair10s";
    splitVsStandScenario.actions = {Action::Split, Action::Stand};
    splitVsStandScenario.addCardValue(20, 5);
    splitVsStandScenario.addCardValue(20, 6);
    splitVsStandScenario.allowSoftHands = false;
    splitVsStandScenario.requirePair = true;  // Only trigger when hand is a splittable pair
    splitVsStandScenario.isInsuranceScenario = false;
    scenarios.push_back(splitVsStandScenario);
    
    //4. Hit vs Double - hard hands only
    MonteCarloScenario hitVsDoubleScenario;
    hitVsDoubleScenario.name = "Hit_vs_Double";
    hitVsDoubleScenario.actions = {Action::Hit, Action::Double};
    hitVsDoubleScenario.addCardValue(10, 10);
    hitVsDoubleScenario.addCardValue(10, 11);
    hitVsDoubleScenario.addCardValue(11, 11);
    hitVsDoubleScenario.addCardValue(9, 2);
    hitVsDoubleScenario.addCardValue(9, 7);
    hitVsDoubleScenario.allowSoftHands = false;
    hitVsDoubleScenario.requirePair = false;
    hitVsDoubleScenario.isInsuranceScenario = false;
    scenarios.push_back(hitVsDoubleScenario);
    
    // 5. Surrender vs Hit - hard hands only
    MonteCarloScenario surrenderVsHitScenario;
    surrenderVsHitScenario.name = "Surrender_vs_Hit";
    surrenderVsHitScenario.actions = {Action::Surrender, Action::Hit};
    surrenderVsHitScenario.addCardValue(15, 9);
    surrenderVsHitScenario.addCardValue(15, 10);
    surrenderVsHitScenario.addCardValue(14, 10);
    surrenderVsHitScenario.addCardValue(15, 11);
    surrenderVsHitScenario.addCardValue(16, 9);
    surrenderVsHitScenario.addCardValue(16, 10);
    surrenderVsHitScenario.addCardValue(16, 11);
    surrenderVsHitScenario.allowSoftHands = false;
    surrenderVsHitScenario.requirePair = false;
    surrenderVsHitScenario.isInsuranceScenario = false;
    scenarios.push_back(surrenderVsHitScenario);
    
    return scenarios;
}

// Helper lambda to create all strategies
auto createStrategies(int numDecksUsed) {
    std::vector<std::unique_ptr<CountingStrategy>> strategies;
    strategies.push_back(std::make_unique<HiLoStrategy>(numDecksUsed));
    //strategies.push_back(std::make_unique<NoStrategy>(numDecksUsed));
    strategies.push_back(std::make_unique<MentorStrategy>(numDecksUsed));
    strategies.push_back(std::make_unique<RPCStrategy>(numDecksUsed));
    strategies.push_back(std::make_unique<RAPCStrategy>(numDecksUsed));
    strategies.push_back(std::make_unique<ZenCountStrategy>(numDecksUsed));
    strategies.push_back(std::make_unique<R14Strategy>(numDecksUsed));
    strategies.push_back(std::make_unique<OmegaIIStrategy>(numDecksUsed));
    strategies.push_back(std::make_unique<OmegaIIStrategyAceCount>(static_cast<float>(numDecksUsed)));
    strategies.push_back(std::make_unique<WongHalvesStrategy>(numDecksUsed));
    return strategies;
}

// NEW: RTP simulation that stores results to file
void runRTPsimsWithResults(int numDecksUsed, int iterations, float deckPenetration, 
    std::unique_ptr<CountingStrategy> strategy, bool dealerHits17,
    bool allowDoubleAfterSplit, bool allowReSplitAces, bool surrender, bool blackJackPayout3to2,
    float kellyFraction, double initialWallet, std::ofstream& resultsFile, std::mutex& fileMutex,
    const std::string& evPerTcOutputRoot = "stats/evPerTC") {

    std::pair<double, double> gameStats = {0, 0};
    EventBus& bus = EventBus::getInstance();
    Deck deck(numDecksUsed);
    BotPlayer robot(false, std::move(strategy)); 
    std::string strategyName = robot.getStrategy()->getName();
    std::map<float,ActionStats> EVperTC;

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; i++){
        deck.reset();
        robot.resetCount(numDecksUsed);

        std::pair<double, double> profit = {initialWallet, 0};

        Engine engine = EngineBuilder()
                            .withEventBus(&bus)
                            .setDeckSize(numDecksUsed)
                            .setDeck(deck)
                            .setPenetrationThreshold(deckPenetration)
                            .setInitialWallet(initialWallet)
                            .setKellyRisk(kellyFraction)
                            .enableEvents(false)
                            .with3To2Payout(blackJackPayout3to2)
                            .withH17Rules(dealerHits17)
                            .allowDoubleAfterSplit(allowDoubleAfterSplit)
                            .allowReSplitAces(allowReSplitAces)
                            .allowSurrender(surrender)
                            .setEVperTC(EVperTC)
                            .build(&robot);
        profit = engine.runner();

        if (i % 5000000 == 0 && i != 0){
            std::cout << strategyName << ": Completed " << i << " / " << iterations << " iterations." << std::endl;
        }

        gameStats.first += profit.first;
        gameStats.second += profit.second;
    } 

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

    double average = gameStats.first / iterations;
    double avgMoneyBet = gameStats.second / iterations;
    double diff = average - initialWallet;
    double normal = initialWallet / avgMoneyBet;
    double money_lost_per = diff * normal;
    double rtp = (initialWallet + money_lost_per) / initialWallet;
    double houseEdge = (1.0 - rtp) * 100; // percentage

    std::string H17Str = dealerHits17 ? "H17" : "S17";

    // Thread-safe file write
    {
        std::lock_guard<std::mutex> lock(fileMutex);
        resultsFile << strategyName << "," 
                << numDecksUsed << "," 
                << deckPenetration << "," 
                << H17Str << ","
                << (allowDoubleAfterSplit ? "DAS" : "NoDAS") << ","
                << (allowReSplitAces ? "RAS" : "NoRAS") << ","
                << (surrender ? "Surrender" : "NoSurrender") << ","
                << (blackJackPayout3to2 ? "3to2" : "6to5") << ","
                << iterations << ","
                << std::fixed << std::setprecision(6) << rtp << ","
                << std::fixed << std::setprecision(4) << houseEdge << ","
                << std::fixed << std::setprecision(2) << average << ","
                << std::fixed << std::setprecision(2) << avgMoneyBet << ","
                << std::fixed << std::setprecision(2) << money_lost_per << ","
                << duration.count() << std::endl;
    }

    std::cout << "=== " << strategyName << " (" << H17Str << ") ===" << std::endl;
    std::cout << "  RTP: " << std::fixed << std::setprecision(4) << (rtp * 100) << "%" << std::endl;
    std::cout << "  House Edge: " << std::fixed << std::setprecision(4) << houseEdge << "%" << std::endl;
    std::cout << "  Avg money bet per shoe: $" << std::fixed << std::setprecision(2) << avgMoneyBet << std::endl;
    std::cout << "  Net gain/loss per $1000 wagered: $" << std::fixed << std::setprecision(2) << money_lost_per << std::endl;
    std::cout << "  Duration: " << duration.count() << "s" << std::endl << std::endl;

    if (!evPerTcOutputRoot.empty()) {
        std::string evDir = evPerTcOutputRoot + "/" + strategyName;
        fs::create_directories(evDir);

        std::ostringstream evFilename;
        evFilename << evDir << "/ev_per_tc_" << strategyName << "_" << numDecksUsed << "deck_"
                   << static_cast<int>(deckPenetration * 100) << "pen_" << H17Str << "_"
                   << (allowDoubleAfterSplit ? "DAS" : "NoDAS") << "_"
                   << (allowReSplitAces ? "RAS" : "NoRAS") << "_"
                   << (surrender ? "Surrender" : "NoSurrender") << "_"
                   << (blackJackPayout3to2 ? "3to2" : "6to5") << ".csv";

        std::ofstream evFile(evFilename.str());
        evFile << "TrueCount,HandsPlayed,TotalMoneyWagered,TotalPayout,EVPerDollar,StdErrorPerDollar" << std::endl;
        for (const auto& entry : EVperTC) {
            const float trueCount = entry.first;
            const ActionStats& stats = entry.second;
            evFile << std::fixed << std::setprecision(1) << trueCount << ","
                   << stats.handsPlayed << ","
                   << std::fixed << std::setprecision(6) << stats.totalMoneyWagered << ","
                   << std::fixed << std::setprecision(6) << stats.totalPayout << ","
                   << std::fixed << std::setprecision(6) << stats.getEV() << ","
                   << std::fixed << std::setprecision(6) << stats.getStdError()
                   << std::endl;
        }
    }
}

// Full-shoe EV per true count using a fixed 1-unit main wager (strategy bet sizing ignored).
void runEvenBetEvPerTcSims(int numDecksUsed, int iterations, float deckPenetration,
    std::unique_ptr<CountingStrategy> strategy, bool dealerHits17,
    bool allowDoubleAfterSplit, bool allowReSplitAces, bool surrender, bool blackJackPayout3to2,
    const std::string& evPerTcOutputRoot) {

    EventBus& bus = EventBus::getInstance();
    Deck deck(numDecksUsed);
    BotPlayer robot(false, std::move(strategy));
    std::string strategyName = robot.getStrategy()->getName();
    std::map<float, ActionStats> EVperTC;
    std::string H17Str = dealerHits17 ? "H17" : "S17";

    std::cout << "Even-bet (1 unit) EV-per-TC simulation: " << strategyName << " (" << H17Str << ")" << std::endl;

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        deck.reset();
        robot.resetCount(numDecksUsed);

        Engine engine = EngineBuilder()
                            .withEventBus(&bus)
                            .setDeckSize(numDecksUsed)
                            .setDeck(deck)
                            .setPenetrationThreshold(deckPenetration)
                            .setInitialWallet(1'000'000'000.0)
                            .setKellyRisk(0.5f)
                            .enableEvents(false)
                            .with3To2Payout(blackJackPayout3to2)
                            .withH17Rules(dealerHits17)
                            .allowDoubleAfterSplit(allowDoubleAfterSplit)
                            .allowReSplitAces(allowReSplitAces)
                            .allowSurrender(surrender)
                            .setEVperTC(EVperTC)
                            .useFixedUnitBet(true)
                            .build(&robot);
        engine.runner();

        if (i % 50000 == 0 && i != 0) {
            std::cout << "  " << strategyName << " even-bet EV/TC: " << i << " / " << iterations << " shoes" << std::endl;
        }
    }

    auto duration = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now() - start_time).count();
    std::cout << "  Even-bet EV/TC finished in " << duration << "s" << std::endl;

    std::string evDir = evPerTcOutputRoot + "/" + strategyName;
    fs::create_directories(evDir);
    std::ostringstream evFilename;
    evFilename << evDir << "/ev_per_tc_even1unit_" << strategyName << "_" << numDecksUsed << "deck_"
               << static_cast<int>(deckPenetration * 100) << "pen_" << H17Str << "_"
               << (allowDoubleAfterSplit ? "DAS" : "NoDAS") << "_"
               << (allowReSplitAces ? "RAS" : "NoRAS") << "_"
               << (surrender ? "Surrender" : "NoSurrender") << "_"
               << (blackJackPayout3to2 ? "3to2" : "6to5") << ".csv";

    std::ofstream evFile(evFilename.str());
    evFile << "TrueCount,HandsPlayed,TotalMoneyWagered,TotalPayout,EVPerDollar,StdErrorPerDollar" << std::endl;
    for (const auto& entry : EVperTC) {
        const float trueCount = entry.first;
        const ActionStats& stats = entry.second;
        evFile << std::fixed << std::setprecision(1) << trueCount << ","
               << stats.handsPlayed << ","
               << std::fixed << std::setprecision(6) << stats.totalMoneyWagered << ","
               << std::fixed << std::setprecision(6) << stats.totalPayout << ","
               << std::fixed << std::setprecision(6) << stats.getEV() << ","
               << std::fixed << std::setprecision(6) << stats.getStdError()
               << std::endl;
    }
    std::cout << "  Wrote even-bet EV per TC → " << evFilename.str() << std::endl;
}

// Run RTP simulations for all strategies and save to CSV
void runAllRTPSimulations(int numDecksUsed, float deckPenetration, int iterations, bool dealerHits17, bool allowDoubleAfterSplit, bool allowReSplitAces, bool surrender, bool blackJackPayout3to2, float kellyFraction) {
    std::string H17Str = dealerHits17 ? "H17" : "S17";
    std::string dasD = allowDoubleAfterSplit ? "DAS" : "NoDAS";
    std::string rasD = allowReSplitAces ? "RAS" : "NoRAS";
    std::string surr = surrender ? "Surrender" : "NoSurrender";
    std::string blackJack3to2 = blackJackPayout3to2 ? "3to2" : "6to5";
    std::string kellyStr = "kelly" + std::to_string(static_cast<int>(kellyFraction * 100));
    std::string rtpDir = "stats/rtp_results";
    fs::create_directories(rtpDir);
    std::string filename = rtpDir + "/rtp_results_" + std::to_string(numDecksUsed) + "deck_" + 
                           std::to_string(static_cast<int>(deckPenetration * 100)) + "pen_" + H17Str + "_" + dasD + "_" + rasD + "_" + surr + "_" + blackJack3to2 + "_" + kellyStr + ".csv";
    
    std::ofstream resultsFile(filename);
    std::mutex fileMutex;
    
    // Write CSV header
    resultsFile << "Strategy,Decks,Penetration,DealerRule,DAS,RAS,Surrender,BlackjackPayout,Iterations,RTP,HouseEdge%,AvgWallet,AvgMoneyBet,NetPer1000,Duration_s" << std::endl;
    
    std::cout << "\n=== RTP SIMULATIONS (" << H17Str << " " << dasD << " " << rasD << " " << surr << " " << blackJack3to2 << ") ===" << std::endl;
    std::cout << "Decks: " << numDecksUsed << ", Penetration: " << deckPenetration 
              << ", Iterations: " << iterations << std::endl;
    std::cout << "Results will be saved to: " << filename << std::endl << std::endl;
    
    auto strategies = createStrategies(numDecksUsed);
    const size_t num_threads = strategies.size();
    std::cout << "Running with " << num_threads << " thread(s)" << std::endl << std::endl;
    
    std::vector<std::thread> workers;
    workers.reserve(num_threads);
    
    for (auto& strategy : strategies) {
        workers.emplace_back([&, strat = std::move(strategy), kellyFraction]() mutable {
            runRTPsimsWithResults(numDecksUsed, iterations, deckPenetration,
                std::move(strat), dealerHits17, allowDoubleAfterSplit, allowReSplitAces,
                surrender, blackJackPayout3to2, kellyFraction, 50000.0, resultsFile, fileMutex);
        });
        
        if (workers.size() >= num_threads) {
            for (auto& t : workers) {
                t.join();
            }
            workers.clear();
        }
    }
    
    for (auto& t : workers) {
        t.join();
    }
    
    resultsFile.close();
    std::cout << "\n=== RTP SIMULATIONS COMPLETE (" << H17Str << " " << dasD << " " << rasD << " " << surr << " " << blackJack3to2 << ") ===" << std::endl;
    std::cout << "Results saved to: " << filename << std::endl;
}

// NEW: Unified simulation setup - runs ALL scenarios in a single pass per strategy
void setUpUnifiedSims(int numDecksUsed, float deckPenetration, int iterations, bool dealerHits17) {
    bool blackJackPayout3to2 = true;
    bool allowDoubleAfterSplit = true;
    bool allowReSplitAces = true;
    
    std::string H17Str = dealerHits17 ? "H17" : "S17";
    std::vector<MonteCarloScenario> scenarios = createAllScenarios();
    
    std::cout << "\n=== UNIFIED BLACKJACK MONTE CARLO SIMULATIONS (" << H17Str << ") ===" << std::endl;
    std::cout << "Decks: " << numDecksUsed << ", Penetration: " << deckPenetration 
              << ", Iterations: " << iterations << std::endl;
    std::cout << "Tracking " << scenarios.size() << " scenarios per simulation:" << std::endl;
    for (const auto& scenario : scenarios) {
        std::cout << "  - " << scenario.name << " (" << scenario.activeCellCount() << " card value pairs)" << std::endl;
    }
    std::cout << std::endl;
    
    auto strategies = createStrategies(numDecksUsed);
    // const unsigned int hwThreads = std::max(1u, std::thread::hardware_concurrency());
    // const size_t maxParallel = std::min(strategies.size(), static_cast<size_t>(hwThreads));
     
    const size_t num_threads = 4;

    std::cout << "Using up to " << num_threads << " thread(s)" << std::endl;

    std::vector<std::thread> workers;
    workers.reserve(num_threads);

    for (auto& strategy : strategies) {
        workers.emplace_back([&, strat = std::move(strategy)]() mutable {
            runUnifiedMonteSims(numDecksUsed, iterations, deckPenetration,
                std::move(strat), scenarios,
                blackJackPayout3to2, dealerHits17, allowDoubleAfterSplit, allowReSplitAces);
        });

        if (workers.size() >= num_threads) {
            for (auto& t : workers) {
                t.join();
            }
            workers.clear();
        }
    }

    for (auto& t : workers) {
        t.join();
    }
    
    std::cout << "\n=== UNIFIED SIMULATIONS COMPLETE (" << H17Str << ") ===" << std::endl;
}

static std::string buildRulesetId(int numDecks, float penetration, bool dealerHits17,
    bool das, bool ras, bool surrender, bool bj3to2) {
    std::ostringstream ss;
    ss << numDecks << "deck_"
       << static_cast<int>(penetration * 100) << "pen_"
       << (dealerHits17 ? "H17" : "S17") << "_"
       << (das ? "DAS" : "NoDAS") << "_"
       << (ras ? "RAS" : "NoRAS") << "_"
       << (surrender ? "Surrender" : "NoSurrender") << "_"
       << (bj3to2 ? "3to2" : "6to5");
    return ss.str();
}

void runShoeTraceSim(
    int numDecks, int numShoes, float penetration,
    std::unique_ptr<CountingStrategy> strategy,
    bool bj3to2, bool dealerHits17, bool das, bool ras, bool surrender,
    const std::string& outputDir,
    bool compress = true)
{
    std::string strategyName = strategy->getName();
    std::string rulesetId = buildRulesetId(numDecks, penetration, dealerHits17,
                                            das, ras, surrender, bj3to2);

    fs::create_directories(outputDir);
    std::string traceFile = outputDir + "/hand_traces_" + strategyName
                          + "_" + rulesetId + (compress ? ".csv.zst" : ".csv");

    FILE* fp = nullptr;
    if (compress) {
        std::string cmd = "zstd -q -o " + traceFile;
        fp = popen(cmd.c_str(), "w");
    } else {
        fp = fopen(traceFile.c_str(), "w");
    }
    if (!fp) {
        std::cerr << "Failed to open output: " << traceFile << "\n";
        return;
    }

    // outcome_code: 0=loss, 1=push, 2=win, 3=blackjack_win,
    //               4=double_win, 5=double_loss, 6=split_net_win, 7=split_net_loss
    fprintf(fp, "shoe_id,hand_number_in_shoe,shoe_progress_before_hand,"
                "tc_bucket,total_money_committed,net_profit,outcome_code,"
                "split_count,double_count,insurance_taken\n");

    EventBus& bus = EventBus::getInstance();
    Deck deck(numDecks);
    BotPlayer robot(false, std::move(strategy));

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < numShoes; i++) {
        deck.reset();
        robot.resetCount(numDecks);

        Engine engine = EngineBuilder()
            .withEventBus(&bus)
            .setDeckSize(numDecks)
            .setDeck(deck)
            .setPenetrationThreshold(penetration)
            .setInitialWallet(1'000'000)
            .enableEvents(false)
            .with3To2Payout(bj3to2)
            .withH17Rules(dealerHits17)
            .allowDoubleAfterSplit(das)
            .allowReSplitAces(ras)
            .allowSurrender(surrender)
            .useFixedUnitBet(true)
            .build(&robot);

        auto traces = engine.runnerShoeTrace(static_cast<uint64_t>(i), rulesetId, strategyName);

        for (const auto& tr : traces) {
            fprintf(fp, "%llu,%d,%.2f,%.1f,%.2f,%.2f,%d,%d,%d,%d\n",
                (unsigned long long)tr.shoe_id,
                tr.hand_number_in_shoe,
                tr.shoe_progress_before_hand,
                tr.true_count_bucket,
                tr.total_money_committed,
                tr.net_profit,
                tr.outcome_code,
                tr.split_count,
                tr.double_count,
                (tr.insurance_taken ? 1 : 0));
        }

        if (i % 10000 == 0 && i != 0) {
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::high_resolution_clock::now() - start_time).count();
            std::cout << "  Shoe trace: " << i << "/" << numShoes
                      << " shoes, " << elapsed << "s elapsed\n";
        }
    }

    compress ? pclose(fp) : fclose(fp);
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::high_resolution_clock::now() - start_time).count();
    std::cout << "Saved " << numShoes << " shoes → " << traceFile
              << " (" << elapsed << "s)\n";
}

// Produces deviation Monte Carlo CSVs, even-bet EV-per-TC, and Kelly-sized RTP for one counting strategy.
// Output paths include strategy name, deck count, and penetration (percent) so runs stay identifiable.
void collectStrategyDeviationEvRtpDataset(
    int numDecks,
    float deckPenetration,
    std::function<std::unique_ptr<CountingStrategy>()> makeStrategy,
    long long monteIterations,
    int evPerTcIterations,
    int rtpIterations,
    float kellyFraction,
    bool dealerHits17,
    bool allowDoubleAfterSplit,
    bool allowReSplitAces,
    bool surrender,
    bool blackJackPayout3to2,
    double initialBankroll)
{
    if (const char* seedEnv = std::getenv("BLACKJACK_SEED")) {
        try {
            const auto parsed = std::stoul(seedEnv);
            Deck::setSeed(static_cast<std::uint32_t>(parsed));
            std::cout << "Using deterministic deck seed: " << parsed << std::endl;
        } catch (const std::exception&) {
            std::cerr << "Invalid BLACKJACK_SEED value '" << seedEnv
                      << "'. Falling back to non-deterministic RNG." << std::endl;
            Deck::clearSeed();
        }
    } else {
        Deck::clearSeed();
    }

    auto strategyForName = makeStrategy();
    const std::string strategyName = strategyForName->getName();

    const int penPct = static_cast<int>(deckPenetration * 100.0f + 0.5f);
    const std::string H17Str = dealerHits17 ? "H17" : "S17";
    const std::string dasStr = allowDoubleAfterSplit ? "DAS" : "NoDAS";
    const std::string rasStr = allowReSplitAces ? "RAS" : "NoRAS";
    const std::string surStr = surrender ? "Surrender" : "NoSurrender";
    const std::string bjStr = blackJackPayout3to2 ? "3to2" : "6to5";
    const long long bankrollTag = static_cast<long long>(initialBankroll + 0.5);

    std::cout << "\n=== " << strategyName << " data collection (" << numDecks << " deck, "
              << penPct << "% pen, " << H17Str << ") ===\n";
    std::cout << "Monte Carlo shoes: " << monteIterations << "\n";
    std::cout << "Even-bet EV/TC shoes: " << evPerTcIterations << "\n";
    std::cout << "RTP shoes (Kelly bet, $" << bankrollTag << " start): " << rtpIterations
              << ", kellyFraction=" << kellyFraction << "\n\n";

    const std::string deviationDir = "data/deviation_data/stats_" + strategyName + "_"
        + std::to_string(numDecks) + "deck_" + std::to_string(penPct) + "pen";
    fs::create_directories(deviationDir);

    // std::vector<MonteCarloScenario> scenarios = createAllScenarios();
    // runUnifiedMonteSims(numDecks, monteIterations, deckPenetration,
    //     std::move(strategyForName),
    //     scenarios, blackJackPayout3to2, dealerHits17, allowDoubleAfterSplit, allowReSplitAces,
    //     deviationDir, true);

    runEvenBetEvPerTcSims(numDecks, evPerTcIterations, deckPenetration,
        makeStrategy(),
        dealerHits17, allowDoubleAfterSplit, allowReSplitAces, surrender, blackJackPayout3to2,
        "data/ev_per_tc_data/evPerTC");

    const std::string rtpDir = "data/rtp_data";
    fs::create_directories(rtpDir);
    const int kellyPct = static_cast<int>(kellyFraction * 100.0f + 0.5f);
    const std::string rtpPath = rtpDir + "/rtp_results_" + strategyName + "_"
        + std::to_string(numDecks) + "deck_" + std::to_string(penPct) + "pen_"
        + H17Str + "_" + dasStr + "_" + rasStr + "_" + surStr + "_" + bjStr + "_kelly"
        + std::to_string(kellyPct) + "_bankroll" + std::to_string(bankrollTag) + ".csv";
    std::ofstream rtpFile(rtpPath);
    rtpFile << "Strategy,Decks,Penetration,DealerRule,DAS,RAS,Surrender,BlackjackPayout,Iterations,RTP,HouseEdge%,AvgWallet,AvgMoneyBet,NetPer1000,Duration_s" << std::endl;
    std::mutex rtpMutex;
    runRTPsimsWithResults(numDecks, rtpIterations, deckPenetration,
        makeStrategy(),
        dealerHits17, allowDoubleAfterSplit, allowReSplitAces, surrender, blackJackPayout3to2,
        kellyFraction, initialBankroll, rtpFile, rtpMutex,
        ""); // EV/TC from Kelly run omitted (even-bet file is the reference EV curve)

    rtpFile.close();
    std::cout << "RTP summary written to " << rtpPath << std::endl;
    std::cout << "=== " << strategyName << " data collection complete ===\n" << std::endl;
}

void runHella(){

    auto start_time = std::chrono::high_resolution_clock::now();

    if (const char* seedEnv = std::getenv("BLACKJACK_SEED")) {
        try {
            const auto parsed = std::stoul(seedEnv);
            Deck::setSeed(static_cast<std::uint32_t>(parsed));
            std::cout << "Using deterministic deck seed: " << parsed << std::endl;
        } catch (const std::exception&) {
            std::cerr << "Invalid BLACKJACK_SEED value '" << seedEnv
                      << "'. Falling back to non-deterministic RNG." << std::endl;
            Deck::clearSeed();
        }
    } else {
        Deck::clearSeed();
    }

    // Configuration for RTP simulations (using 2-deck with updated deviations)
    int numDecksUsed = 2;
    float deckPenetration = 0.65;
    //int rtpIterations = 150000000;  // 10 million iterations per strategy
    
    // Run RTP simulations for all strategies with the updated deviations
    std::cout << "\n========================================" << std::endl;
    std::cout << "BLACKJACK RTP SIMULATION WITH UPDATED DEVIATIONS" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Configuration: " << numDecksUsed << " deck(s), " 
              << (deckPenetration * 100) << "% penetration" << std::endl;
    std::cout << "========================================\n" << std::endl;
    bool dealerHits17[] = {true};
    bool DAS[] = {true};
    bool RAS[] = {false};
    bool SurrenderAllowed[] = {false};
    bool blackJackPayout3to2[] = {true};
    int deckSize[] = {2,4,6,8};
    int rtpIterations[] = {50000,25000,17000,12500};
    float penetrations[] = {0.3f,0.4f,0.5f,0.60f,0.7f,0.80f}; //0.7f, 0.75f,0.80f
    float kellyFractions[] = {0.125f,0.25f,0.5f,0.75f};

    for (bool dh17 : dealerHits17) {
        for (bool dasD : DAS){
            for (bool rasD : RAS){
                for (bool blackJack3to2 : blackJackPayout3to2){
                    for (bool surr : SurrenderAllowed){
                        int i = 0;  // Reset i for each dealer rule iteration
                        for (int ds : deckSize){
                            for (float kellyFraction : kellyFractions){
                                for (float pen : penetrations){
                                    numDecksUsed = ds;
                                    deckPenetration = pen;
                                    std::cout << "\n----------------------------------------" << std::endl;
                                    std::cout << "Settings: " << numDecksUsed << " deck(s), " 
                                            << (deckPenetration * 100) << "% penetration, "
                                            << (dasD ? "DAS allowed, " : "DAS not allowed, ")
                                            << (rasD ? "RAS allowed, " : "RAS not allowed, ")
                                            << (surr ? "Surrender allowed" : "Surrender not allowed") 
                                            << std::endl;
                                    std::cout << "----------------------------------------" << std::endl;
                                    runAllRTPSimulations(numDecksUsed, deckPenetration, rtpIterations[i], dh17, dasD, rasD, surr, blackJack3to2, kellyFraction);
                                }
                                
                            }
                            i++;
                        }
                        
                    }
                }
            }   
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Simulation time for " << duration.count() / 1000000.0 << " seconds." << std::endl;

    
    std::cout << "\n========================================" << std::endl;
    std::cout << "ALL RTP SIMULATIONS COMPLETE" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Legacy Monte Carlo mode (commented out):
    // int monteCarloIterations = 600000000;
    // setUpUnifiedSims(numDecksUsed, deckPenetration, monteCarloIterations, true);   // H17
    // setUpUnifiedSims(numDecksUsed, deckPenetration, monteCarloIterations, false);  // S17
    return;
}

int main(){
    runRTPsims(6, 1000000, 0.75f, std::make_unique<HiLoStrategy>(6));
    return 0;
    //runHella();

    // // Shoe trace library generation — fixed 1-unit bet, portable base traces
    // // Adjust numShoes for the size of the dataset you want.
    // // 6-deck, 75% pen, H17, DAS, no RAS, no surrender, 3:2
    // runShoeTraceSim(6, 1000000, 0.75f,
    //     std::make_unique<OmegaIIStrategyAceCount>(6),
    //     /*bj3to2=*/true, /*dealerHits17=*/true,
    //     /*das=*/true, /*ras=*/false, /*surrender=*/false,
    //     "stats/shoetraces",
    //     /*compress=*/true);

    // return 0;

    // Increase counts for tighter CSVs (millions of shoes is typical for deviation work).
    int numDecks = 6;
    float deckPenetration = 0.75f;
    long long monteIterations = 60'000'000LL;
    int evPerTcIterations = 10'000'000;
    int rtpIterations = 3'000'000;
    float kellyFraction = 0.75f;
    bool dealerHits17 = true;
    bool allowDoubleAfterSplit = true;
    bool allowReSplitAces = false;
    bool surrender = false;
    bool blackJackPayout3to2 = true;
    double initialBankroll = 50'000.0;
    collectStrategyDeviationEvRtpDataset(numDecks, deckPenetration,
        [numDecks]() { return std::make_unique<OmegaIIStrategyAceCount>(static_cast<float>(numDecks)); },
        monteIterations, evPerTcIterations,
        rtpIterations, kellyFraction, dealerHits17, allowDoubleAfterSplit, allowReSplitAces,
        surrender, blackJackPayout3to2, initialBankroll);

    return 0;
}
