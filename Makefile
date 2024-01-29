CPPC=g++

OUT=grphcs

lttf=-lSDL2_ttf
limg=-lSDL2_image

CPPFLAGS=-Wall -Wextra -Wpedantic -lSDL2 -msse -O3 -std=c++11 -ffast-math
CPPFLAGS := $(CPPFLAGS) -Wno-gnu-anonymous-struct

CPPSRC=$(wildcard src/*.cpp)

all: compile run

compile:
	$(CPPC) -o $(OUT) $(CPPSRC) $(CPPFLAGS)

run:
	./$(OUT)
