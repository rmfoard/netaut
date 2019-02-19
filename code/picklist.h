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
    PickList(Pool*);
    ~PickList();

    Pool* get_basePool();
    PickElt get_elt(int);


private:
    Pool* m_basePool;
    std::vector<PickElt> m_list;
};
#endif
