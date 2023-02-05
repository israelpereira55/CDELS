all: build

build:
	@gcc -c -O3 -Wall base/*.c metaheuristic/*.c main.c
	@gcc *.o -lm -o CDELS
	@rm *.o
run:
	clear
	./CDELS
