CPPFLAGS=-std=c++14
all: run

run: main
	./main
main:
	g++ $(CPPFLAGS) -o main main.cpp graph.cpp
clean:
	rm -rf main
