#NAME: Tianyang Zhang
#EMAIL: ztytom1@gmail.com
#ID: 404743024

# macros
CC = gcc
LFLAGS = -Wall -Wextra -g
TARGET = lab0

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(LFLAGS) -o $(TARGET) $(TARGET).c

check:

#Checking --input
	@echo "Hello World!" > input.txt
	@echo "Testing input..."
	@./lab0 --input=input.txt > output.txt ;\
	if [ $$? -ne 0 ] ;\
	then \
	echo "Error occur when testing input." ;\
	else \
	echo "Passed input test." ;\
	fi
	@rm output.txt

#Checking --output
	@echo "Testing output..."
	@./lab0 --input=input.txt --output=output.txt
	@diff input.txt output.txt > difference.txt ;\
	if [ $$? -ne 0 ] ;\
	then \
	echo "Error occur when testing output." ;\
	else \
	echo "Passed output test." ;\
	fi
	@rm output.txt
	@rm difference.txt

#Checking required arguments
	@echo "Testing required arguments..."
	@./lab0 --input= ;\
	if [ $$? -ne 0 ] ;\
	then \
	echo "Error occur when missing argument." ;\
	else \
	echo "Test pass when missing argument." ;\
	fi

#Checking unknown arguments 
	@echo "Testing unknown arguments..."
	@./lab0 --zxc ;\
	if [ $$? -ne 1 ] ;\
	then \
	echo "Error occur when dealing with unknown argument." ;\
	else \
	echo "Test pass when dealing with unknown argument." ;\
	fi

#Checking properly reports success or failure
	@echo "Testing not exist input file..."
	@./lab0 --input=abc.txt > output.txt ;\
	if [ $$? -ne 2 ] ;\
	then \
	echo "Error occur when dealing with not exist input file." ;\
	else \
	echo "Test pass when dealing with not exist input file." ;\
	fi
	@rm output.txt

	@echo "Testing not openable output file..."
	@echo "Hello world" > output.txt
	@chmod -w output.txt
	@./lab0 --output=output.txt < input.txt ;\
	if [ $$? -ne 3 ] ;\
	then \
	echo "Error occur when dealing with not openable output file." ;\
	else \
	echo "Test pass when dealing with not openable output file." ;\
	fi
	@chmod +w output.txt
	@rm output.txt

#Checking catching and reporting segmentation fault
	@echo "Testing catching and reporting segmentation fault"
	@./lab0 --segfault --catch ;\
	if [ $$? -ne 4 ] ;\
	then \
	echo "Error during catching and reporting segmentation fault" ;\
	else \
	echo "Passed test of catching and reporting segmentation fault" ;\
	fi

	@echo "All test cases passed"
	@rm -rf *.txt

clean:
	@$(RM) $(TARGET) *~
	@rm -rf *.txt
	@rm -rf lab0-404743024.tar.gz
	@rm -rf functions.sh

dist:
	@tar -czf lab0-404743024.tar.gz README Makefile *.c *.png
