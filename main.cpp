#include "Engine.h"
#include "HiLoStrategy.h"

int main(){
    int num_decks_used = 6;

    int swag[1000];
    for (int i = 0; i < 1000; i++){
        HiLoStrategy hilo = HiLoStrategy(num_decks_used);
        swag[i] = Engine(num_decks_used, hilo).runner();
    }
    int total = 0;
    for (int i = 0; i < 100; i++){
        total += swag[i];
    }
    std::cout << "Average after 100 rounds: " << total / 100 << std::endl;
    return 0;
}
