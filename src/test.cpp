#include <vector>
#include <cassert>
#include <iostream>
#include <memory>
#include <cmath>
#include <cstdint>
#include <utility>

#include "Engine.h"
#include "HiLoStrategy.h"
#include "NoStrategy.h"
#include "BasicStrategy.h"
#include "LoggingCountingStrategy.h"
#include "CountingStrategy.h"
#include "observers/ConsoleObserver.h"
#include "observers/EventBus.h"
#include "observers/EventObserver.h"
#include "observers/EventType.h"
#include "EngineBuilder.h"
#include "BotPlayer.h"

bool approxEqual(double a, double b, double epsilon = 0.0001) {
    return std::abs(a - b) < epsilon;
}

Engine setupEngine(std::vector<Card> stack, double initialWallet = 1000) {
    const bool visualize = true; 

    static ConsoleObserver consoleObserver; 
    EventBus& bus = EventBus::getInstance();
    bus.detachAll(); // Clear previous observers
    
    if (visualize) {
        bus.registerObserver(&consoleObserver, {
            EventType::CardsDealt, 
            EventType::ActionTaken, 
            EventType::RoundEnded, 
            EventType::GameStats
        });
    }

    static std::unique_ptr<BotPlayer> player; // keep player alive for the returned engine

    auto strategy = std::make_unique<NoStrategy>(0);
    player = std::make_unique<BotPlayer>(false, std::move(strategy));

    Deck riggedDeck = Deck::createTestDeck(stack);

    return EngineBuilder()
            .withEventBus(&bus)
            .setDeckSize(0)
            .setDeck(riggedDeck)
            .setPenetrationThreshold(.5)
            .setInitialWallet(initialWallet)
            .enableEvents(true)
            .with3To2Payout(true)
            .withH17Rules(true)
            .allowDoubleAfterSplit(true)
            .build(player.get());
}

class InsuranceAcceptStrategy : public CountingStrategy {
public:
    int getBetSize() override { return 5; }
    void updateCount(Card) override {}
    void updateDeckSize(int num_cards_left) override {
        decksLeft = num_cards_left > 0 ? 1.0f : 0.0f;
    }
    void setUnitSize(float) override {}

    float getTrueCount() const override { return 0.0f; }
    float getDecksLeft() const override { return decksLeft; }
    float getRunningCount() const override { return 0.0f; }

    bool shouldAcceptInsurance() const override { return true; }
    Action shouldDeviatefromHard(int, Rank, float) override { return Action::Skip; }
    Action shouldDeviatefromSplit(Rank, Rank, float) override { return Action::Skip; }
    Action shouldSurrender(int, Rank, float) override { return Action::Skip; }

    Action getHardHandAction(int, Rank, float) override { return Action::Stand; }
    Action getSoftHandAction(int, Rank) override { return Action::Stand; }
    Action getSplitAction(Rank, Rank, float) override { return Action::Stand; }

    void reset(int) override {}
    std::string getName() override { return "InsuranceAcceptStrategy"; }

private:
    float decksLeft = 1.0f;
};

Engine setupEngineWithStrategy(std::vector<Card> stack, std::unique_ptr<CountingStrategy> strategy, double initialWallet = 1000) {
    const bool visualize = false;

    static ConsoleObserver consoleObserver;
    EventBus& bus = EventBus::getInstance();
    bus.detachAll();

    if (visualize) {
        bus.registerObserver(&consoleObserver, {
            EventType::CardsDealt,
            EventType::ActionTaken,
            EventType::RoundEnded,
            EventType::GameStats
        });
    }

    static std::unique_ptr<BotPlayer> player;
    player = std::make_unique<BotPlayer>(false, std::move(strategy));

    Deck riggedDeck = Deck::createTestDeck(stack);

    return EngineBuilder()
            .withEventBus(&bus)
            .setDeckSize(0)
            .setDeck(riggedDeck)
            .setPenetrationThreshold(.5)
            .setInitialWallet(initialWallet)
            .enableEvents(visualize)
            .with3To2Payout(true)
            .withH17Rules(true)
            .allowDoubleAfterSplit(true)
            .build(player.get());
}

class CountStatsObserver : public EventObserver {
public:
    float lastTrueCount = 0;
    float lastRunningCount = 0;
    float lastDecksLeft = 0;
    int statsEventsSeen = 0;

    void update(EventType eventType, const std::string& message) override {
        if (eventType != EventType::GameStats) {
            return;
        }
        statsEventsSeen++;
        lastTrueCount = extractStat(message, "True Count   : ");
        lastRunningCount = extractStat(message, "Running Count: ");
        lastDecksLeft = extractStat(message, "Decks Left   : ");
    }

private:
    static float extractStat(const std::string& msg, const std::string& label) {
        size_t pos = msg.find(label);
        if (pos == std::string::npos) {
            return 0;
        }
        pos += label.size();
        size_t end = msg.find_first_of("\n\r", pos);
        std::string value = msg.substr(pos, end - pos);
        try {
            return std::stof(value);
        } catch (...) {
            return 0;
        }
    }
};

struct RiggedRunResult {
    double wallet;
    double totalBet;
    float trueCount;
    float runningCount;
    float decksLeft;
    int statsEvents;
};

RiggedRunResult runRiggedDeckWithLogging(const std::vector<Card>& stack, int numDecks, double initialWallet = 1000) {
    EventBus& bus = EventBus::getInstance();
    bus.detachAll();

    CountStatsObserver statsObserver;
    bus.registerObserver(&statsObserver, EventType::GameStats);
    Deck riggedDeck = Deck::createTestDeck(stack);
    auto strategy = std::make_unique<LoggingCountingStrategy>(std::make_unique<HiLoStrategy>(numDecks), bus);
    BotPlayer robot(false, std::move(strategy));
    Engine engine = EngineBuilder()
                        .withEventBus(&bus)
                        .setDeckSize(numDecks)
                        .setDeck(riggedDeck)
                        .setPenetrationThreshold(0.0f) // stop once cards drop to full-deck depth, ensuring only the rigged hands run
                        .setInitialWallet(initialWallet)
                        .enableEvents(true)
                        .with3To2Payout(true)
                        .withH17Rules(true)
                        .allowDoubleAfterSplit(true)
                        .build(&robot);

    std::pair<double, double> result = engine.runner();
    bus.detachAll();

    return {result.first, result.second, statsObserver.lastTrueCount, statsObserver.lastRunningCount, statsObserver.lastDecksLeft, statsObserver.statsEventsSeen};
}

// ----------------------------------------------------------------
// TEST 0: Test double split with aces (3:2)
// ----------------------------------------------------------------
void testSplitAcesTwiceThenStandLogic() {
    std::cout << "\n--- Running testSplitAcesTwiceThenStandLogic ---" << std::endl;

    std::vector<Card> reversedStack = {
        Card(Rank::Ace, Suit::Diamonds),  // 6. Dealer Hit (Total 22)
        Card(Rank::Seven, Suit::Hearts),  // 5. Split Hand 2 Draw
        Card(Rank::Two, Suit::Spades),  // 5. Split Hand 2 Draw
        Card(Rank::Four, Suit::Hearts),   // 4. Split Hand 1 Draw
        Card(Rank::Ace, Suit::Spades),     // 3. Player 2
        Card(Rank::Ace, Suit::Clubs),     // 3. Player 2
        Card(Rank::Ace, Suit::Hearts),    // 2. Player 1
        Card(Rank::Ten, Suit::Clubs),     // 1. Dealer Hole
        Card(Rank::Six, Suit::Clubs)      // 0. Dealer Up
    };

    Engine engine = setupEngine(reversedStack);
    auto result = engine.runner();

    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 999);
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST 1: Natural Blackjack Payout (3:2)
// ----------------------------------------------------------------
void testNaturalBlackjackPayout() {
    std::cout << "\n--- Running testNaturalBlackjackPayout ---" << std::endl;
    
    std::vector<Card> blackjackStack = {
        Card(Rank::King, Suit::Clubs),   // 4. Player Card 2 (King)
        Card(Rank::Ace, Suit::Clubs),    // 3. Player Card 1 (Ace)
        Card(Rank::Five, Suit::Hearts),  // 2. Dealer Hole
        Card(Rank::Five, Suit::Spades),  // 1. Dealer Up
    };

    Engine engine = setupEngine(blackjackStack);
    std::pair<double, double> result = engine.runner();

    // Bet 5. Win 7.5. Wallet 1007.5.
    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 1001.5); 
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST 2: Splitting Aces
// Rule: Split Aces usually get only 1 card each, then turn ends.
// ----------------------------------------------------------------
void testSplitAcesOneCardLogic() {
    std::cout << "\n--- Running testSplitAcesOneCardLogic ---" << std::endl;

    std::vector<Card> reversedStack = {
        Card(Rank::Six, Suit::Diamonds),  // 6. Dealer Hit (Total 22)
        Card(Rank::Queen, Suit::Spades),  // 5. Split Hand 2 Draw
        Card(Rank::King, Suit::Spades),   // 4. Split Hand 1 Draw
        Card(Rank::Ace, Suit::Clubs),     // 3. Player 2
        Card(Rank::Ace, Suit::Hearts),    // 2. Player 1
        Card(Rank::Ten, Suit::Clubs),     // 1. Dealer Hole
        Card(Rank::Six, Suit::Clubs)      // 0. Dealer Up
    };

    Engine engine = setupEngine(reversedStack);
    auto result = engine.runner();

    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 1002);
    std::cout << "PASSED" << std::endl;
}


