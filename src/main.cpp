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

void playManualGame(int numDecksUsed){
    ConsoleObserver consoleObserver;
    EventBus* bus = EventBus::getInstance();
    bus->detachAll();
    bus->registerObserver(&consoleObserver, {EventType::CardsDealt, EventType::ActionTaken, EventType::RoundEnded, EventType::GameStats});


    Deck deck(numDecksUsed);

    auto hilo = std::make_unique<HiLoStrategy>(numDecksUsed);
    HumanPlayer human(false, std::move(hilo)); 

    Engine hiLoEngine = EngineBuilder()
                        .withEventBus(bus)
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
    EventBus* bus = EventBus::getInstance();
    bus->detachAll();
    //bus->registerObserver(&consoleObserver, {EventType::CardsDealt, EventType::ActionTaken, EventType::RoundEnded, EventType::GameStats});

    Deck deck(numDecksUsed);
    BotPlayer robot(false, std::move(strategy)); 

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; i++){
        deck.reset();
        robot.resetCount(numDecksUsed);

        std::pair<double, double> profit = {1000, 0};

        Engine hiLoEngine = EngineBuilder()
                                    .withEventBus(bus)
                                    .setDeckSize(numDecksUsed)
                                    .setDeck(deck)
                                    .setPenetrationThreshold(deckPenetration)
                                    .setInitialWallet(1000)
                                    .enableEvents(false)
                                    .with3To2Payout(true)
                                    .withH17Rules(true)
                                    .allowDoubleAfterSplit(true)
                                    .allowReSplitAces(false)
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

void runMontesims(int numDecksUsed, int iterations,float deckPenetration, std::vector<std::vector<Action>> monteCarloActions, std::vector<std::unique_ptr<CountingStrategy>>& strategies, std::vector<std::map<int,int>> dealerVplayerValuesVec, bool blackJackPayout3to2, bool dealerHits17, bool allowDoubleAfterSplit, bool allowReSplitAces){

    std::pair<double, double> gameStats = {0, 0};
    ConsoleObserver consoleObserver;
    auto* bus = EventBus::getInstance();
    //bus->detachAll();
    //bus->registerObserver(&consoleObserver, {EventType::CardsDealt, EventType::ActionTaken, EventType::RoundEnded, EventType::GameStats});

    Deck deck(numDecksUsed);

    auto start_time = std::chrono::high_resolution_clock::now();

    for (size_t k = 0; k < strategies.size(); k++){
        std::cout << "Running simulations for strategy " << k+1 << " / " << strategies.size() << std::endl;
        BotPlayer robot(false, std::move(strategies[k])); 

        for (size_t j = 0; j < dealerVplayerValuesVec.size(); j++){
            for (auto pair : dealerVplayerValuesVec[j]){
                FixedEngine fixedEngineTotal;

                for (int i = 0; i < iterations; i++){
                    deck.reset();
                    robot.resetCount(numDecksUsed);

                    Engine hiLoEngine = EngineBuilder()
                                                .withEventBus(bus)
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
                                                .setUserHandValue(pair.first)
                                                .setDealerUpcardValue(pair.second)
                                                .setActions(monteCarloActions[j])
                                                .build(&robot);

                    FixedEngine fixedEngine = hiLoEngine.runnerMonte();
                    fixedEngineTotal.merge(fixedEngine);

                    if (i % 100000 == 0 && i != 0){
                        std::cout  << "Completed " << i << " / " << iterations << " iterations." <<std::endl;
                    }
                }
                std::ostringstream filename;
                    filename << "stats/" << robot.getStrategyName()
                            << "_user_" << pair.first
                            << "_dealer_" << pair.second
                            << ".csv";
                fixedEngineTotal.savetoCSVResults(filename.str());
            }
            }
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

int main(){
    int numDecksUsed = 6;
    float deckPenetration = 0.5;
    bool blackJackPayout3to2 = true; 
    bool dealerHits17 = true;
    bool allowDoubleAfterSplit = true;
    bool allowReSplitAces = true;

    std::vector<std::unique_ptr<CountingStrategy>> strategies;
    //std::vector<Action> monteCarloActions = {Action::Hit, Action::Stand, Action::Double, Action::Split, Action::Surrender};
    std::vector<Action> HitvStand = {Action::Hit, Action::Stand};
    std::vector<Action> HitvDouble = {Action::Hit, Action::Double};
    std::vector<Action> StandvDouble = {Action::Stand, Action::Double};
    std::vector<Action> SplitvAll = {Action::Split, Action::Hit, Action::Stand, Action::Double};
    std::vector<Action> SurrendervAll = {Action::Surrender, Action::Hit, Action::Stand, Action::Double, Action::Split};

    std::map<int,int> HitvStandValues = {
            {16,10},
            {15,10},
            {12,3},
            {12,2},
    };

    std::map<int,int> HitvDoubleValues = {
            {10,10},
            {10,11},
            {9,2},
            {9,7},
    };

    std::map<int,int> StandvDoubleValues = {
            {19,6},//add abilityt to desigish stand or double
            {18,6},
    };

    std::vector<std::vector<Action>> actionSets = {
        HitvStand,
        HitvDouble,
        //StandvDouble,
        SplitvAll,
        //SurrendervAll
    };

    std::vector<std::map<int,int>> dealerVplayerValuesVec = {
        HitvStandValues,
        HitvDoubleValues,
        StandvDoubleValues
    };

    strategies.push_back(std::make_unique<HiLoStrategy>(numDecksUsed));
    strategies.push_back(std::make_unique<NoStrategy>(numDecksUsed));
    strategies.push_back(std::make_unique<MentorStrategy>(numDecksUsed));
    strategies.push_back(std::make_unique<RPCStrategy>(numDecksUsed));  
    strategies.push_back(std::make_unique<ZenCountStrategy>(numDecksUsed));
    strategies.push_back(std::make_unique<R14Strategy>(numDecksUsed));
    strategies.push_back(std::make_unique<OmegaIIStrategy>(numDecksUsed));
    strategies.push_back(std::make_unique<WongHalvesStrategy>(numDecksUsed));



    runMontesims(numDecksUsed, 1000000,deckPenetration, actionSets, strategies, dealerVplayerValuesVec,
        blackJackPayout3to2,
        dealerHits17,
        allowDoubleAfterSplit,
        allowReSplitAces
    );

    //unRTPsims(numDecksUsed, 1000000,deckPenetration, std::move(no));
    
    return 0;
}


