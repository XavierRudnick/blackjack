#ifndef EVENTOBSERVER_H
#define EVENTOBSERVER_H

#include <string>

#include "EventType.h"

class EventObserver {
public:
    virtual void update(EventType eventType, const std::string& message) = 0;
    virtual ~EventObserver() {}
};

#endif