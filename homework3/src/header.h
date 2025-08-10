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
