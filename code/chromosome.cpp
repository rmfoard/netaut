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
Chromosome::~Chromosome() {
}

rulenr_t Chromosome::get_ruleNr() { return m_ruleNr; }

//---------------
double Chromosome::get_fitness() {
    if (m_fitness < 0) {
        Runner* r = new Runner(m_ruleNr);
        r->Run();
        m_fitness = r->m_nrCcs;
        assert(m_fitness >= 0);
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
