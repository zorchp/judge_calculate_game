CXX=clang++
CXX_FLAG?=-o main.out
SRC= main.cpp

.PHONY: clean all
all:
	$(CXX) $(SRC) $(CXX_FLAG)

clean:
	rm *.out
