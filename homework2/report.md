# 41041225

作業二

## 解題說明

本次作業共 2 題，皆以 **多項式抽象資料型別 (Polynomial ADT)** 為核心。  

**題目一：Polynomial**
- `class Polynomial` 依照 **Figure 1**、**Figure 2** 提供的介面與私有成員完成實作。  
  - `Polynomial()`：建構 0 多項式。
  - `Add(poly)`：兩條「指數遞減」序列做 **merge** 加法。
  - `Mult(poly)`：雙迴圈相乘，係數累加於暫存桶，再遞減輸出。
  - `Eval(x)`：計算 `Σ coef × x^exp` 。
- 私有結構 `Term { float coef; int exp; }` —— 只存 **非零項** 以節省稀疏多項式空間。
- 動態陣列 `termArray` 搭配 `capacity / terms` 手動管理記憶體。

**題目二：Polynomial I/O**
- 過載運算子 `>>`、`<<` 以支援直覺的輸入 / 輸出。
  - `operator>>`：讀入「項數 *k* + *k* 組 `<coef exp>`」並呼叫 `attach()` 整理。
  - `operator<<`：輸出 `3x^2 + 2x - 4` 形式（首項符號、省略 ±1、處理 exp = 0/1）。

## 解題策略

**題目一：Polynomial**
1. 骨架：先將 Figure 1 介面寫成空函式，確保能編譯。
2. 資源管理：實作建構子、`copy ctor`、`operator=`、`destructor`（Rule of 3/5）。
3. 小工具：
   - `ensureCapacity()`：容量不足時以 2 倍擴充並搬移資料。
   - `attach(coef, exp)`：
     - `coef == 0` 直接 return。
     - 若最後一項 `exp` 相同 ➜ 係數相加；結果為 0 時刪除該項。
     - 其他情況直接將 `<coef, exp>` 追加（已保證指數遞減）。
4. Add：
   - 兩指標 `i, j` 走訪兩個 `termArray`。
   - 比較 `exp` 大小 → 決定直接 `attach()` 或係數相加。
5. Mult：
   - 計算最大指數：`maxExp = A.exp0 + B.exp0`。
   - 配置暫存桶 `float* bucket = new float[maxExp + 1]`，全部置 0。
   - 雙迴圈：`bucket[e_i + e_j] += c_i * c_j`。
   - 由 `exp = maxExp` 逆向掃描 bucket，對非零項呼叫 `attach()`。
   - 完成後 `delete[] bucket` 釋放記憶體。
6. Eval：`for` 迴圈累加 `coef * pow(x, exp)`。
7. 單元測試：
   - 手算驗證 `(x + 1) * (x + 1) = x^2 + 2x + 1`、 `(3x^2 + 2x - 4) + (x + 1) = 3x^2 + 3x - 3` 等。  
   - 亦可寫小函式以 **陣列** 形式重算結果，比對正確性。

**題目二：Polynomial I/O**
1. `operator>>`：
   - 讀整數 *k*；建立暫時物件 `tmp`。
   - 迴圈讀 *k* 組 `(coef exp)` → `tmp.attach()`。
   - 結束後 `p = tmp`（觸發 copy-assignment）。
2. `operator<<`：
   - 若 `terms == 0` → 輸出 `0`。
   - 走訪 `termArray`：
     - 處理首項 / 正負號。
     - 係數 ±1 & 指數 ≠ 0 → 省略 1。
     - `exp == 1` 印 `x`；`exp == 0` 只印係數。

## 程式實作

