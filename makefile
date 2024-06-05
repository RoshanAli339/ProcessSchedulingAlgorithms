# Define the compiler and the flags
CXX = g++
CXXFLAGS = -Wall -std=c++11

# Define the target executable
TARGET = my_program

# Define the object files
OBJS = parser.o main.o

# Rule to build the target executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)

# Rule to build parser.o
parser.o: parser.cpp parser.h
	$(CXX) $(CXXFLAGS) -c parser.cpp

# Rule to build main.o
main.o: main.cpp parser.h
	$(CXX) $(CXXFLAGS) -c main.cpp

# Clean up the build files
clean:
	rm -f $(OBJS)

