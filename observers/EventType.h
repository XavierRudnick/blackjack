#ifndef EVENTTYPE_H
#define EVENTTYPE_H

#include <iostream>
#include <cstdint>

enum struct EventType {
    GameStats, //bet, count, bankroll
    CardsDealt, //cards dealt to player or dealer
    ActionTaken, //player action taken
    RoundEnded, //round has ended
    All
};

//std::ostream& operator<<(std::ostream& os, const EventType eventType);

#endif
