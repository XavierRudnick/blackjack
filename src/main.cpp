#include <cstdlib>
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
#include "R14Strategy.h"
#include "WongHalvesStrategy.h"
#include "RPCStrategy.h"
#include "MonteCarloScenario.h"
#include <thread>
#include <filesystem>

namespace fs = std::filesystem;

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

// Legacy single-action simulation (kept for backward compatibility)
void runMontesims(int numDecksUsed, int iterations, float deckPenetration, 
    const std::vector<Action>& monteCarloActions, 
    std::unique_ptr<CountingStrategy> strategy, 
    const std::set<std::pair<int,int>>& dealerVplayerValues, 
    bool blackJackPayout3to2, bool dealerHits17, bool allowDoubleAfterSplit, bool allowReSplitAces,
    bool allowSoftHandsInMonteCarlo, bool requirePairForMonteCarlo,
    const std::string& actionSetName) {

    EventBus& bus = EventBus::getInstance();
    Deck deck(numDecksUsed);
    std::map<std::pair<int, int>, std::map<float, DecisionPoint>> EVresults;
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
                            .setEVActions(EVresults)
                            .build(&robot);

        FixedEngine fixedEngine = engine.runnerMonte();
        fixedEngineTotal.merge(fixedEngine);

        if (i % 500000 == 0 && i != 0){
            std::cout  << "  Completed " << i << " / " << iterations << " iterations." << std::endl;
        }
    }
    std::string H17Str = dealerHits17 ? "H17" : "S17";
    std::ostringstream filename;
    filename << "stats/" << strategyName << "_" << actionSetName << "_" << H17Str << ".csv";
    fixedEngineTotal.savetoCSVResults(filename.str());
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);
    std::cout << "  Saved results to " << filename.str() << " (" << duration.count() << "s)" << std::endl;
}

// NEW: Unified multi-scenario simulation - tracks ALL action comparisons in a single simulation pass
void runUnifiedMonteSims(int numDecksUsed, int iterations, float deckPenetration,
    std::unique_ptr<CountingStrategy> strategy,
    const std::vector<MonteCarloScenario>& scenarios,
    bool blackJackPayout3to2, bool dealerHits17, bool allowDoubleAfterSplit, bool allowReSplitAces) {

    EventBus& bus = EventBus::getInstance();
    Deck deck(numDecksUsed);
    std::map<std::pair<int, int>, std::map<float, DecisionPoint>> EVresults;
    std::string strategyName = strategy->getName();
    std::string H17Str = dealerHits17 ? "H17" : "S17";
    
    std::cout << "Running unified simulation for strategy " << strategyName << " (" << H17Str << ")" << std::endl;
    std::cout << "  Tracking " << scenarios.size() << " scenario(s) simultaneously" << std::endl;
    
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
                            .setMonteCarloScenarios(scenarios)
                            .setEVActions(EVresults)
                            .build(&robot);

        FixedEngine fixedEngine = engine.runnerMonte();
        fixedEngineTotal.merge(fixedEngine);

        if (i % 50000000 == 0 && i != 0){
            std::cout  << "  Completed " << i << " / " << iterations << " iterations. Time: " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - start_time).count() << "s. Strategy: " << strategyName << std::endl;
        }
    }
    
    // Save results for each scenario to separate CSV files
    for (const auto& scenario : scenarios) {
        std::ostringstream filename;
        filename << "stats/" << strategyName << "_" << scenario.name << "_" << H17Str << ".csv";
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
    insuranceScenario.cardValues = {
        {21,11}, {20,11}, {19,11}, {18,11}, {17,11}, {16,11}, {15,11},
        {14,11}, {13,11}, {12,11}, {11,11}, {10,11}, {9,11}, {8,11},
        {7,11}, {6,11}, {5,11}, {4,11}, {3,11}, {2,11}
    };
    insuranceScenario.allowSoftHands = true;  // Insurance allows soft hands
    insuranceScenario.requirePair = false;
    insuranceScenario.isInsuranceScenario = true;
    scenarios.push_back(insuranceScenario);
    
    // // 2. Hit vs Stand - hard hands only
    // MonteCarloScenario hitVsStandScenario;
    // hitVsStandScenario.name = "Hit_vs_Stand";
    // hitVsStandScenario.actions = {Action::Hit, Action::Stand};
    // hitVsStandScenario.cardValues = {
    //     {16, 10}, {15, 10}, {12, 3}, {12, 2}, {13, 2}, {13, 3}
    // };
    // hitVsStandScenario.allowSoftHands = false;  // Hard hands only
    // hitVsStandScenario.requirePair = false;
    // hitVsStandScenario.isInsuranceScenario = false;
    // scenarios.push_back(hitVsStandScenario);
    
    // 3. Split vs Stand (Pair of 10s) - requires pair
    MonteCarloScenario splitVsStandScenario;
    splitVsStandScenario.name = "Split_vs_Stand_Pair10s";
    splitVsStandScenario.actions = {Action::Split, Action::Stand};
    splitVsStandScenario.cardValues = {
        {20, 5},  // Pair of 10s vs 5
        {20, 6},  // Pair of 10s vs 6
    };
    splitVsStandScenario.allowSoftHands = false;
    splitVsStandScenario.requirePair = true;  // Only trigger when hand is a splittable pair
    splitVsStandScenario.isInsuranceScenario = false;
    scenarios.push_back(splitVsStandScenario);
    
    // 4. Hit vs Double - hard hands only
    // MonteCarloScenario hitVsDoubleScenario;
    // hitVsDoubleScenario.name = "Hit_vs_Double";
    // hitVsDoubleScenario.actions = {Action::Hit, Action::Double};
    // hitVsDoubleScenario.cardValues = {
    //     {10, 10}, {10, 11}, {11, 11}, {9, 2}, {9, 7}
    // };
    // hitVsDoubleScenario.allowSoftHands = false;
    // hitVsDoubleScenario.requirePair = false;
    // hitVsDoubleScenario.isInsuranceScenario = false;
    // scenarios.push_back(hitVsDoubleScenario);
    
    // // 5. Surrender vs Hit - hard hands only
    // MonteCarloScenario surrenderVsHitScenario;
    // surrenderVsHitScenario.name = "Surrender_vs_Hit";
    // surrenderVsHitScenario.actions = {Action::Surrender, Action::Hit};
    // surrenderVsHitScenario.cardValues = {
    //     {15, 9}, {15, 10}, {14, 10}, {15, 11}, {16, 9}, {16, 10}, {16, 11}
    // };
    // surrenderVsHitScenario.allowSoftHands = false;
    // surrenderVsHitScenario.requirePair = false;
    // surrenderVsHitScenario.isInsuranceScenario = false;
    // scenarios.push_back(surrenderVsHitScenario);
    
    return scenarios;
}