// ----------------------------------------------------------------
// TEST 3: Double Down on Soft Hand
// Strategy: A-6 (Soft 17) vs 6 -> Double.
// ----------------------------------------------------------------
void testDoubleSoftHand() {
    std::cout << "\n--- Running testDoubleSoftHand ---" << std::endl;

    std::vector<Card> stack = {
        Card(Rank::Ten, Suit::Diamonds), // Dealer draws 10 -> 21
        Card(Rank::Four, Suit::Clubs),   // Player doubles, gets 4 -> 21
        Card(Rank::Six, Suit::Hearts),   // Player 2
        Card(Rank::Ace, Suit::Hearts),   // Player 1
        Card(Rank::Five, Suit::Clubs),   // Dealer Hole
        Card(Rank::Six, Suit::Spades)    // Dealer Up
    };

    Engine engine = setupEngine(stack);
    auto result = engine.runner();

    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 1000);
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST 4: Dealer Bust Chain
// ----------------------------------------------------------------
void testDealerBustChain() {
    std::cout << "\n--- Running testDealerBustChain ---" << std::endl;
    
    std::vector<Card> stack = {
        Card(Rank::Ten, Suit::Diamonds), // D Hit 3 (Bust)
        Card(Rank::Five, Suit::Diamonds),// D Hit 2 (16)
        Card(Rank::Two, Suit::Diamonds), // D Hit 1 (11)
        Card(Rank::Ten, Suit::Clubs),    // P2
        Card(Rank::Ten, Suit::Hearts),   // P1
        Card(Rank::Four, Suit::Clubs),   // D Hole
        Card(Rank::Five, Suit::Spades)   // D Up
    };

    Engine engine = setupEngine(stack);
    auto result = engine.runner();

    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 1001);
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST 5: Blackjack Push
// ----------------------------------------------------------------
void testBlackjackPush() {
    std::cout << "\n--- Running testBlackjackPush ---" << std::endl;
    
    std::vector<Card> stack = {
        Card(Rank::King, Suit::Clubs),   // P2
        Card(Rank::Ace, Suit::Clubs),    // P1
        Card(Rank::Queen, Suit::Hearts), // D Hole
        Card(Rank::Ace, Suit::Spades)    // D Up
    };

    Engine engine = setupEngine(stack);
    auto result = engine.runner();

    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 1000); // Money returned
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST 6A: Insurance Declined, Dealer Blackjack (Player No BJ)
// Dealer shows Ace, player declines insurance (NoStrategy). Dealer has blackjack.
// Player loses original bet only.
// ----------------------------------------------------------------
void testInsuranceDeclinedDealerBlackjackLoss() {
    std::cout << "\n--- Running testInsuranceDeclinedDealerBlackjackLoss ---" << std::endl;

    std::vector<Card> stack = {
        Card(Rank::Nine, Suit::Diamonds),  // Player 2 (Total 16)
        Card(Rank::Seven, Suit::Clubs),    // Player 1
        Card(Rank::Ten, Suit::Hearts),     // Dealer Hole (Blackjack)
        Card(Rank::Ace, Suit::Spades)      // Dealer Up (Insurance offered)
    };

    Engine engine = setupEngine(stack);
    auto result = engine.runner();

    // Decline insurance, lose 5.
    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 999);
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST 6B: Insurance Declined, Mutual Blackjacks Push
// Dealer shows Ace and has blackjack; player also has blackjack.
// Declining insurance should return original bet (push).
// ----------------------------------------------------------------
void testInsuranceDeclinedMutualBlackjacksPush() {
    std::cout << "\n--- Running testInsuranceDeclinedMutualBlackjacksPush ---" << std::endl;

    std::vector<Card> stack = {
        Card(Rank::King, Suit::Hearts), // Player 2 (Blackjack)
        Card(Rank::Ace, Suit::Hearts),  // Player 1
        Card(Rank::Ten, Suit::Clubs),   // Dealer Hole (Blackjack)
        Card(Rank::Ace, Suit::Clubs)    // Dealer Up (Insurance offered)
    };

    Engine engine = setupEngine(stack);
    auto result = engine.runner();

    // Push: bet returned only.
    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 1000);
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST 6D: Insurance Accepted, Dealer Blackjack, Player No BJ
// Dealer shows Ace, player accepts insurance; dealer has blackjack.
// Main bet lost, insurance wins -> net 0.
// ----------------------------------------------------------------
void testInsuranceAcceptedDealerBlackjackPlayerNoBJ() {
    std::cout << "\n--- Running testInsuranceAcceptedDealerBlackjackPlayerNoBJ ---" << std::endl;

    std::vector<Card> stack = {
        Card(Rank::Seven, Suit::Diamonds),  // Player 2 (Total 16)
        Card(Rank::Nine, Suit::Clubs),      // Player 1
        Card(Rank::Ten, Suit::Hearts),      // Dealer Hole (Blackjack)
        Card(Rank::Ace, Suit::Spades)       // Dealer Up (Insurance offered)
    };

    Engine engine = setupEngineWithStrategy(stack, std::make_unique<InsuranceAcceptStrategy>());
    auto result = engine.runner();

    // Bet 5 + insurance 2.5, insurance wins, main loses => wallet unchanged.
    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 1000);
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST 6E: Insurance Accepted, Dealer Blackjack, Player Blackjack
// Dealer shows Ace, player accepts insurance; both have blackjack.
// Main bet pushes, insurance wins -> net +5.
// ----------------------------------------------------------------
void testInsuranceAcceptedDealerBlackjackPlayerBlackjack() {
    std::cout << "\n--- Running testInsuranceAcceptedDealerBlackjackPlayerBlackjack ---" << std::endl;

    std::vector<Card> stack = {
        Card(Rank::King, Suit::Hearts),  // Player 2 (Blackjack)
        Card(Rank::Ace, Suit::Hearts),   // Player 1
        Card(Rank::Ten, Suit::Clubs),    // Dealer Hole (Blackjack)
        Card(Rank::Ace, Suit::Clubs)     // Dealer Up (Insurance offered)
    };

    Engine engine = setupEngineWithStrategy(stack, std::make_unique<InsuranceAcceptStrategy>());
    auto result = engine.runner();

    // Bet 5 + insurance 2.5, insurance wins (+5), main pushes => wallet +5.
    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 1005);
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST 6F: Insurance Accepted, Dealer No BJ, Player Blackjack
// Dealer shows Ace, player accepts insurance; dealer does NOT have blackjack.
// Insurance loses, main blackjack pays 3:2.
// ----------------------------------------------------------------
void testInsuranceAcceptedDealerNoBJPlayerBlackjack() {
    std::cout << "\n--- Running testInsuranceAcceptedDealerNoBJPlayerBlackjack ---" << std::endl;

    std::vector<Card> stack = {
        Card(Rank::King, Suit::Diamonds), // Player 2 (Blackjack)
        Card(Rank::Ace, Suit::Diamonds),  // Player 1
        Card(Rank::Nine, Suit::Clubs),    // Dealer Hole (No blackjack)
        Card(Rank::Ace, Suit::Spades)     // Dealer Up (Insurance offered)
    };

    Engine engine = setupEngineWithStrategy(stack, std::make_unique<InsuranceAcceptStrategy>());
    auto result = engine.runner();

    // Bet 5 + insurance 2.5, insurance loses, BJ pays 7.5 => wallet +5.
    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 1005);
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST 6G: Insurance Accepted, Dealer No BJ, Player No BJ (Player Wins)
// Dealer shows Ace, player accepts insurance; dealer does NOT have blackjack.
// Insurance loses, player wins normally.
// ----------------------------------------------------------------
void testInsuranceAcceptedDealerNoBJPlayerNoBJWin() {
    std::cout << "\n--- Running testInsuranceAcceptedDealerNoBJPlayerNoBJWin ---" << std::endl;

    std::vector<Card> stack = {
        Card(Rank::Ten, Suit::Diamonds), // Player 2 (Total 20)
        Card(Rank::Ten, Suit::Clubs),    // Player 1
        Card(Rank::Eight, Suit::Clubs),  // Dealer Hole (Soft 19)
        Card(Rank::Ace, Suit::Hearts)    // Dealer Up (Insurance offered)
    };

    Engine engine = setupEngineWithStrategy(stack, std::make_unique<InsuranceAcceptStrategy>());
    auto result = engine.runner();

    // Bet 5 + insurance 2.5, insurance loses, player wins +5 => wallet 1002.5.
    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 1002.5);
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST 6: Splitting Aces Twice
// Rule: Split Aces usually get only 1 card each, then turn ends.
// ----------------------------------------------------------------
void testSplitAcesTwiceLogic() {
    std::cout << "\n--- Running testSplitAcesTwiceCardLogic ---" << std::endl;

    std::vector<Card> reversedStack = {
        Card(Rank::Six, Suit::Diamonds),  // 6. Dealer Hit (Total 22)
        Card(Rank::Seven, Suit::Hearts),  // 5. Split Hand 2 Draw
        Card(Rank::Queen, Suit::Spades),  // 5. Split Hand 2 Draw
        Card(Rank::King, Suit::Spades),   // 4. Split Hand 1 Draw
        Card(Rank::Ace, Suit::Spades),     // 3. Player 2
        Card(Rank::Ace, Suit::Clubs),     // 3. Player 2
        Card(Rank::Ace, Suit::Hearts),    // 2. Player 1
        Card(Rank::Ten, Suit::Clubs),     // 1. Dealer Hole
        Card(Rank::Six, Suit::Clubs)      // 0. Dealer Up
    };

    Engine engine = setupEngine(reversedStack);
    auto result = engine.runner();

    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 1003);
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST 6C: Dealer Shows Ten, Hidden Ace (Dealer Blackjack Robbery)
// Insurance not offered (upcard is 10). Dealer hidden Ace ends round immediately.
// ----------------------------------------------------------------
void testDealerShowsTenHiddenAceBlackjack() {
    std::cout << "\n--- Running testDealerShowsTenHiddenAceBlackjack ---" << std::endl;

    std::vector<Card> stack = {
        Card(Rank::Nine, Suit::Hearts),   // Player 2 (Total 16)
        Card(Rank::Seven, Suit::Diamonds),// Player 1
        Card(Rank::Ace, Suit::Hearts),    // Dealer Hole (Blackjack)
        Card(Rank::King, Suit::Spades)    // Dealer Up (shows ten)
    };

    Engine engine = setupEngine(stack);
    auto result = engine.runner();

    // No insurance offered; player loses 5 immediately.
    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 999);
    std::cout << "PASSED" << std::endl;
}
// ----------------------------------------------------------------
// TEST 7: Dealer Stands on Soft 17 (S17 Rule)
// Scenario: Dealer has Ace + 6 (Soft 17). 
// Rule S17: Dealer must STAND. 
// Player has 20. Player Wins.
// // ----------------------------------------------------------------
// void testDealerStandsOnSoft17() {
//     std::cout << "\n--- Running testDealerStandsOnSoft17 ---" << std::endl;

