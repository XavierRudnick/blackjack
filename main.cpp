#include "Engine.h"
#include "HiLoStrategy.h"

int main(){
    int num_decks_used = 1;
    HiLoStrategy hilo = HiLoStrategy(num_decks_used);
    Engine(num_decks_used, hilo);
    return 0;
}
