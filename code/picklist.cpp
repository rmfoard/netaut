#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include "netaut.h"
#include "picklist.h"

//===============
// class PickList methods
//===============

//---------------
// PickList constructor
//
//---------------
PickList::PickList(Pool* basePool, double cumFitnessExp) {
    m_basePool = basePool;
    m_cumFitnessExp = cumFitnessExp;
    int poolSize = basePool->get_capacity();
    m_list.reserve(poolSize);

    // Assign a list entry for each pool member and accumulate
    // the total fitness in the pool.
    double totFitness = 0.0;
    for (int ix = 0; ix < poolSize; ix += 1) {
        m_list[ix].c = basePool->get_entry(ix);
        totFitness += m_list[ix].c->get_fitness();
    }

    // Pass the list adding normalized fitness.
    for (int ix = 0; ix < poolSize; ix += 1) {
        m_list[ix].normFitness = m_list[ix].c->get_fitness() / totFitness;
    }

    // Sort by decreasing fitness.
    std::sort(&m_list[0], &m_list[poolSize],
      [](const PickElt& a, const PickElt& b) { return a.normFitness > b.normFitness; });

    // Pass the list adding cumulative [exponentiated] fitness.
    double cumFitness = m_list[0].normFitness;
    m_list[0].cumFitness = pow(cumFitness, cumFitnessExp);
    for (int ix = 1; ix < poolSize; ix += 1) {
        cumFitness += m_list[ix].normFitness;
        m_list[ix].cumFitness = pow(cumFitness, cumFitnessExp);
    }
}

//---------------
PickList::~PickList() {
}

Pool* PickList::get_basePool() { return m_basePool; }
double PickList::get_cumFitnessExp() { return m_cumFitnessExp; }
PickElt PickList::get_elt(int ix) { return m_list[ix]; }

//---------------
void PickList::Log(std::ostream& out, int randSeed, int generationNr) {
    int poolSize = m_basePool->get_capacity();
    for (int ix = 0; ix < poolSize; ix += 1) {
    out
      << randSeed << " "
      << generationNr << " "
      << m_list[ix].c->get_ruleNr() << " "
      << m_list[ix].c->get_fitness() << " "
      << m_list[ix].normFitness << " "
      << m_list[ix].cumFitness << std::endl;
    }
}