//     std::vector<Card> stack = {
//         // No Hit cards needed, Dealer stands immediately
//         Card(Rank::King, Suit::Hearts),   // 4. Player 2 (Total 20)
//         Card(Rank::Queen, Suit::Hearts),  // 3. Player 1
//         Card(Rank::Ace, Suit::Clubs),     // 2. Dealer Hole (Soft 17)
//         Card(Rank::Six, Suit::Spades)     // 1. Dealer Up
//     };

//     Engine engine = setupEngine(stack);
//     auto result = engine.runner();

//     // Player (20) > Dealer (17). Bet 5 -> Win 5.
//     std::cout << "Final: " << result.first << std::endl;
//     assert(result.first == 1005);
//     std::cout << "PASSED" << std::endl;
// }

// ----------------------------------------------------------------
// TEST 8: Player Busts (Dealer Play Skipped)
// Scenario: Player hits 16 vs 10, draws 10, busts.
// Optimization check: Dealer should not draw/reveal extra cards if player busts.
// ----------------------------------------------------------------
void testPlayerBusts() {
    std::cout << "\n--- Running testPlayerBusts ---" << std::endl;

    std::vector<Card> stack = {
        Card(Rank::King, Suit::Diamonds), // 5. Player Hit (Bust 26)
        Card(Rank::Six, Suit::Clubs),     // 4. Player 2 (Total 16)
        Card(Rank::King, Suit::Hearts),   // 3. Player 1
        Card(Rank::Seven, Suit::Clubs),   // 2. Dealer Hole
        Card(Rank::Ten, Suit::Spades)     // 1. Dealer Up
    };

    Engine engine = setupEngine(stack);
    auto result = engine.runner();

    // Loss 5.
    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 999);
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST 9: Successful Double Down (Hard 11)
// Scenario: Player 11 vs 6. Doubles. Gets 10 (21). Dealer Busts.
// ----------------------------------------------------------------
void testDoubleDownWin() {
    std::cout << "\n--- Running testDoubleDownWin ---" << std::endl;

    std::vector<Card> stack = {
        Card(Rank::Ten, Suit::Diamonds),  // 6. Dealer Hit (26 Bust)
        Card(Rank::King, Suit::Diamonds), // 5. Player Double Card (21)
        Card(Rank::Six, Suit::Hearts),    // 4. Player 2 (Total 11)
        Card(Rank::Five, Suit::Hearts),   // 3. Player 1
        Card(Rank::Ten, Suit::Clubs),     // 2. Dealer Hole (Total 16)
        Card(Rank::Six, Suit::Spades)     // 1. Dealer Up
    };

    Engine engine = setupEngine(stack);
    auto result = engine.runner();

    // Bet 5 -> Double to 10. Win 10. Total 1010.
    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 1002);
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST 10B: Double Down Loss
// Scenario: Player 11 vs 10. Doubles, draws low card to 13. Dealer stands on 20. Loss of doubled bet.
// ----------------------------------------------------------------
void testDoubleDownLoss() {
    std::cout << "\n--- Running testDoubleDownLoss ---" << std::endl;

    std::vector<Card> stack = {
        Card(Rank::Two, Suit::Clubs),     // 4. Player double draw (13 total)
        Card(Rank::Five, Suit::Diamonds), // 3. Player 2
        Card(Rank::Six, Suit::Hearts),    // 2. Player 1 (11)
        Card(Rank::Ten, Suit::Hearts),    // 1. Dealer Hole (20)
        Card(Rank::Ten, Suit::Spades)     // 0. Dealer Up
    };

    Engine engine = setupEngine(stack);
    auto result = engine.runner();

    // Bet 5, double to 10, lose -> wallet 990.
    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 998);
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST 10: Split 8s with Mixed Result (1 Win, 1 Loss)
// Scenario: 8,8 vs 10. Split.
// Hand 1: 8 + 10 (18) vs Dealer 19 (Loss)
// Hand 2: 8 + 3 + 8 (19) vs Dealer 19 (Push) -> Actually lets do Win for clarity
// Let's do: Dealer 18. Hand 1 (17 Loss), Hand 2 (19 Win). Net 0.
// ----------------------------------------------------------------
void testSplitMixedResults() {
    std::cout << "\n--- Running testSplitMixedResults ---" << std::endl;

    std::vector<Card> stack = {
        Card(Rank::Eight, Suit::Diamonds), // 7. Hand 2 Hit (Total 19)
        Card(Rank::Three, Suit::Clubs),    // 6. Hand 2 Draw (Total 11)
        Card(Rank::Nine, Suit::Spades),    // 5. Hand 1 Draw (Total 17)
        Card(Rank::Eight, Suit::Hearts),   // 4. Player 2
        Card(Rank::Eight, Suit::Clubs),    // 3. Player 1
        Card(Rank::Eight, Suit::Spades),   // 2. Dealer Hole (Total 18)
        Card(Rank::Ten, Suit::Spades)      // 1. Dealer Up
    };

    // Logic Trace:
    // 1. Deal: D(10, 8=18), P(8, 8).
    // 2. Split.
    // 3. Hand 1 (8): Draws 9 -> 17. Stands.
    // 4. Hand 2 (8): Draws 3 -> 11. Hits. Draws 8 -> 19. Stands.
    // 5. Dealer reveals 18.
    // Result: H1(17) vs D(18) = Lose 5. H2(19) vs D(18) = Win 5.
    // Net change: 0.

    Engine engine = setupEngine(stack);
    auto result = engine.runner();

    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 1001);
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST 10C: Split Hands Both Lose vs Dealer 20
// Scenario: 8,8 vs 10. Split; draw to 17 and 18. Dealer stands on 20. Both hands lose.
// ----------------------------------------------------------------
void testSplitBothLose() {
    std::cout << "\n--- Running testSplitBothLose ---" << std::endl;

    std::vector<Card> stack = {
        Card(Rank::Ten, Suit::Hearts),   // 5. Split Hand 2 Draw -> 18
        Card(Rank::Nine, Suit::Clubs),   // 4. Split Hand 1 Draw -> 17
        Card(Rank::Eight, Suit::Hearts), // 3. Player 2
        Card(Rank::Eight, Suit::Clubs),  // 2. Player 1
        Card(Rank::Ten, Suit::Diamonds), // 1. Dealer Hole (20)
        Card(Rank::Ten, Suit::Spades)    // 0. Dealer Up
    };

    Engine engine = setupEngine(stack);
    auto result = engine.runner();

    // Initial bet 5 + split 5 = 10 lost. Wallet 990.
    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 998);
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST 11: Multi-Hit to 21
// Scenario: Player gets small cards, hits 3 times to get exactly 21.
// ----------------------------------------------------------------
void testMultiHit21() {
    std::cout << "\n--- Running testMultiHit21 ---" << std::endl;

    std::vector<Card> stack = {
        Card(Rank::Five, Suit::Diamonds),  // 6. Player Hit 3 (Total 21)
        Card(Rank::Four, Suit::Clubs),     // 5. Player Hit 2 (Total 16)
        Card(Rank::Three, Suit::Hearts),   // 4. Player Hit 1 (Total 12)
        Card(Rank::Four, Suit::Spades),    // 3. Player 2
        Card(Rank::Five, Suit::Spades),    // 2. Player 1 (Total 9)
        Card(Rank::Eight, Suit::Clubs),    // 1. Dealer Hole (18)
        Card(Rank::Ten, Suit::Spades)      // 0. Dealer Up
    };

    Engine engine = setupEngine(stack);
    auto result = engine.runner();

    // Player 21 vs Dealer 18. Win 5.
    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 1001);
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST 12B: Multi-Card 21 Push vs Dealer 21 (non-blackjack)
// Ensures push logic works when both reach 21 without naturals and dealer draws appropriately.
// ----------------------------------------------------------------
void testPush21NonBlackjack() {
    std::cout << "\n--- Running testPush21NonBlackjack ---" << std::endl;

    std::vector<Card> stack = {
        Card(Rank::Five, Suit::Clubs),    // 5. Dealer Hit to 21
        Card(Rank::Nine, Suit::Hearts),   // 4. Player Hit to 21
        Card(Rank::Seven, Suit::Spades),  // 3. Player 2 (initial 12)
        Card(Rank::Five, Suit::Hearts),   // 2. Player 1
        Card(Rank::Seven, Suit::Diamonds),// 1. Dealer Hole (16)
        Card(Rank::Nine, Suit::Clubs)     // 0. Dealer Up
    };

    Engine engine = setupEngine(stack);
    auto result = engine.runner();

    // Player doubles? No (only one hit), ends on 21; dealer hits to 21; push returns bet.
    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 1000);
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST 12: Double After Split Allowed
// Scenario: 8,8 vs 6. Split; both hands double after split and win vs dealer bust.
// Verifies double-after-split path and correct bet accounting.
// ----------------------------------------------------------------
void testDoubleAfterSplitAllowed() {
    std::cout << "\n--- Running testDoubleAfterSplitAllowed ---" << std::endl;

    std::vector<Card> stack = {
        Card(Rank::King, Suit::Diamonds), // 0. Dealer Hit -> 25 (bust)
        Card(Rank::Nine, Suit::Clubs),    // 1. Hand 2 Double Draw (19)
        Card(Rank::Ten, Suit::Diamonds),  // 2. Hand 1 Double Draw (21)
        Card(Rank::Two, Suit::Clubs),     // 3. Split Hand 2 Draw (total 10)
        Card(Rank::Three, Suit::Diamonds),// 4. Split Hand 1 Draw (total 11)
        Card(Rank::Eight, Suit::Hearts),  // 5. Player 2 (initial)
        Card(Rank::Eight, Suit::Clubs),   // 6. Player 1 (initial)
        Card(Rank::Nine, Suit::Hearts),   // 7. Dealer Hole (15)
        Card(Rank::Six, Suit::Spades)     // 8. Dealer Up (shows 6)
    };

    Engine engine = setupEngine(stack);
    auto result = engine.runner();

    // Initial bets: 5 + split 5 + double 5 + double 5 = 20 withdrawn; two double wins (+40) -> final 1020.
    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 1004);
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST 13: Split Aces with Ten = 21 (Not Blackjack)
// Rule: Splitting Aces and drawing a 10-value card counts as 21, 
// NOT a Natural Blackjack. Payout is 1:1, not 3:2.
// ----------------------------------------------------------------
void testSplitAcesTenIsNotBlackjack() {
    std::cout << "\n--- Running testSplitAcesTenIsNotBlackjack ---" << std::endl;

    // Stack Order (Back/Bottom pops first):
    // 1. Dealer Up (10)
    // 2. Dealer Hole (10) -> Dealer has 20
    // 3. Player 1 (Ace)
    // 4. Player 2 (Ace) -> Player splits
    // 5. Split Hand 1 Hit -> King (Total 21)
    // 6. Split Hand 2 Hit -> Queen (Total 21)
    std::vector<Card> stack = {
        Card(Rank::Queen, Suit::Diamonds), // 6. Hand 2 Draw
        Card(Rank::King, Suit::Diamonds),  // 5. Hand 1 Draw
        Card(Rank::Ace, Suit::Clubs),      // 4. Player 2
        Card(Rank::Ace, Suit::Hearts),     // 3. Player 1
        Card(Rank::Ten, Suit::Clubs),      // 2. Dealer Hole (20)
        Card(Rank::Ten, Suit::Spades)      // 1. Dealer Up
    };

    Engine engine = setupEngine(stack);
    auto result = engine.runner();
    
    // Bet 5. Split (Total Bet 10).
    // Hand 1 (21) beats Dealer (20) -> Win 5 (1:1 payout)
    // Hand 2 (21) beats Dealer (20) -> Win 5 (1:1 payout)
    // Wallet: 1000 - 10 (bets) + 10 (return) + 10 (winnings) = 1010.
    // IF this was calculated as BJ (3:2), wallet would be 1015.
    
    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 1002); 
    std::cout << "PASSED" << std::endl;
}


