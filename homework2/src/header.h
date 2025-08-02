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
