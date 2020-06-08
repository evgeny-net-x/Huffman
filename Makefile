P=Huffman
SRC=$(shell find . -type f -name "*.c")
HDR=$(shell find . -type f -name "*.h")
OBJ=$(SRC:.c=.o)
CFLAGS=-c -g -O3 -Wall -Wno-unused-function
CC=gcc

$(echo SRC)
all: $(SRC) $(P)
.c.o:
	$(CC) $(CFLAGS) $< -o $@
$(P): $(OBJ)
	$(CC) $(OBJ) -o $(P)
