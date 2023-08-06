FLAGS=-O2 -Wall -Wextra -lpthread

OUT= align_up.out qsort-mt.out

all: $(OUT)
clean:
	rm -rf $(OUT)

%.out: %.c
	@$(CC) -o $@ $^ $(FLAGS)
