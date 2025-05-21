# Makefile for Password Manager
CC = gcc

CFLAGS = -Wall -Wextra -g `pkg-config --cflags gtk4 openssl`

LIBS = `pkg-config --libs gtk4 openssl`

SRC_DIR = src
SRC_FILES = main.c gui.c storage.c crypto.c  utils.c
SRCS = $(addprefix $(SRC_DIR)/, $(SRC_FILES))

HDR_FILES = gui.h storage.h crypto.h utils.h
HDRS = $(addprefix $(SRC_DIR)/, $(HDR_FILES))

TARGET = password-manager

all: $(TARGET)

$(TARGET): $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)

setup:
	mkdir -p data ui

run: $(TARGET)
	./$(TARGET)