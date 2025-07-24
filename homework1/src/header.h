#ifndef HOMEWORK1_HEADER_H
#define HOMEWORK1_HEADER_H

// ---------- Problem 1 : Ackermann ----------
unsigned long long AckermannRec(unsigned int m, unsigned int n);  // 遞迴
unsigned long long AckermannIter(unsigned int m, unsigned int n); // 非遞迴

// ---------- Problem 2 : Power-Set ----------
void PowerSet(const char *set, int n); // 直接把所有子集合輸出到螢幕

#endif // HOMEWORK1_HEADER_H