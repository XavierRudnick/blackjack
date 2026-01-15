#include <algorithm>
#include <iostream>

#include "EventBus.h"

void EventBus::registerObserver(EventObserver* observer) {
    registerObserver(observer, EventType::All);
}

void EventBus::registerObserver(EventObserver* observer, EventType event) {
    registerObserver(observer, std::vector<EventType>{event});
}

void EventBus::registerObserver(EventObserver* observer, std::vector<EventType> events) {
    for (EventType event : events) {
        if (!observers.count(event)) {
            observers.emplace(event, std::vector<EventObserver*>());
        }
        observers[event].push_back(observer);
    }
}

void EventBus::removeObserver(EventObserver* observer) {
    for (auto& pair : observers) {
        std::vector<EventObserver*>& eventObservers = pair.second;
        eventObservers.erase(
            std::remove(eventObservers.begin(), eventObservers.end(), observer),
            eventObservers.end());
    }
}

void EventBus::notifyObservers(EventType eventType, const std::string& message) {
    std::vector<EventObserver*> eventObservers = getObservers(eventType);
    for (EventObserver* eventObserver : eventObservers) {
        eventObserver->update(eventType, message);
    }
}

void EventBus::detachAll() { observers.clear(); }

EventBus::~EventBus() {
    std::cout << "Goodbye, I was the EventBus.\n";
}

std::vector<EventObserver*> EventBus::getObservers(EventType eventType){
        std::vector<EventObserver*> observersOfEvent;
        if (observers.count(eventType)){
            observersOfEvent.insert(observersOfEvent.end(),
            observers.at(eventType).begin(),
            observers.at(eventType).end());
        }
        if (observers.count(EventType::All)){
            observersOfEvent.insert(observersOfEvent.end(),
            observers.at(EventType::All).begin(),
            observers.at(EventType::All).end());
        }
        return observersOfEvent;
    }
    