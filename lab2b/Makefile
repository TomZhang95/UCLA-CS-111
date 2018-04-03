CC = gcc
DEBUG = -g
LFLAGS = -lpthread -Wall -Wextra $(DEBUG)
TARGET = SortedList.c SortedList.h lab2_list.c Makefile lab2b_list.csv lab2b_1.png lab2b_2.png lab2b_3.png lab2b_4.png lab2b_5.png lab2b_list.gp lab2b_test.sh profile.out README

.PHONY : dist all tests graphs profile

default: build

build: lab2_list

lab2_list: lab2_list.c
	$(CC) $(LFLAGS) -o lab2_list lab2_list.c SortedList.c

tests:
	./lab2b_test.sh

graphs:
	gnuplot lab2b_list.gp

profile:
	$(CC) $(LFLAGS) -o lab2_list lab2_list.c SortedList.c  
	rm -f ./raw.gprof
	LD_PRELOAD=~/gperftools-2.6.1/.libs/libprofiler.so.0 CPUPROFILE=raw.gprof ./lab2_list --threads=12 --iterations=1000 --sync=s
	pprof --text lab2_list ./raw.gprof > profile.out
	pprof --list=thread_function lab2_list ./raw.gprof >> profile.out
	rm -f ./raw.gprof


clean:
	rm -f *.o *.csv *.png lab2_list
	rm lab2b-404743024.tar.gz

dist: default tests graphs profile
	@tar -czf lab2b-404743024.tar.gz $(TARGET)