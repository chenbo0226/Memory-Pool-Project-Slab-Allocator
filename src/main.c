#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "memory_pool.h"

int main()
{
    pool_init(); //1.初始化記憶體池
    printf("--------------------------------------------------------------\n\n");

    // 2. 嘗試配置第一個區塊 (p1)，並使用它
    char* p1 = (char*)pool_alloc();
    if (p1 != NULL) {
        // 使用 snprintf 來寫入資料，比 strcpy 安全 (防止 buffer overflow)
        snprintf(p1, BLOCK_SIZE, "Data Block 1");
        printf("[Alloc] p1 allocated at: %p \tContent: %s\n", p1, p1);
    }
    printf("\n");

    // 3. 嘗試配置第一個區塊 (p2)
    char* p2 = (char*)pool_alloc();
    if (p2 != NULL) {
        snprintf(p2, BLOCK_SIZE, "Data Block 2");
        printf("[Alloc] p2 allocated at: %p \tContent: %s\n", p2, p2);
    }
    printf("\n");

    // 4. 釋放第一個區塊 (p1)
    printf("[Free ] Freeing p1 (%p)...\n", p1);
    pool_free(p1);

    printf("--------------------------------------------------------------\n\n");

    // 5. 嘗試配置第三個區塊 (p3)
    // 因為剛剛 p1 被插回 List 的最前面 (Head)
    // 現在 alloc，應該要馬上拿到剛剛釋放的 p1 位址
    char* p3 = (char*)pool_alloc();
    if (p3 != NULL) {
        printf("[Alloc] p3 allocated at: %p\n", p3);
        
        // 寫入新資料
        snprintf(p3, BLOCK_SIZE, "Data: Block 3 (Reused)");
        printf("        New Content in p3: %s\n", p3);
    }
    printf("\n");

    // 6. 驗證結果
    printf("--------------------------------------------------------------\n");
    printf("Address Check:\n");
    printf("  p1 addr: %p\n", p1);
    printf("  p3 addr: %p\n", p3);

    if (p1 == p3) {
        printf("\n[Success] p3 successfully reused p1's memory block!\n");
        printf("          This confirms the LIFO (Stack-like) behavior of the allocator.\n");
    } else {
        printf("\n[Fail] p3 did not reuse p1. Check pool_free logic.\n");
    }
    printf("--------------------------------------------------------------\n");

    // 釋放剩下的資源 
    pool_free(p2);
    pool_free(p3);


    // 暫停視窗 (清空緩衝區 + 等待輸入)
    //while (getchar() != '\n');
    //getchar();
    return 0;
}


