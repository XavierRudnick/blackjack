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
    assert(result.first == 995);
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
    assert(result.first == 995);
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
    assert(result.first == 995);
    std::cout << "PASSED" << std::endl;
}
// ----------------------------------------------------------------
// TEST 7: Dealer Stands on Soft 17 (S17 Rule)
// Scenario: Dealer has Ace + 6 (Soft 17). 
// Rule S17: Dealer must STAND. 
// Player has 20. Player Wins.
// ----------------------------------------------------------------
void testDealerStandsOnSoft17() {
    std::cout << "\n--- Running testDealerStandsOnSoft17 ---" << std::endl;

    std::vector<Card> stack = {
        // No Hit cards needed, Dealer stands immediately
        Card(Rank::King, Suit::Hearts),   // 4. Player 2 (Total 20)
        Card(Rank::Queen, Suit::Hearts),  // 3. Player 1
        Card(Rank::Ace, Suit::Clubs),     // 2. Dealer Hole (Soft 17)
        Card(Rank::Six, Suit::Spades)     // 1. Dealer Up
    };

    Engine engine = setupEngine(stack);
    auto result = engine.runner();

    // Player (20) > Dealer (17). Bet 5 -> Win 5.
    std::cout << "Final: " << result.first << std::endl;
    assert(result.first == 1005);
    std::cout << "PASSED" << std::endl;
}

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
    assert(result.first == 995);
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
    assert(result.first == 1010);
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
    assert(result.first == 990);
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
    assert(result.first == 1005);
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
    assert(result.first == 990);
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
    assert(result.first == 1005);
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
    assert(result.first == 1020);
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
    assert(result.first == 1010); 
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
    assert(result.first == 995);
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
    assert(result.first == 995);
    std::cout << "PASSED" << std::endl;
}

int main() {
    std::cout << "=== STARTING BLACKJACK TESTS ===" << std::endl;
    
    testSplitAcesTwiceThenStandLogic();
    testNaturalBlackjackPayout();
    testSplitAcesOneCardLogic();
    testDoubleSoftHand();
    testDealerBustChain();
    testBlackjackPush();
    testInsuranceDeclinedDealerBlackjackLoss();
    testInsuranceDeclinedMutualBlackjacksPush();
    testSplitAcesTwiceLogic();
    testDealerShowsTenHiddenAceBlackjack();

    testDealerStandsOnSoft17();
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

    std::cout << "\nAll tests passed successfully!" << std::endl;
    return 0;
}