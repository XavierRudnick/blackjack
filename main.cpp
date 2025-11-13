#include "Engine.h"
#include "HiLoStrategy.h"
#include "NoStrategy.h"
int main(){
    uint8_t num_decks_used = 2;
    int total[10000];

    for (int j = 0; j < 10000; j++){
        int wallet = 10000;
        for (int i = 0; i < 12; i++){
            NoStrategy no  = NoStrategy(num_decks_used);
            HiLoStrategy hilo = HiLoStrategy(num_decks_used);
            wallet = Engine(num_decks_used,wallet, hilo).runner(); 
        }
        total[j] = wallet;
    }
    int swag = 0;
    for (int i = 0; i < 10000; i++){
        swag += total[i];
    }

    std::cout << "Average after 1000 rounds: " << swag/10000 << std::endl;
    return 0;
}
