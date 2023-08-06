GIT_HOOKS := .git/hooks/applied
CFLAGS=-O0 -Wall -Wextra
LDFLAGS=-lpthread
FLAGS= $(CFLAGS) $(LDFLAGS)

STREE_OBJS=s_tree.o treeint.o

SRCS=$(wildcard *.c)
OUT= treeint.out align_up.out qsort-mt.out

all: $(OUT)
clean:
	rm -rf $(OUT) $(STREE_OBJS)

%.o: %.c
	@$(CC) -c $(CFLAGS) $< -o $@

treeint.out: $(STREE_OBJS)
	@$(CC) -o $@ $(STREE_OBJS) $(LDFLAGS)

%.out: %.c
	@$(CC) -o $@ $^ $(FLAGS)
