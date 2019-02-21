.PHONY:all
all:httpserver utils

httpserver:httpserver.cpp
	g++ -o $@ $^ -l pthread -std=c++11

utils:utils.cpp
	g++ -o $@ $^ -l pthread -std=c++11

.PHONY:clean
clean:
	rm httpserver utils -f
