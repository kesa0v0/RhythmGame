# Makefile for Rhythm Game

CC = gcc
LDFLAGS = -lncurses -lSDL2 -lSDL2_mixer

SRC = ingame.c audio.c
TARGET = game

.PHONY: all clean deps run

all: deps $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

deps:
	sudo apt-get update
	sudo apt-get install -y libncurses-dev;
	sudo apt install -y libsdl2-mixer-dev;

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
