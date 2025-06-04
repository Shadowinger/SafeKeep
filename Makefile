# Makefile for Password Manager
CC = gcc

CFLAGS = -Wall -Wextra -g `pkg-config --cflags gtk4 openssl`

LIBS = `pkg-config --libs gtk4 openssl`

SRC_DIR = src
SRC_FILES = main.c gui.c storage.c crypto.c
SRCS = $(addprefix $(SRC_DIR)/, $(SRC_FILES))

HDR_FILES = gui.h storage.h crypto.h
HDRS = $(addprefix $(SRC_DIR)/, $(HDR_FILES))

TARGET = password-manager

all: $(TARGET)

$(TARGET): $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)

setup:
	mkdir -p data ui
	@if [ ! -f ui/styles.css ]; then \
		echo "/* SafeKeep Password Manager Styles */" > ui/styles.css; \
		echo "#main-window { background-color: #f5f5f5; }" >> ui/styles.css; \
		echo "#login-window { background-color: #ffffff; }" >> ui/styles.css; \
		echo "button { margin: 5px; padding: 8px 16px; }" >> ui/styles.css; \
		echo ".copy-button { background-color: #007acc; color: white; }" >> ui/styles.css; \
		echo ".delete-button { background-color: #dc3545; color: white; }" >> ui/styles.css; \
		echo ".show-button { background-color: #28a745; color: white; }" >> ui/styles.css; \
	fi

run: $(TARGET)
	./$(TARGET)