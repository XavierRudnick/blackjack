CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
CPPFLAGS = -Iinclude -Iinclude/core -Iinclude/strategy -Iinclude/strategy/balanced -Iinclude/strategy/unbalanced -Iinclude/players -Iinclude/observers
OBJDIR = build

CORE_SOURCES = \
    src/core/rank.cpp \
    src/core/suit.cpp \
    src/core/action.cpp \
    src/core/Card.cpp \
    src/core/Hand.cpp \
    src/core/Deck.cpp \
    src/core/Engine.cpp \
    src/core/EngineBuilder.cpp \
    src/core/GameReporter.cpp \
    src/core/Bankroll.cpp \
    src/core/FixedEngine.cpp

STRATEGY_SOURCES = \
    $(wildcard src/strategy/*.cpp src/strategy/balanced/*.cpp src/strategy/unbalanced/*.cpp)

PLAYER_SOURCES = \
    src/players/BotPlayer.cpp \
    src/players/HumanPlayer.cpp

OBSERVER_SOURCES = \
    src/observers/EventBus.cpp \
    src/observers/ConsoleObserver.cpp

COMMON_SOURCES = $(CORE_SOURCES) $(STRATEGY_SOURCES) $(PLAYER_SOURCES) $(OBSERVER_SOURCES)
COMMON_OBJECTS = $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(COMMON_SOURCES))

BLACKJACK_SOURCES = src/main.cpp
BLACKJACK_OBJECTS = $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(BLACKJACK_SOURCES))

TEST_SOURCES = src/test.cpp
TEST_OBJECTS = $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(TEST_SOURCES))

all: blackjack test

blackjack: $(BLACKJACK_OBJECTS) $(COMMON_OBJECTS)
	$(CXX) $(CXXFLAGS) -o blackjack $(BLACKJACK_OBJECTS) $(COMMON_OBJECTS)

test: $(TEST_OBJECTS) $(COMMON_OBJECTS)
	$(CXX) $(CXXFLAGS) -o run_tests $(TEST_OBJECTS) $(COMMON_OBJECTS)

$(OBJDIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf blackjack run_tests $(OBJDIR)
	find . -name "*.o" -delete
