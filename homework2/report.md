# 41041225

作業二

## 解題說明

本次作業共 2 題，皆以 **多項式抽象資料型別 (Polynomial ADT)** 為核心。  

**題目一：Polynomial**
- `class Polynomial` 依照 **Figure 1**、**Figure 2** 提供的介面與私有成員完成實作。  
  - `Polynomial()`：建構 0 多項式  
  - `Add(poly)`：兩條「指數遞減」序列做 **merge** 加法  
  - `Mult(poly)`：雙迴圈相乘，係數累加於暫存桶，再遞減輸出  
  - `Eval(x)`：計算 `Σ coef × x^exp`
- 私有結構 `Term { float coef; int exp; }` —— 只存 **非零項** 以節省稀疏多項式空間  
- 動態陣列 `termArray` 搭配 `capacity / terms` 手動管理記憶體

**題目二：Polynomial I/O**
- 過載運算子 `>>`、`<<` 以支援直覺的輸入 / 輸出  
  - `operator>>`：讀入「項數 *k* + *k* 組 `<coef exp>`」並呼叫 `attach()` 整理  
  - `operator<<`：輸出 `3x^2 + 2x - 4` 形式（首項符號、省略 ±1、處理 exp = 0/1）

## 解題策略

**題目一：Polynomial**
1. 骨架：先將 Figure 1 介面寫成空函式，確保能編譯。
2. 資源管理：實作建構子、`copy ctor`、`operator=`、`destructor`（Rule of 3/5）。
3. 小工具：
   - `ensureCapacity()`：容量不足時以 2 倍擴充並搬移資料  
   - `attach(coef, exp)`：  
     - `coef == 0` 直接 return  
     - 若最後一項 `exp` 相同 ➜ 係數相加；結果為 0 時刪除該項  
     - 其他情況直接將 `<coef, exp>` 追加（已保證指數遞減）
4. Add：
   - 兩指標 `i, j` 走訪兩個 `termArray`  
   - 比較 `exp` 大小 → 決定直接 `attach()` 或係數相加  
   - 時間複雜度 `O(m + n)`
5. Mult：
   - 計算最大指數：`maxExp = A.exp0 + B.exp0`  
   - 配置暫存桶 `float* bucket = new float[maxExp + 1]`，全部置 0  
   - 雙迴圈：`bucket[e_i + e_j] += c_i * c_j`  
   - 由 `exp = maxExp` 逆向掃描 bucket，對非零項呼叫 `attach()`  
   - 完成後 `delete[] bucket` 釋放記憶體
6. Eval：`for` 迴圈累加 `coef * pow(x, exp)`。
7. 單元測試：
   - 手算驗證 `(x + 1) * (x + 1) = x^2 + 2x + 1`、  
     `(3x^2 + 2x - 4) + (x + 1) = 3x^2 + 3x - 3` 等。  
   - 亦可寫小函式以 **陣列** 形式重算結果，比對正確性。

**題目二：Polynomial I/O**
1. `operator>>`  
   - 讀整數 *k*；建立暫時物件 `tmp`  
   - 迴圈讀 *k* 組 `(coef exp)` → `tmp.attach()`  
   - 結束後 `p = tmp`（觸發 copy-assignment）
2. `operator<<`  
   - 若 `terms == 0` → 輸出 `0`  
   - 走訪 `termArray`：  
     - 處理首項 / 正負號  
     - 係數 ±1 & 指數 ≠ 0 → 省略 1  
     - `exp == 1` 印 `x`；`exp == 0` 只印係數

## 程式實作

**header.h**
```cpp
```
**main.cpp**
```cpp
```

## 效能分析

**題目一：**
1. **時間複雜度**：
2. **空間複雜度**：


**題目二：**
1. **時間複雜度**：
2. **空間複雜度**：

## 測試與驗證

### 測試案例

| 測試案例 | 題目編號 | 輸入參數 | 預期輸出 | 實際輸出 |
|---------|---------|---------|---------|---------|
| 測試一 | 題目一 |  |  |  |
| 測試二 | 題目一 |  |  |  |
| 測試三 | 題目一 |  |  |  |
| 測試四 | 題目二 |  |  |  |
| 測試五 | 題目二 |  |  |  |

### 編譯與執行指令

```bash
cd homework2/src
g++ -std=c++17 -Wall -O2 -I. -o hw2 main.cpp
./hw2
```

### 結論

## 申論及開發報告