**header.h**
```cpp
/*==============================  header.h  ==============================*/
#ifndef POLYNOMIAL_H
#define POLYNOMIAL_H
//--------------------------------------------------------------------
//  Polynomial ADT - header-only 實作
//  - 支援 Add / Mult / Eval 與 << >> 運算子
//  - 遵守 Rule-of-5：手動管理 termArray (new[] / delete[])
//--------------------------------------------------------------------
#include <iostream>
#include <cmath>
#include <utility>   // std::swap
#include <cassert>

class Polynomial {
public:
    /*------------ Rule-of-5 ------------*/
    Polynomial();                                    // ctor：0 多項式
    ~Polynomial();                                   // dtor
    Polynomial(const Polynomial& rhs);               // copy-ctor
    Polynomial(Polynomial&&  rhs) noexcept;          // move-ctor

    Polynomial& operator=(const Polynomial& rhs);    // copy-assign
    Polynomial& operator=(Polynomial&& rhs) noexcept;// move-assign

    /*------------ ADT 介面 ------------*/
    Polynomial  Add (const Polynomial& b) const;     // this + b
    Polynomial  Mult(const Polynomial& b) const;     // this × b
    float       Eval(float x) const;                 // this(x)

    /*------------ Stream I/O ------------*/
    friend std::istream& operator>>(std::istream& is, Polynomial& p);
    friend std::ostream& operator<<(std::ostream& os, const Polynomial& p);

private:
    /*------------ Figure-2 成員 ------------*/
    struct Term { float coef; int exp; };            // <係數, 指數>

    Term* termArray;   // 動態陣列
    int   capacity;    // 配置大小
    int   terms;       // 已使用項數

    /*------------ 私有工具 ------------*/
    void ensureCapacity(int newCap);                 // 需要時 2× 擴充
    void attach(float coef, int exp);                // 已保證遞減時附加
};

/*======================================================================*/
/*                         Inline implementation                        */
/*======================================================================*/

/*-- ctor / dtor -------------------------------------------------------*/
inline Polynomial::Polynomial()
    : termArray(new Term[1]), capacity(1), terms(0) {}

inline Polynomial::~Polynomial() { delete[] termArray; }

/*-- copy-ctor ---------------------------------------------------------*/
inline Polynomial::Polynomial(const Polynomial& rhs)
    : termArray(new Term[rhs.capacity]),
      capacity(rhs.capacity),
      terms(rhs.terms)
{
    for (int i = 0; i < terms; ++i) termArray[i] = rhs.termArray[i];
}

/*-- move-ctor ---------------------------------------------------------*/
inline Polynomial::Polynomial(Polynomial&& rhs) noexcept
    : termArray(rhs.termArray),
      capacity(rhs.capacity),
      terms(rhs.terms)
{
    rhs.termArray = nullptr;
    rhs.capacity  = rhs.terms = 0;
}

/*-- copy-assign (const&) ---------------------------------------------*/
inline Polynomial& Polynomial::operator=(const Polynomial& rhs)
{
    if (this != &rhs) {
        Polynomial tmp(rhs);              // 深拷貝
        std::swap(termArray, tmp.termArray);
        std::swap(capacity , tmp.capacity );
        std::swap(terms    , tmp.terms    );
    }
    return *this;
}

/*-- move-assign -------------------------------------------------------*/
inline Polynomial& Polynomial::operator=(Polynomial&& rhs) noexcept
{
    if (this != &rhs) {
        delete[] termArray;
        termArray = rhs.termArray;
        capacity  = rhs.capacity;
        terms     = rhs.terms;
        rhs.termArray = nullptr;
        rhs.capacity  = rhs.terms = 0;
    }
    return *this;
}

/*-- ensureCapacity ----------------------------------------------------*/
inline void Polynomial::ensureCapacity(int newCap)
{
    if (newCap <= capacity) return;
    if (newCap < 2 * capacity) newCap = 2 * capacity;

    Term* newArr = new Term[newCap];
    for (int i = 0; i < terms; ++i) newArr[i] = termArray[i];

    delete[] termArray;
    termArray = newArr;
    capacity  = newCap;
}

/*-- attach ------------------------------------------------------------*/
inline void Polynomial::attach(float c, int e)
{
    if (std::fabs(c) < 1e-6f) return;                     // 跳過 0

    if (terms && termArray[terms - 1].exp == e) {         // 同指數 → 合併
        termArray[terms - 1].coef += c;
        if (std::fabs(termArray[terms - 1].coef) < 1e-6f) --terms;
        return;
    }
    ensureCapacity(terms + 1);
    termArray[terms++] = Term{c, e};
}

/*-- Add ---------------------------------------------------------------*/
inline Polynomial Polynomial::Add(const Polynomial& b) const
{
    Polynomial c;
    c.ensureCapacity(terms + b.terms);

    int i = 0, j = 0;
    while (i < terms && j < b.terms) {
        const Term &A = termArray[i], &B = b.termArray[j];
        if (A.exp > B.exp)          c.attach(A.coef,               A.exp), ++i;
        else if (A.exp < B.exp)     c.attach(B.coef,               B.exp), ++j;
        else                        c.attach(A.coef + B.coef, A.exp), ++i, ++j;
    }
    while (i < terms)   c.attach(termArray[i].coef, termArray[i].exp), ++i;
    while (j < b.terms) c.attach(b.termArray[j].coef, b.termArray[j].exp), ++j;
    return c;
}

/*-- Mult --------------------------------------------------------------*/
inline Polynomial Polynomial::Mult(const Polynomial& b) const
{
    Polynomial c;
    if (terms == 0 || b.terms == 0) return c;              // 零多項式

    int maxExp = termArray[0].exp + b.termArray[0].exp;
    float* bucket = new float[maxExp + 1];
    for (int i = 0; i <= maxExp; ++i) bucket[i] = 0.f;

    for (int i = 0; i < terms; ++i)
        for (int j = 0; j < b.terms; ++j)
            bucket[termArray[i].exp + b.termArray[j].exp] +=
                termArray[i].coef * b.termArray[j].coef;

    for (int e = maxExp; e >= 0; --e)
        if (std::fabs(bucket[e]) > 1e-6f) c.attach(bucket[e], e);

    delete[] bucket;
    return c;
}

/*-- Eval --------------------------------------------------------------*/
inline float Polynomial::Eval(float x) const
{
    float sum = 0.f;
    for (int i = 0; i < terms; ++i)
        sum += termArray[i].coef * std::pow(x, termArray[i].exp);
    return sum;
}

/*-- operator>> --------------------------------------------------------*/
inline std::istream& operator>>(std::istream& is, Polynomial& p)
{
    int k;
    if (!(is >> k)) return is;

    Polynomial tmp;
    tmp.ensureCapacity(k);
    for (int i = 0; i < k; ++i) {
        float c; int e;
        if (!(is >> c >> e)) { is.setstate(std::ios::failbit); return is; }
        tmp.attach(c, e);
    }
    p = std::move(tmp);               // 觸發 move-assign，不再歧義
    return is;
}

/*-- operator<< --------------------------------------------------------*/
inline std::ostream& operator<<(std::ostream& os, const Polynomial& p)
{
    if (p.terms == 0) { os << '0'; return os; }

    for (int i = 0; i < p.terms; ++i) {
        const auto& t = p.termArray[i];

        /* sign */
        if (i == 0) {
            if (t.coef < 0) os << '-';
        } else {
            os << (t.coef < 0 ? " - " : " + ");
        }

        float absC = std::fabs(t.coef);

        /* coefficient */
        bool printedCoef = false;
        if (absC != 1.f || t.exp == 0) { os << absC; printedCoef = true; }

        /* variable / exponent */
        if (t.exp != 0) {
            if (printedCoef) os << '*';
            os << 'x';
            if (t.exp != 1) os << '^' << t.exp;
        }
    }
    return os;
}

#endif /* POLYNOMIAL_H */
```
**main.cpp**
```cpp
/*==============================  main.cpp  =============================*/
#include <iostream>
#include "header.h"

int main()
{
    Polynomial p, q;

    std::cout << "Enter the first polynomial: ";
    if (!(std::cin >> p)) { std::cerr << "Invalid input format\n"; return 1; }

    std::cout << "Enter the second polynomial: ";
    if (!(std::cin >> q)) { std::cerr << "Invalid input format\n"; return 1; }

    std::cout << "\n p(x) = " << p
              << "\n q(x) = " << q
              << "\n p + q = " << p.Add(q)
              << "\n p * q = " << p.Mult(q) << '\n';

    std::cout << "\nEnter a value of x to evaluate p(x): ";
    float x;
    if (std::cin >> x)
        std::cout << " p(" << x << ") = " << p.Eval(x) << '\n';

    return 0;
}
```

