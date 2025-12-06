#ifndef GAMEREPORTER_H
#define GAMEREPORTER_H

#include <string>
#include <vector>
#include "observers/EventBus.h"
#include "Hand.h"
#include "action.h"
#include "Bankroll.h"
#include "CountingStrategy.h"

class GameReporter {
private:
    EventBus* eventBus;
    bool enabled;

    std::string describeHand(const std::string& label, Hand& hand, bool hideHoleCard = false);

public:
    GameReporter(EventBus* bus, bool enabled);
    
    void setEnabled(bool enabled);

    void reportHand(Hand& hand, const std::string& label, bool hideHoleCard = false);
    void reportAction(Action action, Hand& hand, const std::string& label);
    void reportSplit(const std::string& label, Hand& hand1, Hand& hand2);
    void reportInsuranceResult(const std::string& message);
    void reportRoundResult(const std::string& message);
    void reportDealerFlip(Hand& dealer);
    void reportStats(const Bankroll& bankroll, const CountingStrategy& strategy);
    void reportMessage(EventType type, const std::string& message);
};

#endif
