CC := gcc
CFLAGS := -Wall -Wextra -std=gnu11 -pedantic -O3 -g -fdiagnostics-color=always
LIBSX11 := -lGL -lm -lpthread -ldl -lrt -lX11 -lglut -l:libGLEW.a -lglfw -lglfw3 -I cglm/include -L cglm/build -lcglm
LIBSX112 := -lGL -lm -lX11 -l:libGLEW.a -I cglm/include -L cglm/build -lcglm
LIBSW32 := -lopengl32 -lgdi32 -lwinmm -mwindows -lwinpthread --static
OUTPUT := 1337
SRC := main.c

linux:
	$(CC) $(SRC) $(CFLAGS) $(LIBSX112) -DX11 -o $(OUTPUT)

windows:
	$(CC) -g $(SRC) $(CFLAGS) $(LIBSW32) -o $(OUTPUT).scr

build_resources:
	xxd -i textures/sprite_1k.png > textures/sprite_1k.h
	xxd -i textures/sprite_2k.png > textures/sprite_2k.h
	xxd -i textures/sprite_4k.png > textures/sprite_4k.h
	xxd -i shaders/char_fs.glsl > shaders/char_fs.h
	xxd -i shaders/char_vs.glsl > shaders/char_vs.h
	xxd -i shaders/rect_fs.glsl > shaders/rect_fs.h
	xxd -i shaders/rect_vs.glsl > shaders/rect_vs.h

install-linux: linux
	sudo cp 1337 /usr/libexec/xscreensaver/

run:
	./$(OUTPUT)

clean:
	rm $(OUTPUT)
