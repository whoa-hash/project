
all: combine

combine: combinemake testsmake
	./test
	./combined

combinemake: my_mem.o main.o
	gcc my_mem.o main.o -o combined;

testsmake: my_mem.o tests.o
	gcc my_mem.o tests.o -o test;

my_mem.o: my_mem.c
	gcc -g -c my_mem.c
	
main.o: main.c
	gcc -g -c main.c	

tests.o: tests.c
	gcc -g -c tests.c