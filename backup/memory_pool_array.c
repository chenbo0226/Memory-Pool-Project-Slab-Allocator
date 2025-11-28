#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define BLOCK_SIZE 32
#define NUM_BLOCKS 100


// 宣告一塊 100 * 32 bytes 的連續記憶體 memory pool
static char memory_pool[NUM_BLOCKS][BLOCK_SIZE];

// 宣告一個「管理員」來追蹤哪些區塊被使用了
// (先試試布林陣列檢查該陣列是否 "是空的" or "以配置")
// true 是表示 "以配置"  false 表示 "是空的"
static bool is_used_block[NUM_BLOCKS];

// 初始化 memory_pool
void pool_init()
{
    for (int i = 0; i < NUM_BLOCKS; i++) {
        is_used_block[i] = false;
    }
    printf("Memory pool initialized.\n");
}

// 配置memory_pool
void* my_alloc()
{
    for (int i = 0; i < NUM_BLOCKS; i++) {

        // 如果找到一個未使用的區塊  
        if (!is_used_block[i]) {

            //標記為已使用
            is_used_block[i] = true;

            //顯示 配置第幾個區塊 和 它的位址的值
            printf("Allocated block %d at address %p\n", i, (void*)memory_pool[i]);
            return (void*)memory_pool[i];
        }
        
    }
    //如果遍歷所有區塊都找不到空位，回傳NULL
    printf("Allocation failed: No free block\n");
    return NULL;
    
}

//釋放memory_pool
void my_free(void *ptr)
{
    if (ptr == NULL) {
        return;
    }

    // 透過指標運算，計算出 ptr 是第幾個區塊
    // (char*) ptr 是傳入的地址
    // (char*) memory_pool 是記憶體池的起始地址
    // 相減得到「位移量」，再除以「區塊大小」
    int index = ((char*)ptr - (char*)memory_pool) / BLOCK_SIZE;

    if (index >= 0 && index < NUM_BLOCKS) {
        if (is_used_block[index]) {
            //釋放掉空間了，標記為false
            is_used_block[index] = false;
            printf("釋放掉第 %d 的區塊 在位址 %p\n", index, ptr);          
        }
        else{
            printf("警告要釋放已經是空區塊");
        }
    }
    else{
        printf("錯誤配置區塊，配置的區塊不在記憶體池中");
    }

    
}



int main()
{
    pool_init(); //1.初始化記憶體池
    printf("--------------------------------------------------------------\n\n");
    

    // 2. 嘗試配置第一個區塊 (p1)，並使用它
    void* p1 = my_alloc();
    if (p1 != NULL) {
        // 使用 p1 (例如：複製一個字串進去)
        strcpy((char*)p1, "第一個區塊");
        printf("Data in p1: %s\n", (char*)p1);
    }
    printf("\n--------------------------------------------------------------\n\n");
   

    // 3. 嘗試配置第二個區塊 (p2)
    void* p2 = my_alloc(); // 應該會得到 block 1
    if (p2 != NULL) {
        strcpy((char*)p2, "第二個區塊");
        printf("Data in p2: %s\n", (char*)p2);
    }
    printf("\n--------------------------------------------------------------\n\n");
   

    // 4. 釋放第一個區塊(p1)
    my_free(p1);

    // (現在 p1 指向的 memory_pool[0] 應該是空的了)
    printf("\n--------------------------------------------------------------\n\n");
    

    // 5. 嘗試配置第三個區塊 (p3)
    // 因為 block 0 剛被釋放，"my_alloc" 應該會優先找到它
    void* p3 = my_alloc();
    if (p3 != NULL) {
        printf("Data in p3 (should be old p1 data): %s\n", (char*)p3); // 舊資料可能還在
        strcpy((char*)p3, "重配置p3");
        printf("New Data in p3: %s\n", (char*)p3);
    }
    printf("\n--------------------------------------------------------------\n\n");
    

    // 6. 檢查 p1 和 p3 的地址 (應該要一樣)
    printf("Address of p1: %p\n", p1);
    printf("Address of p3: %p\n", p3);
    if (p1 == p3) {
        printf("Success: p3 re-used p1's memory block.\n");
    }

    // (釋放剩下的記憶體)
    my_free(p2);
    my_free(p3);    

    while (getchar() != '\n');
    getchar();
    //system("pause");
    return 0;
}