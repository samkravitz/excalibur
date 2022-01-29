CXX = g++
CXXFLAGS = -g -std=c++2a -I $(INCLUDE)
BIN = bin
SOURCE = src
INCLUDE = include
OBJ = \
	bishop.o \
	board.o \
	engine.o \
	king.o \
	knight.o \
	pawn.o \
	piece.o \
	queen.o \
	rook.o \
	util.o \

LIST = $(addprefix $(BIN)/, $(OBJ))
VPATH = $(SOURCE) $(SOURCE)/backup

all: excalibur
excalibur: $(LIST) main.cpp
	$(CXX) $(CXXFLAGS) -o excalibur $(SOURCE)/main.cpp $(LIST)

$(BIN)/%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f excalibur *.o bin/*