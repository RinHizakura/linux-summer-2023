FLAGS=-O2 -Wall -Wextra -lpthread

OUT= align_up.out qsort-mt.out

ifeq ("$(ASAN)","1")
    FLAGS += -g -fsanitize=address -fno-omit-frame-pointer -fno-common
endif

ifeq ("$(TSAN)", "1")
    CFLAGS += -fsanitize=thread
    LDFLAGS += -fsanitize=thread
endif

all: $(OUT)
clean:
	rm -rf $(OUT)

%.out: %.c
	@$(CC) -o $@ $^ $(FLAGS)
