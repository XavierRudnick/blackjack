#include <cassert>
#include <iostream>
#include <cmath>
#include <memory>
#include <vector>

#include "FixedEngine.h"
#include "NoStrategy.h"
#include "BotPlayer.h"
#include "Deck.h"
#include "Hand.h"
#include "Card.h"
#include "action.h"

// Helper function to compare floating point values
bool approxEqual(double a, double b, double epsilon = 0.0001) {
    return std::abs(a - b) < epsilon;
}

// Test 1: Constructor and initialization
void testFixedEngineConstruction() {
    std::cout << "Running testFixedEngineConstruction... ";
    
    // Default constructor
    FixedEngine engine1;
    assert(engine1.getResults().empty());
    
    // Constructor with monte carlo actions
    std::vector<Action> actions = {Action::Hit, Action::Stand, Action::Double};
    FixedEngine engine2(actions);
    assert(engine2.getResults().empty());
    
    std::cout << "PASSED" << std::endl;
}

// Test 2: ActionStats basic operations
void testActionStats() {
    std::cout << "Running testActionStats... ";
    
    FixedEngine::ActionStats stats;
    assert(stats.totalPayout == 0.0);
    assert(stats.handsPlayed == 0);
    assert(stats.getEV() == 0.0);
    
    // Add positive result
    stats.addResult(1.0);
    assert(stats.handsPlayed == 1);
    assert(approxEqual(stats.totalPayout, 1.0));
    assert(approxEqual(stats.getEV(), 1.0));
    
    // Add negative result
    stats.addResult(-1.0);
    assert(stats.handsPlayed == 2);
    assert(approxEqual(stats.totalPayout, 0.0));
    assert(approxEqual(stats.getEV(), 0.0));
    
    // Add another result
    stats.addResult(0.5);
    assert(stats.handsPlayed == 3);
    assert(approxEqual(stats.totalPayout, 0.5));
    assert(approxEqual(stats.getEV(), 0.5 / 3.0));
    
    std::cout << "PASSED" << std::endl;
}

// Test 3: Calculate EV with simple scenario - player stands on 20
void testCalculateEVStandOnTwenty() {
    std::cout << "Running testCalculateEVStandOnTwenty... ";
    
    // Dealer has 16 (6+10), user has 10+10=20
    // Dealer will draw 10 to bust at 26
    // Deck only contains cards to be drawn during play
    std::vector<Card> stack = {
        Card(Rank::Ten, Suit::Hearts)     // Dealer will draw -> 26 (bust)
    };
    
    Deck deck = Deck::createTestDeck(stack);
    
    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));
    
    Hand dealer(Card(Rank::Six, Suit::Clubs), 10);
    dealer.addCard(Card(Rank::Ten, Suit::Diamonds));
    Hand user(std::make_pair(Card(Rank::Ten, Suit::Spades), Card(Rank::Ten, Suit::Hearts)), 10);
    
    // Update count with visible cards
    player.updateCount(Card(Rank::Six, Suit::Clubs));  // Dealer upcard
    player.updateCount(Card(Rank::Ten, Suit::Spades));  // User card 1
    player.updateCount(Card(Rank::Ten, Suit::Hearts));  // User card 2
    
    std::vector<Action> actions = {Action::Stand};
    FixedEngine engine(actions);
    
    float trueCount = 0.0f;
    engine.calculateEV(player, deck, dealer, user, trueCount);
    
    const auto& results = engine.getResults();
    assert(results.count(trueCount) == 1);
    
    const auto& decisionPoint = results.at(trueCount);
    assert(decisionPoint.standStats.handsPlayed == 1);
    assert(decisionPoint.hitStats.handsPlayed == 0);
    assert(decisionPoint.doubleStats.handsPlayed == 0);
    
    // Player has 20, dealer has 16 and will draw to bust
    // Expected: player wins (+1)
    if (!approxEqual(decisionPoint.standStats.getEV(), 1.0)) {
        std::cout << "FAILED - Expected 1.0, got " << decisionPoint.standStats.getEV() << std::endl;
    }
    assert(approxEqual(decisionPoint.standStats.getEV(), 1.0));
    
    std::cout << "PASSED" << std::endl;
}

// Test 4: Calculate EV for Hit action - player busts
void testCalculateEVHitAndBust() {
    std::cout << "Running testCalculateEVHitAndBust... ";
    
    // Player has 20 and will hit a 5 to bust
    // Cards dealt from BACK of vector, so reverse order
    std::vector<Card> stack = {
        Card(Rank::Five, Suit::Hearts)   // Hit card for player -> bust
    };
    
    Deck deck = Deck::createTestDeck(stack);
    
    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));
    
    Hand dealer(Card(Rank::Five, Suit::Clubs), 10);
    dealer.addCard(Card(Rank::Five, Suit::Diamonds));
    Hand user(std::make_pair(Card(Rank::Ten, Suit::Spades), Card(Rank::Ten, Suit::Hearts)), 10);
    
    // Update count with visible cards
    player.updateCount(Card(Rank::Five, Suit::Clubs));
    player.updateCount(Card(Rank::Ten, Suit::Spades));
    player.updateCount(Card(Rank::Ten, Suit::Hearts));
    
    std::vector<Action> actions = {Action::Hit};
    FixedEngine engine(actions);
    
    float trueCount = 0.0f;
    engine.calculateEV(player, deck, dealer, user, trueCount);
    
    const auto& results = engine.getResults();
    const auto& decisionPoint = results.at(trueCount);
    
    assert(decisionPoint.hitStats.handsPlayed == 1);
    // Player busts = -1
    assert(approxEqual(decisionPoint.hitStats.getEV(), -1.0));
    
    std::cout << "PASSED" << std::endl;
}

