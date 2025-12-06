CXX = g++
# -std=c++17 fixes your "enum class" and initialization errors
CXXFLAGS = -std=c++17 -Wall -Wextra -g

# 1. COMMON_OBJECTS: Everything EXCEPT main.o and test.o
COMMON_OBJECTS = rank.o suit.o Card.o Hand.o Deck.o HiLoStrategy.o NoStrategy.o \
                 BasicStrategy.o action.o Engine.o EngineBuilder.o \
                 observers/EventBus.o observers/ConsoleObserver.o \
                 BotPlayer.o HumanPlayer.o Bankroll.o GameReporter.o

# 2. Default Target: Build both the game and the tests
all: blackjack test

# 3. Link the Game (main.o + Common)
blackjack: main.o $(COMMON_OBJECTS)
	$(CXX) $(CXXFLAGS) -o blackjack main.o $(COMMON_OBJECTS)

# 4. Link the Tests (test.o + Common)
test: test.o $(COMMON_OBJECTS)
	$(CXX) $(CXXFLAGS) -o run_tests test.o $(COMMON_OBJECTS)

# --- Compilation Rules ---

main.o: main.cpp Engine.h Deck.h Hand.h CountingStrategy.h NoStrategy.h HiLoStrategy.h Card.h rank.h suit.h action.h observers/EventBus.h observers/ConsoleObserver.h
	$(CXX) $(CXXFLAGS) -c main.cpp

# New rule for test.cpp
test.o: test.cpp Engine.h Deck.h Hand.h Card.h rank.h suit.h
	$(CXX) $(CXXFLAGS) -c test.cpp

rank.o: rank.cpp rank.h
	$(CXX) $(CXXFLAGS) -c rank.cpp

suit.o: suit.cpp suit.h
	$(CXX) $(CXXFLAGS) -c suit.cpp

Card.o: Card.cpp Card.h rank.h suit.h
	$(CXX) $(CXXFLAGS) -c Card.cpp

Hand.o: Hand.cpp Hand.h Card.h rank.h suit.h
	$(CXX) $(CXXFLAGS) -c Hand.cpp

Deck.o: Deck.cpp Deck.h Card.h
	$(CXX) $(CXXFLAGS) -c Deck.cpp

HiLoStrategy.o: HiLoStrategy.cpp HiLoStrategy.h Card.h rank.h suit.h CountingStrategy.h
	$(CXX) $(CXXFLAGS) -c HiLoStrategy.cpp

NoStrategy.o: NoStrategy.cpp NoStrategy.h Card.h rank.h suit.h CountingStrategy.h
	$(CXX) $(CXXFLAGS) -c NoStrategy.cpp

BasicStrategy.o: BasicStrategy.cpp BasicStrategy.h rank.h action.h
	$(CXX) $(CXXFLAGS) -c BasicStrategy.cpp

action.o: action.cpp action.h
	$(CXX) $(CXXFLAGS) -c action.cpp

Engine.o: Engine.cpp Engine.h Deck.h Hand.h CountingStrategy.h BasicStrategy.h action.h observers/EventBus.h observers/EventType.h
	$(CXX) $(CXXFLAGS) -c Engine.cpp

observers/EventBus.o: observers/EventBus.cpp observers/EventBus.h observers/EventIssuingObservable.h observers/EventObserver.h observers/EventType.h
	$(CXX) $(CXXFLAGS) -c observers/EventBus.cpp -o observers/EventBus.o

observers/ConsoleObserver.o: observers/ConsoleObserver.cpp observers/ConsoleObserver.h observers/EventObserver.h observers/EventType.h
	$(CXX) $(CXXFLAGS) -c observers/ConsoleObserver.cpp -o observers/ConsoleObserver.o

EngineBuilder.o: EngineBuilder.cpp EngineBuilder.h Engine.h CountingStrategy.h
	$(CXX) $(CXXFLAGS) -c EngineBuilder.cpp

BotPlayer.o: BotPlayer.cpp BotPlayer.h Player.h BasicStrategy.h
	$(CXX) $(CXXFLAGS) -c BotPlayer.cpp

HumanPlayer.o: HumanPlayer.cpp HumanPlayer.h Player.h action.h BasicStrategy.h
	$(CXX) $(CXXFLAGS) -c HumanPlayer.cpp

# 5. Clean up (Added run_tests and test.o)
clean:
	rm -f *.o observers/*.o blackjack run_tests
