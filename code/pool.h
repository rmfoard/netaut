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

    int get_size();
    Chromosome* get_entry(int);
    void put_entry(Chromosome*, int);

    double AvgFitness();
    bool Read(std::string);
    bool Write(std::string);

private:
    int m_size;
    std::vector<Chromosome*> m_pool;
};
#endif
