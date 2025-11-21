#include <cstdlib>
#include <iostream>

#include "Engine.h"
#include "HiLoStrategy.h"
#include "NoStrategy.h"
#include "observers/ConsoleObserver.h"
#include "observers/EventBus.h"
#include "EngineBuilder.h"

int main(){
    uint8_t num_decks_used = 6;
    const bool visualize = false;
    const int iterations = visualize ? 1 : 200000;

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
        NoStrategy noStrat = NoStrategy(num_decks_used);
        Engine hiLoEngine = EngineBuilder()
                                    .setDeckSize(num_decks_used)
                                    .setInitialWallet(1000)
                                    //.enableEvents(visualize)
                                    .with3To2Payout()
                                    .withS17Rules()
                                    .allowDoubleAfterSplit()
                                    //.allowSurrender()
                                    .build(hilo);
        profit = hiLoEngine.runner();
        swag.first += profit.first;
        swag.second += profit.second;
    }

    if (visualize) {
        EventBus::getInstance()->removeObserver(&consoleObserver);
    }

    double average = swag.first / iterations;
    double avg_money_bet = static_cast<double>(swag.second) / iterations;
    double diff = average-1000;
    double normal =   1000.0 / avg_money_bet;
    double money_lost_per = diff * normal;
    double rtp = (1000+money_lost_per) /1000;

    std::cout << "Average after " << iterations << " rounds: " << average << std::endl;
    std::cout << "Average money bet: " << avg_money_bet << std::endl;
    std::cout << "Difference: " << diff << std::endl;
    std::cout << "Money gained/lost per 1000$ " << money_lost_per << "$" << std::endl;
    std::cout << "RTP " << rtp << std::endl;
    //std::cout << "RTP: " << rtp << std::endl;
    return 0;
}