// Test 5: Calculate EV for Double action
void testCalculateEVDouble() {
    std::cout << "Running testCalculateEVDouble... ";
    
    // Player has 11, doubles and gets a 10 for 21
    // Dealer has 6, will draw and bust
    // Cards dealt from BACK of vector, so reverse order
    std::vector<Card> stack = {
        Card(Rank::Ten, Suit::Spades),    // 5. Dealer draws
        Card(Rank::Ten, Suit::Hearts),    // 4. Double card for player -> 21
        // Card(Rank::Five, Suit::Hearts),   // 3. Player card 2
        // Card(Rank::Six, Suit::Spades),    // 2. Player card 1
        // Card(Rank::Six, Suit::Diamonds),  // 1. Dealer hole card
        // Card(Rank::Six, Suit::Clubs)      // 0. Dealer up card
    };
    
    Deck deck = Deck::createTestDeck(stack);
    
    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));
    
    Hand dealer(std::make_pair(Card(Rank::Six, Suit::Clubs), Card(Rank::Six, Suit::Diamonds)),10);
    Hand user(std::make_pair(Card(Rank::Six, Suit::Spades), Card(Rank::Five, Suit::Hearts)), 10);
    
    // Update count with visible cards
    player.updateCount(Card(Rank::Six, Suit::Clubs));  // Dealer upcard
    player.updateCount(Card(Rank::Six, Suit::Spades)); // User card 1
    player.updateCount(Card(Rank::Five, Suit::Hearts)); // User card 2
    
    std::vector<Action> actions = {Action::Double};
    FixedEngine engine(actions);
    
    float trueCount = 0.0f;
    engine.calculateEV(player, deck, dealer, user, trueCount);
    
    const auto& results = engine.getResults();
    const auto& decisionPoint = results.at(trueCount);
    
    assert(decisionPoint.doubleStats.handsPlayed == 1);
    // Debug: print actual EV
    if (!approxEqual(decisionPoint.doubleStats.getEV(), 2.0)) {
        std::cout << "FAILED - Expected 2.0, got " << decisionPoint.doubleStats.getEV() << std::endl;
    }
    // Double down win = +2 (doubled bet)
    assert(approxEqual(decisionPoint.doubleStats.getEV(), 2.0));
    
    std::cout << "PASSED" << std::endl;
}

// Test 6: Multiple actions with different true counts
void testMultipleTrueCounts() {
    std::cout << "Running testMultipleTrueCounts... ";
    
    FixedEngine engine({Action::Hit, Action::Stand});
    
    auto strategy1 = std::make_unique<NoStrategy>(0);
    BotPlayer player1(false, std::move(strategy1));
    
    // Scenario 1: True count = 0 - Both Hit and Stand
    // For Hit action: player will hit and bust
    // For Stand action: dealer will draw to beat player or bust
    std::vector<Card> stack1 = {
        Card(Rank::Ten, Suit::Diamonds),  // Dealer draws (if Stand action)
        Card(Rank::Five, Suit::Hearts)    // Hit card -> bust at 25 OR dealer draws (if Stand)
    };
    Deck deck1 = Deck::createTestDeck(stack1);
    Hand dealer1(Card(Rank::Five, Suit::Clubs), 10);
    dealer1.addCard(Card(Rank::Five, Suit::Diamonds));
    Hand user1(std::make_pair(Card(Rank::Ten, Suit::Spades), Card(Rank::Ten, Suit::Hearts)), 10);
    
    player1.updateCount(Card(Rank::Five, Suit::Clubs));
    player1.updateCount(Card(Rank::Ten, Suit::Spades));
    player1.updateCount(Card(Rank::Ten, Suit::Hearts));
    
    engine.calculateEV(player1, deck1, dealer1, user1, 0.0f);
    
    auto strategy2 = std::make_unique<NoStrategy>(0);
    BotPlayer player2(false, std::move(strategy2));
    
    // Scenario 2: True count = 2 - Both Hit and Stand  
    std::vector<Card> stack2 = {
        Card(Rank::Ten, Suit::Clubs),   // Dealer draws (if Stand action)
        Card(Rank::Five, Suit::Spades)  // Hit card -> bust at 25 OR dealer draws (if Stand)
    };
    Deck deck2 = Deck::createTestDeck(stack2);
    Hand dealer2(Card(Rank::Five, Suit::Clubs), 10);
    dealer2.addCard(Card(Rank::Five, Suit::Diamonds));
    Hand user2(std::make_pair(Card(Rank::Ten, Suit::Spades), Card(Rank::Ten, Suit::Hearts)), 10);
    
    player2.updateCount(Card(Rank::Five, Suit::Clubs));
    player2.updateCount(Card(Rank::Ten, Suit::Spades));
    player2.updateCount(Card(Rank::Ten, Suit::Hearts));
    
    engine.calculateEV(player2, deck2, dealer2, user2, 2.0f);
    
    const auto& results = engine.getResults();
    assert(results.count(0.0f) == 1);
    assert(results.count(2.0f) == 1);
    assert(results.size() == 2);
    
    std::cout << "PASSED" << std::endl;
}

