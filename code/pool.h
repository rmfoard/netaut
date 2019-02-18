#ifndef POOL_H
#define POOL_H

#include <vector>
#include "netaut.h"
#include "chromosome.h"

//---------------
class Pool {

public:
    Pool();
    ~Pool();

    //double get_fitness();

private:
    void Placeholder(int);

    std::vector<Chromosome*> m_pool;
};
#endif
