# 41041225

作業一

## 解題說明

本次作業共 2 題，皆以 **遞迴 (recursive)** 為核心。

1. **題目一 Ackermann 函式**  
   * `AckermannRec(m,n)`：依數學定義直接遞迴  
   * `AckermannIter(m,n)`：以「固定大小陣列」手動模擬呼叫堆疊（非遞迴）

2. **題目二 Power Set (冪集)**  
   * 對集合 `S`（大小為 `n`）列舉所有 `2^n` 個子集合  
   * 使用遞迴 DFS：每個元素「選／不選」兩條分支

### 解題策略

#### 題目一 Ackermann

* **遞迴版**  
  若 `m == 0` → 回傳 `n + 1`  
  若 `n == 0` → 呼叫 `A(m-1, 1)`  
  其他情況 → 呼叫 `A(m-1, A(m, n-1))`

* **非遞迴版**  
  1. 準備靜態陣列 `stack[MAX]` 與整數 `top` 作為手動堆疊  
  2. push 第一個 **Frame** `(m,n)`  
  3. 依三種情況調整 **新的 `(m,n)`，並 push / pop 整個 Frame**  
  4. 當 `top < 0` 時結束（表示最外層已取回結果）

#### 題目二 Power Set

* 參數 `idx` 表示目前處理到的元素位置  
* 以布林陣列 `chosen[n]` 紀錄是否選取  
* 遞迴到 `idx == n` 時輸出一次子集合  

## 程式實作

### header.h
```cpp
#ifndef HOMEWORK1_HEADER_H
#define HOMEWORK1_HEADER_H

// ---------- Problem 1 : Ackermann ----------
unsigned long long AckermannRec(unsigned int m, unsigned int n);  // 遞迴
unsigned long long AckermannIter(unsigned int m, unsigned int n); // 非遞迴

// ---------- Problem 2 : Power-Set ----------
void PowerSet(const char *set, int n); // 直接把所有子集合輸出到螢幕

#endif // HOMEWORK1_HEADER_H
```
### main.cpp
```cpp
/************************************************************
 *  homework1/src/main.cpp
 *  ---------------------------------------------------------
 *  Problem 1 : Ackermann Function
 *      - AckermannRec  ──► Recursive  version
 *      - AckermannIter ──► Iterative version (non-recursive, using a fixed-size array as stack)
 *
 *  Problem 2 : Power-Set
 *      - PowerSet      ──► List every subset of a set S
 *
 *  NOTE : 本檔只使用 <iostream> 其餘容器一律以原生陣列實作。
 ************************************************************/

#include <iostream>
#include <climits>
#include "header.h"

using std::cout;
using std::endl;

/* ============== Problem 1 : Ackermann ================= */
/* ---------- Recursive version ---------- */
unsigned long long AckermannRec(unsigned int m, unsigned int n)
{
    if (m == 0)          return n + 1;
    else if (n == 0)     return AckermannRec(m - 1, 1);
    else                 return AckermannRec(m - 1,
                                 AckermannRec(m, n - 1));
}

/* ---------- Iterative version (non-recursive) ---------- */
unsigned long long AckermannIter(unsigned int m, unsigned int n)
{
    constexpr int MAX_STACK = 500000;      // 若要更大可再調
    unsigned int stack[MAX_STACK];
    int top = -1;

    while (true)
    {
        if (m == 0)                // A(0,n) = n+1
        {
            n = n + 1;
            if (top < 0)           // 已回到最外層 → 結束
                return n;
            m = stack[top--];      // pop 回上一層 m
        }
        else if (n == 0)           // A(m,0) = A(m-1,1)
        {
            m = m - 1;
            n = 1;
        }
        else                       // A(m,n) = A(m-1, A(m,n-1))
        {
            stack[++top] = m - 1;  // 先記住 (m-1)
            n = n - 1;             // 繼續計算 A(m, n-1)
            // m 保持不變，迴圈再跑一次
        }

        if (top >= MAX_STACK - 1)  // 安全檢查
        {
            std::cout << "Stack overflow in AckermannIter\n";
            return 0;
        }
    }
}

/* ============== Problem 2 : Power-Set ================== */
static void listSubset(const char *set, int n, int idx,
                       bool *chosen)
{
    if (idx == n)                       // 列印一個子集合
    {
        cout << "{ ";
        for (int i = 0; i < n; ++i)
            if (chosen[i]) cout << set[i] << ' ';
        cout << "}" << endl;
        return;
    }

    /* 不選第 idx 個 */
    chosen[idx] = false;
    listSubset(set, n, idx + 1, chosen);

    /* 選第 idx 個 */
    chosen[idx] = true;
    listSubset(set, n, idx + 1, chosen);
}

void PowerSet(const char *set, int n)
{
    bool *chosen = new bool[n];
    listSubset(set, n, 0, chosen);
    delete [] chosen;
}

/* ==================== TEST ==================== */
int main()
{
    cout << "===== Ackermann =====" << endl;
    cout << "A(2,3)  Recursive : " << AckermannRec(2, 3) << endl;
    cout << "A(2,3)  Iterative : " << AckermannIter(2, 3) << endl;
    cout << "A(3,2)  Recursive : " << AckermannRec(3, 2) << endl;
    cout << "A(3,2)  Iterative : " << AckermannIter(3, 2) << endl;

    cout << "\n===== Power-Set =====" << endl;
    const char S[3] = {'a', 'b', 'c'};
    PowerSet(S, 3);

    return 0;
}
```

