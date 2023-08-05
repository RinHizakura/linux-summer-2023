GIT_HOOKS := .git/hooks/applied
FLAGS=-O0 -Wall -Wextra -lpthread

SRCS=$(wildcard *.c)
OUT=$(SRCS:.c=.out)

all: $(OUT)
clean:
	rm -rf $(OUT)

%.out: %.c
	gcc -o $@ $^ $(FLAGS)
