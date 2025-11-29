# 嵌入式 O(1) 固定區塊記憶體池 (Fixed-Size Memory Pool)

## 📌 專案動機
標準 C 語言的 `malloc/free` 在頻繁配置與釋放時，容易產生 **記憶體碎片 (Fragmentation)**，且時間複雜度不穩定。
本專案為銜接韌體實務，**自主於 Linux 環境實作** 高效能記憶體池，解決上述問題。

## ✨ 核心技術
* **O(1) 極致效能**：捨棄傳統 O(N) 陣列搜尋，改用 **Linked List (Free List)** 管理空閒區塊。
* **Embedded Pointer**：利用指標技巧，**不需額外 malloc 管理節點**，直接在閒置記憶體上建立結構 (Zero Overhead)。
* **LIFO 行為**：實作 Stack 行為，釋放的區塊優先被重用，提升 Cache Locality。

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

## 🔮 未來規劃
* **導入 Thread-Safety 機制** (Mutex/Spinlock) 以支援多工環境。
* **加入 Canary (金絲雀)** 機制偵測 Buffer Overflow。
