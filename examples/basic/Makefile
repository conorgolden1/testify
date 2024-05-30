CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I.
TARGET = build/main
SRC_DIR = src

.PHONY: all clean run

all: $(TARGET)

$(TARGET): src/main.c
	mkdir -p build
	$(CC) $(CFLAGS) -o $(TARGET) src/main.c

clean:
	rm -rf build

run: all
	./$(TARGET)

export SRC_DIR
test:
	cd testify && $(MAKE)
