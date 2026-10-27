CC = gcc
CFLAGS ?= -std=c17 -Wall -Wextra -g -Iinclude -pthread

CURRENT_SRCS := src/memory_pool.c src/main.c
CURRENT_OBJS := $(CURRENT_SRCS:.c=.o)

LEGACY_SRCS := backup/memory_pool_freelist_v1.c \
	backup/memory_pool_freelist_demo.c

ifeq ($(OS),Windows_NT)
CURRENT_TARGET := my_pool.exe
LEGACY_TARGET := backup/memory_pool_freelist.exe
else
CURRENT_TARGET := my_pool
LEGACY_TARGET := backup/memory_pool_freelist
endif

# Keep the current allocator as the default build.
all: current

# Current allocator: canary checks and pthread protection.
current: $(CURRENT_TARGET)

$(CURRENT_TARGET): $(CURRENT_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

src/%.o: src/%.c include/memory_pool.h
	$(CC) $(CFLAGS) -c $< -o $@

# Legacy allocator: simple free-list implementation and its standalone demo.
legacy: $(LEGACY_TARGET)

$(LEGACY_TARGET): $(LEGACY_SRCS) include/memory_pool.h
	$(CC) $(CFLAGS) -o $@ $(LEGACY_SRCS)

clean:
ifeq ($(OS),Windows_NT)
	-cmd /C del /Q $(subst /,\,$(CURRENT_OBJS) $(CURRENT_TARGET) $(LEGACY_TARGET)) 2>NUL
else
	rm -f $(CURRENT_OBJS) $(CURRENT_TARGET) $(LEGACY_TARGET)
endif

.PHONY: all current legacy clean
