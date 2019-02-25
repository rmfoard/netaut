#ifndef PICKLIST_H
#define PICKLIST_H
#include "netaut.h"
#include "chromosome.h"
#include "pool.h"

//---------------
struct PickElt {
    Chromosome* c;
    double normFitness;
    double cumFitness;
};

//---------------
class PickList {

public:
    PickList(Pool*, double);
    ~PickList();

    Pool* get_basePool();
    double get_cumFitnessExp();
    PickElt get_elt(int);


private:
    Pool* m_basePool;
    double m_cumFitnessExp;
    std::vector<PickElt> m_list;
};
#endif
