CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

OBJECTS = main.o rank.o suit.o Card.o Hand.o HiLoStrategy.o NoStrategy.o BasicStrategy.o action.o

blackjack: $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o blackjack $(OBJECTS)

main.o: main.cpp Engine.h Deck.h Hand.h HiLoStrategy.h NoStrategy.h Card.h rank.h suit.h action.h
	$(CXX) $(CXXFLAGS) -c main.cpp

rank.o: rank.cpp rank.h
	$(CXX) $(CXXFLAGS) -c rank.cpp

suit.o: suit.cpp suit.h
	$(CXX) $(CXXFLAGS) -c suit.cpp

Card.o: Card.cpp Card.h rank.h suit.h
	$(CXX) $(CXXFLAGS) -c Card.cpp

Hand.o: Hand.cpp Hand.h Card.h rank.h suit.h
	$(CXX) $(CXXFLAGS) -c Hand.cpp

HiLoStrategy.o: HiLoStrategy.cpp HiLoStrategy.h Card.h rank.h suit.h
	$(CXX) $(CXXFLAGS) -c HiLoStrategy.cpp

NoStrategy.o: NoStrategy.cpp NoStrategy.h Card.h rank.h suit.h
	$(CXX) $(CXXFLAGS) -c NoStrategy.cpp

BasicStrategy.o: BasicStrategy.cpp BasicStrategy.h rank.h action.h
	$(CXX) $(CXXFLAGS) -c BasicStrategy.cpp

action.o: action.cpp action.h
	$(CXX) $(CXXFLAGS) -c action.cpp

clean:
	rm -f *.o blackjack