.PHONY: all clean

all: test

test: test.cpp include/resource.h
	g++ -std=c++2a test.cpp -o test -pthread

clean:
	rm test
