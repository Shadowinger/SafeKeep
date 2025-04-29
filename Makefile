# Kompilátor
CC = gcc

# CFLAGS: zobrazuj varování a debug info + cesta ke knihovnám
CFLAGS = -Wall -Wextra -g `pkg-config --cflags gtk4 openssl`

# LDFLAGS: linkování GTK4 + OpenSSL
LIBS = `pkg-config --libs gtk4 openssl`

# Umístění zdrojových souborů
SRC_DIR = src
SRC_FILES = main.c gui.c storage.c crypto.c
SRCS = $(addprefix $(SRC_DIR)/, $(SRC_FILES))

# Výstupní název binárky
TARGET = password-manager

# Výchozí cíl
all: $(TARGET)

# Linknutí a build
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

# Vyčištění buildu
clean:
	rm -f $(TARGET)