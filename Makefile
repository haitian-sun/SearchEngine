# Makefile

CC = g++
LD = g++

CFLAGS = -pedantic -Wall -c -std=c++11
SERVER_LFLAGS = -ldl -pthread -lpthread
INDEXER_LFLAGS = 

SERVER_OBJS = server_main.o Index_server.o mongoose.o
INDEXER_OBJS = indexer_main.o Indexer.o

SERVER = indexServer
INDEXER = indexer

default: $(SERVER)

debug: CFLAGS += -g
debug: SERVER_LFLAGS += -g
debug: INDEXER_LFLAGS += -g
debug: $(SERVER) $(INDEXER)

$(SERVER): $(SERVER_OBJS)
	$(LD) $(SERVER_OBJS) -o $(SERVER) $(SERVER_LFLAGS)

server_main.o: server_main.cpp Index_server.h
	$(CC) $(CFLAGS) server_main.cpp

Index_server.o: Index_server.cpp Index_server.h
	$(CC) $(CFLAGS) Index_server.cpp

mongoose.o: mongoose.c mongoose.h
	gcc -c mongoose.c 

$(INDEXER): $(INDEXER_OBJS)
	$(LD) $(INDEXER_LFLAGS) $(INDEXER_OBJS) -o $(INDEXER)

indexer_main.o: indexer_main.cpp Indexer.h
	$(CC) $(CFLAGS) indexer_main.cpp

Indexer.o: Indexer.cpp Indexer.h
	$(CC) $(CFLAGS) Indexer.cpp

clean:
	rm -rf *.o $(SERVER) $(INDEXER)
