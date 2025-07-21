APP_NAME=lily
COMMIT_HASH=$(shell git rev-parse HEAD | head -c 8)

SRC=src
INCLUDE=include

PREFIX=$(shell pwd)/install
TMP=tmp
OUTPUT_DIRS=$(PREFIX) \
            $(TMP) \
            $(PREFIX)/bin

CC=gcc
CFLAGS=-Wall -Werror -Wpedantic -std=c23 -I$(INCLUDE) -g -O3 \
       -DCOMMIT_HASH=\"$(COMMIT_HASH)\"

LD=gcc
LDFLAGS=

OBJS=$(patsubst $(SRC)/%.c,$(TMP)/%.o,$(wildcard $(SRC)/*.c))

.PHONEY: all dirs clean

all: dirs $(PREFIX)/bin/$(APP_NAME)

dirs: $(OUTPUT_DIRS)

clean: $(TMP)
	rm -rf $(TMP)

$(OUTPUT_DIRS):
	mkdir -p $@

$(PREFIX)/bin/$(APP_NAME): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@

$(OBJS): $(TMP)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

