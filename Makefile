CC = g++
LIBS = -pthread -lrt -std=c++11

all: myServer myClient

myServer: myServer.cpp
	$(CC) myServer.cpp -o myServer $(LIBS)

myClient: myClient.cpp
	$(CC) myClient.cpp -o myClient $(LIBS)

clean:
	rm -f myClient myServer *.log
