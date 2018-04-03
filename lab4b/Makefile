CC = gcc
DEBUG = -g
LFLAGS = -lmraa -lm -Wall -Wextra $(DEBUG)
TARGET = Makefile README lab4b.c

default: lab4b

lab4b: lab4b.c
	$(CC) $(LFLAGS) -o lab4b lab4b.c

check:
	./P4B_check.sh 404743024

clean:
	rm -f *.o lab4b
	rm -f lab4b-404743024.tar.gz

dist: 
	@tar -czf lab4b-404743024.tar.gz $(TARGET)