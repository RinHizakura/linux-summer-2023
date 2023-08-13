CFLAGS=-O2 -Wall -Wextra -I.. -MMD -DUSE_LINUX
LDFLAGS= -lpthread

OUT ?= build
BINARY = $(OUT)/pi-test

SHELL_HACK := $(shell mkdir -p $(OUT))

CSRCS = $(shell find . -name '*.c')
_COBJ =  $(notdir $(CSRCS))
COBJS = $(_COBJ:%.c=$(OUT)/%.o)

all: $(BINARY)

# This should be run as root because we'll modify the schedule policy
run: $(BINARY)
	sudo $(BINARY)
clean:
	$(RM) $(COBJS)
	$(RM) $(BINARY)

$(BINARY): $(COBJS)
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OUT)/%.o: %.c
	@$(CC) -c $(CFLAGS) $< -o $@

-include $(OUT)/*.d
