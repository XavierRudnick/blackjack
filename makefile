CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

OBJECTS = main.o rank.o suit.o Card.o Hand.o Deck.o HiLoStrategy.o NoStrategy.o BasicStrategy.o action.o EngineBuilder.o \
	observers/EventBus.o observers/ConsoleObserver.o

blackjack: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o blackjack $(OBJECTS)

main.o: main.cpp Engine.h Deck.h Hand.h CountingStrategy.h NoStrategy.h HiLoStrategy.h Card.h rank.h suit.h action.h observers/EventBus.h observers/ConsoleObserver.h
	$(CXX) $(CXXFLAGS) -c main.cpp

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

observers/EventBus.o: observers/EventBus.cpp observers/EventBus.h observers/EventIssuingObservable.h observers/EventObserver.h observers/EventType.h
	$(CXX) $(CXXFLAGS) -c observers/EventBus.cpp -o observers/EventBus.o

observers/ConsoleObserver.o: observers/ConsoleObserver.cpp observers/ConsoleObserver.h observers/EventObserver.h observers/EventType.h
	$(CXX) $(CXXFLAGS) -c observers/ConsoleObserver.cpp -o observers/ConsoleObserver.o

EngineBuilder.o: EngineBuilder.cpp EngineBuilder.h Engine.h CountingStrategy.h
	$(CXX) $(CXXFLAGS) -c EngineBuilder.cpp

clean:
	rm -f *.o blackjack