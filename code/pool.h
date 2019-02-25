#ifndef POOL_H
#define POOL_H

#include <vector>
#include "netaut.h"
#include "chromosome.h"

//---------------
class Pool {

public:
    Pool(int);
    ~Pool();

    int get_capacity();
    Chromosome* get_entry(int);
    Chromosome* put_entry(Chromosome*, int);

    double AvgFitness();
    bool Contains(const rulenr_t);
    double MaxFitness();
    bool Read(const std::string);
    bool Write(const std::string);

private:
    int m_capacity;
    std::vector<Chromosome*> m_pool;
};
#endif
