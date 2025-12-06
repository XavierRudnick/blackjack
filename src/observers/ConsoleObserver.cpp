#include "ConsoleObserver.h"

#include <iostream>

void ConsoleObserver::update(EventType eventType, const std::string& message) {
    std::cout << "[" << eventTypeToString(eventType) << "] " << message << std::endl;
}

const std::string ConsoleObserver::eventTypeToString(EventType eventType) {
    switch (eventType) {
        case EventType::GameStats:
            return "GameStats";
        case EventType::CardsDealt:
            return "CardsDealt";
        case EventType::ActionTaken:
            return "ActionTaken";
        case EventType::RoundEnded:
            return "RoundEnded";
        case EventType::All:
            return "All";
    }
    return "Unknown";
}