// Test 7: Merge functionality
void testMergeEngines() {
    std::cout << "Running testMergeEngines... ";
    
    FixedEngine engine1({Action::Hit, Action::Stand});
    FixedEngine engine2({Action::Hit, Action::Stand});
    
    // Manually populate engine1 results
    engine1.EVresults[0.0f].hitStats.addResult(1.0);
    engine1.EVresults[0.0f].hitStats.addResult(-1.0);
    engine1.EVresults[0.0f].standStats.addResult(0.5);
    
    // Manually populate engine2 results with same true count
    engine2.EVresults[0.0f].hitStats.addResult(1.0);
    engine2.EVresults[0.0f].standStats.addResult(0.5);
    engine2.EVresults[0.0f].standStats.addResult(-0.5);
    
    // Manually populate engine2 with different true count
    engine2.EVresults[1.0f].hitStats.addResult(2.0);
    
    engine1.merge(engine2);
    
    const auto& results = engine1.getResults();
    
    // Check true count 0.0
    const auto& point0 = results.at(0.0f);
    assert(point0.hitStats.handsPlayed == 3); // 2 from engine1 + 1 from engine2
    assert(approxEqual(point0.hitStats.totalPayout, 1.0)); // (1-1) + 1
    
    assert(point0.standStats.handsPlayed == 3); // 1 from engine1 + 2 from engine2
    assert(approxEqual(point0.standStats.totalPayout, 0.5)); // 0.5 + 0.5 - 0.5
    
    // Check true count 1.0 (only from engine2)
    const auto& point1 = results.at(1.0f);
    assert(point1.hitStats.handsPlayed == 1);
    assert(approxEqual(point1.hitStats.totalPayout, 2.0));
    
    std::cout << "PASSED" << std::endl;
}

// Test 8: Blackjack payout (1.5x)
void testBlackjackPayout() {
    std::cout << "Running testBlackjackPayout... ";
    
    // Player gets blackjack (Ace + Ten)
    std::vector<Card> stack = {}; // Blackjack, no cards drawn
    
    Deck deck = Deck::createTestDeck(stack);
    
    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));
    
    Hand dealer(Card(Rank::Five, Suit::Clubs), 10);
    dealer.addCard(Card(Rank::Five, Suit::Diamonds));
    Hand user(std::make_pair(Card(Rank::Ten, Suit::Spades), Card(Rank::Ace, Suit::Hearts)), 10);
    
    player.updateCount(Card(Rank::Five, Suit::Clubs));
    player.updateCount(Card(Rank::Ten, Suit::Spades));
    player.updateCount(Card(Rank::Ace, Suit::Hearts));
    
    std::vector<Action> actions = {Action::Stand};
    FixedEngine engine(actions);
    
    float trueCount = 0.0f;
    engine.calculateEV(player, deck, dealer, user, trueCount);
    
    const auto& results = engine.getResults();
    const auto& decisionPoint = results.at(trueCount);
    
    // Blackjack pays 1.5x
    assert(approxEqual(decisionPoint.standStats.getEV(), 1.5));
    
    std::cout << "PASSED" << std::endl;
}

// Test 9: Push scenario (tie)
void testPushScenario() {
    std::cout << "Running testPushScenario... ";
    
    // Both player and dealer have 20
    std::vector<Card> stack = {}; // Both stand on 20, no cards drawn
    
    Deck deck = Deck::createTestDeck(stack);
    
    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));
    
    Hand dealer(Card(Rank::King, Suit::Clubs), 10);
    dealer.addCard(Card(Rank::Queen, Suit::Diamonds));
    Hand user(std::make_pair(Card(Rank::Ten, Suit::Spades), Card(Rank::Ten, Suit::Hearts)), 10);
    
    player.updateCount(Card(Rank::King, Suit::Clubs));
    player.updateCount(Card(Rank::Ten, Suit::Spades));
    player.updateCount(Card(Rank::Ten, Suit::Hearts));
    
    std::vector<Action> actions = {Action::Stand};
    FixedEngine engine(actions);
    
    float trueCount = 0.0f;
    engine.calculateEV(player, deck, dealer, user, trueCount);
    
    const auto& results = engine.getResults();
    const auto& decisionPoint = results.at(trueCount);
    
    // Push = 0
    assert(approxEqual(decisionPoint.standStats.getEV(), 0.0));
    
    std::cout << "PASSED" << std::endl;
}

// Test 10: Dealer busts scenario
void testDealerBusts() {
    std::cout << "Running testDealerBusts... ";
    
    // Player stands on 19, dealer has 15 (5+10) and draws a 10 to bust (25)
    std::vector<Card> stack = {
        Card(Rank::Ten, Suit::Hearts)     // Dealer draws -> 25 (bust)
    };
    
    Deck deck = Deck::createTestDeck(stack);
    
    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));
    
    Hand dealer(Card(Rank::Five, Suit::Clubs), 10);
    dealer.addCard(Card(Rank::Ten, Suit::Diamonds));
    Hand user(std::make_pair(Card(Rank::Ten, Suit::Spades), Card(Rank::Nine, Suit::Hearts)), 10);
    
    player.updateCount(Card(Rank::Five, Suit::Clubs));
    player.updateCount(Card(Rank::Ten, Suit::Spades));
    player.updateCount(Card(Rank::Nine, Suit::Hearts));
    
    std::vector<Action> actions = {Action::Stand};
    FixedEngine engine(actions);
    
    float trueCount = 0.0f;
    engine.calculateEV(player, deck, dealer, user, trueCount);
    
    const auto& results = engine.getResults();
    const auto& decisionPoint = results.at(trueCount);
    
    // Dealer busts, player wins
    assert(approxEqual(decisionPoint.standStats.getEV(), 1.0));
    
    std::cout << "PASSED" << std::endl;
}

