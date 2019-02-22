#include <assert.h>
//#include <inttypes.h>
#include <stdio.h>
//#include <stdint.h>
//#include <algorithm>
#include <iostream>
//#include <limits>
#include <string>
#include <vector>
#include "netaut.h"
#include "chromosome.h"
#include "runner.h"

//===============
// class Chromosome methods
//===============

//---------------
// Chromosome constructor
//
//---------------
Chromosome::Chromosome(rulenr_t ruleNr) {
    m_ruleNr = ruleNr;
    m_fitness = -1;
}

//---------------
Chromosome::Chromosome(rulenr_t ruleNr, double fitness) {
    m_ruleNr = ruleNr;
    m_fitness = fitness;
}

//---------------
Chromosome::~Chromosome() {
}

rulenr_t Chromosome::get_ruleNr() { return m_ruleNr; }

//---------------
double Chromosome::get_fitness() {
    if (m_fitness < 0) {
        Runner* r = new Runner(m_ruleNr);
        r->Run();

/* Clustering Coefficient = 0.30
        if (r->m_avgClustCoef > 0.3)
            m_fitness = 1.0 - (r->m_avgClustCoef - 0.3);
        else
            m_fitness = 1.0 - (0.3 - r->m_avgClustCoef);
*/
        m_fitness = r->m_nrCcs / (double) Runner::s_initNrNodes;
        assert(m_fitness >= 0);
        delete r;
    }
    return m_fitness;
}

//---------------
void Chromosome::put_fitness(double fitness) {
    m_fitness = fitness;
}

//---------------
void Chromosome::Placeholder(int placeholder) {
}
