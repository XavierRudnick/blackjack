#ifndef LOGGINGCOUNTINGSTRATEGY_H
#define LOGGINGCOUNTINGSTRATEGY_H

#include <memory>
#include <string>
#include <iostream>
#include <sstream>

#include "CountingStrategy.h"
#include "observers/EventBus.h"

class LoggingCountingStrategy : public CountingStrategy {

private:
    std::unique_ptr<CountingStrategy> inner_;
    EventBus* bus_;

    static std::string toStr(float value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }

    static std::string rankToString(Card& card) {
        std::ostringstream oss;
        oss << card.getRank();
        return oss.str();
    }

    void emit(const std::string& message) const {
        if (bus_) {
            bus_->notifyObservers(EventType::GameStats, message);
        } else {
            std::cout << "[CountLog] " << message << "\n";
        }
    }

public:
    LoggingCountingStrategy(std::unique_ptr<CountingStrategy> inner, EventBus* bus = nullptr) : inner_(std::move(inner)), bus_(bus) {}

    int getBetSize() override {
        const int bet = inner_->getBetSize();
        emit("Bet size decided: " + std::to_string(bet));
        return bet;
    }

    void updateCount(Card card) override {
        inner_->updateCount(card);
        emit("Update count with card: rank=" + rankToString(card) +" running=" + toStr(inner_->getRunningCount()) +" true=" + toStr(inner_->getTrueCount()));
    }

    void updateDeckSize(int num_cards_left) override {
        inner_->updateDeckSize(num_cards_left);
        emit("Deck size updated: " + std::to_string(num_cards_left) +" decks left= " + toStr(inner_->getDecksLeft()));
    }

    float getTrueCount() const override { return inner_->getTrueCount(); }
    float getDecksLeft() const override { return inner_->getDecksLeft(); }
    float getRunningCount() const override { return inner_->getRunningCount(); }

    bool shouldAcceptInsurance() const override {
        const bool accept = inner_->shouldAcceptInsurance();
        emit("Insurance decision: " + std::string(accept ? "accept" : "decline"));
        return accept;
    }

    Action shouldDeviatefromHard(int playerTotal, Rank dealerUpcard, float trueCount) override {
        Action action = inner_->shouldDeviatefromHard(playerTotal, dealerUpcard, trueCount);
        emit("Logged deviation query: shouldDeviatefromHard");
        return action;
    }

    Action shouldDeviatefromSplit(Rank playerSplitRank, Rank dealerUpcard, float trueCount) override {
        Action action = inner_->shouldDeviatefromSplit(playerSplitRank, dealerUpcard, trueCount);
        emit("Logged deviation query: shouldDeviatefromSplit");
        return action;
    }

    Action shouldSurrender(int playerTotal, Rank dealerUpcard, float trueCount) override {
        Action action = inner_->shouldSurrender(playerTotal, dealerUpcard, trueCount);
        emit("Logged deviation query: shouldSurrender");
        return action;
    }

    Action getHardHandAction(int playerTotal, Rank dealerUpcard, float trueCount) override {
        Action action = inner_->getHardHandAction(playerTotal, dealerUpcard, trueCount);
        emit("Logged action: getHardHandAction");
        return action;
    }

    Action getSoftHandAction(int playerTotal, Rank dealerUpcard) override {
        Action action = inner_->getSoftHandAction(playerTotal, dealerUpcard);
        emit("Logged action: getSoftHandAction");
        return action;
    }

    Action getSplitAction(Rank playerSplitRank, Rank dealerUpcard, float trueCount) override {
        Action action = inner_->getSplitAction(playerSplitRank, dealerUpcard, trueCount);
        emit("Logged action: getSplitAction");
        return action;
    }

    ~LoggingCountingStrategy() override = default;

};

#endif
