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
