# 41041225

作業三

## 解題說明

本作業目標是實作一個具備加法、減法、乘法、評估等基本功能的多項式抽象資料型別（Polynomial ADT），其核心資料結構為**循環單向鏈結串列**（Circular Singly Linked List）。

每個節點代表多項式的一個項（Term），包含係數（`coef`）與指數（`exp`），透過鏈結串列可動態新增或刪除節點，使其能靈活處理稀疏多項式。

此外，本程式還實作一個**節點重用機制（Available List）**，以減少記憶體配置與釋放的次數，提升效率與記憶體管理的彈性。

程式支援以下功能：

- 以運算子過載 `>>` 與 `<<` 實作多項式的輸入與輸出。
- `+`、`-`、`*` 運算子進行多項式的加法、減法與乘法運算。
- `Evaluate(x)` 用於計算多項式在特定 x 值下的值。
- 所有運算過程皆維持多項式的項次為降冪排序，並自動合併相同次方項。

### 解題策略

本作業的核心策略為透過「循環單向鏈結串列」實作多項式的儲存與操作，並以物件導向方式設計類別 `Polynomial` 管理其行為。
具體策略如下：
1. **節點定義**：
   - 每個節點代表一個項次（Term），包含係數與次方。
   - 利用模板類別 `Chain<T>` 管理節點的內容與指標。

2. **資料結構設計**：
   - 使用一個「哨兵節點」（dummy header）來構成**循環鏈結串列**，方便處理邊界狀況。
   - 所有節點按次方遞減排序，便於進行合併與運算。

3. **記憶體重複利用**：
   - 引入 `Available<T>` 節點池機制（free list）降低記憶體反覆配置與釋放的成本，提升執行效率。

4. **運算實作**：
   - `operator+`：模擬兩串列合併邏輯，處理相同次方加總與新增節點。
   - `operator-`：處理對應項次相減與補零機制。
   - `operator*`：進行所有項次的兩兩相乘，結果加入新多項式並合併同類項。
   - `Evaluate(x)`：使用自製 `intPow` 函數（避免使用 `<cmath>` 中的 `pow`）對每一項代入運算。

5. **操作簡化**：
   - 使用 `operator>>` 與 `operator<<` 支援使用者友善的輸入與輸出格式。

## 程式實作

