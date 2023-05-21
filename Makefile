CC=gcc
CFLAGS=-O2 -Wall -Wextra
SRC=$(wildcard src/*.c)
OBJ=$(patsubst src/%.c,obj/%.o,$(SRC))
BIN=csetup

default: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

obj/%.o: src/%.c
	mkdir -p obj/
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN) $(OBJ) obj/
