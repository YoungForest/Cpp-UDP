CC = g++
LIBS = -pthread

all: myServer myClient

myServer:
	$(CC) myServer.cpp -o myServer $(LIBS)

myClient:
	$(CC) myClient.cpp -o myClient $(LIBS)

clean:
	rm -f myClient myServer