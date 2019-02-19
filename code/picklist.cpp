#include <assert.h>
#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include "netaut.h"
#include "chromosome.h"
#include "pool.h"
#include "picklist.h"

//===============
// class PickList methods
//===============

//---------------
// PickList constructor
//
//---------------
PickList::PickList(Pool* basePool) {
    m_basePool = basePool;
    int poolSize = basePool->get_size();
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
    //std::sort(m_list.begin(), m_list.end(),
    std::sort(&m_list[0], &m_list[poolSize-1],
      [](const PickElt& a, const PickElt& b) { return a.normFitness > b.normFitness; });

    // Pass the list adding cumulative fitness.
    double cumFitness = m_list[0].normFitness;
    m_list[0].cumFitness = cumFitness;
    for (int ix = 1; ix < poolSize; ix += 1) {
        cumFitness += m_list[ix].normFitness;
        m_list[ix].cumFitness = cumFitness;
    }
}

//---------------
PickList::~PickList() {
}

Pool* PickList::get_basePool() { return m_basePool; }
PickElt PickList::get_elt(int ix) { return m_list[ix]; }
