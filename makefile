CC := gcc
CFLAGS := -Wall -std=gnu11 -pedantic -O3 -fdiagnostics-color=always
LIBSX11 := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
LIBSW32 := -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows -I raylib\src -L raylib\src
OUTPUT := 1337-screensaver
SRC := main.c

linux:
	$(CC) $(SRC) $(CFLAGS) $(LIBSX11) -o $(OUTPUT)

windows:
	$(CC) -g $(SRC) $(CFLAGS) $(LIBSW32) -o $(OUTPUT).scr

run:
	./$(OUTPUT)

clean:
	rm $(OUTPUT)
