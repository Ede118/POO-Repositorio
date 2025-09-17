CXX      := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -pedantic

SRC := main.cpp App.cpp CLI.cpp File.cpp serial.cpp
HDR := App.h CLI.h File.h Types.h serial.h

TARGET := app

all: $(TARGET)

$(TARGET): $(SRC) $(HDR)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC)

clean:
	rm -f $(TARGET)
