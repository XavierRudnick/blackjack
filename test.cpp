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

// --- Helper for Setup ---
// Uses reverse order: {LastDealt, ..., Player2, Player1, DealerHole, DealerUp}
Engine setupEngine(std::vector<Card> stack, double initialWallet = 1000) {
    int num_decks_used = 2;
    const bool visualize = true; // Set to true to see logs in console

    static ConsoleObserver consoleObserver; // Static to persist
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

    // Use BasicStrategy to ensure predictable hits/stands
    auto strategy = std::make_unique<NoStrategy>(0); 
    auto player = std::make_unique<BotPlayer>(false); // Default bot player
    Deck deck = Deck(num_decks_used);
    Deck riggedDeck = deck.testDeck(stack);

    return EngineBuilder()
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

    // Order: Dealer Hit, Player Hit, P2, P1, D_Hole, D_Up
    std::vector<Card> stackedCards = {
        Card(Rank::Three, Suit::Clubs),  // 6. Dealer Hit (if needed)
        Card(Rank::Five, Suit::Hearts),  // 5. Player Hit (if needed)
        Card(Rank::Five, Suit::Clubs),   // 4. Player Card 2 (Total 15 w/ below)
        Card(Rank::King, Suit::Clubs),   // 3. Player Card 1 
        Card(Rank::Five, Suit::Hearts),  // 2. Dealer Hole
        Card(Rank::Ace, Suit::Spades),   // 1. Dealer Up (Ace)
    };
    
    // NOTE: The stack above is from your example. 
    // It actually results in Player 15 vs Dealer Ace.
    // Let's force a Blackjack:
    // P: Ace, King. D: 5, 5.
    
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

    // Scenario:
    // Dealer: 6 (Up), 5 (Hole) -> 11.
    // Player: Ace, Ace. -> Split.
    // Hand 1: Ace + King -> 21. (Stop).
    // Hand 2: Ace + Queen -> 21. (Stop).
    // Dealer plays: Draws 10 -> 21. Push. (Or Bust). Let's make Dealer Bust.
    // Dealer: 6 + 5 + 6(Draw) = 17? No, make bust. 6+5+10 = 21.
    // Let's make dealer bust: 6+5 = 11. Draw 10 = 21 (Push).
    // Draw King = 21 (Push). 
    // Let's make Dealer draw 5, then 10 -> 26 Bust.
    
    // Correct Vector for your engine (LIFO):
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

    // Bet 5 -> Split (5+5=10 risk).
    // Both Hands 21. Dealer 22 (Bust).
    // Win 5 on Hand 1, Win 5 on Hand 2. Total Profit 10.
    // Final Wallet: 1010.
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

    // Dealer: 6, 5 (11)
    // Player: A, 6 (17) -> Double
    // Player Draw: 4 -> 21.
    // Dealer Draw: 10 -> 21. (Push).
    
    // Stack (Bottom to Top / Draw Order):
    // D_Hit(10), P_Double_Card(4), P2(6), P1(A), D_Hole(5), D_Up(6)
    
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

    // Bet 5 -> Double -> Bet 10.
    // Player 21. Dealer 21. Push.
    // Wallet should be 1000.
    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 1000);
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST 4: Dealer Bust Chain
// ----------------------------------------------------------------
void testDealerBustChain() {
    std::cout << "\n--- Running testDealerBustChain ---" << std::endl;

    // P: 10, 10 (20). Stand.
    // D: 5, 4 (9). Hit(2) -> 11. Hit(5) -> 16. Hit(10) -> 26 Bust.
    
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

    // Bet 5. Win. Wallet 1005.
    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 1005);
    std::cout << "PASSED" << std::endl;
}

// ----------------------------------------------------------------
// TEST 5: Blackjack Push
// ----------------------------------------------------------------
void testBlackjackPush() {
    std::cout << "\n--- Running testBlackjackPush ---" << std::endl;

    // P: A, K (BJ)
    // D: A, Q (BJ)
    
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

    // Scenario:
    // Dealer: 6 (Up), 5 (Hole) -> 11.
    // Player: Ace, Ace. -> Split.
    // Hand 1: Ace + King -> 21. (Stop).
    // Hand 2: Ace + Queen -> 21. (Stop).
    // Dealer plays: Draws 10 -> 21. Push. (Or Bust). Let's make Dealer Bust.
    // Dealer: 6 + 5 + 6(Draw) = 17? No, make bust. 6+5+10 = 21.
    // Let's make dealer bust: 6+5 = 11. Draw 10 = 21 (Push).
    // Draw King = 21 (Push). 
    // Let's make Dealer draw 5, then 10 -> 26 Bust.
    
    // Correct Vector for your engine (LIFO):
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

    // Bet 5 -> Split (5+5=10 risk).
    // Both Hands 21. Dealer 22 (Bust).
    // Win 5 on Hand 1, Win 5 on Hand 2. Total Profit 10.
    // Final Wallet: 1010.
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