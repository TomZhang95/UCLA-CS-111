CC = gcc
DEBUG = -g
LFLAGS = -lssl -lcrypto -lmraa -lm -Wall -Wextra $(DEBUG)
TARGET = Makefile README lab4c_tcp.c lab4c_tls.c

default: lab4c

lab4c: lab4c_tcp lab4c_tls

lab4c_tcp:
ifeq ($(shell uname -r | grep 'edison'),)
	$(CC) $(LFLAGS) -o lab4c_tcp lab4c_tcp.c -DDUMMY
else
	$(CC) $(LFLAGS) -o lab4c_tcp lab4c_tcp.c
endif

lab4c_tls:
ifeq ($(shell uname -r | grep 'beaglebone'),)
	$(CC) $(LFLAGS) -o lab4c_tls lab4c_tls.c -DDUMMY
else
	$(CC) $(LFLAGS) -o lab4c_tls lab4c_tls.c
endif

check:
	./P4C_check.sh 404743024

clean:
	rm -f *.o lab4c_tcp lab4c_tls *.log
	rm -f lab4c-404743024.tar.gz

dist: 
	@tar -czf lab4c-404743024.tar.gz $(TARGET)