// Test 11: Player hits multiple times then stands
void testMultipleHitsBeforeStand() {
    std::cout << "Running testMultipleHitsBeforeStand... ";
    
    // Player has 12, hits to get to 19, then should stand (based on basic strategy)
    // Cards dealt from BACK of vector, so reverse order
    std::vector<Card> stack = {
        Card(Rank::Eight, Suit::Diamonds),  // 1. Dealer draws
        Card(Rank::Three, Suit::Hearts),   // 0. Hit card -> 19
        Card(Rank::Three, Suit::Diamonds),   // 2. Hit card -> 16
    };
    
    Deck deck = Deck::createTestDeck(stack);
    
    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));
    
    Hand dealer(Card(Rank::Ten, Suit::Clubs), 10);
    dealer.addCard(Card(Rank::Five, Suit::Diamonds));
    Hand user(std::make_pair(Card(Rank::Ten, Suit::Spades), Card(Rank::Two, Suit::Hearts)), 10);
    
    // Force first action to be Hit, subsequent actions follow player strategy
    std::vector<Action> actions = {Action::Hit};
    FixedEngine engine(actions);
    
    float trueCount = 2.0f;
    engine.calculateEV(player, deck, dealer, user, trueCount);
    
    const auto& results = engine.getResults();
    const auto& decisionPoint = results.at(trueCount);
    
    assert(decisionPoint.hitStats.handsPlayed == 1);
    // Player should win after hitting to 19
    if (!approxEqual(decisionPoint.hitStats.getEV(), 1.0)) {
        std::cout << "FAILED - Expected 1.0, got " << decisionPoint.hitStats.getEV() << std::endl;
    }
    assert(approxEqual(decisionPoint.hitStats.getEV(), 1.0));
    
    std::cout << "PASSED" << std::endl;
}

// Test 12: Accumulating results over multiple simulations
void testAccumulatingResults() {
    std::cout << "Running testAccumulatingResults... ";
    
    FixedEngine engine({Action::Stand});
    
    float trueCount = 0.0f;
    
    // Run multiple simulations with same true count
    for (int i = 0; i < 5; i++) {
        auto strategy = std::make_unique<NoStrategy>(0);
        BotPlayer player(false, std::move(strategy));
        
        std::vector<Card> stack = {
            Card(Rank::Ten, Suit::Hearts),    // 1. Dealer draws -> bust
            Card(Rank::Five, Suit::Diamonds)   // 0. Dealer draws
        };
        
        Deck deck = Deck::createTestDeck(stack);
        Hand dealer(Card(Rank::Ten, Suit::Clubs), 10);
        Hand user(std::make_pair(Card(Rank::Ten, Suit::Spades), Card(Rank::Ten, Suit::Hearts)), 10);
        
        player.updateCount(Card(Rank::Five, Suit::Clubs));
        player.updateCount(Card(Rank::Ten, Suit::Spades));
        player.updateCount(Card(Rank::Ten, Suit::Hearts));
        
        engine.calculateEV(player, deck, dealer, user, trueCount);
    }
    
    const auto& results = engine.getResults();
    const auto& decisionPoint = results.at(trueCount);
    
    assert(decisionPoint.standStats.handsPlayed == 5);
    // Each simulation should result in +1
    assert(approxEqual(decisionPoint.standStats.totalPayout, 5.0));
    assert(approxEqual(decisionPoint.standStats.getEV(), 1.0));
    
    std::cout << "PASSED" << std::endl;
}

// Test 13: Both blackjacks (push)
void testBothBlackjacks() {
    std::cout << "Running testBothBlackjacks... ";
    
    std::vector<Card> stack = {}; // Both have blackjack, no cards drawn
    
    Deck deck = Deck::createTestDeck(stack);
    
    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));
    
    Hand dealer(Card(Rank::Ace, Suit::Clubs), 10);
    dealer.addCard(Card(Rank::Ten, Suit::Diamonds));
    Hand user(std::make_pair(Card(Rank::Ten, Suit::Spades), Card(Rank::Ace, Suit::Hearts)), 10);
    
    player.updateCount(Card(Rank::Ace, Suit::Clubs));
    player.updateCount(Card(Rank::Ten, Suit::Spades));
    player.updateCount(Card(Rank::Ace, Suit::Hearts));
    
    std::vector<Action> actions = {Action::Stand};
    FixedEngine engine(actions);
    
    float trueCount = 0.0f;
    engine.calculateEV(player, deck, dealer, user, trueCount);
    
    const auto& results = engine.getResults();
    const auto& decisionPoint = results.at(trueCount);
    
    // Both blackjacks = push = 0
    // Note: According to the code, when both have blackjack, player gets 1.5 initially
    // but then evaluateHand compares and returns 0 for equal blackjacks
    // Actually, the code checks if player has blackjack AND dealer doesn't
    // So both blackjacks should push
    // Looking at the code more carefully, it returns 1.5 only if player has BJ and dealer doesn't
    // So this should be 0 (push)
    assert(approxEqual(decisionPoint.standStats.getEV(), 0.0));
    
    std::cout << "PASSED" << std::endl;
}

