# Define the C++ compiler to use
CXX = g++

# Define the C++ standard to use (This is the crucial part!)
CXXFLAGS = -std=c++20

# Define the name of the final executable program
TARGET = blackjack

# Define your source files (e.g., main.cpp, game.cpp)
# Adjust this list to match your actual .cpp files
SOURCES = card.cpp

# Automatically generate a list of object files (.o) from the sources
OBJECTS = $(SOURCES:.cpp=.o)

# --- Targets ---

# The default target (what happens when you just type 'make')
# This links all the object files into the final executable.
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

# Rule for compiling individual C++ source files into object files
# $< is the first prerequisite (the .cpp file)
# $@ is the target (the .o file)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Target for cleaning up (removes the executable and all object files)
.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJECTS)