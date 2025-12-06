#ifndef CONSOLEOBSERVER_H
#define CONSOLEOBSERVER_H

#include <string>

#include "EventObserver.h"

class ConsoleObserver : public EventObserver {
private:
    const std::string eventTypeToString(EventType eventType);

public:
    void update(EventType eventType, const std::string& message) override;

};

#endif