// ----------------------------------------------------------------
// TEST 14: Dealer Ace Dynamic Valuation (Soft -> Hard -> Win)
// Scenario: Dealer has Ace, 2 (Soft 13). 
// Hit 3 -> Soft 16.
// Hit 10 -> Hard 16 (Ace becomes 1).
// Hit 5 -> 21. Beats Player 20.
// ----------------------------------------------------------------
void testDealerAceValuationChange() {
    std::cout << "\n--- Running testDealerAceValuationChange ---" << std::endl;

    std::vector<Card> stack = {
        Card(Rank::Five, Suit::Diamonds),  // 6. Dealer Hit 3 (Total 21)
        Card(Rank::Ten, Suit::Diamonds),   // 5. Dealer Hit 2 (Total Hard 16)
        Card(Rank::Three, Suit::Diamonds), // 4. Dealer Hit 1 (Total Soft 16)
        Card(Rank::Ten, Suit::Clubs),      // 3. Player 2 (20)
        Card(Rank::Ten, Suit::Hearts),     // 2. Player 1
        Card(Rank::Two, Suit::Clubs),      // 1. Dealer Hole (Soft 13)
        Card(Rank::Ace, Suit::Spades)      // 0. Dealer Up
    };

    Engine engine = setupEngine(stack);
    auto result = engine.runner();

    // Player 20 vs Dealer 21. Loss 5.
    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 999);
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST 16: Player Hits to 21 vs Dealer Blackjack
// Scenario: Player gets 21 via hits. Dealer reveals Blackjack.
// Result: Dealer BJ trumps Player non-natural 21.
// ----------------------------------------------------------------
void testPlayer21VsDealerBlackjack() {
    std::cout << "\n--- Running testPlayer21VsDealerBlackjack ---" << std::endl;

    // Stack Note: If dealer has BJ and Upcard is Ten/Ace, round usually ends immediately (peek).
    // We provide a stack where player *would* have 21 if they played, 
    // to ensure the engine stops them.
    std::vector<Card> stack = {
        Card(Rank::Six, Suit::Clubs),     // 3. Player 2 (16)
        Card(Rank::Ten, Suit::Hearts),    // 2. Player 1
        Card(Rank::Ace, Suit::Clubs),     // 1. Dealer Hole (BJ)
        Card(Rank::Queen, Suit::Spades)   // 0. Dealer Up (Peek triggered)
    };

    Engine engine = setupEngine(stack);
    auto result = engine.runner();

    // Player loses immediately to BJ. Wallet 995.
    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 999);
    std::cout << "PASSED" << std::endl;
}

// ================================================================
// HI-LO COUNTING STRATEGY TESTS
// ================================================================

