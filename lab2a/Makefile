CC = gcc
DEBUG = -g
LFLAGS = -lpthread -Wall -Wextra $(DEBUG)
TARGET = lab2_add.c SortedList.c SortedList.h lab2_list.c Makefile lab2_add.csv lab2_list.csv lab2_add-1.png lab2_add-2.png lab2_add-3.png lab2_add-4.png lab2_add-5.png lab2_list-1.png lab2_list-2.png lab2_list-3.png lab2_list-4.png lab2_add.gp lab2_list.gp lab2a_test.sh README

.PHONY : dist all tests graphs

all: lab2_add lab2_list

lab2_add: lab2_add.c
	$(CC) $(LFLAGS) -o lab2_add lab2_add.c

lab2_list: lab2_list.c
	$(CC) $(LFLAGS) -o lab2_list lab2_list.c

tests:
	./lab2a_test.sh

graphs:
	gnuplot lab2_add.gp
	gnuplot lab2_list.gp

clean:
	rm -f *.o *.csv *.png lab2_add lab2_list
	rm lab2a-404743024.tar.gz

dist: all tests graphs
	@tar -czf lab2a-404743024.tar.gz $(TARGET)