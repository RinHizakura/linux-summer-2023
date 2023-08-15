CFLAGS=-O0 -Wall -Wextra -I.. -MMD -DUSE_LINUX -g -fsanitize=thread
LDFLAGS= -lpthread -fsanitize=thread

OUT ?= build
BINARY = $(OUT)/perf-test

SHELL_HACK := $(shell mkdir -p $(OUT))

CSRCS = $(shell find . -name '*.c')
_COBJ =  $(notdir $(CSRCS))
COBJS = $(_COBJ:%.c=$(OUT)/%.o)

all: $(BINARY)

run: $(BINARY)
	$(BINARY)

clean:
	$(RM) $(COBJS)
	$(RM) $(BINARY)

$(BINARY): $(COBJS)
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OUT)/%.o: %.c
	@$(CC) -c $(CFLAGS) $< -o $@

-include $(OUT)/*.d
