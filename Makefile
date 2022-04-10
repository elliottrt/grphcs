CC=gcc

OUT=grphcs

CCFLAGS=-Wall -Wextra -Wpedantic -lSDL2 -lSDL2_ttf -lSDL2_image -msse -O3

CSRC=$(wildcard src/*.c)
CLIB=$(wildcard src/lib/*.c)

all: com run

com:
	$(CC) -o $(OUT) $(CSRC) $(CLIB) $(CCFLAGS)

	rm -f ./src/*.o

run:
	./$(OUT)
