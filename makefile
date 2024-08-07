CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -L/opt/X11/lib -lX11 -lcairo
SRC_DIR = src
BIN_DIR = bin

# List of source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# List of object files
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SRCS))

.PHONY: all run clean

all: $(BIN_DIR)/NGOWebcam

run: all
	$(BIN_DIR)/NGOWebcam

clean:
	rm -f $(BIN_DIR)/*.o $(BIN_DIR)/NGOWebcam

$(BIN_DIR)/NGOWebcam: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN_DIR)/NGOWebcam $(OBJS)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@
