# 編譯器設定
CC = gcc
CFLAGS = -Wall -g -Iinclude  # -Iinclude 代表去 include 資料夾找 .h 檔

# 目標檔案
TARGET = my_pool
SRCS = src/memory_pool.c src/main.c
OBJS = $(SRCS:.c=.o)

# 預設動作: 編譯 TARGET
all: $(TARGET)

# 連結 (Linking)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# 編譯 (Compiling)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清除編譯產生的檔案
clean:
	rm -f $(OBJS) $(TARGET)

# 偽目標 (避免跟檔名衝突)
.PHONY: all clean