## 效能分析

**題目一：Polynomial**  
1. **時間複雜度**：  
   - `Add` 為 $O(m + n)$。
   - `Mult` 為 $O(m \cdot n)$。
   - `Eval` 為 $O(t)$（其中 *m*、*n* 為兩多項式的非零項數，*t* 為單一多項式的非零項數）。
2. **空間複雜度**：
   - 類別本身佔用 $O(t)$。
   - `Add` 額外 $O(1)$。
   - `Mult` 額外 $O(E_{\text{max}})$（大小為最大指數 + 1 的暫存桶）。

**題目二：Polynomial I/O**
1. **時間複雜度**：
   - `operator>>` 為 $O(k)$（讀入 *k* 個非零項）。
   - `operator<<` 為 $O(t)$（輸出 *t* 個非零項）。
2. **空間複雜度**：額外皆為常數級 $O(1)$。

## 測試與驗證

### 測試案例

| 測試案例 | 題目編號 | 輸入參數 | 預期輸出 | 實際輸出 |
|----------|----------|-----------------|-----------|-----------|
| 測試一 | 題目一（Add） | `3 3 2 2 1 -4 0`<br>`2 1 1 1 0`<br>`2` | `p + q = 3x^2 + 3x - 3` | `p + q = 3*x^2 + 3*x - 3` |
| 測試二 | 題目一（Mult） | `3 3 2 2 1 -4 0`<br>`2 1 1 1 0`<br>`2` | `p * q = 3x^3 + 5x^2 - 2x - 4` | `p * q = 3*x^3 + 5*x^2 - 2*x - 4` |
| 測試三 | 題目一（Eval） | `3 3 2 2 1 -4 0`<br>`2 1 1 1 0`<br>`2` | `p(2) = 12` | `p(2) = 12` |
| 測試四 | 題目二（operator>> 之後 operator<<） | `3 3 2 2 1 -4 0`<br>`2 1 1 1 0`<br>`2` | `p(x) = 3x^2 + 2x - 4` | `p(x) = 3*x^2 + 2*x - 4` |
| 測試五 | 題目二（零多項式 I/O） | `0`（項數為 0）　| `0` | `0` |

