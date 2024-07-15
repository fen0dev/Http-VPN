CC = gcc
CFLAGS = -Wall -Wextra -Werror -pthread -Iinclude
LDFLAGS = -lssl -lcrypto

SRC = src/main.c src/http_server.c src/vpn_server.c src/logger.c
OBJ = $(SRC:.c=.o)

EXEC = http_vpn_server

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ) $(EXEC)
