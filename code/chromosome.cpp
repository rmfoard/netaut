#include <assert.h>
#include <stdio.h>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "netaut.h"
#include "chromosome.h"
#include "runner.h"

//===============
// class Chromosome static storage
//===============
    bool Chromosome::s_paramsSet = false;

    std::string Chromosome::s_statName;
    double Chromosome::s_statMin;
    double Chromosome::s_statMax;

//===============
// class Runner-associated static methods
//===============
void Chromosome::SetParameters(std::string statName, double statMin, double statMax) {
    Chromosome::s_statName = statName;
    Chromosome::s_statMin = statMin;
    Chromosome::s_statMax = statMax;

    Chromosome::s_paramsSet = true;
}

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
Chromosome::~Chromosome() { }

rulenr_t Chromosome::get_ruleNr() { return m_ruleNr; }

//---------------
double Chromosome::get_fitness() {
    assert(s_paramsSet);

    // Return the cached value if we have it.
    if (m_fitness >= 0) return m_fitness;

    // It's not cached, so run the automaton to set outcome statistics.
    Runner* r = new Runner(m_ruleNr);
    r->Run();

    // Set 'v' to the value of the statistic of interest.
    double v = -1;
    if (s_statName == "nrCcs") v = r->m_nrCcs;
    if (s_statName == "cycleLength") v = r->m_cycleLength;
    if (s_statName == "nrIterations") v = r->m_nrIterations;
    if (s_statName == "finNrNodes") v = r->m_finNrNodes;
    if (s_statName == "avgClustCoef") v = r->m_avgClustCoef;
    if (s_statName == "diameter") v = r->m_diameter;
    if (s_statName == "effDiameter90Pctl") v = r->m_effDiameter90Pctl;
    if (s_statName == "inDegreeEntropy") v = r->m_inDegreeEntropy;
    if (s_statName == "nrInDegrees") v = r->m_nrInDegrees;
    if (s_statName == "nrCcSizes") v = r->m_nrCcSizes;
    if (s_statName == "nrOpenTriads") v = r->m_nrOpenTriads;
    if (s_statName == "maxInDegree") v = r->m_maxInDegree;
    if (s_statName == "nrClosedTriads") v = r->m_nrClosedTriads;
    assert(v >= 0);

    // Compute the fitness if we seek the [center of] a range,
    if (s_statMin >= 0 && s_statMax >= 0) {
        double target = (s_statMin + s_statMax) / 2.0;
        m_fitness = 1.0 / std::abs(v - target);
    }

    // ...or if maximizing,
    else if (s_statMin >= 0 && s_statMax < 0) {
        m_fitness = v - s_statMin;
    }

    // ...or minimizing.
    else {
        m_fitness = s_statMax - v;
    }

    assert(m_fitness >= 0);
    return m_fitness;
}

//---------------
void Chromosome::put_fitness(double fitness) {
    m_fitness = fitness;
}

//---------------
void Chromosome::Placeholder(int placeholder) {
}