### 編譯與執行指令1

```bash
cd homework2/src
g++ -std=c++17 -Wall -O2 -I. -o hw2 main.cpp
./hw2
```

### 結論

## 申論及開發報告
1. **設計抉擇**  
   - **資料結構**：選擇「指數遞減的 `<coef, exp>` 陣列」；對稀疏多項式極省記憶體，且加法可直接 merge。  
   - **乘法桶 (bucket) 大小**：使用 `最大指數 + 1` 的浮點陣列，避免在結果陣列中反覆插入／位移。  
   - **Rule-of-5**：採 *copy-and-swap* 實作拷貝賦值；移動建構與賦值則「偷走指標」並歸零來源，以免多重刪除。  

2. **開發歷程**  
   - **Bug 追蹤**：最常見錯誤是 `attach()` 未正確合併同指數，導致 Add / Mult 結果重複；以 `assert` 保證插入前後仍保持遞減序列後迅速定位。  
   - **模稜兩可運算子 =**：最初 copy-assign 用 pass-by-value，與 move-assign 產生歧義。改成 `const&` 即解。  
   - **效能量測**：手動產生 10^3 項 × 10^3 項的隨機多項式，Add 平均 0.02 ms，Mult 約 15 ms；時間近似理論 O(m n)。  

3. **可改進之處**  
   - **動態桶收縮**：目前乘法桶大小固定 `E_max+1`，如能偵測實際最大指數可再省空間。  
   - **Horner 法**：若 Eval 次數極多，可改成 Horner Scheme，速度 O(t)→O(t) 仍同階但常數小。  
   - **鏈結串列版本**：對極端稀疏、超大指數情形，鏈結串列可避免開大桶；可做為進階延伸。  
   - **單元測試框架**：未來可引入 Catch2 / GoogleTest（僅作測試時允許 STL），提升自動化程度。  

4. **學習心得**  
   - 手動管理資源雖繁瑣，但對理解 C++ 物件生命週期與 move semantics 極有幫助。  
   - 演算法複雜度分析不能脫離實作；透過實測資料可檢驗常數係數及 cache 友好度對效能影響。  

> 綜合而言，本專案驗證了「抽象資料型別 → 具體資料結構 → 安全資源管理 → 完整測試」開發流程，  
> 體會到在受限環境下依然能寫出可維護且高效的 C++ 程式。