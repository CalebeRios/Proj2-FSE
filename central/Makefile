CC := gcc
CFLAGS := -Wall -lpthread -lncurses

SRCFILES := $(wildcard ./src/*.c)

all: $(SRCFILES:./src/%.c=./obj/%.o)
	$(CC) ./obj/*.o $(CFLAGS) -o ./bin/saida

./obj/%.o : ./src/%.c
	$(CC) $(CFLAGS) -c $< -o $@ -I./inc

.PHONY: clean
clean: 
	rm -rf ./obj/*
	rm -rf ./bin/*

run: 
	./bin/saida