// Helper lambda to create all strategies
auto createStrategies(int numDecksUsed) {
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
}

// NEW: RTP simulation that stores results to file
void runRTPsimsWithResults(int numDecksUsed, int iterations, float deckPenetration, 
    std::unique_ptr<CountingStrategy> strategy, bool dealerHits17,
    bool allowDoubleAfterSplit, bool allowReSplitAces, bool surrender, bool blackJackPayout3to2,
    std::ofstream& resultsFile, std::mutex& fileMutex) {

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

        std::pair<double, double> profit = {1000, 0};

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
                            .allowSurrender(surrender)
                            .setEVperTC(EVperTC)
                            .build(&robot);
        profit = engine.runner();

        if (i % 500000 == 0 && i != 0){
            std::cout << strategyName << ": Completed " << i << " / " << iterations << " iterations." << std::endl;
        }

        gameStats.first += profit.first;
        gameStats.second += profit.second;
    } 

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

    double average = gameStats.first / iterations;
    double avgMoneyBet = gameStats.second / iterations;
    double diff = average - 1000;
    double normal = 1000.0 / avgMoneyBet;
    double money_lost_per = diff * normal;
    double rtp = (1000 + money_lost_per) / 1000;
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

    std::string evDir = "stats/evPerTC/" + strategyName;
    fs::create_directories(evDir);

    std::ostringstream evFilename;
    evFilename << evDir << "/ev_per_tc_" << strategyName << "_" << numDecksUsed << "deck_"
               << static_cast<int>(deckPenetration * 100) << "pen_" << H17Str << "_"
               << (allowDoubleAfterSplit ? "DAS" : "NoDAS") << "_"
               << (allowReSplitAces ? "RAS" : "NoRAS") << "_"
               << (surrender ? "Surrender" : "NoSurrender") << "_"
               << (blackJackPayout3to2 ? "3to2" : "6to5") << ".csv";

    std::ofstream evFile(evFilename.str());
    evFile << "TrueCount,HandsPlayed,TotalPayout,EV,StdError" << std::endl;
    for (const auto& entry : EVperTC) {
        const float trueCount = entry.first;
        const ActionStats& stats = entry.second;
        evFile << std::fixed << std::setprecision(1) << trueCount << ","
               << stats.handsPlayed << ","
               << std::fixed << std::setprecision(6) << stats.totalPayout << ","
               << std::fixed << std::setprecision(6) << stats.getEV() << ","
               << std::fixed << std::setprecision(6) << stats.getStdError()
               << std::endl;
    }
}

