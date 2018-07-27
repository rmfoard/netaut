#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdelete-non-virtual-dtor"
#include "Snap.h"
#pragma GCC diagnostic pop
#include "rule.h"
#include "machine.h"

//---------------
// GetDegStats
//---------------
void Machine::GetDegStats(DegStats& degStats) {
    degStats.nrInDeg = 99;
}
/*
    Json::Value inDegreeCount;
    TVec<TPair<TInt, TInt> > inDegCnt;
    TSnap::GetInDegCnt(machine->get_graph(), inDegCnt); 
    for (int i = 0; i < inDegCnt.Len(); i += 1) {
        Json::Value inDegreeCountPair;
        inDegreeCountPair.append((int) inDegCnt[i].Val1);
        inDegreeCountPair.append((int) inDegCnt[i].Val2);
        inDegreeCount.append(inDegreeCountPair);
    }
    info["inDegreeCount"] = inDegreeCount;
    info["nrInDegrees"] = inDegCnt.Len();

    Json::Value outDegreeCount;
    TVec<TPair<TInt, TInt> > outDegCnt;
    TSnap::GetOutDegCnt(machine->get_graph(), outDegCnt);
    for (int i = 0; i < outDegCnt.Len(); i += 1) {
        Json::Value outDegreeCountPair;
        outDegreeCountPair.append((int) outDegCnt[i].Val1);
        outDegreeCountPair.append((int) outDegCnt[i].Val2);
        outDegreeCount.append(outDegreeCountPair);
    }
    info["outDegreeCount"] = outDegreeCount;
    info["nrOutDegrees"] = outDegCnt.Len();
*/
