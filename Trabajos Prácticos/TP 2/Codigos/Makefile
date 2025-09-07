CXX=g++
CXXFLAGS=-std=c++17 -Wall -Wextra -O2
OBJS=Files_101.o CLI.o App.o main.o

programa: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) programa
