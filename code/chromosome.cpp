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

//===============
// class Chromosome methods
//===============

//---------------
// Chromosome constructor
//
//---------------
Chromosome::Chromosome(rulenr_t ruleNr) {
    m_ruleNr = ruleNr;
    m_fitness = -1.0;
}

//---------------
Chromosome::~Chromosome() {
}

//---------------
rulenr_t Chromosome::get_ruleNr() {
    return m_ruleNr;
}

//---------------
double Chromosome::get_fitness() {
    if (m_fitness < 0) {
        m_fitness = 0.0; // stub for Runner invocation
    }
    return m_fitness;
}

//---------------
void Chromosome::Placeholder(int placeholder) {
}
