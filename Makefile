all: iss

iss: main.c
	gcc -Wall main.c -o iss
