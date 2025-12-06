#include <vector>
#include <cassert>
#include <iostream>
#include <memory>
#include <cmath>

#include "Engine.h"
#include "HiLoStrategy.h"
#include "NoStrategy.h"
#include "BasicStrategy.h"
#include "observers/ConsoleObserver.h"
#include "observers/EventBus.h"
#include "EngineBuilder.h"
#include "BotPlayer.h"

Engine setupEngine(std::vector<Card> stack, double initialWallet = 1000) {
    const bool visualize = true; 

    static ConsoleObserver consoleObserver; 
    auto* bus = EventBus::getInstance();
    bus->detachAll(); // Clear previous observers
    
    if (visualize) {
        bus->registerObserver(&consoleObserver, {
            EventType::CardsDealt, 
            EventType::ActionTaken, 
            EventType::RoundEnded, 
            EventType::GameStats
        });
    }

    auto strategy = std::make_unique<NoStrategy>(0); 
    auto player = std::make_unique<BotPlayer>(false);

    Deck riggedDeck = Deck::createTestDeck(stack);

    return EngineBuilder()
            .withEventBus(bus)
            .setDeckSize(0)
            .setDeck(riggedDeck)
            .setInitialWallet(initialWallet)
            .enableEvents(true)
            .with3To2Payout()
            .withS17Rules()
            .allowDoubleAfterSplit()
            .build(std::move(strategy), std::move(player));
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
    assert(result.first == 1007.5); 
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
    assert(result.first == 1010);
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
    assert(result.first == 1005);
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
    assert(result.first == 1015);
    std::cout << "PASSED" << std::endl;
}


int main() {
    std::cout << "=== STARTING BLACKJACK TESTS ===" << std::endl;
    
    testNaturalBlackjackPayout();
    testSplitAcesOneCardLogic();
    testDoubleSoftHand();
    testDealerBustChain();
    testBlackjackPush();
    testSplitAcesTwiceLogic();

    std::cout << "\nAll tests passed successfully!" << std::endl;
    return 0;
}