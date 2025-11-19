#ifndef EVENTISSUINGOBSERVABLE_H
#define EVENTISSUINGOBSERVABLE_H

#include <string>
#include <vector>

#include "EventObserver.h"

class EventIssuingObservable {
public:
    virtual void registerObserver(EventObserver* observer) = 0;
    virtual void registerObserver(EventObserver* observer, EventType type) = 0;
    virtual void registerObserver(EventObserver* observer, std::vector<EventType> types) = 0;
    virtual void removeObserver(EventObserver* observer) = 0;
    virtual void notifyObservers(EventType eventType, const std::string& message) = 0;
    virtual ~EventIssuingObservable() {}
};

#endif