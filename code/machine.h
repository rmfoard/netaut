#ifndef MACHINE_H
#define MACHINE_H

#include <getopt.h>
#include <json/json.h>
#include <queue>
#include "Snap.h"

//---------------
class Machine {

public:
    // TODO: Move structure def to 2D.
    struct MachineState {
        int *nodeStates;
        PNGraph graph;
        unsigned int stateHash;
    };

    virtual ~Machine() = 0;
    virtual void BuildMachine(rulenr_t, int, int, std::string, int, std::string) = 0;
    virtual void AddCommandOptions(struct option[], int) = 0;
    virtual std::string get_machineType() = 0;
    virtual PNGraph get_graph() = 0;
    virtual int* get_nodeStates() = 0;
    virtual int IterateMachine(int) = 0;
    virtual void ParseCommand(int, char**) = 0;

    virtual void AddSummaryInfo(Json::Value&) = 0;
    virtual void AdvanceNode(TNGraph::TNodeI) = 0;
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

    std::string m_machineType;
    int m_nrNodes;

    Rule* m_rule;
    const int* m_ruleParts;

    PNGraph m_graph;
    PNGraph m_nextGraph;
    int* m_nodeStates;
    int* m_nextNodeStates;
    int* m_nextL;
    int* m_nextR;

    unsigned char* m_stateHistoryHashTable;
    unsigned int m_cycleCheckDepth;
    std::queue<Machine::MachineState> m_stateHistory;
};
#endif
