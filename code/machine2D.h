#ifndef MACHINE2D_H
#define MACHINE2D_H

#include <getopt.h>
#include <json/json.h>
#include "machine.h"

//---------------
class Machine2D : public Machine {

public:

    struct Statistics {
        long unsigned int multiEdgesAvoided;
        long unsigned int selfEdgesAvoided;
        long unsigned int triadOccurrences[NR_TRIAD_STATES];
        long unsigned int hashCollisions;
        double initDegEntropy;
        double finDegEntropy;
    };

    Machine2D(std::string type) : Machine() { m_machineType = type; };
    ~Machine2D();
    virtual void BuildMachine(rulenr_t, int, int, std::string, int, std::string, int);
    virtual PNEGraph get_graph();
    virtual int* get_nodeStates();
    virtual int IterateMachine(int);


    virtual void AddSummaryInfo(Json::Value&);
    virtual void AdvanceNode(TNEGraph::TNodeI);
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

    Statistics m_stats;
};

#define STATE_HISTORY_HASH_TABLE_LEN 65521
#define HASH_MULTIPLIER 641
#endif
