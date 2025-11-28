#include <stdio.h>
#include <stdlib.h>
#include "memory_pool.h"


//用這個指標型別來「解釋」記憶體
typedef struct FreeNode {
    struct FreeNode* next;
} FreeNode;

// 真正的記憶體池  10*32
static char memory_pool[POOL_SIZE * BLOCK_SIZE];

// 指向第一個空閒區塊的指標 
FreeNode* free_list_head = NULL;

//初始化memory pool
void pool_init() {
    free_list_head = (FreeNode*)memory_pool; // 頭指向記憶體起點
    FreeNode* current = free_list_head;

    for (int i = 0; i < POOL_SIZE - 1; i++) {
        // 計算下一塊的位址：當前位址 + 32 bytes
        // 這裡要轉成 (char*) 才能做 byte 級別的加法，再轉回 (FreeNode*)
        FreeNode* next_node = (FreeNode*)((char*)current + BLOCK_SIZE);
        
        current->next = next_node; // 當前塊指向下一塊
        current = next_node;       // 移動到下一塊
    }
    
    current->next = NULL; // 最後一塊指向 NULL

    printf("Memory Pool Initialized. Block Size: %d, Count: %d\n", BLOCK_SIZE, POOL_SIZE);
}

//配置記憶體
void* pool_alloc() {
    if (free_list_head == NULL) {
        printf("Out of Memory!\n");
        return NULL;
    }

    // 1. 拿出頭部指向的區塊
    FreeNode* block_to_give = free_list_head;

    // 2. 把頭部移往下一格 (Pop from Stack)
    free_list_head = free_list_head->next;

    // 3. 回傳這塊記憶體 (轉成 void* 給使用者用)
    // 使用者拿到後，會覆蓋掉原本裡面的 next 指標，但沒關係，因為它已經不是空閒的了
    return (void*)block_to_give;
}

//釋放記憶體池
void pool_free(void* ptr) {
    if (ptr == NULL) return;

    // 檢查指標是否在我們的 Pool 範圍內 
    // ptr要是  memory_pool < ptr <= memory_pool + 10 * 32
    if (ptr < (void*)memory_pool || ptr >= (void*)(memory_pool + POOL_SIZE * BLOCK_SIZE)) {
        printf("Error: Pointer not in pool!\n");
        return;
    }

    // 1. 把傳回來的 void* 當作 FreeNode* 看待
    FreeNode* node_to_return = (FreeNode*)ptr;

    // 2. 頭插法 (Insert at Head)：
    //    這塊的 next 指向原本的頭
    node_to_return->next = free_list_head;

    // 3. 更新 指向的頭，讓頭指向這塊剛回來的
    free_list_head = node_to_return;
}


