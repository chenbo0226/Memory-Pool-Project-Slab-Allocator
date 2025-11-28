# Fixed-Size-Memory-Pool

專案名稱: 專案名稱: 嵌入式 O(1) 固定區塊記憶體池 (Fixed-Size Memory Pool)

專案動機: 這個專案為銜接韌體實務(探討malloc碎片化問題)，自主於Linux環境實作客製化記憶體池


實作細節:

捨棄傳統 O(N) 陣列搜尋，改用 Linked List (Free List) 管理空閒區塊

利用Pointer 技巧，不需額外 malloc 管理節點，直接在閒置記憶體上建立結構

實作 Stack (LIFO) 行為，達成 Alloc/Free 皆為 O(1) 的極致效能

規劃：導入 Thread-Safety 機制以支援 多工環境



