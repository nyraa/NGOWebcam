CC = gcc
CFLAGS = -Wall -g
LDFLAGS = -L/opt/X11/lib -lX11 -lcairo
SRC_DIR = src
BIN_DIR = bin

# List of source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# List of object files
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BIN_DIR)/%.o, $(SRCS))

$(BIN_DIR)/NGOWebcam: $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(BIN_DIR)/NGOWebcam $(OBJS)

$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@
