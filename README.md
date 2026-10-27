#  Fixed-Size Block Memory Pool in C

## 📌 專案動機
探索固定區塊配置與 O(1) Free List 管理方式。
本專案為銜接韌體實務，**自主於 Linux 環境實作** 高效能記憶體池。

## ✨ 核心技術
* **Free List**：捨棄傳統 O(N) 陣列搜尋，改用 **Linked List (Free List)** 管理空閒區塊。
* **Embedded Pointer**：利用閒置區塊儲存 Next Pointer，減少額外管理節點的記憶體需求。
* **LIFO 行為**：實作 Stack 行為，釋放的區塊優先被重用，提升 Cache Locality。
* **Synchronization & Canary**： 使用 pthread mutex 保護共享 Free List，並加入 Canary 檢查機制，偵測特定位置的記憶體破壞

## 🛠️ 實作細節

### 核心結構
我們利用 **Pointer Casting (指標轉型)** 技巧，將閒置區塊的前 8 bytes 強制轉型為 `next` 指標使用：

```c
typedef struct FreeNode {
    struct FreeNode* next;
} FreeNode;

// 記憶體池 (Static Allocation)
static char memory_pool[POOL_SIZE * BLOCK_SIZE];
```
### 效能比較

| 方法 | 配置 (Alloc) | 釋放 (Free) | 額外開銷 |
| :--- | :--- | :--- | :--- |
| **陣列搜尋 (Array)** | O(N) | O(1) | O(N) (bool array) |
| **鏈結串列 (This Project)** | **O(1)** | **O(1)** | **Zero** |

## 🔮 測試與改進
* 已建立基本配置與釋放、LIFO 重用、Canary 異常及多執行緒操作測試；目前仍有 Memory Alignment 與釋放指標驗證等待改善項目
* **(此專案為個人自主研習項目，專注於鞏固記憶體管理觀念)**