// Run RTP simulations for all strategies and save to CSV
void runAllRTPSimulations(int numDecksUsed, float deckPenetration, int iterations, bool dealerHits17, bool allowDoubleAfterSplit, bool allowReSplitAces, bool surrender, bool blackJackPayout3to2) {
    std::string H17Str = dealerHits17 ? "H17" : "S17";
    std::string dasD = allowDoubleAfterSplit ? "DAS" : "NoDAS";
    std::string rasD = allowReSplitAces ? "RAS" : "NoRAS";
    std::string surr = surrender ? "Surrender" : "NoSurrender";
    std::string blackJack3to2 = blackJackPayout3to2 ? "3to2" : "6to5";
    std::string rtpDir = "stats/rtp_results";
    fs::create_directories(rtpDir);
    std::string filename = rtpDir + "/rtp_results_" + std::to_string(numDecksUsed) + "deck_" + 
                           std::to_string(static_cast<int>(deckPenetration * 100)) + "pen_" + H17Str + "_" + dasD + "_" + rasD + "_" + surr + "_" + blackJack3to2 + ".csv";
    
    std::ofstream resultsFile(filename);
    std::mutex fileMutex;
    
    // Write CSV header
    resultsFile << "Strategy,Decks,Penetration,DealerRule,DAS,RAS,Surrender,BlackjackPayout,Iterations,RTP,HouseEdge%,AvgWallet,AvgMoneyBet,NetPer1000,Duration_s" << std::endl;
    
    std::cout << "\n=== RTP SIMULATIONS (" << H17Str << " " << dasD << " " << rasD << " " << surr << " " << blackJack3to2 << ") ===" << std::endl;
    std::cout << "Decks: " << numDecksUsed << ", Penetration: " << deckPenetration 
              << ", Iterations: " << iterations << std::endl;
    std::cout << "Results will be saved to: " << filename << std::endl << std::endl;
    
    auto strategies = createStrategies(numDecksUsed);
    const size_t num_threads = std::min(static_cast<size_t>(9), strategies.size());
    
    std::cout << "Running with " << num_threads << " thread(s)" << std::endl << std::endl;
    
    std::vector<std::thread> workers;
    workers.reserve(num_threads);
    
    for (auto& strategy : strategies) {
        workers.emplace_back([&, strat = std::move(strategy)]() mutable {
            runRTPsimsWithResults(numDecksUsed, iterations, deckPenetration,
                std::move(strat), dealerHits17, allowDoubleAfterSplit, allowReSplitAces,
                surrender, blackJackPayout3to2, resultsFile, fileMutex);
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
        std::cout << "  - " << scenario.name << " (" << scenario.cardValues.size() << " card value pairs)" << std::endl;
    }
    std::cout << std::endl;
    
    auto strategies = createStrategies(numDecksUsed);
    // const unsigned int hwThreads = std::max(1u, std::thread::hardware_concurrency());
    // const size_t maxParallel = std::min(strategies.size(), static_cast<size_t>(hwThreads));
     
    const size_t num_threads = 9;

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

int main(){
    // Configuration for RTP simulations (using 2-deck with updated deviations)
    int numDecksUsed = 2;
    float deckPenetration = 0.65;
    int rtpIterations = 50000000;  // 10 million iterations per strategy
    
    // Run RTP simulations for all strategies with the updated deviations
    std::cout << "\n========================================" << std::endl;
    std::cout << "BLACKJACK RTP SIMULATION WITH UPDATED DEVIATIONS" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Configuration: " << numDecksUsed << " deck(s), " 
              << (deckPenetration * 100) << "% penetration" << std::endl;
    std::cout << "Iterations per strategy: " << rtpIterations << std::endl;
    std::cout << "========================================\n" << std::endl;
    bool dealerHits17[] = {false};
    bool DAS[] = {true};
    bool RAS[] = {false};
    bool SurrenderAllowed[] = {false};
    bool blackJackPayout3to2[] = {true};
    int deckSize[] = {2,6,8};
    float penetrations[] = {0.3f,0.35f,0.4f, 0.45f, 0.5f, 0.55f, 0.60f,0.65f,0.7f, 0.75f,0.80f}; //0.7f, 0.75f,0.80f
    for (bool dh17 : dealerHits17) {
        for (bool dasD : DAS){
            for (bool rasD : RAS){
                for (bool blackJack3to2 : blackJackPayout3to2){
                    for (bool surr : SurrenderAllowed){
                        for (int ds : deckSize){
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
                                runAllRTPSimulations(numDecksUsed, deckPenetration, rtpIterations, dh17, dasD, rasD, surr, blackJack3to2);
                            }
                        }
                    }
                }
            }   
        }
    }
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "ALL RTP SIMULATIONS COMPLETE" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Legacy Monte Carlo mode (commented out):
    // int monteCarloIterations = 600000000;
    // setUpUnifiedSims(numDecksUsed, deckPenetration, monteCarloIterations, true);   // H17
    // setUpUnifiedSims(numDecksUsed, deckPenetration, monteCarloIterations, false);  // S17
    
    return 0;
}