// Test 14: Double down loss scenario
void testDoubleDownLoss() {
    std::cout << "Running testDoubleDownLoss... ";
    
    // Player has 11, doubles and gets a 2 for 13
    // Dealer has 20
    std::vector<Card> stack = {
        Card(Rank::Two, Suit::Hearts)     // Double card for player -> 13
    };
    
    Deck deck = Deck::createTestDeck(stack);
    
    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));
    
    Hand dealer(Card(Rank::Ten, Suit::Clubs), 10);
    dealer.addCard(Card(Rank::Ten, Suit::Diamonds));
    Hand user(std::make_pair(Card(Rank::Six, Suit::Spades), Card(Rank::Five, Suit::Hearts)), 10);
    
    player.updateCount(Card(Rank::Ten, Suit::Clubs));
    player.updateCount(Card(Rank::Six, Suit::Spades));
    player.updateCount(Card(Rank::Five, Suit::Hearts));
    
    std::vector<Action> actions = {Action::Double};
    FixedEngine engine(actions);
    
    float trueCount = 0.0f;
    engine.calculateEV(player, deck, dealer, user, trueCount);
    
    const auto& results = engine.getResults();
    const auto& decisionPoint = results.at(trueCount);
    
    assert(decisionPoint.doubleStats.handsPlayed == 1);
    // Double down loss = -2 (doubled bet)
    assert(approxEqual(decisionPoint.doubleStats.getEV(), -2.0));
    
    std::cout << "PASSED" << std::endl;
}

// Test 15: Negative true count tracking
void testNegativeTrueCount() {
    std::cout << "Running testNegativeTrueCount... ";
    
    FixedEngine engine({Action::Stand});
    
    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));
    
    std::vector<Card> stack = {
        Card(Rank::Ten, Suit::Hearts),    // 1. Dealer draws -> bust
        Card(Rank::Ten, Suit::Diamonds)   // 0. Dealer draws
    };
    
    Deck deck = Deck::createTestDeck(stack);
    Hand dealer(Card(Rank::Five, Suit::Clubs), 10);
    dealer.addCard(Card(Rank::Five, Suit::Diamonds));
    Hand user(std::make_pair(Card(Rank::Ten, Suit::Spades), Card(Rank::Ten, Suit::Hearts)), 10);
    
    player.updateCount(Card(Rank::Five, Suit::Clubs));
    player.updateCount(Card(Rank::Ten, Suit::Spades));
    player.updateCount(Card(Rank::Ten, Suit::Hearts));
    
    float trueCount = -2.5f;
    engine.calculateEV(player, deck, dealer, user, trueCount);
    
    const auto& results = engine.getResults();
    assert(results.count(-2.5f) == 1);
    
    const auto& decisionPoint = results.at(-2.5f);
    assert(decisionPoint.standStats.handsPlayed == 1);
    
    std::cout << "PASSED" << std::endl;
}

// Test 16: Soft hand - Player has A+7 (soft 18)
void testSoftHand() {
    std::cout << "Running testSoftHand... ";
    
    // Player has A+7 (soft 18), dealer has 17
    std::vector<Card> stack = {};
    
    Deck deck = Deck::createTestDeck(stack);
    
    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));
    
    Hand dealer(Card(Rank::Seven, Suit::Clubs), 10);
    dealer.addCard(Card(Rank::Ten, Suit::Diamonds));
    Hand user(std::make_pair(Card(Rank::Ace, Suit::Spades), Card(Rank::Seven, Suit::Hearts)), 10);
    
    std::vector<Action> actions = {Action::Stand};
    FixedEngine engine(actions);
    
    engine.calculateEV(player, deck, dealer, user, 0.0f);
    
    const auto& results = engine.getResults();
    const auto& decisionPoint = results.at(0.0f);
    
    // Player has 18, dealer has 17 -> player wins
    assert(approxEqual(decisionPoint.standStats.getEV(), 1.0));
    
    std::cout << "PASSED" << std::endl;
}

// Test 17: Soft hand becomes hard - A+6 hits 10
void testSoftHandBecomesHard() {
    std::cout << "Running testSoftHandBecomesHard... ";
    
    // Player has A+6 (soft 17), hits and gets 10 -> hard 17
    std::vector<Card> stack = {
        Card(Rank::Ten, Suit::Hearts)  // Hit card -> hard 17
    };
    
    Deck deck = Deck::createTestDeck(stack);
    
    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));
    
    Hand dealer(Card(Rank::Seven, Suit::Clubs), 10);
    dealer.addCard(Card(Rank::Ten, Suit::Diamonds));
    Hand user(std::make_pair(Card(Rank::Ace, Suit::Spades), Card(Rank::Six, Suit::Hearts)), 10);
    
    std::vector<Action> actions = {Action::Hit};
    FixedEngine engine(actions);
    
    engine.calculateEV(player, deck, dealer, user, 0.0f);
    
    const auto& results = engine.getResults();
    const auto& decisionPoint = results.at(0.0f);
    
    // Both have 17 -> push
    assert(approxEqual(decisionPoint.hitStats.getEV(), 0.0));
    
    std::cout << "PASSED" << std::endl;
}

// Test 18: Dealer has soft 17 and stands
void testDealerSoft17() {
    std::cout << "Running testDealerSoft17... ";
    
    // Dealer has A+6 (soft 17), player has 18
    std::vector<Card> stack = {};
    
    Deck deck = Deck::createTestDeck(stack);
    
    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));
    
    Hand dealer(Card(Rank::Ace, Suit::Clubs), 10);
    dealer.addCard(Card(Rank::Six, Suit::Diamonds));
    Hand user(std::make_pair(Card(Rank::Ten, Suit::Spades), Card(Rank::Eight, Suit::Hearts)), 10);
    
    std::vector<Action> actions = {Action::Stand};
    FixedEngine engine(actions);
    
    engine.calculateEV(player, deck, dealer, user, 0.0f);
    
    const auto& results = engine.getResults();
    const auto& decisionPoint = results.at(0.0f);
    
    // Player 18 beats dealer soft 17
    assert(approxEqual(decisionPoint.standStats.getEV(), 1.0));
    
    std::cout << "PASSED" << std::endl;
}

