#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "netaut.h"
#include "chromosome.h"
#include "pool.h"
#include "runner.h"

//===============
// class Pool methods
//===============

//---------------
// Pool constructor
//
//---------------
Pool::Pool(int capacity) {
    m_capacity = capacity;
    m_pool.reserve(capacity);
    for (int i = 0; i < m_capacity; i += 1) m_pool[i] = nullptr;
}

//---------------
Pool::~Pool() {
    for (int i = 0; i < m_capacity; i += 1) if (m_pool[i] != nullptr) delete m_pool[i];
}

Chromosome* Pool::get_entry(int ix) { return m_pool[ix]; }
int Pool::get_capacity() { return m_capacity; }
void Pool::put_entry(Chromosome* c, int ix) { m_pool[ix] = c; }

//---------------
double Pool::AvgFitness() {
    assert(m_capacity > 0);
    double sum = 0.0;
    for (int ix = 0; ix < m_capacity; ix += 1) sum += m_pool[ix]->get_fitness();
    return sum / m_capacity;
}

//---------------
bool Pool::Contains(const rulenr_t ruleNr) {
    for (int ix = 0; ix < m_capacity; ix += 1)
        if (m_pool[ix] && m_pool[ix]->get_ruleNr() == ruleNr) return true;

    return false;
}

//---------------
double Pool::MaxFitness() {
    assert(m_capacity > 0);
    double maxV = m_pool[0]->get_fitness();
    for (int ix = 0; ix < m_capacity; ix += 1)
        if (m_pool[ix]->get_fitness() > maxV) maxV = m_pool[ix]->get_fitness();
    return maxV;
}

//---------------
bool Pool::Read(const std::string filename) {
    std::ifstream infile;
    infile.open(filename, std::ios::in);
    if (!infile.is_open()) return false;
    int ix = 0;
    rulenr_t ruleNr;
    while (infile >> ruleNr) {
        double fitness;
        m_pool[ix] = new Chromosome(ruleNr);
        assert(infile >> fitness);
        m_pool[ix]->put_fitness(fitness);
        ix += 1;
    } 
    m_capacity = ix;
    infile.close();
    return true;
}

//---------------
bool Pool::Write(const std::string filename) {
    if (m_capacity == 0) return false;
    std::ofstream outfile;
    outfile.open(filename, std::ios::trunc);
    if (!outfile.is_open()) return false;
    for (int ix = 0; ix < m_capacity; ix += 1)
        outfile << m_pool[ix]->get_ruleNr() << " " << m_pool[ix]->get_fitness() << std::endl;
    return true;
}
