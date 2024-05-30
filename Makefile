BUILD_DIR := ../build/tests
TARGET := test_runner


CFLAGS := -Wall -Wextra

all: $(TARGET)

$(BUILD_DIR)/$(TARGET):
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $(TARGET).c -o $@.o
	$(CC) $(CFLAGS) $@.o -o $@

$(TARGET): $(BUILD_DIR)/$(TARGET)
	cd ../ && build/tests/$(TARGET) $(SRC_DIR)

