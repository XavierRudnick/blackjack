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
        oss << label << " chose " << action << ". " << describeHand(label, hand);
        eventBus->notifyObservers(EventType::ActionTaken, oss.str());
    }
}

void GameReporter::reportSplit(const std::string& label, Hand& hand1, Hand& hand2) {
    if (enabled && eventBus) {
        std::ostringstream oss;
        oss << label << " splits into -> "
            << describeHand(label + " (hand 1)", hand1) << " | "
            << describeHand(label + " (hand 2)", hand2);
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
        oss << "Dealer flipped blackjack. " << describeHand("Dealer", dealer);
        eventBus->notifyObservers(EventType::RoundEnded, oss.str());
    }
}

void GameReporter::reportStats(const Bankroll& bankroll, const CountingStrategy& strategy) {
    if (enabled && eventBus) {
        std::ostringstream oss;
        oss << "Wallet: " << bankroll.getBalance() 
            << " | True Count: " << strategy.getTrueCount() 
            << " | Running Count: " << strategy.getRunningCount() 
            << " | Decks Left: " << strategy.getDecksLeft();
        eventBus->notifyObservers(EventType::GameStats, oss.str());
        eventBus->notifyObservers(EventType::GameStats, "============================================================================");
    }
}

void GameReporter::reportMessage(EventType type, const std::string& message) {
    if (enabled && eventBus) {
        eventBus->notifyObservers(type, message);
    }
}

std::string GameReporter::describeHand(const std::string& label, Hand& hand, bool hideHoleCard) {
    std::ostringstream oss;
    oss << label << " hand: ";
    std::vector<Card> cards = hand.getCards();

    if (cards.empty()){
        oss << "<empty>";
    } else {
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
        oss << " | score: [hidden]";
    }
    else{
        oss << " | score: " << hand.getScore();
    }
    if (hand.getBetSize() > 0){
        oss << " | bet: " << hand.getBetSize();
    }
    if (hand.isBlackjack()){
        oss << " | Blackjack";
    }
    else if (hand.check_over()){
        oss << " | Bust";
    }

    return oss.str();
}
