#include <cstdlib>
#include <iostream>

#include "Engine.h"
#include "HiLoStrategy.h"
#include "NoStrategy.h"
#include "observers/ConsoleObserver.h"
#include "observers/EventBus.h"

int main(){
    uint8_t num_decks_used = 2;
    const bool visualize = true;
    const int iterations = visualize ? 1 : 100000;

    ConsoleObserver consoleObserver;
    if (visualize) {
        auto* bus = EventBus::getInstance();
        bus->detachAll();
        bus->registerObserver(&consoleObserver, {EventType::CardsDealt, EventType::ActionTaken, EventType::RoundEnded, EventType::GameStats});
    }

    std::pair<double, int> swag = {0, 0};
    for (int i = 0; i < iterations; i++){
        std::pair<double, int> profit = {1000, 0};
        HiLoStrategy hilo = HiLoStrategy(num_decks_used);
        profit = Engine(num_decks_used,profit.first, hilo, visualize).runner(); 
        swag.first += profit.first;
        swag.second += profit.second;
    }

    if (visualize) {
        EventBus::getInstance()->removeObserver(&consoleObserver);
    }

    double average = swag.first / iterations;
    double avg_money_bet = static_cast<double>(swag.second) / iterations;
    double diff = average-1000;
    double rtp = (1000 - ((avg_money_bet / 1000) * diff)) / 1000;

    std::cout << "Average after " << iterations << " rounds: " << average << std::endl;
    std::cout << "Average money bet: " << avg_money_bet << std::endl;
    //std::cout << "RTP: " << rtp << std::endl;
    return 0;
}
