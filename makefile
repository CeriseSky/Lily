APP_NAME:=lily
COMMIT_HASH:=$(shell git rev-parse HEAD | head -c 8)

SRC:=src
INCLUDE:=include

PREFIX:=$(shell pwd)/install
TMP:=tmp
OUTPUT_DIRS:=$(PREFIX) \
             $(TMP) \
             $(PREFIX)/bin

CC:=gcc
DEF_CFLAGS:=-Wall -Werror -Wpedantic -std=gnu23 -I$(INCLUDE) \
            -DCOMMIT_HASH=\"$(COMMIT_HASH)\" -MMD -MP
REL_CFLAGS:=$(DEF_CFLAGS) -O3
DBG_CFLAGS:=$(DEF_CFLAGS) -O0 -g
CFLAGS:=$(DBG_CFLAGS)

LD:=gcc
LDFLAGS:=

SOURCES:=$(wildcard $(SRC)/*.c)
OBJS:=$(patsubst $(SRC)/%.c,$(TMP)/%.o,$(SOURCES))
DEPENDS:=$(patsubst $(SRC)/%.c,$(TMP)/%.d,$(SOURCES))

.PHONEY: all dirs clean

all: dirs $(PREFIX)/bin/$(APP_NAME)

dirs: $(OUTPUT_DIRS)

clean: $(TMP)
	rm -rf $(TMP)

$(OUTPUT_DIRS):
	mkdir -p $@

$(PREFIX)/bin/$(APP_NAME): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

-include $(DEPENDS)

$(OBJS): $(TMP)/%.o: $(SRC)/%.c makefile
	$(CC) $(CFLAGS) -c $< -o $@

