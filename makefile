CC := gcc
CFLAGS := -Wall -std=gnu11 -pedantic -O3
LIBS := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
OUTPUT := 1337-screensaver

SRC := main.c

all:
	$(CC) $(SRC) $(CFLAGS) $(LIBS) -o $(OUTPUT)

run:
	./$(OUTPUT)

clean:
	rm $(OUTPUT)
