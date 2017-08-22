/*
 *  Author:
 *      Indestinee
 *  Date:
 *      2017/08/17
 *  Name:
 *      test_ga.cpp
 */

#ifndef STD_HEADER
#define STD_HEADER
#include <bits/stdc++.h>
using namespace std;
#endif
#include "ga.h"

class point{
public:
    int x, y;
    point(const int &x = 0, const int &y = 0) : x(x), y(y) { }
    inline friend point operator - (const point &a, const point &b) {
        return point(a.x - b.x, a.y - b.y);
    }
    inline friend double dot(const point &a,  const point &b) {
        return a.x * b.x + a.y * b.y;
    }
    inline friend double dist(const point &a, const point &b) {
        return sqrt(dot(a - b, a - b));
    }
};


vector<point> v;
double dist(const int &a, const int &b) {
    return dist(v[a], v[b]);
}

double fun(Life &a) {
    double ret = 0;
    for (int i = 1; i < int(a.gene.size()); i++) {
        ret += dist(a.gene[i], a.gene[i - 1]);
    }
    return 1.0 / ret;
}

Config config(0.5, 0.5, 1000, 25, &fun);
GA ga(config);

int main() {
    srand(clock());
    for (int i = 0; i < 25; i++)
        v.push_back(point(i / 5, i % 5));
    ga.out();
    for (int i = 0; i <= 10000; i++) {
        ga.next_generation();
        if (i % 100 == 0) {
            ga.out();
            print(ga.leader());
        }
    }

    return 0;
}
