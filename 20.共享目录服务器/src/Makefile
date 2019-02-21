.PHONY:all
all:main utils

main:httpserver.cpp
	g++ -o $@ $^ -l pthread -std=c++11

utils:utils.cpp
	g++ -o $@ $^ -l pthread -std=c++11

.PHONY:clean
clean:
	rm main utils -f
