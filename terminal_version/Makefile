CXX=g++

SRC=main.cpp
OBJ=check.out

CXX_FLAG?= -O2 -o $(OBJ) -std=c++14

.PHONY: clean compile run all


all: compile run

compile:
	@$(CXX) $(SRC) $(CXX_FLAG) 

run:
	@./$(OBJ)

clean:
	-rm *.out
