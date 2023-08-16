CFLAGS := -I.. -Wall -O2 -D_GNU_SOURCE
LDFLAGS := -lpthread

OUT ?= build
ALL := test_pthread test_linux

SHELL_HACK := $(shell mkdir -p $(OUT))

all: $(ALL)
.PHONY: all

$(OUT)/test_%: main.c
	$(CC) $(CFLAGS) main.c -o $@ $(LDFLAGS)

test_pthread: CFLAGS += -DUSE_PTHREADS
test_pthread: $(OUT)/test_pthread
test_linux: CFLAGS += -DUSE_LINUX
test_linux: $(OUT)/test_linux

clean:
	$(RM) $(OUT)/test_pthread
	$(RM) $(OUT)/test_linux
.PHONY: clean
