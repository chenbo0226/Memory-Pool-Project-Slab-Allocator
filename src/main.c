#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h> // 多執行緒用
#include <unistd.h>  // 為了用 usleep
#include "memory_pool.h"


// 新增 給多執行緒跑的極簡任務：一直申請又釋放
void* thread_work(void* arg) {
    int id = *(int*)arg;
    for (int i = 0; i < 3; i++) { // 做 3 次
        void* p = pool_alloc();
        if (p) {
            printf("[Thread %d] Alloc %p\n", id, p);
            usleep(100); // 休息 0.1ms 讓其他執行緒有機會搶入
            pool_free(p);
        }
    }
    return NULL;
}

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

    // === 新增 測試 7: Canary 破壞測試 ===
    // 這裡模擬，故意寫壞 Header 來看 pool_free 有沒有報警
    printf("\n-------------------------Canary Test--------------------------\n");
    char* p_bad = (char*)pool_alloc();
    if (p_bad) {
        // 往回推 4 bytes (一個 int) 就是 Header
        unsigned int* header = (unsigned int*)p_bad - 1; 
        *header = 0xDEAD0000; // 把原本的 0xDEADBEEF 改壞
        printf("\nCorrupted header at %p. Freeing...\n", p_bad);
        pool_free(p_bad); // 預期結果：印出 !!! MEMORY CORRUPTION DETECTED !!!
    }

    // === 新增 測試 8: Mutex 多執行緒測試 ===
    printf("\n-------------------------Mutex Test--------------------------\n");
    pthread_t t1, t2;
    int id1 = 1, id2 = 2;
    // 啟動兩個執行緒同時搶資源
    pthread_create(&t1, NULL, thread_work, &id1);
    pthread_create(&t2, NULL, thread_work, &id2);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("\nMulti-thread test finished.\n");

  

    // 暫停視窗 (清空緩衝區 + 等待輸入)
    //while (getchar() != '\n');
    //getchar();
    return 0;
}


