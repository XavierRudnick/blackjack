#ifndef EVENTBUS_H
#define EVENTBUS_H

#include <map>
#include <string>
#include <vector>

#include "EventIssuingObservable.h"

class EventBus : public EventIssuingObservable {
private:
    EventBus() = default;
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete; //deletes the copy-assignment operator to keep EventBus a true singleton

    std::vector<EventObserver*> getObservers(EventType eventType);

    std::map<EventType, std::vector<EventObserver*>> observers;
    static EventBus* instance;

public:
    static EventBus* getInstance();

    void registerObserver(EventObserver* observer) override;
    void registerObserver(EventObserver* observer, EventType event) override;
    void registerObserver(EventObserver* observer, std::vector<EventType> events) override;
    void removeObserver(EventObserver* observer) override;

    void notifyObservers(EventType eventType, const std::string& message) override;

    void detachAll();

    ~EventBus() override;
};

#endif
