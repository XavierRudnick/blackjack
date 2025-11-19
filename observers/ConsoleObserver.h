#ifndef CONSOLEOBSERVER_H
#define CONSOLEOBSERVER_H

#include <string>

#include "EventObserver.h"

class ConsoleObserver : public EventObserver {
public:
    void update(EventType eventType, const std::string& message) override;

private:
    const char* eventTypeToString(EventType eventType) const;
};

#endif
