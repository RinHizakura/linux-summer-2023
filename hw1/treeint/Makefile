CFLAGS=-O2 -Wall -Wextra -MMD
LDFLAGS=

OUT ?= build
BINARY = $(OUT)/treeint

CSRCS = $(shell find . -name '*.c')
_COBJ =  $(notdir $(CSRCS))
COBJS = $(_COBJ:%.c=$(OUT)/%.o)

SHELL_HACK := $(shell mkdir -p $(OUT))

all: $(BINARY)
clean:
	$(RM) $(COBJS)
	$(RM) $(BINARY)

test-stree: $(BINARY)
	$(BINARY) s-tree 100 0

test-rbtree: $(BINARY)
	$(BINARY) rbtree 100 0

%.o: %.c
	@$(CC) -c $(CFLAGS) $< -o $@

$(BINARY): $(COBJS)
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OUT)/%.o: %.c
	@$(CC) -c $(CFLAGS) $< -o $@

-include $(OUT)/*.d
