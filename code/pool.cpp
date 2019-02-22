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
Pool::Pool(int size) {
    m_size = size;
    m_pool.reserve(size);
    for (int i = 0; i < m_size; i += 1) m_pool[i] = nullptr;
}

//---------------
Pool::~Pool() {
    for (int i = 0; i < m_size; i += 1) if (m_pool[i] != nullptr) delete m_pool[i];
}

Chromosome* Pool::get_entry(int ix) { return m_pool[ix]; }
int Pool::get_size() { return m_size; }
void Pool::put_entry(Chromosome* c, int ix) { m_pool[ix] = c; }

//---------------
double Pool::AvgFitness() {
    if (m_size == 0) return 0.0;
    double sum = 0.0;
    for (int ix = 0; ix < m_size; ix += 1) sum += m_pool[ix]->get_fitness();
    return sum / m_size;
}

//---------------
bool Pool::Contains(const rulenr_t ruleNr) {
    for (int ix = 0; ix < m_size; ix += 1)
        if (m_pool[ix] && m_pool[ix]->get_ruleNr() == ruleNr) return true;

    return false;
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
    m_size = ix;
    infile.close();
    return true;
}

//---------------
bool Pool::Write(const std::string filename) {
    if (m_size == 0) return false;
    std::ofstream outfile;
    outfile.open(filename, std::ios::trunc);
    if (!outfile.is_open()) return false;
    for (int ix = 0; ix < m_size; ix += 1)
        outfile << m_pool[ix]->get_ruleNr() << " " << m_pool[ix]->get_fitness() << std::endl;
    return true;
}