// ----------------------------------------------------------------
// TEST: Running Count with Low Cards (2-6 = +1 each)
// ----------------------------------------------------------------
void testRunningCountLowCards() {
    std::cout << "\n--- Running testRunningCountLowCards ---" << std::endl;
    
    HiLoStrategy strategy(1.0); // 1 deck
    
    // Low cards: 2, 3, 4, 5, 6 should each add +1 to running count
    strategy.updateCount(Card(Rank::Two, Suit::Hearts));
    assert(strategy.getRunningCount() == 1);
    
    strategy.updateCount(Card(Rank::Three, Suit::Clubs));
    assert(strategy.getRunningCount() == 2);
    
    strategy.updateCount(Card(Rank::Four, Suit::Diamonds));
    assert(strategy.getRunningCount() == 3);
    
    strategy.updateCount(Card(Rank::Five, Suit::Spades));
    assert(strategy.getRunningCount() == 4);
    
    strategy.updateCount(Card(Rank::Six, Suit::Hearts));
    assert(strategy.getRunningCount() == 5);
    
    std::cout << "Running count after 5 low cards: " << strategy.getRunningCount() << std::endl;
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST: Running Count with High Cards (10, J, Q, K, A = -1 each)
// ----------------------------------------------------------------
void testRunningCountHighCards() {
    std::cout << "\n--- Running testRunningCountHighCards ---" << std::endl;
    
    HiLoStrategy strategy(1.0); // 1 deck
    
    // High cards: 10, J, Q, K, A should each subtract -1 from running count
    strategy.updateCount(Card(Rank::Ten, Suit::Hearts));
    assert(strategy.getRunningCount() == -1);
    
    strategy.updateCount(Card(Rank::Jack, Suit::Clubs));
    assert(strategy.getRunningCount() == -2);
    
    strategy.updateCount(Card(Rank::Queen, Suit::Diamonds));
    assert(strategy.getRunningCount() == -3);
    
    strategy.updateCount(Card(Rank::King, Suit::Spades));
    assert(strategy.getRunningCount() == -4);
    
    strategy.updateCount(Card(Rank::Ace, Suit::Hearts));
    assert(strategy.getRunningCount() == -5);
    
    std::cout << "Running count after 5 high cards: " << strategy.getRunningCount() << std::endl;
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST: Running Count with Neutral Cards (7, 8, 9 = 0)
// ----------------------------------------------------------------
void testRunningCountNeutralCards() {
    std::cout << "\n--- Running testRunningCountNeutralCards ---" << std::endl;
    
    HiLoStrategy strategy(1.0); // 1 deck
    
    // Neutral cards: 7, 8, 9 should not change the count
    strategy.updateCount(Card(Rank::Seven, Suit::Hearts));
    assert(strategy.getRunningCount() == 0);
    
    strategy.updateCount(Card(Rank::Eight, Suit::Clubs));
    assert(strategy.getRunningCount() == 0);
    
    strategy.updateCount(Card(Rank::Nine, Suit::Diamonds));
    assert(strategy.getRunningCount() == 0);
    
    std::cout << "Running count after 3 neutral cards: " << strategy.getRunningCount() << std::endl;
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST: True Count Calculation with Single Deck
// True Count = Running Count / Decks Remaining
// ----------------------------------------------------------------
void testTrueCountSingleDeck() {
    std::cout << "\n--- Running testTrueCountSingleDeck ---" << std::endl;
    
    HiLoStrategy strategy(1.0); // 1 deck
    
    // Add 4 low cards -> running count = +4
    strategy.updateCount(Card(Rank::Two, Suit::Hearts));
    strategy.updateCount(Card(Rank::Three, Suit::Clubs));
    strategy.updateCount(Card(Rank::Four, Suit::Diamonds));
    strategy.updateCount(Card(Rank::Five, Suit::Spades));
    
    // True count = 4 / 1.0 = 4.0
    std::cout << "Running Count: " << strategy.getRunningCount() << std::endl;
    std::cout << "Decks Left: " << strategy.getDecksLeft() << std::endl;
    std::cout << "True Count: " << strategy.getTrueCount() << std::endl;
    
    assert(strategy.getRunningCount() == 4);
    assert(strategy.getTrueCount() == 4.0);
    
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST: True Count Calculation with Multiple Decks
// ----------------------------------------------------------------
void testTrueCountMultipleDecks() {
    std::cout << "\n--- Running testTrueCountMultipleDecks ---" << std::endl;
    
    HiLoStrategy strategy(4.0); // 4 decks
    
    // Add 8 low cards -> running count = +8
    for (int i = 0; i < 8; i++) {
        strategy.updateCount(Card(Rank::Five, Suit::Hearts));
    }
    
    // True count = 8 / 4.0 = 2.0
    std::cout << "Running Count: " << strategy.getRunningCount() << std::endl;
    std::cout << "Decks Left: " << strategy.getDecksLeft() << std::endl;
    std::cout << "True Count: " << strategy.getTrueCount() << std::endl;
    
    assert(strategy.getRunningCount() == 8);
    assert(strategy.getTrueCount() == 2.0);
    
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST: True Count with Half Deck Increments
// Verifies the 0.5 rounding behavior for true count
// ----------------------------------------------------------------
void testTrueCountHalfIncrements() {
    std::cout << "\n--- Running testTrueCountHalfIncrements ---" << std::endl;
    
    HiLoStrategy strategy(2.0); // 2 decks
    
    // Running count = +3, decks = 2.0 -> True count = 1.5
    strategy.updateCount(Card(Rank::Two, Suit::Hearts));
    strategy.updateCount(Card(Rank::Three, Suit::Clubs));
    strategy.updateCount(Card(Rank::Four, Suit::Diamonds));
    
    std::cout << "Running Count: " << strategy.getRunningCount() << std::endl;
    std::cout << "Decks Left: " << strategy.getDecksLeft() << std::endl;
    std::cout << "True Count: " << strategy.getTrueCount() << std::endl;
    
    // 3 / 2.0 = 1.5, rounded to nearest 0.5 = 1.5
    assert(strategy.getRunningCount() == 3);
    assert(strategy.getTrueCount() == 1.5);
    
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST: Deck Size Updates with Exact Deck Fractions
// ----------------------------------------------------------------
void testDeckSizeUpdateRounding() {
    std::cout << "\n--- Running testDeckSizeUpdateRounding ---" << std::endl;
    
    HiLoStrategy strategy(6.0); // Start with 6 decks
    
    // 52 cards = 1 deck
    strategy.updateDeckSize(52);
    std::cout << "52 cards -> Decks: " << strategy.getDecksLeft() << std::endl;
    assert(approxEqual(strategy.getDecksLeft(), 1.0));
    
    // 78 cards = 1.5 decks
    strategy.updateDeckSize(78);
    std::cout << "78 cards -> Decks: " << strategy.getDecksLeft() << std::endl;
    assert(approxEqual(strategy.getDecksLeft(), 1.5));
    
    // 104 cards = 2 decks
    strategy.updateDeckSize(104);
    std::cout << "104 cards -> Decks: " << strategy.getDecksLeft() << std::endl;
    assert(approxEqual(strategy.getDecksLeft(), 2.0));
    
    // 130 cards = 2.5 decks
    strategy.updateDeckSize(130);
    std::cout << "130 cards -> Decks: " << strategy.getDecksLeft() << std::endl;
    assert(approxEqual(strategy.getDecksLeft(), 2.5));
    
    // 40 cards ~= 0.769 decks
    strategy.updateDeckSize(40);
    std::cout << "40 cards -> Decks: " << strategy.getDecksLeft() << std::endl;
    assert(approxEqual(strategy.getDecksLeft(), 40.0 / 52.0));
    
    // 26 cards = 0.5 decks
    strategy.updateDeckSize(26);
    std::cout << "26 cards -> Decks: " << strategy.getDecksLeft() << std::endl;
    assert(approxEqual(strategy.getDecksLeft(), 0.5));
    
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST: True Count adjusts when deck size changes (recompute on updateDeckSize)
// ----------------------------------------------------------------
void testTrueCountWithDeckDepletion() {
    std::cout << "\n--- Running testTrueCountWithDeckDepletion ---" << std::endl;

    HiLoStrategy strategy(2.0); // Start with 2 decks (104 cards)

    // Deal 6 low cards -> running count = +6, decks left = 2.0 -> true = 3.0
    for (int i = 0; i < 6; i++) {
        strategy.updateCount(Card(Rank::Five, Suit::Hearts));
    }
    assert(strategy.getRunningCount() == 6);
    assert(strategy.getTrueCount() == 3.0);

    // Now update deck size to 52 cards (1 deck) and expect true count to double
    strategy.updateDeckSize(52);
    std::cout << "After depletion to 1 deck: True = " << strategy.getTrueCount() << std::endl;
    assert(strategy.getTrueCount() == 6.0);

    // Deplete further to half deck (26 cards)
    strategy.updateDeckSize(26);
    std::cout << "After depletion to 0.5 deck: True = " << strategy.getTrueCount() << std::endl;
    assert(strategy.getTrueCount() == 12.0);

    std::cout << "PASSED" << std::endl;
}


// ----------------------------------------------------------------
// TEST: Mixed Card Sequence (Realistic Shoe)
// ----------------------------------------------------------------
void testMixedCardSequence() {
    std::cout << "\n--- Running testMixedCardSequence ---" << std::endl;
    
    HiLoStrategy strategy(1.0); // 1 deck
    
    // Deal a realistic sequence:
    // 2 (+1), K (-1), 5 (+1), A (-1), 7 (0), 3 (+1), Q (-1), 4 (+1), 9 (0), 6 (+1)
    // Expected running count: +1 -1 +1 -1 +0 +1 -1 +1 +0 +1 = +2
    
    strategy.updateCount(Card(Rank::Two, Suit::Hearts));    // +1 -> 1
    strategy.updateCount(Card(Rank::King, Suit::Clubs));    // -1 -> 0
    strategy.updateCount(Card(Rank::Five, Suit::Diamonds)); // +1 -> 1
    strategy.updateCount(Card(Rank::Ace, Suit::Spades));    // -1 -> 0
    strategy.updateCount(Card(Rank::Seven, Suit::Hearts));  // +0 -> 0
    strategy.updateCount(Card(Rank::Three, Suit::Clubs));   // +1 -> 1
    strategy.updateCount(Card(Rank::Queen, Suit::Diamonds));// -1 -> 0
    strategy.updateCount(Card(Rank::Four, Suit::Spades));   // +1 -> 1
    strategy.updateCount(Card(Rank::Nine, Suit::Hearts));   // +0 -> 1
    strategy.updateCount(Card(Rank::Six, Suit::Clubs));     // +1 -> 2
    
    std::cout << "Running count after mixed sequence: " << strategy.getRunningCount() << std::endl;
    assert(strategy.getRunningCount() == 2);
    assert(strategy.getTrueCount() == 2.0);
    
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST: Negative True Count
// ----------------------------------------------------------------
void testNegativeTrueCount() {
    std::cout << "\n--- Running testNegativeTrueCount ---" << std::endl;
    
    HiLoStrategy strategy(2.0); // 2 decks
    
    // Deal 6 high cards -> running count = -6
    strategy.updateCount(Card(Rank::Ten, Suit::Hearts));
    strategy.updateCount(Card(Rank::Jack, Suit::Clubs));
    strategy.updateCount(Card(Rank::Queen, Suit::Diamonds));
    strategy.updateCount(Card(Rank::King, Suit::Spades));
    strategy.updateCount(Card(Rank::Ace, Suit::Hearts));
    strategy.updateCount(Card(Rank::Ten, Suit::Clubs));
    
    std::cout << "Running Count: " << strategy.getRunningCount() << std::endl;
    std::cout << "True Count: " << strategy.getTrueCount() << std::endl;
    
    // True count = -6 / 2.0 = -3.0
    assert(strategy.getRunningCount() == -6);
    assert(strategy.getTrueCount() == -3.0);
    
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST: Bet Sizing Based on True Count
// ----------------------------------------------------------------
void testBetSizing() {
    std::cout << "\n--- Running testBetSizing ---" << std::endl;
    
    // Test various true count thresholds
    HiLoStrategy strategy1(1.0);
    // No cards dealt, true count = 0 -> bet = 25
    std::cout << "True count 0, bet size: " << strategy1.getBetSize() << std::endl;
    assert(strategy1.getBetSize() == 25);
    
    // True count = 1 (add 1 low card)
    HiLoStrategy strategy2(1.0);
    strategy2.updateCount(Card(Rank::Five, Suit::Hearts));
    std::cout << "True count 1, bet size: " << strategy2.getBetSize() << std::endl;
    assert(strategy2.getBetSize() == 25);
    
    // True count = 2 (add 2 low cards)
    HiLoStrategy strategy3(1.0);
    strategy3.updateCount(Card(Rank::Five, Suit::Hearts));
    strategy3.updateCount(Card(Rank::Five, Suit::Clubs));
    std::cout << "True count 2, bet size: " << strategy3.getBetSize() << std::endl;
    assert(strategy3.getBetSize() == 25);
    
    // True count = 3 (add 3 low cards)
    HiLoStrategy strategy4(1.0);
    for (int i = 0; i < 3; i++) {
        strategy4.updateCount(Card(Rank::Five, Suit::Hearts));
    }
    std::cout << "True count 3, bet size: " << strategy4.getBetSize() << std::endl;
    assert(strategy4.getBetSize() == 50);
    
    // True count = 4 (add 4 low cards)
    HiLoStrategy strategy5(1.0);
    for (int i = 0; i < 4; i++) {
        strategy5.updateCount(Card(Rank::Five, Suit::Hearts));
    }
    std::cout << "True count 4, bet size: " << strategy5.getBetSize() << std::endl;
    assert(strategy5.getBetSize() == 75);
    
    // True count = 5 (add 5 low cards)
    HiLoStrategy strategy6(1.0);
    for (int i = 0; i < 5; i++) {
        strategy6.updateCount(Card(Rank::Five, Suit::Hearts));
    }
    std::cout << "True count 5, bet size: " << strategy6.getBetSize() << std::endl;
    assert(strategy6.getBetSize() == 100);
    
    // True count = 7 (add 7 low cards) -> max bet
    HiLoStrategy strategy7(1.0);
    for (int i = 0; i < 7; i++) {
        strategy7.updateCount(Card(Rank::Five, Suit::Hearts));
    }
    std::cout << "True count 7, bet size: " << strategy7.getBetSize() << std::endl;
    assert(strategy7.getBetSize() == 150);
    
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST: Insurance Decision Based on True Count
// Insurance is profitable at true count >= 3
// ----------------------------------------------------------------
void testInsuranceDecision() {
    std::cout << "\n--- Running testInsuranceDecision ---" << std::endl;
    
    // True count = 0 -> no insurance
    HiLoStrategy strategy1(1.0);
    std::cout << "True count 0, insurance: " << strategy1.shouldAcceptInsurance() << std::endl;
    assert(strategy1.shouldAcceptInsurance() == false);
    
    // True count = 2 -> no insurance
    HiLoStrategy strategy2(1.0);
    strategy2.updateCount(Card(Rank::Five, Suit::Hearts));
    strategy2.updateCount(Card(Rank::Five, Suit::Clubs));
    std::cout << "True count 2, insurance: " << strategy2.shouldAcceptInsurance() << std::endl;
    assert(strategy2.shouldAcceptInsurance() == false);
    
    // True count = 3 -> take insurance
    HiLoStrategy strategy3(1.0);
    for (int i = 0; i < 3; i++) {
        strategy3.updateCount(Card(Rank::Five, Suit::Hearts));
    }
    std::cout << "True count 3, insurance: " << strategy3.shouldAcceptInsurance() << std::endl;
    assert(strategy3.shouldAcceptInsurance() == true);
    
    // True count = 5 -> take insurance
    HiLoStrategy strategy4(1.0);
    for (int i = 0; i < 5; i++) {
        strategy4.updateCount(Card(Rank::Five, Suit::Hearts));
    }
    std::cout << "True count 5, insurance: " << strategy4.shouldAcceptInsurance() << std::endl;
    assert(strategy4.shouldAcceptInsurance() == true);
    
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST: Full Deck Balance (All cards in deck sum to 0)
// A balanced count system should sum to 0 over a full deck
// ----------------------------------------------------------------
void testFullDeckBalance() {
    std::cout << "\n--- Running testFullDeckBalance ---" << std::endl;
    
    HiLoStrategy strategy(1.0);
    
    // In one deck: 4 each of 2-A (13 ranks * 4 suits = 52 cards)
    // Low cards (2-6): 5 ranks * 4 = 20 cards -> +20
    // Neutral (7-9): 3 ranks * 4 = 12 cards -> 0
    // High (10-A): 5 ranks * 4 = 20 cards -> -20
    // Total: +20 - 20 = 0
    
    Suit suits[] = {Suit::Hearts, Suit::Diamonds, Suit::Clubs, Suit::Spades};
    Rank ranks[] = {Rank::Two, Rank::Three, Rank::Four, Rank::Five, Rank::Six,
                    Rank::Seven, Rank::Eight, Rank::Nine,
                    Rank::Ten, Rank::Jack, Rank::Queen, Rank::King, Rank::Ace};
    
    for (Rank rank : ranks) {
        for (Suit suit : suits) {
            strategy.updateCount(Card(rank, suit));
        }
    }
    
    std::cout << "Running count after full deck: " << strategy.getRunningCount() << std::endl;
    assert(strategy.getRunningCount() == 0);
    
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST: Deck seed reproducibility for deterministic simulations
// ----------------------------------------------------------------
void testDeckSeedReproducibility() {
    std::cout << "\n--- Running testDeckSeedReproducibility ---" << std::endl;

    auto drawSequence = [](std::uint32_t seed, int draws) {
        Deck::setSeed(seed);
        Deck deck(1);
        std::vector<std::pair<Rank, Suit>> seq;
        seq.reserve(draws);
        for (int i = 0; i < draws; ++i) {
            Card c = deck.hit();
            seq.emplace_back(c.getRank(), c.getSuit());
        }
        return seq;
    };

    const auto seqA = drawSequence(12345u, 10);
    const auto seqB = drawSequence(12345u, 10);
    const auto seqC = drawSequence(67890u, 10);

    assert(seqA == seqB);
    assert(seqA != seqC);

    Deck::clearSeed();
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST: Rigged three-hand shoe, no hits, verify final running/true count
// Each hand: dealer up Ten (-1), dealer hole Seven (0), player 10 (-1), player 10 (-1)
// Three hands total running = -9; plenty of neutral filler so decksLeft rounds to 1.0
// ----------------------------------------------------------------
void testRiggedThreeHandFinalCount() {
    std::cout << "\n--- Running testRiggedThreeHandFinalCount ---" << std::endl;

    std::vector<Card> stack;
    // 52 neutral cards (7/8/9 mix) to keep decksLeft stable at ~1.0
    for (int i = 0; i < 52; ++i) {
        Rank r = (i % 3 == 0) ? Rank::Seven : (i % 3 == 1 ? Rank::Eight : Rank::Nine);
        stack.emplace_back(r, Suit::Hearts);
    }

    auto append_hand = [&](Rank up, Rank hole, Rank p1, Rank p2, Suit s0, Suit s1, Suit s2, Suit s3) {
        stack.emplace_back(p2, s3);   // player card 2 (last popped among player)
        stack.emplace_back(p1, s2);   // player card 1
        stack.emplace_back(hole, s1); // dealer hole
        stack.emplace_back(up, s0);   // dealer upcard (first popped this hand)
    };

    append_hand(Rank::Ten, Rank::Seven, Rank::Ten, Rank::Ten, Suit::Clubs, Suit::Diamonds, Suit::Spades, Suit::Hearts);
    append_hand(Rank::Ten, Rank::Seven, Rank::Ten, Rank::Ten, Suit::Hearts, Suit::Spades, Suit::Diamonds, Suit::Clubs);
    append_hand(Rank::Ten, Rank::Seven, Rank::Ten, Rank::Ten, Suit::Spades, Suit::Clubs, Suit::Hearts, Suit::Diamonds);

    RiggedRunResult result = runRiggedDeckWithLogging(stack, /*numDecks=*/1);

    std::cout << "Wallet: " << result.wallet << " Running: " << result.runningCount << " True: " << result.trueCount << " Decks: " << result.decksLeft << " StatsEvents: " << result.statsEvents << std::endl;

    assert(result.statsEvents >= 1);
    assert(result.runningCount == -9.0f);
    assert(result.decksLeft > 0.0f);
    assert(approxEqual(result.trueCount, result.runningCount / result.decksLeft));

    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST: Rigged four-hand shoe, all stand hands, verify final running count
// Each hand: dealer up King (-1), hole Seven (0) => dealer stands on 17; player 10,10 (-2)
// Running per hand = -3, four hands = -12
// ----------------------------------------------------------------
void testRiggedFourHandFinalCount() {
    std::cout << "\n--- Running testRiggedFourHandFinalCount ---" << std::endl;

    std::vector<Card> stack;
    // 52 neutral fillers to keep decksLeft near 1.0 throughout
    for (int i = 0; i < 52; ++i) {
        Rank r = (i % 3 == 0) ? Rank::Seven : (i % 3 == 1 ? Rank::Eight : Rank::Nine);
        stack.emplace_back(r, Suit::Clubs);
    }

    auto append_hand = [&](Rank up, Rank hole, Rank p1, Rank p2, Suit s0, Suit s1, Suit s2, Suit s3) {
        stack.emplace_back(p2, s3);
        stack.emplace_back(p1, s2);
        stack.emplace_back(hole, s1);
        stack.emplace_back(up, s0);
    };

    append_hand(Rank::King, Rank::Seven, Rank::Ten, Rank::Ten, Suit::Clubs, Suit::Hearts, Suit::Spades, Suit::Diamonds); // -3
    append_hand(Rank::King, Rank::Seven, Rank::Ten, Rank::Ten, Suit::Hearts, Suit::Spades, Suit::Diamonds, Suit::Clubs); // -3
    append_hand(Rank::King, Rank::Seven, Rank::Ten, Rank::Ten, Suit::Spades, Suit::Clubs, Suit::Hearts, Suit::Diamonds); // -3
    append_hand(Rank::King, Rank::Seven, Rank::Ten, Rank::Ten, Suit::Diamonds, Suit::Clubs, Suit::Spades, Suit::Hearts); // -3

    RiggedRunResult result = runRiggedDeckWithLogging(stack, /*numDecks=*/1);

    float expectedRunning = -12.0f;

    std::cout << "Wallet: " << result.wallet << " Running: " << result.runningCount << " True: " << result.trueCount << " Decks: " << result.decksLeft << " StatsEvents: " << result.statsEvents << std::endl;

    assert(result.statsEvents >= 1);
    assert(result.runningCount == expectedRunning);
    assert(result.decksLeft > 0.0f);
    assert(approxEqual(result.trueCount, result.runningCount / result.decksLeft));

    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST: Rigged five-hand shoe ending strongly positive
// Each hand: dealer up 5 (+1), hole 6 (+1), player 2 (+1), player 3 (+1) => +4 per hand, five hands => +20
// Penetration config stops once 52 cards remain, so exactly five hands are played.
// ----------------------------------------------------------------
void testRiggedFiveHandPositiveCount() {
    std::cout << "\n--- Running testRiggedFiveHandPositiveCount ---" << std::endl;

    std::vector<Card> stack;
    // 52 neutral fillers (7/8/9) keep decksLeft ~1.0 after five hands
    for (int i = 0; i < 52; ++i) {
        Rank r = (i % 3 == 0) ? Rank::Seven : (i % 3 == 1 ? Rank::Eight : Rank::Nine);
        stack.emplace_back(r, Suit::Diamonds);
    }

    auto append_hand = [&](Rank up, Rank hole, Rank p1, Rank p2, Suit s0, Suit s1, Suit s2, Suit s3) {
        stack.emplace_back(p2, s3);
        stack.emplace_back(p1, s2);
        stack.emplace_back(hole, s1);
        stack.emplace_back(up, s0);
    };

    append_hand(Rank::Five, Rank::Six, Rank::Two, Rank::Three, Suit::Clubs, Suit::Hearts, Suit::Spades, Suit::Diamonds);   // +4
    append_hand(Rank::Five, Rank::Six, Rank::Two, Rank::Three, Suit::Hearts, Suit::Spades, Suit::Diamonds, Suit::Clubs);   // +4
    append_hand(Rank::Five, Rank::Six, Rank::Two, Rank::Three, Suit::Spades, Suit::Clubs, Suit::Hearts, Suit::Diamonds);   // +4
    append_hand(Rank::Five, Rank::Six, Rank::Two, Rank::Three, Suit::Diamonds, Suit::Clubs, Suit::Spades, Suit::Hearts);   // +4
    append_hand(Rank::Five, Rank::Six, Rank::Two, Rank::Three, Suit::Clubs, Suit::Diamonds, Suit::Hearts, Suit::Spades);   // +4

    RiggedRunResult result = runRiggedDeckWithLogging(stack, /*numDecks=*/1);

    float expectedRunning = 20.0f;

    std::cout << "Wallet: " << result.wallet << " Running: " << result.runningCount << " True: " << result.trueCount << " Decks: " << result.decksLeft << " StatsEvents: " << result.statsEvents << std::endl;

    assert(result.statsEvents >= 1);
    assert(result.runningCount == expectedRunning);
    assert(result.decksLeft > 0.0f);
    assert(approxEqual(result.trueCount, result.runningCount / result.decksLeft));

    std::cout << "PASSED" << std::endl;
}

void testRiggedFiveDifHandPositiveCount() {
    std::cout << "\n--- Running testRiggedFiveHandPositiveCount ---" << std::endl;

    std::vector<Card> stack;
    // 52 neutral fillers (7/8/9) keep decksLeft ~1.0 after five hands
    for (int i = 0; i < 104; ++i) {
        Rank r = (i % 3 == 0) ? Rank::Seven : (i % 3 == 1 ? Rank::Eight : Rank::Nine);
        stack.emplace_back(r, Suit::Diamonds);
    }

    auto append_hand = [&](Rank up, Rank hole, Rank p1, Rank p2, Suit s0, Suit s1, Suit s2, Suit s3) {
        stack.emplace_back(p2, s3);
        stack.emplace_back(p1, s2);
        stack.emplace_back(hole, s1);
        stack.emplace_back(up, s0);
    };

    append_hand(Rank::Five, Rank::Six, Rank::Two, Rank::Three, Suit::Clubs, Suit::Hearts, Suit::Spades, Suit::Diamonds);   // +4
    append_hand(Rank::Five, Rank::Six, Rank::Two, Rank::Three, Suit::Hearts, Suit::Spades, Suit::Diamonds, Suit::Clubs);   // +4
    append_hand(Rank::Five, Rank::Six, Rank::Two, Rank::Three, Suit::Spades, Suit::Clubs, Suit::Hearts, Suit::Diamonds);   // +4
    append_hand(Rank::Five, Rank::Six, Rank::Two, Rank::Three, Suit::Diamonds, Suit::Clubs, Suit::Spades, Suit::Hearts);   // +4
    append_hand(Rank::Five, Rank::Six, Rank::Two, Rank::Three, Suit::Clubs, Suit::Diamonds, Suit::Hearts, Suit::Spades);   // +4

    append_hand(Rank::Ten, Rank::Ace, Rank::Five, Rank::Ace, Suit::Clubs, Suit::Hearts, Suit::Spades, Suit::Diamonds);   // -2

    append_hand(Rank::Ace, Rank::Ten, Rank::Queen, Rank::Three, Suit::Hearts, Suit::Spades, Suit::Diamonds, Suit::Clubs);   // -2

    RiggedRunResult result = runRiggedDeckWithLogging(stack, /*numDecks=*/2.5);

    float expectedRunning = 16.0f;
    std::cout << "Wallet: " << result.wallet << " Running: " << result.runningCount << " True: " << result.trueCount << " Decks: " << result.decksLeft << " StatsEvents: " << result.statsEvents << std::endl;

    assert(result.statsEvents >= 1);
    assert(result.runningCount == expectedRunning);
    assert(result.decksLeft > 0.0f);
    assert(approxEqual(result.trueCount, result.runningCount / result.decksLeft));

    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST: Dealer draws 5 cards to bust. Verify dynamic counting.
// Player stands on 20 (-2). Dealer (5) hits multiple small cards (+7).
// Final Running Count should be +5 for this single hand.
// ----------------------------------------------------------------
void testRiggedDealerDeepDraw() {
    std::cout << "\n--- Running testRiggedDealerDeepDraw ---" << std::endl;

    std::vector<Card> stack;
    // 52 neutral fillers
    for (int i = 0; i < 52; ++i) {
        Rank r = (i % 3 == 0) ? Rank::Seven : (i % 3 == 1 ? Rank::Eight : Rank::Nine);
        stack.emplace_back(r, Suit::Clubs);
    }

    // Manual push for the deep draw hand
    // Stack Order: Last In -> First Out. 
    // We need: Dealer Last Hit, ..., Dealer Hit 1, P2, P1, Hole, Up
    
    // Dealer Burst Card (6) -> +1
    stack.emplace_back(Rank::Six, Suit::Spades);
    // Dealer Hit 4 (2) -> +1
    stack.emplace_back(Rank::Two, Suit::Hearts);
    // Dealer Hit 3 (2) -> +1
    stack.emplace_back(Rank::Two, Suit::Diamonds);
    // Dealer Hit 2 (2) -> +1
    stack.emplace_back(Rank::Two, Suit::Clubs);
    // Dealer Hit 1 (2) -> +1
    stack.emplace_back(Rank::Two, Suit::Spades);

    // Initial Deal
    // Player Card 2 (Ten) -> -1
    stack.emplace_back(Rank::Ten, Suit::Hearts);
    // Player Card 1 (Ten) -> -1
    stack.emplace_back(Rank::Ten, Suit::Clubs);
    // Dealer Hole (Three) -> +1
    stack.emplace_back(Rank::Three, Suit::Diamonds);
    // Dealer Up (Two) -> +1
    stack.emplace_back(Rank::Two, Suit::Spades); 

    // Total Calculation:
    // Dealer Cards: 2, 3, 2, 2, 2, 2, 6 (All Low) = 7 cards * (+1) = +7
    // Player Cards: 10, 10 = 2 cards * (-1) = -2
    // Net: +5

    RiggedRunResult result = runRiggedDeckWithLogging(stack, /*numDecks=*/1);

    std::cout << "Running: " << result.runningCount << " True: " << result.trueCount << std::endl;

    assert(result.statsEvents >= 1);
    assert(result.runningCount == 5.0f);
    assert(result.decksLeft > 0.0f);
    assert(approxEqual(result.trueCount, result.runningCount / result.decksLeft));
    
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST: Player busts. Verify Dealer Hole card is still counted.
// Many engines fail here by ending the round before counting the hole card.
// ----------------------------------------------------------------
void testRiggedPlayerBustReveal() {
    std::cout << "\n--- Running testRiggedPlayerBustReveal ---" << std::endl;

    std::vector<Card> stack;
    for (int i = 0; i < 52; ++i) {
        stack.emplace_back(Rank::Eight, Suit::Clubs); // Neutral
    }

    // Stack: Hit(10), P2(6), P1(10), Hole(5), Up(10)
    stack.emplace_back(Rank::Ten, Suit::Spades);   // Player Hit (Bust) -> -1
    stack.emplace_back(Rank::Six, Suit::Hearts);   // Player 2 -> +1
    stack.emplace_back(Rank::Ten, Suit::Clubs);    // Player 1 -> -1
    stack.emplace_back(Rank::Five, Suit::Diamonds);// Dealer Hole -> +1
    stack.emplace_back(Rank::Ten, Suit::Spades);   // Dealer Up -> -1

    // Net: -1 + 1 - 1 + 1 - 1 = -1

    RiggedRunResult result = runRiggedDeckWithLogging(stack, /*numDecks=*/1);

    std::cout << "Running: " << result.runningCount << std::endl;

    // If result is -2, the engine counted the Upcard (-1) and Player Bust (-1)
    // but FORGOT the Dealer Hole (+1).
    assert(result.runningCount == -1.0f);
    
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST: Precision True Count at 0.5 Decks remaining
// We consume 26 cards total. 
// ----------------------------------------------------------------
void testRiggedHalfDeckPrecision() {
    std::cout << "\n--- Running testRiggedHalfDeckPrecision ---" << std::endl;

    std::vector<Card> stack;

    for (int i = 0; i < 26; ++i) {
        stack.emplace_back(Rank::Seven, Suit::Clubs);
    }

    // Top 22 cards (Played as "burn" or previous hands): Neutral
    for (int i = 0; i < 26; ++i) {
        stack.emplace_back(Rank::Eight, Suit::Diamonds);
    }


    stack.emplace_back(Rank::Two, Suit::Spades); // P2
    stack.emplace_back(Rank::Two, Suit::Hearts); // P1
    stack.emplace_back(Rank::Two, Suit::Clubs);  // Hole
    stack.emplace_back(Rank::Two, Suit::Diamonds);// Up
    
    RiggedRunResult result = runRiggedDeckWithLogging(stack, /*numDecks=*/1);

    std::cout << "Running: " << result.runningCount << " DecksLeft: " << result.decksLeft << " True: " << result.trueCount << std::endl;

    assert(result.runningCount == 4.0f);

    std::cout << "PASSED" << std::endl;
}

void testRiggedSplitHandPositiveCount() {
    std::cout << "\n--- Running testRiggedSplitHandPositiveCount ---" << std::endl;

    std::vector<Card> stack;
    // 52 neutral fillers (7/8/9) keep decksLeft ~1.0 after five hands
    for (int i = 0; i < 104; ++i) {
        Rank r = (i % 3 == 0) ? Rank::Seven : (i % 3 == 1 ? Rank::Eight : Rank::Nine);
        stack.emplace_back(r, Suit::Diamonds);
    }

    auto append_hand = [&](Rank up, Rank hole, Rank p1, Rank p2, Suit s0, Suit s1, Suit s2, Suit s3) {
        stack.emplace_back(p2, s3);
        stack.emplace_back(p1, s2);
        stack.emplace_back(hole, s1);
        stack.emplace_back(up, s0);
    };

    append_hand(Rank::Five, Rank::Six, Rank::Two, Rank::Three, Suit::Clubs, Suit::Hearts, Suit::Spades, Suit::Diamonds);   // +4
    append_hand(Rank::Five, Rank::Six, Rank::Two, Rank::Three, Suit::Hearts, Suit::Spades, Suit::Diamonds, Suit::Clubs);   // +4
    append_hand(Rank::Five, Rank::Six, Rank::Two, Rank::Three, Suit::Spades, Suit::Clubs, Suit::Hearts, Suit::Diamonds);   // +4
    append_hand(Rank::Five, Rank::Six, Rank::Two, Rank::Three, Suit::Diamonds, Suit::Clubs, Suit::Spades, Suit::Hearts);   // +4
    append_hand(Rank::Five, Rank::Six, Rank::Two, Rank::Three, Suit::Clubs, Suit::Diamonds, Suit::Hearts, Suit::Spades);   // +4

    append_hand(Rank::Ten, Rank::Ace, Rank::Five, Rank::Ace, Suit::Clubs, Suit::Hearts, Suit::Spades, Suit::Diamonds);   // -2

    append_hand(Rank::Ace, Rank::Ten, Rank::Queen, Rank::Three, Suit::Hearts, Suit::Spades, Suit::Diamonds, Suit::Clubs);   // -2
    
    stack.emplace_back(Rank::Queen, Suit::Diamonds); // Player Split Card -1
    stack.emplace_back(Rank::Queen, Suit::Hearts); // Player Split Card -1
    stack.emplace_back(Rank::Queen, Suit::Clubs);  // Player Split Card -1

    append_hand(Rank::Five, Rank::Five, Rank::Ace, Rank::Ace, Suit::Clubs, Suit::Hearts, Suit::Spades, Suit::Diamonds);   // 0


    RiggedRunResult result = runRiggedDeckWithLogging(stack, /*numDecks=*/2.5);

    float expectedRunning = 13.0f;
    float expectedTrue = expectedRunning / result.decksLeft;

    std::cout << "Wallet: " << result.wallet << " Running: " << result.runningCount << " True: " << result.trueCount << " Decks: " << result.decksLeft << " StatsEvents: " << result.statsEvents << std::endl;

    assert(result.statsEvents >= 1);
    assert(result.runningCount == expectedRunning);
    assert(result.decksLeft > 0.0f);
    assert(approxEqual(result.trueCount, expectedTrue));

    std::cout << "PASSED" << std::endl;
}

int main() {
    std::cout << "=== STARTING BLACKJACK TESTS ===" << std::endl;
    
    // Game mechanics tests
    std::cout << "\n=== GAME MECHANICS TESTS ===" << std::endl;
    testSplitAcesTwiceThenStandLogic();
    testNaturalBlackjackPayout();
    testSplitAcesOneCardLogic();
    testDoubleSoftHand();
    testDealerBustChain();
    testBlackjackPush();
    testInsuranceDeclinedDealerBlackjackLoss();
    testInsuranceDeclinedMutualBlackjacksPush();
    testInsuranceAcceptedDealerBlackjackPlayerNoBJ();
    testInsuranceAcceptedDealerBlackjackPlayerBlackjack();
    testInsuranceAcceptedDealerNoBJPlayerBlackjack();
    testInsuranceAcceptedDealerNoBJPlayerNoBJWin();
    testSplitAcesTwiceLogic();
    testDealerShowsTenHiddenAceBlackjack();

    //testDealerStandsOnSoft17();
    testPlayerBusts();
    testDoubleDownWin();
    testDoubleDownLoss();
    testSplitMixedResults();
    testSplitBothLose();
    testMultiHit21();
    testPush21NonBlackjack();
    testDoubleAfterSplitAllowed();

    testSplitAcesTenIsNotBlackjack();
    testDealerAceValuationChange();
    testPlayer21VsDealerBlackjack();

    // Hi-Lo counting strategy tests
    std::cout << "\n=== HI-LO COUNTING STRATEGY TESTS ===" << std::endl;
    testRunningCountLowCards();
    testRunningCountHighCards();
    testRunningCountNeutralCards();
    testTrueCountSingleDeck();
    testTrueCountMultipleDecks();
    testTrueCountHalfIncrements();
    testDeckSizeUpdateRounding();
    testTrueCountWithDeckDepletion();
    testMixedCardSequence();
    testNegativeTrueCount();
    testBetSizing();
    testInsuranceDecision();
    testFullDeckBalance();
    testDeckSeedReproducibility();
    testRiggedThreeHandFinalCount();
    testRiggedFourHandFinalCount();
    testRiggedFiveHandPositiveCount();
    testRiggedFiveDifHandPositiveCount();
    testRiggedDealerDeepDraw();
    testRiggedPlayerBustReveal();
    testRiggedSplitHandPositiveCount();
    
    std::cout << "\nAll tests passed successfully!" << std::endl;
    return 0;
}
