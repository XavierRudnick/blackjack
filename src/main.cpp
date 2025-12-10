#include <cstdlib>
#include <iostream>

#include "Engine.h"
#include "HiLoStrategy.h"
#include "NoStrategy.h"
#include "observers/ConsoleObserver.h"
#include "observers/EventBus.h"
#include "EngineBuilder.h"
#include "HumanPlayer.h"
#include "BotPlayer.h"
#include <chrono> 
#include "LoggingCountingStrategy.h"

int main(){
    int numDecksUsed = 2;
    const bool visualize = false;
    const int iterations = visualize ? 1 : 1000000;
    // float scores[iterations];

    ConsoleObserver consoleObserver;
    auto* bus = EventBus::getInstance();
    bus->detachAll();

    if (visualize) {
        bus->registerObserver(&consoleObserver, {EventType::CardsDealt, EventType::ActionTaken, EventType::RoundEnded, EventType::GameStats});
    }

    std::pair<double, double> gameStats = {0, 0};
    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; i++){
        std::pair<double, double> profit = {1000, 0};

        auto hilo = std::make_unique<HiLoStrategy>(numDecksUsed);
        auto hiloLog = std::make_unique<LoggingCountingStrategy>(std::make_unique<HiLoStrategy>(numDecksUsed), visualize ? bus : nullptr);
        auto no = std::make_unique<NoStrategy>(numDecksUsed);

        auto player = std::make_unique<HumanPlayer>(false,std::move(hilo)); // false for allowSurrender, matching commented out .allowSurrender()
        auto robot = std::make_unique<BotPlayer>(false, std::move(hilo)); // false for allowSurrender, matching commented out .allowSurrender()
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
                                    //.allowSurrender()
                                    .build(std::move(robot));
        profit = hiLoEngine.runner();

    //   scores[i] = profit.first;
        gameStats.first += profit.first;
        gameStats.second += profit.second;
    }

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


