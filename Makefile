CC = g++
CFLAGS = -Wall -Wextra

SRC_DIR = src
BUILD_DIR = build

SERVER = $(BUILD_DIR)/server
CLIENT = $(BUILD_DIR)/client

server: $(SERVER)

client: $(CLIENT)

$(SERVER): $(SRC_DIR)/server.cpp $(SRC_DIR)/Socket.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(CLIENT): $(SRC_DIR)/client.cpp $(SRC_DIR)/Socket.cpp | $(BUILD_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

run:
	./$(BUILD_DIR)/$(word 2,$(MAKECMDGOALS))

server client:
	@:

clean:
	rm -rf $(BUILD_DIR)

.PHONY: server client run clean