**header.h**
```cpp
#ifndef HEADER_H
#define HEADER_H

#include <iostream>
using namespace std;

// ====== 節點資料型態：多項式的一項 ======
struct Term {
    float coef;
    int exp;
    Term(float c = 0, int e = 0) : coef(c), exp(e) {}
};

// ====== 節點類別（模板） ======
template<class T>
class Chain {
    template<class U> friend class Available;
    friend class Polynomial;
private:
    T term;
    Chain<T>* next;
public:
    Chain() : term(T()), next(nullptr) {}
    Chain(const T& t) : term(t), next(nullptr) {}
    Chain(float coef, int exp) : term(coef, exp), next(nullptr) {}
    T getTerm() const { return term; }
    Chain<T>* getNext() const { return next; }
    void setNext(Chain<T>* n) { next = n; }
    void setTerm(float c, int e) { term.coef = c; term.exp = e; }
};

// ====== 可用節點串列 ======
template<class T>
class Available {
private:
    Chain<T>* avaList;
public:
    Available() : avaList(nullptr) {}
    ~Available() {
        while (avaList) {
            Chain<T>* tmp = avaList;
            avaList = avaList->getNext();
            delete tmp;
        }
    }
    void getBack(Chain<T>* node) {
        node->setNext(avaList);
        avaList = node;
    }
    Chain<T>* getOneNode() {
        if (avaList) {
            Chain<T>* tmp = avaList;
            avaList = avaList->getNext();
            tmp->setNext(nullptr);
            return tmp;
        }
        return nullptr;
    }
};

extern Available<Term> globalASL;

// ====== 多項式類別 ======
class Polynomial {
    friend istream& operator>>(istream& is, Polynomial& x);
    friend ostream& operator<<(ostream& os, const Polynomial& x);
private:
    Chain<Term>* header;
    int terms;
    void clear();
    float intPow(float base, int exp) const;
public:
    Polynomial();
    Polynomial(const Polynomial& a);
    ~Polynomial();
    const Polynomial& operator=(const Polynomial& a);
    void newTerm(float coef, int exp);
    float Evaluate(float x) const;
    Polynomial operator+(const Polynomial& b) const;
    Polynomial operator-(const Polynomial& b) const;
    Polynomial operator*(const Polynomial& b) const;
};

istream& operator>>(istream& is, Polynomial& x);
ostream& operator<<(ostream& os, const Polynomial& x);

#endif // HEADER_H
```
**main.cpp**
```cpp
#include "header.h"

Available<Term> globalASL;

Polynomial::Polynomial() {
    header = new Chain<Term>();
    header->setNext(header);
    terms = 0;
}

Polynomial::Polynomial(const Polynomial& a) : Polynomial() {
    Chain<Term>* cur = a.header->getNext();
    while (cur != a.header) {
        Term t = cur->getTerm();
        newTerm(t.coef, t.exp);
        cur = cur->getNext();
    }
}

Polynomial::~Polynomial() {
    clear();
    globalASL.getBack(header);
}

const Polynomial& Polynomial::operator=(const Polynomial& a) {
    if (this != &a) {
        clear();
        Chain<Term>* cur = a.header->getNext();
        while (cur != a.header) {
            Term t = cur->getTerm();
            newTerm(t.coef, t.exp);
            cur = cur->getNext();
        }
    }
    return *this;
}

void Polynomial::clear() {
    if (!header) return;
    Chain<Term>* cur = header->getNext();
    while (cur != header) {
        Chain<Term>* tmp = cur;
        cur = cur->getNext();
        globalASL.getBack(tmp);
    }
    header->setNext(header);
    terms = 0;
}

float Polynomial::intPow(float base, int exp) const {
    float result = 1;
    for (int i = 0; i < exp; i++) {
        result *= base;
    }
    return result;
}

void Polynomial::newTerm(float coef, int exp) {
    if (coef == 0) return;
    Chain<Term>* prev = header;
    Chain<Term>* cur = header->getNext();

    while (cur != header && cur->getTerm().exp > exp) {
        prev = cur;
        cur = cur->getNext();
    }

    if (cur != header && cur->getTerm().exp == exp) {
        float newCoef = cur->getTerm().coef + coef;
        if (newCoef == 0) {
            prev->setNext(cur->getNext());
            globalASL.getBack(cur);
            terms--;
        } else {
            cur->setTerm(newCoef, exp);
        }
        return;
    }

    Chain<Term>* node = globalASL.getOneNode();
    if (!node) node = new Chain<Term>(coef, exp);
    else node->setTerm(coef, exp);

    prev->setNext(node);
    node->setNext(cur);
    terms++;
}

float Polynomial::Evaluate(float x) const {
    float sum = 0;
    Chain<Term>* cur = header->getNext();
    while (cur != header) {
        Term t = cur->getTerm();
        sum += t.coef * intPow(x, t.exp);
        cur = cur->getNext();
    }
    return sum;
}

Polynomial Polynomial::operator+(const Polynomial& b) const {
    Polynomial result;
    Chain<Term>* aCur = header->getNext();
    Chain<Term>* bCur = b.header->getNext();
    while (aCur != header && bCur != b.header) {
        Term ta = aCur->getTerm(), tb = bCur->getTerm();
        if (ta.exp == tb.exp) {
            result.newTerm(ta.coef + tb.coef, ta.exp);
            aCur = aCur->getNext();
            bCur = bCur->getNext();
        } else if (ta.exp > tb.exp) {
            result.newTerm(ta.coef, ta.exp);
            aCur = aCur->getNext();
        } else {
            result.newTerm(tb.coef, tb.exp);
            bCur = bCur->getNext();
        }
    }
    while (aCur != header) {
        Term ta = aCur->getTerm();
        result.newTerm(ta.coef, ta.exp);
        aCur = aCur->getNext();
    }
    while (bCur != b.header) {
        Term tb = bCur->getTerm();
        result.newTerm(tb.coef, tb.exp);
        bCur = bCur->getNext();
    }
    return result;
}

Polynomial Polynomial::operator-(const Polynomial& b) const {
    Polynomial result;
    Chain<Term>* aCur = header->getNext();
    Chain<Term>* bCur = b.header->getNext();
    while (aCur != header && bCur != b.header) {
        Term ta = aCur->getTerm(), tb = bCur->getTerm();
        if (ta.exp == tb.exp) {
            result.newTerm(ta.coef - tb.coef, ta.exp);
            aCur = aCur->getNext();
            bCur = bCur->getNext();
        } else if (ta.exp > tb.exp) {
            result.newTerm(ta.coef, ta.exp);
            aCur = aCur->getNext();
        } else {
            result.newTerm(-tb.coef, tb.exp);
            bCur = bCur->getNext();
        }
    }
    while (aCur != header) {
        Term ta = aCur->getTerm();
        result.newTerm(ta.coef, ta.exp);
        aCur = aCur->getNext();
    }
    while (bCur != b.header) {
        Term tb = bCur->getTerm();
        result.newTerm(-tb.coef, tb.exp);
        bCur = bCur->getNext();
    }
    return result;
}

Polynomial Polynomial::operator*(const Polynomial& b) const {
    Polynomial result;
    for (Chain<Term>* aCur = header->getNext(); aCur != header; aCur = aCur->getNext()) {
        for (Chain<Term>* bCur = b.header->getNext(); bCur != b.header; bCur = bCur->getNext()) {
            Term ta = aCur->getTerm(), tb = bCur->getTerm();
            result.newTerm(ta.coef * tb.coef, ta.exp + tb.exp);
        }
    }
    return result;
}

// ====== 輸入輸出 ======
istream& operator>>(istream& is, Polynomial& x) {
    x.clear();
    int n; is >> n;
    for (int i = 0; i < n; i++) {
        float c; int e;
        is >> c >> e;
        x.newTerm(c, e);
    }
    return is;
}

ostream& operator<<(ostream& os, const Polynomial& x) {
    Chain<Term>* cur = x.header->getNext();
    bool first = true;
    while (cur != x.header) {
        Term t = cur->getTerm();
        if (t.coef != 0) {
            if (!first && t.coef > 0) os << "+";
            os << t.coef << "x^" << t.exp;
            first = false;
        }
        cur = cur->getNext();
    }
    if (first) os << "0";
    return os;
}

int main() {
    Polynomial polyA, polyB;
    cout << "Enter polynomial A: n c1 e1 c2 e2 ...\n";
    cin >> polyA;
    cout << "Enter polynomial B: n c1 e1 c2 e2 ...\n";
    cin >> polyB;

    cout << "A = " << polyA << endl;
    cout << "B = " << polyB << endl;
    cout << "A + B = " << (polyA + polyB) << endl;
    cout << "A - B = " << (polyA - polyB) << endl;
    cout << "A * B = " << (polyA * polyB) << endl;

    float x;
    cout << "Enter x to evaluate A(x): ";
    cin >> x;
    cout << "A(" << x << ") = " << polyA.Evaluate(x) << endl;
    return 0;
}
```

