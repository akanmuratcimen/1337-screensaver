CC := g++

CFLAGS := -O3 -Wall -lm -lfreetype -lGL -lGLU -pthread \
	-lglut -ldl -lGLEW -lglfw -lglfw3 -I/usr/include/freetype2

OUTPUT = 1337-screen-saver

all: output

output: main.o glad.o
	$(CC) -o $(OUTPUT) main.o glad.o $(CFLAGS)

glad.o:
	$(CC) -c glad.c

main.o:
	$(CC) -c main.cpp $(CFLAGS)

run: clean all
	./$(OUTPUT)

clean:
	rm -f main.o glad.o $(OUTPUT)

.PHONY: clean
