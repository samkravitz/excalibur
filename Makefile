CXX = g++
CXXFLAGS = -g -O2 -std=c++2a -I $(INCLUDE)
LIBS = -lpthread
SOURCE = src
INCLUDE = include
OBJ = \
	board.o \
	game.o \
	movegen.o \
	movelist.o \
	perft.o \
	search.o \
	uci.o \
	util.o \
	zobrist.o \

VPATH = $(SOURCE)

all: excalibur
excalibur: $(OBJ) main.cpp
	$(CXX) $(CXXFLAGS) -o excalibur $(SOURCE)/main.cpp $(OBJ) $(LIBS)

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f excalibur *.o
