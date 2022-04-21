CC=gcc
CFLAGS=-Wall -Wextra -std=c99

TARGETS=image_editor

build: $(TARGETS)

image_editor: main.c functions.c auxiliary.c
	$(CC) $(CFLAGS) main.c functions.c auxiliary.c -lm -o image_editor

clean:
	rm -f $(TARGETS)
