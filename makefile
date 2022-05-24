all:
	gcc main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -O0 -o 1337-screensaver

run:
	./1337-screensaver
