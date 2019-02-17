#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdelete-non-virtual-dtor"
#include "Snap.h"
#pragma GCC diagnostic pop
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
#include "rule.h"
#include "machine.h"
#include "machine2D.h"
#include "runner.h"

//===============
// class Runner methods
//===============

//---------------
// Runner constructor
//
//---------------
Runner::Runner(rulenr_t ruleNr, int nrNodes, int maxIterations, int cycleCheckDepth,
  std::string tapeStructure, int tapePctBlack, std::string topoStructure, int noChangeTopo) {
    m_maxIterations = maxIterations;
    m_machine = new Machine2D("C");
    m_machine->BuildMachine(ruleNr, nrNodes, cycleCheckDepth, tapeStructure,
      tapePctBlack, topoStructure, noChangeTopo);
}

//---------------
Runner::~Runner() {
    delete m_machine;
}

//---------------
void Runner::Run() {
    int iter = 0;
    int cycleLength;
    for ( ; iter < m_maxIterations; iter += 1) {
        // Iterate once. Stop afterward if a state cycle or graph collapse was detected.
        cycleLength = m_machine->IterateMachine(iter);

        if (cycleLength > 0 || cycleLength < 0) { iter += 1; break; }
    } // The residual value of 'iter' is the actual number of iterations completed.

    // Assemble and record outcome data.
    m_nrIterations = iter;
    m_cycleLength = cycleLength;

    // Analyze connected components and count nodes.
    TVec<TPair<TInt, TInt> > sizeCount;
    TSnap::GetWccSzCnt(m_machine->get_graph(), sizeCount);
    int nrCcs = 0;
    int nrNodes = 0;
    for (int i = 0; i < sizeCount.Len(); i += 1) {
        nrNodes += sizeCount[i].Val1 * sizeCount[i].Val2;
        nrCcs += (int) sizeCount[i].Val2;
    }
    m_nrCcSizes = sizeCount.Len();
    m_nrCcs = nrCcs;
    m_nrNodes = nrNodes;

    // Triads (graph-theoretic sense)
    TFltPrV DegCCfV;
    int64 ClosedTriads, OpenTriads;
    const double CCF = TSnap::GetClustCf(m_machine->get_graph(), DegCCfV, ClosedTriads, OpenTriads);
    m_avgClustCoef = CCF;
    m_nrClosedTriads = (uint64_t) TUInt64(ClosedTriads);
    m_nrOpenTriads = (uint64_t) TUInt64(OpenTriads);

    // Diameter stats
    int FullDiam;
    double EffDiam;
    TSnap::GetBfsEffDiam(m_machine->get_graph(), 1000, false, EffDiam, FullDiam);
    m_diameter = FullDiam;
    m_effDiameter90Pctl = EffDiam;

    Machine::DegStats degStats;
    m_machine->GetDegStats(degStats);
    int maxInDegree = 0;
    for (int i = 0; i < degStats.nrInDeg; i += 1)
        if (degStats.inDegCnt[i].Val1 > maxInDegree) maxInDegree = degStats.inDegCnt[i].Val1;
    m_nrInDegrees = degStats.nrInDeg;
    m_maxInDegree = maxInDegree;
    m_inDegreeEntropy = degStats.inDegEntropy;
}

//---------------
void Runner::Placeholder(int placeholder) {
}