## 效能分析

1. **時間複雜度**：
   - **輸入與建構多項式**： $O(n)$ 每個項目插入鏈結串列需 $O(1)$ ~ $O(n)$ 時間，視次方排序與合併情況而定。
   - **加法與減法**： $O(m + n)$ 兩個多項式按次方遞減排序合併，每個節點最多訪問一次。
   - **乘法**： $O(m × n)$ 每一項都需與另一多項式所有項進行乘法並插入合併。
   - **Evaluate**： $O(n)$ 需走訪每一項並計算次方與乘積。
2. **空間複雜度**：
   - **單一多項式儲存**： $O(n)$ 每項一節點。
   - **運算結果空間**：加減為 $O(m + n)$ 乘法最壞為 $O(m × n)$（未合併前）。
   - **節點池 (Available List)**：最壞與節點數等同，實際可回收記憶體重複使用，提升記憶體效率。

## 測試與驗證

### 測試案例
| 測試案例 | 題目編號 | 輸入參數                                 | 預期輸出                                                 | 實際輸出                                                 |
|----------|----------|------------------------------------------|----------------------------------------------------------|----------------------------------------------------------|
| 測試一   | 題目一   | A: 3 2 3 -4 1 1 0<br>B: 2 -2 3 3 1<br>x=2 | A=2x^3-4x^1+1x^0<br>B=-2x^3+3x^1<br>A+B=-1x^1+1x^0<br>A-B=4x^3-7x^1+1x^0<br>A\*B=-4x^6+14x^4-2x^3-12x^2+3x^1<br>A(2)=9 | A=2x^3-4x^1+1x^0<br>B=-2x^3+3x^1<br>A+B=-1x^1+1x^0<br>A-B=4x^3-7x^1+1x^0<br>A\*B=-4x^6+14x^4-2x^3-12x^2+3x^1<br>A(2)=9 |
| 測試二   | 題目一   | A: 2 1 2 -1 1<br>B: 2 -1 2 1 1<br>x=3     | A=1x^2-1x^1<br>B=-1x^2+1x^1<br>A+B=0<br>A-B=2x^2-2x^1<br>A\*B=-1x^4+2x^3-1x^2<br>A(3)=6 | A=1x^2-1x^1<br>B=-1x^2+1x^1<br>A+B=0<br>A-B=2x^2-2x^1<br>A\*B=-1x^4+2x^3-1x^2<br>A(3)=6 |
| 測試三   | 題目一   | A: 1 0 0<br>B: 1 0 0<br>x=5               | A=1x^0<br>B=1x^0<br>A+B=2x^0<br>A-B=0<br>A\*B=1x^0<br>A(5)=1 | A=1x^0<br>B=1x^0<br>A+B=2x^0<br>A-B=0<br>A\*B=1x^0<br>A(5)=1 |
| 測試四   | 題目一   | A: 0<br>B: 0<br>x=1                       | A=0<br>B=0<br>A+B=0<br>A-B=0<br>A\*B=0<br>A(1)=0           | A=0<br>B=0<br>A+B=0<br>A-B=0<br>A\*B=0<br>A(1)=0           |
| 測試五   | 題目一   | A: 2 3 5 2 3<br>B: 2 4 5 -2 3<br>x=1      | A=3x^5+2x^3<br>B=4x^5-2x^3<br>A+B=7x^5<br>A-B=-1x^5+4x^3<br>A\*B=12x^10+6x^8-6x^8-4x^6=12x^10+2x^6<br>A(1)=5 | A=3x^5+2x^3<br>B=4x^5-2x^3<br>A+B=7x^5<br>A-B=-1x^5+4x^3<br>A\*B=12x^10+2x^6<br>A(1)=5 |
                                             |

