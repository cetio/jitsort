CC=clang
LDFLAGS=

SRC_DIR=source
BIN_DIR=bin
TARGET=$(BIN_DIR)/jitsort

SOURCES=$(SRC_DIR)/main.c
OBJECTS=$(SOURCES:.c=.o)

.PHONY: all clean run

all: $(TARGET)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(TARGET): $(BIN_DIR) $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BIN_DIR) $(OBJECTS)


