CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I.
TARGET = build/main

.PHONY: all clean run

all: $(TARGET)

$(TARGET): src/main.c
	mkdir -p build
	$(CC) $(CFLAGS) -o $(TARGET) src/main.c

clean:
	rm -rf build

run: all
	./$(TARGET)

SRC_DIRS := src
export SRC_DIRS
test:
	cd testify && $(MAKE)
