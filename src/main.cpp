#include <cstdlib>
#include <iostream>

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


int main(){
    int numDecksUsed = 6;
    const bool visualize = true;
    const int iterations = visualize ? 1 : 5000000;
    // float scores[iterations];

    ConsoleObserver consoleObserver;
    auto* bus = EventBus::getInstance();
    bus->detachAll();

    if (visualize) {
        bus->registerObserver(&consoleObserver, {EventType::CardsDealt, EventType::ActionTaken, EventType::RoundEnded, EventType::GameStats});
    }

    std::pair<double, double> gameStats = {0, 0};
    auto start_time = std::chrono::high_resolution_clock::now();
    int userHandValuesStand[] = {16,15,12,12};
    int dealerUpcardValuesStand[] = {10,10,3,2};

    int userHandValuesDouble[] = {10,10,9,9};
    int dealerUpcardValuesDouble[] = {10,11,2,7};

    std::vector<Action> monteCarloActions = {Action::Hit, Action::Double};

    using StrategyFactory = std::function<std::unique_ptr<CountingStrategy>()>;
    std::array<StrategyFactory, 6> strategyFactories = {
        [numDecksUsed]() { return std::make_unique<MentorStrategy>(numDecksUsed); },
        [numDecksUsed]() { return std::make_unique<OmegaIIStrategy>(numDecksUsed); },
        [numDecksUsed]() { return std::make_unique<R14Strategy>(numDecksUsed); },
        [numDecksUsed]() { return std::make_unique<RPCStrategy>(numDecksUsed); },
        [numDecksUsed]() { return std::make_unique<WongHalvesStrategy>(numDecksUsed); },
        [numDecksUsed]() { return std::make_unique<NoStrategy>(numDecksUsed); }
    };

    for (int i = 0; i < iterations; i++){
        std::pair<double, double> profit = {1000, 0};

        auto hilo = std::make_unique<HiLoStrategy>(numDecksUsed);
        //auto hiloLog = std::make_unique<LoggingCountingStrategy>(std::make_unique<HiLoStrategy>(numDecksUsed), visualize ? bus : nullptr);
        //auto no = std::make_unique<NoStrategy>(numDecksUsed);
        auto player = std::make_unique<HumanPlayer>(false,std::move(hilo)); // false for allowSurrender, matching commented out .allowSurrender()
        //auto robot = std::make_unique<BotPlayer>(false, std::move(hilo)); 
        Deck deck = Deck(numDecksUsed);

        Engine hiLoEngine = EngineBuilder()
                                    .withEventBus(bus)
                                    .setDeckSize(numDecksUsed)
                                    .setDeck(deck)
                                    .setPenetrationThreshold(.75)
                                    .setInitialWallet(1000)
                                    .enableEvents(visualize)
                                    .with3To2Payout()
                                    .withS17Rules()
                                    .allowDoubleAfterSplit()
                                    .build(std::move(player));
        profit = hiLoEngine.runner();
        //FixedEngine fixedEngine = hiLoEngine.runnerMonte();
        //fixedEngineTotal.merge(fixedEngine);

        if (i % 100000 == 0 && i != 0){
            std::cout  << "Completed " << i << " / " << iterations << " iterations." <<std::endl;
        }

        // gameStats.first += profit.first;
        // gameStats.second += profit.second;
    } 

    // for (size_t k = 0; k < 6; k++){
    //     std::cout << "Running simulations for strategy " << k+1 << " / " << strategyFactories.size() << std::endl;
    //     for (size_t j = 0; j < sizeof(userHandValuesStand)/sizeof(userHandValuesStand[0]); j++){
    //         FixedEngine fixedEngineTotal;
    //         for (int i = 0; i < iterations; i++){
    //             [[maybe_unused]] std::pair<double, double> profit = {1000, 0};

    //             //auto hilo = std::make_unique<HiLoStrategy>(numDecksUsed);
    //             //auto hiloLog = std::make_unique<LoggingCountingStrategy>(std::make_unique<HiLoStrategy>(numDecksUsed), visualize ? bus : nullptr);
    //             //auto no = std::make_unique<NoStrategy>(numDecksUsed);
    //             //auto player = std::make_unique<HumanPlayer>(false,std::move(hilo)); // false for allowSurrender, matching commented out .allowSurrender()
    //             auto robot = std::make_unique<BotPlayer>(false, strategyFactories[k]()); 
    //             Deck deck = Deck(numDecksUsed);

    //             Engine hiLoEngine = EngineBuilder()
    //                                         .withEventBus(bus)
    //                                         .setDeckSize(numDecksUsed)
    //                                         .setDeck(deck)
    //                                         .setPenetrationThreshold(.75)
    //                                         .setInitialWallet(1000)
    //                                         .enableEvents(visualize)
    //                                         .with3To2Payout()
    //                                         .withS17Rules()
    //                                         .allowDoubleAfterSplit()
    //                                         .enableMontiCarlo()
    //                                         .setUserHandValue(userHandValuesStand[j])
    //                                         .setDealerUpcardValue(dealerUpcardValuesStand[j])
    //                                         .setActions(monteCarloActions)
    //                                         .build(std::move(robot));
    //             //profit = hiLoEngine.runner();
    //             FixedEngine fixedEngine = hiLoEngine.runnerMonte();
    //             fixedEngineTotal.merge(fixedEngine);

    //             if (i % 100000 == 0 && i != 0){
    //                 std::cout  << "Completed " << i << " / " << iterations << " iterations." <<std::endl;
    //             }

    //             // gameStats.first += profit.first;
    //             // gameStats.second += profit.second;
    //         }
    //         std::ostringstream filename;
    //             filename << "stats/" << "strategy_" << k+4
    //                 << "_user_" << userHandValuesStand[j]
    //                 << "_dealer_" << dealerUpcardValuesStand[j]
    //                 << ".csv";
    //         fixedEngineTotal.savetoCSVResults(filename.str());
    //     }
    // }
    

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Simulation time for " << numDecksUsed << " decks and " << iterations << " iterations: " << duration.count() / 1000000.0 << " seconds." << std::endl;

    if (visualize) {
        EventBus::getInstance()->removeObserver(&consoleObserver);
    }

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

    return 0;
}


