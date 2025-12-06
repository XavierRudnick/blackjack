#include "GameReporter.h"
#include <sstream>

GameReporter::GameReporter(EventBus* bus, bool enabled) : eventBus(bus), enabled(enabled) {}

void GameReporter::setEnabled(bool enabled) {
    this->enabled = enabled;
}

void GameReporter::reportHand(Hand& hand, const std::string& label, bool hideHoleCard) {
    if (enabled && eventBus) {
        eventBus->notifyObservers(EventType::CardsDealt, describeHand(label, hand, hideHoleCard));
    }
}

void GameReporter::reportAction(Action action, Hand& hand, const std::string& label) {
    if (enabled && eventBus) {
        std::ostringstream oss;
        oss << label << " chose " << action << "." << describeHand(label, hand);
        eventBus->notifyObservers(EventType::ActionTaken, oss.str());
    }
}

void GameReporter::reportSplit(const std::string& label, Hand& hand1, Hand& hand2) {
    if (enabled && eventBus) {
        std::ostringstream oss;
        oss << label << " splits into:" << describeHand(label + " (hand 1)", hand1)
            << "\n" << describeHand(label + " (hand 2)", hand2);
        eventBus->notifyObservers(EventType::ActionTaken, oss.str());
    }
}

void GameReporter::reportInsuranceResult(const std::string& message) {
    if (enabled && eventBus) {
        eventBus->notifyObservers(EventType::RoundEnded, message);
    }
}

void GameReporter::reportRoundResult(const std::string& message) {
    if (enabled && eventBus) {
        eventBus->notifyObservers(EventType::RoundEnded, message);
    }
}

void GameReporter::reportDealerFlip(Hand& dealer) {
    if (enabled && eventBus) {
        std::ostringstream oss;
        oss << "Dealer flipped blackjack." << describeHand("Dealer", dealer);
        eventBus->notifyObservers(EventType::RoundEnded, oss.str());
    }
}

void GameReporter::reportStats(const Bankroll& bankroll, const CountingStrategy& strategy) {
    if (enabled && eventBus) {
        std::ostringstream oss;
        oss << "\n==================\n"
            << "Table Stats\n"
            << "------------------\n"
            << "  Wallet       : " << bankroll.getBalance() << "\n"
            << "  True Count   : " << strategy.getTrueCount() << "\n"
            << "  Running Count: " << strategy.getRunningCount() << "\n"
            << "  Decks Left   : " << strategy.getDecksLeft() << "\n"
            << "==================";
        eventBus->notifyObservers(EventType::GameStats, oss.str());
    }
}

void GameReporter::reportMessage(EventType type, const std::string& message) {
    if (enabled && eventBus) {
        eventBus->notifyObservers(type, message);
    }
}

std::string GameReporter::describeHand(const std::string& label, Hand& hand, bool hideHoleCard) {
    std::ostringstream oss;
    oss << "\n" << label << " hand\n";
    std::vector<Card> cards = hand.getCards();

    if (cards.empty()){
        oss << "  Cards: <empty>";
    } else {
        oss << "  Cards: ";
        for (size_t i = 0; i < cards.size(); ++i){
            if (hideHoleCard && i == 1){
                oss << "[hidden]";
            }
            else {
                Card card = cards[i];
                oss << card.getRank() << " of " << card.getSuit();
            }

            if (i + 1 < cards.size()){
                oss << ", ";
            }
        }
    }

    if (hideHoleCard){
        oss << "\n  Score: [hidden]";
    }
    else{
        oss << "\n  Score: " << hand.getScore();
    }
    if (hand.getBetSize() > 0){
        oss << "\n  Bet  : " << hand.getBetSize();
    }
    if (hand.isBlackjack()){
        oss << "\n  Status: Blackjack";
    }
    else if (hand.checkOver()){
        oss << "\n  Status: Bust";
    }

    return oss.str();
}
