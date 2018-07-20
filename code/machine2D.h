#ifndef MACHINE2D_H
#define MACHINE2D_H

#include <getopt.h>
#include <queue>
#include <json/json.h>
#include "machine.h"
#include "machine2D.h"

//---------------
class Machine2D : public Machine {

public:

    struct Statistics {
        long unsigned int multiEdgesAvoided;
        long unsigned int selfEdgesAvoided;
        long unsigned int triadOccurrences[NR_TRIAD_STATES];
        long unsigned int hashCollisions;
    };

    Machine2D() : Machine() {};
    ~Machine2D();
    virtual void BuildMachine(rulenr_t, int, int, std::string, int, std::string);
    virtual void AddMachineCommandOptions(struct option[], int);
    virtual std::string get_machineType();
    virtual PNGraph get_graph();
    virtual int* get_nodeStates();
    virtual int IterateMachine(int);
    virtual void ParseCommand(int, char**);

    std::string m_machineType;
    PNGraph m_graph;
    PNGraph m_nextGraph;
    int* m_nodeStates;
    int* m_nextNodeStates;
    int* m_nextL;
    int* m_nextR;
    unsigned char* m_stateHistoryHashTable;
    unsigned int m_cycleCheckDepth;
    std::queue<Machine::MachineState> m_stateHistory;

    virtual void AddSummaryInfo(Json::Value&);
    virtual void AdvanceNode(TNGraph::TNodeI);
    virtual void BuildRing();
    virtual void BuildTree();
    virtual void BuildRandomGraph();
    virtual int Cycling(unsigned int);
    virtual int EliminateMultiEdges();
    virtual void EliminateNode(int);
    virtual void InitTape(std::string, int);
    virtual void InitTopo(std::string);
    virtual void RandomizeTapeState(int);
    virtual unsigned int CurStateHash();
    virtual bool StateMatchesCurrent(Machine::MachineState);

    Rule* m_rule;
    int m_nrNodes;
    const int* m_ruleParts;
    Statistics m_stats;

};

#define STATE_HISTORY_HASH_TABLE_LEN 65521
#define HASH_MULTIPLIER 641
#endif
