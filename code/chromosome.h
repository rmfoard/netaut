#ifndef CHROMOSOME_H
#define CHROMOSOME_H
#include "netaut.h"

//---------------
class Chromosome {

public:
    Chromosome(rulenr_t);
    Chromosome(rulenr_t, double);
    ~Chromosome();

    rulenr_t get_ruleNr();
    double get_fitness();
    void put_fitness(double);

private:
    void Placeholder(int);

    rulenr_t m_ruleNr;
    double m_fitness;
};
#endif