// Test 19: Player gets 21 with 3 cards
void testThreeCard21() {
    std::cout << "Running testThreeCard21... ";
    
    // Player has 7+7, hits 7 for 21
    std::vector<Card> stack = {
        Card(Rank::Seven, Suit::Hearts)  // Hit -> 21
    };
    
    Deck deck = Deck::createTestDeck(stack);
    
    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));
    
    Hand dealer(Card(Rank::Ten, Suit::Clubs), 10);
    dealer.addCard(Card(Rank::Nine, Suit::Diamonds));
    Hand user(std::make_pair(Card(Rank::Seven, Suit::Spades), Card(Rank::Seven, Suit::Hearts)), 10);
    
    std::vector<Action> actions = {Action::Hit};
    FixedEngine engine(actions);
    
    engine.calculateEV(player, deck, dealer, user, 0.0f);
    
    const auto& results = engine.getResults();
    const auto& decisionPoint = results.at(0.0f);
    
    // Player 21 beats dealer 19
    assert(approxEqual(decisionPoint.hitStats.getEV(), 1.0));
    
    std::cout << "PASSED" << std::endl;
}

// Test 20: Dealer draws to exactly 21
void testDealerDrawsTo21() {
    std::cout << "Running testDealerDrawsTo21... ";
    
    // Dealer has 11, draws 10 for 21, player has 20
    std::vector<Card> stack = {
        Card(Rank::Ten, Suit::Hearts)  // Dealer draws -> 21
    };
    
    Deck deck = Deck::createTestDeck(stack);
    
    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));
    
    Hand dealer(Card(Rank::Five, Suit::Clubs), 10);
    dealer.addCard(Card(Rank::Six, Suit::Diamonds));
    Hand user(std::make_pair(Card(Rank::Ten, Suit::Spades), Card(Rank::Ten, Suit::Hearts)), 10);
    
    std::vector<Action> actions = {Action::Stand};
    FixedEngine engine(actions);
    
    engine.calculateEV(player, deck, dealer, user, 0.0f);
    
    const auto& results = engine.getResults();
    const auto& decisionPoint = results.at(0.0f);
    
    // Dealer 21 beats player 20
    assert(approxEqual(decisionPoint.standStats.getEV(), -1.0));
    
    std::cout << "PASSED" << std::endl;
}

// Test 21: Double down on soft hand (A+6 vs 5)
void testDoubleSoftHand() {
    std::cout << "Running testDoubleSoftHand... ";
    
    // Player A+6 (soft 17), doubles and gets 4 for 21
    std::vector<Card> stack = {
        Card(Rank::Ten, Suit::Spades),   // Dealer draws
        Card(Rank::Four, Suit::Hearts)   // Double card -> 21
    };
    
    Deck deck = Deck::createTestDeck(stack);
    
    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));
    
    Hand dealer(Card(Rank::Six, Suit::Clubs), 10);
    dealer.addCard(Card(Rank::Six, Suit::Diamonds));
    Hand user(std::make_pair(Card(Rank::Ace, Suit::Spades), Card(Rank::Six, Suit::Hearts)), 10);
    
    std::vector<Action> actions = {Action::Double};
    FixedEngine engine(actions);
    
    engine.calculateEV(player, deck, dealer, user, 0.0f);
    
    const auto& results = engine.getResults();
    const auto& decisionPoint = results.at(0.0f);
    
    // Player 21 beats dealer bust on double (+2)
    assert(approxEqual(decisionPoint.doubleStats.getEV(), 2.0));
    
    std::cout << "PASSED" << std::endl;
}

// Test 22: Player has 12 (stiff hand) vs dealer 2
void testStiffHand() {
    std::cout << "Running testStiffHand... ";
    
    // Player 12 stands, dealer has 2+10, draws 9 for 21
    std::vector<Card> stack = {
        Card(Rank::Nine, Suit::Hearts)  // Dealer draws -> 21
    };
    
    Deck deck = Deck::createTestDeck(stack);
    
    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));
    
    Hand dealer(Card(Rank::Two, Suit::Clubs), 10);
    dealer.addCard(Card(Rank::Ten, Suit::Diamonds));
    Hand user(std::make_pair(Card(Rank::Ten, Suit::Spades), Card(Rank::Two, Suit::Hearts)), 10);
    
    std::vector<Action> actions = {Action::Stand};
    FixedEngine engine(actions);
    
    engine.calculateEV(player, deck, dealer, user, 0.0f);
    
    const auto& results = engine.getResults();
    const auto& decisionPoint = results.at(0.0f);
    
    // Dealer 21 beats player 12
    assert(approxEqual(decisionPoint.standStats.getEV(), -1.0));
    
    std::cout << "PASSED" << std::endl;
}

// Test 23: Multiple simulations with Hit and Stand actions
void testMultipleActionsPerTrueCount() {
    std::cout << "Running testMultipleActionsPerTrueCount... ";
    
    FixedEngine engine({Action::Hit, Action::Split});
    
    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));
    
    // Deck for hit: player will bust
    std::vector<Card> stack = {
        Card(Rank::Four, Suit::Diamonds),  // Hit card -> bust
        Card(Rank::Ten, Suit::Hearts),  // Hit card -> bust
        Card(Rank::Five, Suit::Hearts),  // Hit card -> bust
        Card(Rank::Ten, Suit::Spades)  // Hit card -> bust
    };
    Deck deck = Deck::createTestDeck(stack);
    
    Hand dealer(Card(Rank::Seven, Suit::Clubs), 10);
    dealer.addCard(Card(Rank::Six, Suit::Diamonds));
    Hand user(std::make_pair(Card(Rank::Eight, Suit::Spades), Card(Rank::Eight, Suit::Hearts)), 10);
    
    engine.calculateEV(player, deck, dealer, user, 0.0f);
    
    const auto& results = engine.getResults();
    const auto& decisionPoint = results.at(0.0f);
    
    // Both actions should have been simulated
    assert(decisionPoint.hitStats.handsPlayed == 1);
    assert(decisionPoint.splitStats.handsPlayed == 2);
    // Hit should lose (bust), stand should win
    std::cout << "Split EV: " << decisionPoint.splitStats.getEV() << std::endl; 
    assert(approxEqual(decisionPoint.hitStats.getEV(), -1.0));
    assert(approxEqual(decisionPoint.splitStats.getEV(), 0));
    
    std::cout << "PASSED" << std::endl;
}

