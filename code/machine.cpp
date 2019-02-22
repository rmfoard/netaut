#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdelete-non-virtual-dtor"
#include "Snap.h"
#pragma GCC diagnostic pop
#include <assert.h>
#include <cmath>
#include <vector>
#include "boost/math/special_functions/zeta.hpp"
#include "rule.h"
#include "machine.h"


//---------------
// TODO: Use the proper form for "getters."
//---------------
std::string Machine::get_machineType() { return m_machineType; }
void Machine::set_machineType(std::string type) { m_machineType = type; }

//---------------
// DegEntropy
//---------------
static double Entropy(TVec<TPair<TInt, TInt> > degCnt) {

    // Total the frequencies.
    // (The total should always equal 2 * (nodes in the graph).)
    int nrDegs = degCnt.Len();
    double totalFreq;
    for (int i = 0; i < nrDegs; i += 1) {
        int freq = degCnt[i].Val2;
        totalFreq += freq;
    }

    // Compute and return Shannon's entropy, normalized.
    double sum = 0.0;
    for (int i = 0; i < nrDegs; i += 1) {
        int freq = degCnt[i].Val2;
        double pk = freq / totalFreq;
        sum += pk * log2(pk);
    }

    if (nrDegs == 1)
        return 0;
    else
        return -sum / log2(nrDegs);
}

//---------------
// Zeta
//---------------
//static double Zeta(double s) {
//    printf("Zeta(%f)\n", s);
//    double rv;
//    try {
//        rv = boost::math::zeta(s);
//    }
//    catch (...) {
//        printf("caught some exception\n");
//        rv = 0.001;
//    }
//    return rv;
//}

//---------------
// GetDegStats
//---------------
void Machine::GetDegStats(DegStats& degStats) {
    TSnap::GetInDegCnt(m_graph, degStats.inDegCnt);
    TSnap::GetOutDegCnt(m_graph, degStats.outDegCnt);
    degStats.nrInDeg = degStats.inDegCnt.Len();
    degStats.nrOutDeg = degStats.outDegCnt.Len();
    degStats.inDegEntropy = Entropy(degStats.inDegCnt);
    degStats.outDegEntropy = Entropy(degStats.outDegCnt);
}

//---------------
// EstimateDegExp
//
// Estimate the degree exponent for the power law underlying the degree
// distribution. (Shamelessly short-cut the procedure given in Barabasi
// pp. 156-157 for maximum likelihood estimation by fixing Kmin at 2,
// forgoing a search, and making no effort on the Kmax side.)
//---------------
double Machine::EstimateDegExp(std::vector<int>& nk, int  kmax) {
    // Fix Kmin.
    int Kmin;
    if (kmax >= 1)
        Kmin = 1;
    else
        Kmin = kmax;

    // Estimate deg exp (gamma).
    double sum = 0.0;
    int N = 0;
    for (int i = 0; i <= kmax; i += 1) if (nk[i] != 0) {
        N += 1;
        sum += log(nk[i] / (Kmin - 0.5));
    }
    return 1 + N / sum; // gamma
}
