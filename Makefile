CC=gcc
CFLAGS= -O3
CFLAGSDEBUG= -Wall -Wextra -g
LIBS= -lm


all: build

build:
	$(CC) -c $(CFLAGS) common/*.c metaheuristic/*.c main.c
	$(CC) *.o $(LIBS) -o CDELS
	@rm *.o
	
debug:
	$(CC) -c $(CFLAGSDEBUG) common/*.c metaheuristic/*.c main.c
	$(CC) *.o $(LIBS) -o CDELS
	@rm *.o
	
run:
	clear
	./CDELS