// Test 24: Split produces two winning hands (no doubles)
void testSplitTwoHandsWin() {
    std::cout << "Running testSplitTwoHandsWin... ";

    // Draw order (back first):
    // 1) First split hand gets 10 -> 18
    // 2) Second split hand gets 9 -> 17
    // 3) Dealer draws 10 and busts (16 -> 26)
    std::vector<Card> stack = {
        Card(Rank::Ten, Suit::Hearts),
        Card(Rank::Nine, Suit::Clubs),
        Card(Rank::Ten, Suit::Spades)
    };

    Deck deck = Deck::createTestDeck(stack);

    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));

    Hand dealer(std::make_pair(Card(Rank::Six, Suit::Clubs), Card(Rank::Ten, Suit::Diamonds)), 10);
    Hand user(std::make_pair(Card(Rank::Eight, Suit::Spades), Card(Rank::Eight, Suit::Hearts)), 10);

    std::vector<Action> actions = {Action::Split};
    FixedEngine engine(actions);

    float trueCount = 0.0f;
    engine.calculateEV(player, deck, dealer, user, trueCount);

    const auto& decisionPoint = engine.getResults().at(trueCount);

    assert(decisionPoint.splitStats.handsPlayed == 2);
    assert(approxEqual(decisionPoint.splitStats.getEV(), 1.0));

    std::cout << "PASSED" << std::endl;
}

// Test 25: Split when double-after-split is disallowed
void testSplitDoubleAfterSplitDisallowed() {
    std::cout << "Running testSplitDoubleAfterSplitDisallowed... ";

    // Draw order (back first):
    // 1) First split hand gets 7 -> 11 (would double)
    // 2) Second split hand gets 7 -> 11 (would double)
    // 3) First hand forced hit (no DAS) gets 9 -> 20
    // 4) Second hand forced hit (no DAS) gets 8 -> 19
    // 5) Dealer draws 10 and busts (16 -> 26)
    std::vector<Card> stack = {
        Card(Rank::Ten, Suit::Hearts),
        Card(Rank::Eight, Suit::Clubs),
        Card(Rank::Nine, Suit::Diamonds),
        Card(Rank::Seven, Suit::Spades),
        Card(Rank::Seven, Suit::Hearts)
    };

    Deck deck = Deck::createTestDeck(stack);

    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));

    Hand dealer(std::make_pair(Card(Rank::Six, Suit::Clubs), Card(Rank::Ten, Suit::Diamonds)), 10);
    Hand user(std::make_pair(Card(Rank::Four, Suit::Spades), Card(Rank::Four, Suit::Hearts)), 10);

    GameConfig cfg;
    cfg.doubleAfterSplitAllowed = false;
    std::vector<Action> actions = {Action::Split};
    FixedEngine engine(actions, cfg);

    float trueCount = 0.0f;
    engine.calculateEV(player, deck, dealer, user, trueCount);

    const auto& decisionPoint = engine.getResults().at(trueCount);

    assert(decisionPoint.splitStats.handsPlayed == 2);
    assert(approxEqual(decisionPoint.splitStats.getEV(), 1.0));
    assert(decisionPoint.doubleStats.handsPlayed == 0);

    std::cout << "PASSED" << std::endl;
}

// Test 26: Very high true count
void testVeryHighTrueCount() {
    std::cout << "Running testVeryHighTrueCount... ";
    
    FixedEngine engine({Action::Stand});
    
    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));
    
    std::vector<Card> stack = {
        Card(Rank::Ten, Suit::Hearts)  // Dealer draws -> bust
    };
    Deck deck = Deck::createTestDeck(stack);
    
    Hand dealer(Card(Rank::Five, Suit::Clubs), 10);
    dealer.addCard(Card(Rank::Five, Suit::Diamonds));
    Hand user(std::make_pair(Card(Rank::Ten, Suit::Spades), Card(Rank::Ten, Suit::Hearts)), 10);
    
    float trueCount = 10.5f;
    engine.calculateEV(player, deck, dealer, user, trueCount);
    
    const auto& results = engine.getResults();
    assert(results.count(10.5f) == 1);

    const auto& decisionPoint = engine.getResults().at(trueCount);
    //std::cout << "Stand EV: " << decisionPoint.standStats << std::endl;
    std::cout << "PASSED" << std::endl;
}

// Test 27: Dealer has Ace upcard (no blackjack)
void testDealerAceUpcard() {
    std::cout << "Running testDealerAceUpcard... ";
    
    // Dealer has A+6 (soft 17), player has 18
    std::vector<Card> stack = {};
    
    Deck deck = Deck::createTestDeck(stack);
    
    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));
    
    Hand dealer(Card(Rank::Ace, Suit::Clubs), 10);
    dealer.addCard(Card(Rank::Six, Suit::Diamonds));
    Hand user(std::make_pair(Card(Rank::Nine, Suit::Spades), Card(Rank::Nine, Suit::Hearts)), 10);
    
    std::vector<Action> actions = {Action::Stand};
    FixedEngine engine(actions);
    
    engine.calculateEV(player, deck, dealer, user, 0.0f);
    
    const auto& results = engine.getResults();
    const auto& decisionPoint = results.at(0.0f);
    
    // Player 18 beats dealer soft 17
    assert(approxEqual(decisionPoint.standStats.getEV(), 1.0));
    
    std::cout << "PASSED" << std::endl;
}

