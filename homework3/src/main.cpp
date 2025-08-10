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
