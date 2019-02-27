#define POOLSIZE 100

#include <iostream>
#include <vector>
#include "netaut.h"
#include "chromosome.h"

Chromosome::Chromosome(rulenr_t r) {
    m_ruleNr = r;
}

Chromosome::~Chromosome() {}

class Pool {

public:
    Pool();
    ~Pool();

    std::vector<Chromosome*> m_fred;
};

//---------------
Pool::Pool() {
    std::cout << "Pool()" << std::endl;
    m_fred.reserve(100);
    for (int i = 0; i < 100; i += 1) m_fred[i] = new Chromosome((rulenr_t) i);
}

Pool::~Pool() {
    std::cout << "~Pool()" << std::endl;
    for (int i = 0; i < 100; i += 1) delete m_fred[i];
}

//---------------
#define REF
#ifdef REF
void changepn(int*& pn) {
    int* pm = new int;
    *pm = 2;

    delete pn;
    pn = pm;
}
#endif
#ifndef REF
void changepn(int **pp) {
    int* pm = new int;
    *pm = 2;

    *pp = pm;
}
#endif

//---------------
int main(const int argc, char** argv) {
    std::cout << "initium" << std::endl;

    int a[100];
    int n = 0;
    do {
        std::cin >> a[n++];
    } while (a[n-1] > 0);
    n = n-1;
    std::cout << n << " elements" << std::endl;

    int mv = a[0];
    int wx = 1;
    int sx = 1;
    while (sx < n) {
        while (sx < n && a[sx] == mv) sx += 1;
        if (sx < n) {
            a[wx] = a[sx];
            wx += 1;
            mv = a[sx];
            sx += 1;
        }
    }
    n = wx;

    for (int ix = 0; ix < n; ix += 1) std::cout << a[ix] << std::endl;
/*
    rulenr_t bignum = 722204136308736; // 72**8
    for (int i = 1; i <= 50; i += 1) {
        rulenr_t v = ((rulenr_t) 1) << i;
        std::cout << "1 << " << i << " => " << v << "; ";
        std::cout << (bignum ^ v) << std::endl;
    }
*/
/*
    int* pn = new int;
    *pn = 1;

#ifdef REF
    changepn(pn);
#endif
#ifndef REF
    changepn(&pn);
#endif

    std::cout << *pn << std::endl;
*/
    std::cout << "finis." << std::endl;
}
