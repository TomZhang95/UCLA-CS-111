CC = gcc
DEBUG = -g
LFLAGS = -Wall $(DEBUG)
CFLAGS = -Wall -Wextra
TARGET = lab1a

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

check:
	@./lab1a
	@./lab1a --shell
	
clean:
	@$(RM) $(TARGET)
	@rm -rf lab1a-404743024.tar.gz

dist: 
	@tar -czf lab1a-404743024.tar.gz README Makefile *.c