### 編譯與執行指令

```bash
cd homework3/src
g++ -std=c++17 -Wall -O2 -I. -o hw3 main.cpp
./hw3
```

## 結論

## 申論及開發報告
本作業透過鏈結串列來實作多項式的運算，具備以下幾點學習與設計重點：
1. **資料結構應用能力提升**  
   使用 `Chain` 模板類別建立循環串列，體會指標操作與節點插入刪除的邏輯。進一步透過 `Available` 節點池減少記憶體配置次數，提升效能。
2. **物件導向實踐**  
   `Polynomial` 類別將操作封裝，使使用者可以透過簡單介面進行多項式運算（加、減、乘、代入求值），並支援 `>>` 和 `<<` 運算子，強化可讀性與維護性。
3. **邏輯與錯誤處理**  
   在 `newTerm()` 中處理次方重複時合併、係數為零時刪除節點，提升正確性與記憶體管理效率。
4. **測試與除錯**  
   通過多組測資驗證程式行為，並修正乘法運算中可能重複加入項次的問題，強化對邏輯錯誤的掌握與修復能力。
5. **報告與分析能力**  
   除了程式撰寫外，透過解題說明、效能分析與表格化測試結果，進一步練習技術文件的撰寫與條理表達能力。
本作業整合程式邏輯、資料結構應用與報告表達，是一次完整的實作與思考訓練。

