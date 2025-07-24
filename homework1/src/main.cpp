/************************************************************
 *  homework1/src/main.cpp
 *  ---------------------------------------------------------
 *  Problem 1 : Ackermann Function
 *      - AckermannRec  ──► Recursive version
 *      - AckermannIter ──► Iterative version
 *
 *  Problem 2 : Power-Set
 *      - PowerSet      ──► List every subset of a set S
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
