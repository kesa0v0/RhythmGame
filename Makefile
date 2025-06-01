# Makefile for Rhythm Game

CC = gcc
LDFLAGS = -lncurses -lSDL2 -lSDL2_mixer

SRC = ingame.c audio.c rank.c network.c select_musics.c
TARGET = game

SERVER_SRC = ./server/server.c
SERVER_TARGET = ./server/server

.PHONY: all clean deps run server

all: deps $(TARGET) server

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

deps:
	sudo apt-get update
	sudo apt-get install -y libncurses-dev
	sudo apt install -y libsdl2-mixer-dev

run: $(TARGET)
	./$(TARGET)

server: $(SERVER_SRC)
	$(CC) -o $(SERVER_TARGET) $(SERVER_SRC)

clean:
	rm -f $(TARGET)

