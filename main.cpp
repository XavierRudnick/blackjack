#include "Engine.h"
#include "HiLoStrategy.h"

int main(){
    int num_decks_used = 1;
    int wallet = 10000;
    int swag[5000];
    for (int i = 0; i < 5000; i++){
        HiLoStrategy hilo = HiLoStrategy(num_decks_used);
        wallet = Engine(num_decks_used,wallet, hilo).runner();
        swag[i] = wallet;
    }
    int total = 0;
    for (int i = 0; i < 5000; i++){
        total += swag[i];
    }
    //std::cout << "Average after 1000 rounds: " << total / 100 << std::endl;
    return 0;
}
