.PHONY: all clean
all: client server

client: Client.cpp chatI.cpp
	g++ -I. Client.cpp chatI.cpp chat.cpp -lIce -lIceUtil -lpthread -o client

server: Server.cpp chatI.cpp
	g++ -I. Server.cpp chatI.cpp chat.cpp -lIce -lIceUtil -lpthread -o server

clean:
	-rm client server
