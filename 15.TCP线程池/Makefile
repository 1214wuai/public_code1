.PHONY:all
all:server client
cc=g++

client:TcpClient.cc
	$(cc) -o $@ $^ 
server:TcpServer.cc
	$(cc) -o $@ $^ -std=c++11 -lpthread
	
.PHONY:clean
clean:
	rm -f server client
