P=Huffman
SRC=$(shell find . -type f -name "*.cpp")
HDR=$(shell find . -type f -name "*.h")
OBJ=$(SRC:.c=.o)
CFLAGS=-c -O3 -Wall
CC=g++

$(echo SRC)
all: $(SRC) $(P)
.c.o:
	$(CC) $(CFLAGS) $< -o $@
$(P): $(OBJ)
	$(CC) $(OBJ) -o $(P)
