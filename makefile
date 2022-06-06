CC := gcc
CFLAGS := -Wall -Wextra -std=gnu11 -pedantic -O0 -fdiagnostics-color=always
LIBSX11 := -lGL -lm -lpthread -ldl -lrt -lX11 -lglut -lGLEW -lglfw -lglfw3 -I cglm/include -L cglm/build -lcglm
LIBSW32 := -lopengl32 -lgdi32 -lwinmm -mwindows -lwinpthread --static
OUTPUT := 1337
SRC := main.c texture.c

linux:
	$(CC) $(SRC) $(CFLAGS) $(LIBSX11) -o $(OUTPUT)

windows:
	$(CC) -g $(SRC) $(CFLAGS) $(LIBSW32) -o $(OUTPUT).scr

run:
	./$(OUTPUT)

clean:
	rm $(OUTPUT)
