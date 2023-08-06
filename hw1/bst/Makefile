CFLAGS=-O2 -Wall -Wextra
LDFLAGS=

OUT ?= build
BINARY = $(OUT)/bst

CSRCS = $(shell find . -name '*.c')
_COBJ =  $(notdir $(CSRCS))
COBJS = $(_COBJ:%.c=$(OUT)/%.o)

SHELL_HACK := $(shell mkdir -p $(OUT))

all: $(BINARY)
clean:
	$(RM) $(COBJS)
	$(RM) $(BINARY)

%.o: %.c
	@$(CC) -c $(CFLAGS) $< -o $@

$(BINARY): $(COBJS)
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OUT)/%.o: %.c
	@$(CC) -c $(CFLAGS) $< -o $@
