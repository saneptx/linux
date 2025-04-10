Srcs := $(wildcard *.c)
Objs := $(patsubst %.c, %.o, $(Srcs))

CC := gcc
server:$(Objs)
	$(CC) $^ -o $@ -lpthread

%.o: %.c
	$(CC) -c $^ -o $@ -g
			
clean:
	$(RM) $(Objs) server

rebuild: clean Out

