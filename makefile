CC := gcc

CFLAGS := -O0 -Wall -lm -lfreetype -lGL -lGLU -pthread \
	-lglut -ldl -lGLEW -lglfw -lglfw3 -I/usr/include/freetype2

HDRS :=
SRCS := glad.c main.c
OBJS := $(SRCS:.c=.o)
EXEC := output

all: $(EXEC)

$(EXEC): $(OBJS) $(HDRS) makefile
	$(CC) -o $@ $(OBJS) $(CFLAGS)

clean:
	rm -f $(EXEC) $(OBJS)

.PHONY: all clean
