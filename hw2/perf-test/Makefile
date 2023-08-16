CFLAGS=-O0 -Wall -Wextra -I.. -MMD #-g -fsanitize=thread
LDFLAGS= -lpthread #-fsanitize=thread

OUT ?= build
BINARY = $(OUT)/perf-test

SHELL_HACK := $(shell mkdir -p $(OUT))

CSRCS = $(shell find . -name '*.c')
_COBJ =  $(notdir $(CSRCS))
COBJS = $(_COBJ:%.c=$(OUT)/%.o)

ifeq ("$(LINUX)","1")
    CFLAGS +=  -DUSE_LINUX
endif

ifeq ("$(PTHREADS)","1")
    CFLAGS +=  -DUSE_PTHREADS
endif

all: clean $(BINARY)

run: $(BINARY)
	$(BINARY)

clean:
	$(RM) $(COBJS)
	$(RM) $(BINARY)

$(BINARY): $(COBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OUT)/%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

-include $(OUT)/*.d
