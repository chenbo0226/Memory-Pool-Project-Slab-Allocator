#ifndef MEMORY_POOL_H

#define MEMORY_POOL_H

#include <stddef.h> // 為了讓 .h 認識 NULL 或 size_t (雖然這裡沒用到)


// 1. 定義常數
#define BLOCK_SIZE 32
#define POOL_SIZE 10  

// 2. 函式宣告 (Prototypes) - 告訴 main 說這些函式長什麼樣子
// 注意：不要把變數定義放在這裡！
void pool_init();
void* pool_alloc();
void pool_free();

#endif