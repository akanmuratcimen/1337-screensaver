all:
	gcc main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o 1337-screensaver