## 效能分析

### 題目一 ─ Ackermann 函式
1. **時間複雜度**  
   Ackermann 函式的時間複雜度近指數級，隨 `m`、`n` 急遽爆炸。  

2. **空間複雜度**  
   * 遞迴版：系統呼叫堆疊，約為 $O(\text{呼叫深度})$。  
   * 非遞迴版：以固定陣列模擬堆疊，空間複雜度為 $O(k)$，其中 $k=\text{MAX\_STACK}$。

### 題目二 ─ Power Set（冪集）
1. **時間複雜度**  
   列舉所有子集合必須 $O(2^n)$。  

2. **空間複雜度**  
   遞迴同時僅維持一路徑，空間複雜度為 $O(n)$。

## 測試與驗證

### 測試案例

| 測試案例 | 題目編號 | 輸入參數 | 預期輸出 | 實際輸出 |
|---------|---------|---------|---------|---------|
| 測試一 | 題目一 | `A(0,0)`  | 1        | 1       |
| 測試二 | 題目一 | `A(1,2)`  | 4        | 4       |
| 測試三 | 題目一 | `A(2,3)`  | 9        | 9       |
| 測試四 | 題目二 | `{a,b}`   | 4 子集合 | 相符     |
| 測試五 | 題目二 | `{a,b,c}` | 8 子集合 | 相符     |

### 編譯與執行指令

```bash
cd homework1/src
g++ -std=c++17 -Wall -O2 -I. -o hw1 main.cpp
./hw1
```

### 結論

1. 兩版 **Ackermann** 在 `m, n ≤ 3` 時輸出一致；  
 *遞迴版* 簡潔直觀，*迭代版* 透過手動堆疊可突破語言遞迴深度限制（可調整 `MAX_STACK`）。
2. **Power Set** 函式正確列舉 `2^n` 個子集合，結果與預期相符。  
3. 程式全程未使用 `vector` 或其他 STL 容器，完全符合作業規範。

## 申論及開發報告

### 選擇遞迴的原因

* **對應數學定義直觀**  
  Ackermann 函式與冪集列舉皆以遞迴形式定義；以遞迴實作能直接映射數學公式，最易於驗證正確性。  

* **程式碼簡潔易讀**  
  遞迴能將「大問題」拆解為「同型態的小問題」，程式碼行數少且語意清楚，有助於教學與維護。  

* **訓練思維**  
  本作業目標在培養遞迴與堆疊的概念；先用遞迴確立正確邏輯，再以迭代版本體驗手動管理呼叫堆疊的差異。  

> **備註**  
> 為避免深層遞迴造成 Stack Overflow，額外提供「固定陣列堆疊」迭代實作，並可依需求調整 `MAX_STACK` 支援較大的輸入範圍。
