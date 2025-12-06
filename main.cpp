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


int main(){
    int num_decks_used = 2;
    const bool visualize = false;
    const int iterations = visualize ? 1 : 1000000;
    // float scores[iterations];

    ConsoleObserver consoleObserver;
    auto* bus = EventBus::getInstance();
    bus->detachAll();
    if (visualize) {
        bus->registerObserver(&consoleObserver, {EventType::CardsDealt, EventType::ActionTaken, EventType::RoundEnded, EventType::GameStats});
    }

    std::pair<double, double> swag = {0, 0};
    auto start_time = std::chrono::high_resolution_clock::now();


    for (int i = 0; i < iterations; i++){
        std::pair<double, double> profit = {1000, 0};
        auto hilo = std::make_unique<HiLoStrategy>(num_decks_used);
        auto no = std::make_unique<NoStrategy>(num_decks_used);
        auto player = std::make_unique<HumanPlayer>(false); // false for allowSurrender, matching commented out .allowSurrender()
        auto robot = std::make_unique<BotPlayer>(false); // false for allowSurrender, matching commented out .allowSurrender()
        Deck deck = Deck(num_decks_used);
        Engine hiLoEngine = EngineBuilder()
                                    .withEventBus(bus)
                                    .setDeckSize(num_decks_used)
                                    .setDeck(deck)
                                    .setPenetrationThreshold(.75)
                                    .setInitialWallet(1000)
                                    .enableEvents(visualize)
                                    .with3To2Payout()
                                    .withS17Rules()
                                    .allowDoubleAfterSplit()
                                    //.allowSurrender()
                                    .build(std::move(hilo), std::move(robot));
        profit = hiLoEngine.runner();

    //   scores[i] = profit.first;
        swag.first += profit.first;
        swag.second += profit.second;
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Simulation time for " << num_decks_used << " decks and " << iterations << " iterations: " << duration.count() / 1000000.0 << " seconds." << std::endl;


    if (visualize) {
        EventBus::getInstance()->removeObserver(&consoleObserver);
    }

    double average = swag.first / iterations;
    double avg_money_bet = swag.second / iterations;
    double diff = average-1000;
    double normal =  1000.0 / avg_money_bet;
    double money_lost_per = diff * normal;
    double rtp = (1000+money_lost_per) /1000;

    std::cout << "Average after " << iterations << " rounds: " << average << std::endl;
    std::cout << "Average money bet: " << avg_money_bet << std::endl;
    std::cout << "Difference: " << diff << std::endl;
    std::cout << "Money gained/lost per 1000$ " << money_lost_per << "$" << std::endl;
    std::cout << "RTP " << rtp << std::endl;

    return 0;
}