// Test 28: Player doubles on 10 vs dealer 9
void testDoubleOn10() {
    std::cout << "Running testDoubleOn10... ";
    
    // Player 10, doubles and gets Ace for 21
    std::vector<Card> stack = {
        Card(Rank::Ten, Suit::Hearts),   // Dealer draws -> 19
        Card(Rank::Ace, Suit::Hearts)    // Double card -> 21
    };
    
    Deck deck = Deck::createTestDeck(stack);
    
    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));
    
    Hand dealer(Card(Rank::Nine, Suit::Clubs), 10);
    dealer.addCard(Card(Rank::Ten, Suit::Diamonds));
    Hand user(std::make_pair(Card(Rank::Six, Suit::Spades), Card(Rank::Four, Suit::Hearts)), 10);
    
    std::vector<Action> actions = {Action::Double};
    FixedEngine engine(actions);
    
    engine.calculateEV(player, deck, dealer, user, 0.0f);
    
    const auto& results = engine.getResults();
    const auto& decisionPoint = results.at(0.0f);
    
    // Player 21 beats dealer 19 on double (+2)
    assert(approxEqual(decisionPoint.doubleStats.getEV(), 2.0));
    
    std::cout << "PASSED" << std::endl;
}

// Test 29: Player has pair of 5s (treated as 10)
void testPairOfFives() {
    std::cout << "Running testPairOfFives... ";
    
    // Player 5+5=10, stands
    std::vector<Card> stack = {
        Card(Rank::Ten, Suit::Hearts)  // Dealer draws -> bust
    };
    
    Deck deck = Deck::createTestDeck(stack);
    
    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));
    
    Hand dealer(Card(Rank::Six, Suit::Clubs), 10);
    dealer.addCard(Card(Rank::Six, Suit::Diamonds));
    Hand user(std::make_pair(Card(Rank::Five, Suit::Spades), Card(Rank::Five, Suit::Hearts)), 10);
    
    std::vector<Action> actions = {Action::Stand};
    FixedEngine engine(actions);
    
    engine.calculateEV(player, deck, dealer, user, 0.0f);
    
    const auto& results = engine.getResults();
    const auto& decisionPoint = results.at(0.0f);
    
    // Player loses with 10 vs dealer bust... wait dealer should bust
    assert(approxEqual(decisionPoint.standStats.getEV(), 1.0));
    
    std::cout << "PASSED" << std::endl;
}

// Test 30: Dealer multiple card draw to 17
void testDealerMultipleDraws() {
    std::cout << "Running testDealerMultipleDraws... ";
    
    // Dealer has 6+2=8, draws 4, 5 for 17
    std::vector<Card> stack = {
        Card(Rank::Five, Suit::Hearts),   // Dealer draw 2 -> 17
        Card(Rank::Four, Suit::Diamonds)  // Dealer draw 1 -> 12
    };
    
    Deck deck = Deck::createTestDeck(stack);
    
    auto strategy = std::make_unique<NoStrategy>(0);
    BotPlayer player(false, std::move(strategy));
    
    Hand dealer(Card(Rank::Six, Suit::Clubs), 10);
    dealer.addCard(Card(Rank::Two, Suit::Diamonds));
    Hand user(std::make_pair(Card(Rank::Ten, Suit::Spades), Card(Rank::Eight, Suit::Hearts)), 10);
    
    std::vector<Action> actions = {Action::Stand};
    FixedEngine engine(actions);
    
    engine.calculateEV(player, deck, dealer, user, 0.0f);
    
    const auto& results = engine.getResults();
    const auto& decisionPoint = results.at(0.0f);
    
    // Player 18 beats dealer 17
    assert(approxEqual(decisionPoint.standStats.getEV(), 1.0));
    
    std::cout << "PASSED" << std::endl;
}

int main() {
    std::cout << "\n=== FIXED ENGINE TESTS ===" << std::endl;
    
    // Basic structure tests
    testFixedEngineConstruction();
    testActionStats();
    
    // Core functionality tests
    testCalculateEVStandOnTwenty();
    testCalculateEVHitAndBust();
    testCalculateEVDouble();
    
    // True count and merging tests
    testMultipleTrueCounts();
    testMergeEngines();
    testNegativeTrueCount();
    testVeryHighTrueCount();
    
    // Specific game scenario tests
    testBlackjackPayout();
    testPushScenario();
    testDealerBusts();
    testBothBlackjacks();
    
    // Complex simulation tests
    testMultipleHitsBeforeStand();
    testAccumulatingResults();
    testDoubleDownLoss();
    
    // Edge case tests
    testSoftHand();
    testSoftHandBecomesHard();
    testDealerSoft17();
    testThreeCard21();
    testDealerDrawsTo21();
    testDoubleSoftHand();
    testStiffHand();
    testMultipleActionsPerTrueCount();
    testSplitTwoHandsWin();
    testSplitDoubleAfterSplitDisallowed();
    testDealerAceUpcard();
    testDoubleOn10();
    testPairOfFives();
    testDealerMultipleDraws();
    
    std::cout << "\nAll FixedEngine tests passed successfully!" << std::endl;
    return 0;
}
