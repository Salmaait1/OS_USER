# Makefile pour le projet SH13

CC = gcc
CFLAGS = -Wall

SERVER_SRC = server.c
CLIENT_SRC = sh13.c

SERVER_BIN = server
CLIENT_BIN = sh13

SDL_FLAGS = -lSDL2 -lSDL2_image -lSDL2_ttf -lpthread

all: $(SERVER_BIN) $(CLIENT_BIN)

$(SERVER_BIN): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER_BIN) $(SERVER_SRC)

$(CLIENT_BIN): $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $(CLIENT_BIN) $(CLIENT_SRC) $(SDL_FLAGS)

clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)

