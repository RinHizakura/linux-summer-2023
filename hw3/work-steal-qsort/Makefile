CFLAGS = -O2 -Wall -Wextra -Iinclude -g
LDFLAGS = -lpthread

ifeq ("$(TSAN)", "1")
    CFLAGS +=  -fsanitize=thread
    LDFLAGS += -fsanitize=thread
endif

CURDIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
OUT ?= build
BINARY = $(OUT)/qsort-mt
SHELL_HACK := $(shell mkdir -p $(OUT))

GIT_HOOKS := .git/hooks/applied

CSRCS = $(shell find ./src -name '*.c')
_COBJ =  $(notdir $(CSRCS))
COBJ = $(_COBJ:%.c=$(OUT)/%.o)

vpath %.c $(sort $(dir $(CSRCS)))

all: $(BINARY)

run: $(BINARY)
	$(BINARY) -n 100000 -h 16 -t

$(OUT)/%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(BINARY): $(COBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	$(RM) $(COBJ)
	$(RM) $(BINARY)
