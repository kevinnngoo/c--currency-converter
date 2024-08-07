CXX = g++
CXXFLAGS = -std=c++11 -Wall
LDFLAGS = -lcurl -ljsoncpp

TARGET = main
SRCS = main.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LDFLAGS)

clean:
	rm -f $(TARGET)

all: main

main: main.cpp
		g++ main.cpp -o main -lcurl -ljsoncpp
