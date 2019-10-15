.PHONY: all clean

all: test

test: test.cpp include/resource.h
	g++ -std=c++17 test.cpp -o test -pthread

clean:
	rm test
