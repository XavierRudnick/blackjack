#ifndef EVENTBUS_H
#define EVENTBUS_H

#include <map>
#include <string>
#include <vector>

#include "EventIssuingObservable.h"

class EventBus : public EventIssuingObservable {
public:
    static EventBus* getInstance();

    void registerObserver(EventObserver* observer) override;
    void registerObserver(EventObserver* observer, EventType event) override;
    void registerObserver(EventObserver* observer, std::vector<EventType> events) override;
    void removeObserver(EventObserver* observer) override;

    void notifyObservers(EventType eventType, const std::string& message) override;

    void detachAll();

    ~EventBus() override;

private:
    EventBus() = default;
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;

    std::vector<EventObserver*> getObservers(EventType eventType);

    std::map<EventType, std::vector<EventObserver*>> observers;
    static EventBus* instance;
};

#endif
