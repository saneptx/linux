Srcs := $(wildcard *.c)
Exes := $(patsubst %.c, %, $(Srcs))

CC := gcc
CFLAGS := -Wall -g

.PHONY: Out clean rebuild

Out: $(Exes)

%: %.c
		$(CC) $^ -o $@ $(CFLAGS) -pthread
			
clean:
		$(RM) $(Exes)

rebuild: clean Out

