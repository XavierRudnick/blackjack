#include "Engine.h"
#include "HiLoStrategy.h"
#include "NoStrategy.h"
int main(){
    uint8_t num_decks_used = 6;
    //int total[100000];
    std::pair<double, int> swag = {0, 0};
    for (int j = 0; j < 100000; j++){
        std::pair<double, int> profit = {1000, 0};
        for (int i = 0; i < 4; i++){
            NoStrategy no  = NoStrategy(num_decks_used);
            HiLoStrategy hilo = HiLoStrategy(num_decks_used);
            profit = Engine(num_decks_used,profit.first, hilo).runner(); 
            swag.second += profit.second;
        }
        swag.first += profit.first;
        
        //total[j] = profit.first;
    }

    double average = swag.first / 100000;
    double avg_money_bet = static_cast<double>(swag.second) / 100000;
    double diff = average-1000;
    double rtp = (1000 - ((avg_money_bet / 1000) * diff)) / 1000;

    std::cout << "Average after 100000 rounds: " << average << std::endl;
    std::cout << "Average after 100000 rounds: " << avg_money_bet << std::endl;
    //std::cout << "RTP: " << rtp << std::endl;
    return 0;
}
