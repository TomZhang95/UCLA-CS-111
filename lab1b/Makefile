CC = gcc
DEBUG = -g
LFLAGS = -lmcrypt $(DEBUG) -Wall -Wextra -o
TARGET = lab1b-client.c lab1b-server.c README Makefile my.key

all: lab1b-client lab1b-server

lab1b-client: lab1b-client.c
	$(CC) $(LFLAGS) $@ lab1b-client.c

lab1b-server: lab1b-server.c
	$(CC) $(LFLAGS) $@ lab1b-server.c

dist: 
	@tar -czf lab1b-404743024.tar.gz $(TARGET)

clean: 
	@rm -rf lab1b-client lab1b-server *.o *.log
	@rm -rf lab1b-404743024.tar.gz

