#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> // 引入執行緒庫
#include "memory_pool.h"

// 定義 Canary，用來檢查記憶體是否被踩壞
#define CANARY_MAGIC 0xDEADBEEF

// 定義區塊標頭 (Header)，用來存放 Canary
typedef struct {
    unsigned int canary;
} BlockHeader;

// 計算「實際」每個區塊的大小 = Header + 使用者要的 32 bytes
// 這樣我們才能在不影響使用者的情況下，塞一個 Header 在前面
#define TOTAL_BLOCK_SIZE (sizeof(BlockHeader) + BLOCK_SIZE)

// 自由串列的節點結構 (Embedded Pointer)
typedef struct FreeNode {
    struct FreeNode* next;
} FreeNode;

// 1. 調整記憶體池大小：要包含 Header 的空間
static char memory_pool[POOL_SIZE * TOTAL_BLOCK_SIZE];

// 2. 指向第一個空閒區塊的指標
FreeNode* free_list_head = NULL;

// 3. 宣告 Mutex 鎖 (互斥鎖)
pthread_mutex_t pool_lock;

// 初始化 Memory Pool
void pool_init() {
    // 初始化 Mutex
    if (pthread_mutex_init(&pool_lock, NULL) != 0) {
        printf("Mutex init failed!\n");
        return;
    }

    free_list_head = (FreeNode*)memory_pool;
    FreeNode* current = free_list_head;

    for (int i = 0; i < POOL_SIZE - 1; i++) {
        // 這裡的位移量要改成 TOTAL_BLOCK_SIZE (包含 Header)
        FreeNode* next_node = (FreeNode*)((char*)current + TOTAL_BLOCK_SIZE);
        current->next = next_node;
        current = next_node;
    }
    current->next = NULL;

    printf("Memory Pool Initialized. Block Size (User): %d, Real Size: %lu\n", 
           BLOCK_SIZE, (unsigned long)TOTAL_BLOCK_SIZE);
}

// 配置記憶體
void* pool_alloc() {
    // 【上鎖】 進入 Critical Section
    pthread_mutex_lock(&pool_lock);

    if (free_list_head == NULL) {
        printf("Out of Memory!\n");
        pthread_mutex_unlock(&pool_lock); // 記得解鎖
        return NULL;
    }

    // 1. 拿出空閒區塊
    FreeNode* block = free_list_head;

    // 2. 移動 Head
    free_list_head = free_list_head->next;

    // 3. 設定 Canary (金絲雀)
    // 我們把 block 轉型成 Header 指標，寫入 Magic Number
    BlockHeader* header = (BlockHeader*)block;
    header->canary = CANARY_MAGIC;

    // 【解鎖】
    pthread_mutex_unlock(&pool_lock);

    // 4. 回傳給使用者的位址，要跳過 Header
    // 使用者只會看到 Data 區域，不知道前面有 Header
    return (void*)((char*)block + sizeof(BlockHeader));
}

// 釋放記憶體
void pool_free(void* ptr) {
    if (ptr == NULL) return;

    // 【上鎖】
    pthread_mutex_lock(&pool_lock);

    // 1. 推算出原本的區塊起始位置 (使用者給的是 Data，我們要往回推找到 Header)
    char* block_start = (char*)ptr - sizeof(BlockHeader);
    
    // 檢查範圍 (安全性檢查)
    if (block_start < memory_pool || block_start >= memory_pool + (POOL_SIZE * TOTAL_BLOCK_SIZE)) {
        printf("Error: Pointer not in pool!\n");
        pthread_mutex_unlock(&pool_lock);
        return;
    }

    // 2. 檢查 Canary (關鍵！)
    BlockHeader* header = (BlockHeader*)block_start;
    if (header->canary != CANARY_MAGIC) {
        printf("!!! MEMORY CORRUPTION DETECTED !!!\n");
        printf("Canary value is 0x%X, expected 0x%X\n", header->canary, CANARY_MAGIC);
        printf("Buffer Overflow likely occurred in the previous block usage.\n");
        // 在真實系統中，這裡可能會 panic 或 assert
        pthread_mutex_unlock(&pool_lock);
        return;
    }

    // 3. 將區塊加回 Free List
    // 這裡我們把整個區塊 (從 Header 開始) 再次當作 FreeNode
    FreeNode* node_to_return = (FreeNode*)block_start;
    node_to_return->next = free_list_head;
    free_list_head = node_to_return;

    // 【解鎖】
    pthread_mutex_unlock(&pool_lock);
}