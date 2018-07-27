#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdelete-non-virtual-dtor"
#include "Snap.h"
#pragma GCC diagnostic pop
#include <math.h>
#include "rule.h"
#include "machine.h"


//---------------
// DegEntropy
//---------------
static double Entropy(TVec<TPair<TInt, TInt> > degCnt) {

    // Total the frequencies.
    int nrDegs = degCnt.Len();
    double totalFreq;
    for (int i = 0; i < nrDegs; i += 1) {
        int freq = degCnt[i].Val2;
        totalFreq += freq;
    }

    // Compute and return Shannon's entropy.
    double sum = 0.0;
    for (int i = 0; i < nrDegs; i += 1) {
        int freq = degCnt[i].Val2;
        double pk = freq / totalFreq;
        sum += pk * log2(pk);
    }
    return -sum;
}

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
