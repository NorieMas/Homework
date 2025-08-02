/*==============================  main.cpp  =============================*/
#include <iostream>
#include "header.h"

int main()
{
    Polynomial p, q;

    std::cout << "輸入第一個多項式：";
    std::cin  >> p;
    if (!std::cin) { std::cerr << "輸入格式錯誤\n"; return 1; }

    std::cout << "輸入第二個多項式：";
    std::cin  >> q;
    if (!std::cin) { std::cerr << "輸入格式錯誤\n"; return 1; }

    std::cout << "\n p(x) = " << p
              << "\n q(x) = " << q
              << "\n p + q = " << p.Add(q)
              << "\n p * q = " << p.Mult(q) << '\n';

    std::cout << "\n請輸入 x 以評估 p(x)：";
    float x;
    if (std::cin >> x)
        std::cout << " p(" << x << ") = " << p.Eval(x) << '\n';

    return 0;
}
