#ifndef MACHINE_H
#define MACHINE_H

#include <getopt.h>
#include <json/json.h>
#include <queue>
#include <vector>
#include "Snap.h"
#include "rule.h"

//---------------
class Machine {

public:
    virtual ~Machine() = 0;
    // TODO: Move structure def to 2D.
    struct MachineState {
        int *nodeStates;
        PNEGraph graph;
        unsigned int stateHash;
    };

    typedef struct DegStats {
        int nrInDeg;
        TVec<TPair<TInt, TInt> > inDegCnt;
        int nrOutDeg;
        TVec<TPair<TInt, TInt> > outDegCnt;
        double inDegEntropy;
        double outDegEntropy;
    } DegStats;

    std::string get_machineType();
    void set_machineType(std::string);
    virtual void BuildMachine(rulenr_t, int, int, std::string, int, std::string, int) = 0;
    virtual PNEGraph get_graph() = 0;
    virtual int* get_nodeStates() = 0;
    virtual int IterateMachine(int) = 0;

    virtual void AddSummaryInfo(Json::Value&) = 0;
    virtual void AdvanceNode(TNEGraph::TNodeI) = 0;
    virtual void BuildRing() = 0;
    virtual void BuildTree() = 0;
    virtual void BuildRandomGraph() = 0;
    virtual int Cycling(unsigned int) = 0;
    virtual int EliminateMultiEdges() = 0;
    virtual void EliminateNode(int) = 0;
    virtual void InitTape(std::string, int) = 0;
    virtual void InitTopo(std::string) = 0;
    virtual void RandomizeTapeState(int) = 0;
    virtual unsigned int CurStateHash() = 0;
    virtual bool StateMatchesCurrent(MachineState) = 0;

    void GetDegStats(DegStats&);
    double EstimateDegExp(std::vector<int>&, int);

    std::string m_machineType;
    int m_nrNodes;
    int m_noChangeTopo;

    Rule* m_rule;
    const int* m_ruleParts;

    PNEGraph m_graph;
    PNEGraph m_nextGraph;
    TRnd* m_snapRnd;
    int* m_nodeStates;
    int* m_nextNodeStates;
    int* m_nextL;
    int* m_nextR;

    unsigned char* m_stateHistoryHashTable;
    unsigned int m_cycleCheckDepth;
    std::queue<Machine::MachineState> m_stateHistory;
};
#endif
