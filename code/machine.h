#ifndef MACHINE_H
#define MACHINE_H

#include <getopt.h>
#include <queue>

//---------------
class MachineS {

public:
    struct Statistics {
        long unsigned int multiEdgesAvoided;
        long unsigned int selfEdgesAvoided;
        long unsigned int triadOccurrences[NR_TRIAD_STATES];
        long unsigned int hashCollisions;
    };

    MachineS(rulenr_t, int, int, std::string, int, std::string, int, char* argv[], struct option[]);
    ~MachineS();
    std::string get_machineType();
    PNGraph get_graph();
    int* get_nodeStates();
    Statistics* get_stats();
    int IterateMachine(int);
    void ShowDepthFirst(int);

    Rule* m_rule;
    int m_nrNodes;
    const int* m_ruleParts;
    Statistics m_stats;

private:
    struct MachineState {
        int *nodeStates;
        PNGraph graph;
        unsigned int stateHash;
    };

    std::string m_machineType;
    PNGraph m_graph;
    PNGraph m_nextGraph;
    int* m_nodeStates;
    int* m_nextNodeStates;
    int* m_nextL;
    int* m_nextR;
    unsigned char* m_stateHistoryHashTable;
    unsigned int m_cycleCheckDepth;
    std::queue<MachineState> m_stateHistory;

    void AdvanceNode(TNGraph::TNodeI);
    void BuildRing();
    void BuildTree();
    void BuildRandomGraph();
    int Cycling(unsigned int);
    int EliminateMultiEdges();
    void EliminateNode(int);
    void InitTape(std::string, int);
    void InitTopo(std::string);
    void RandomizeTapeState(int);
    unsigned int CurStateHash();
    bool StateMatchesCurrent(MachineState);
};

#define STATE_HISTORY_HASH_TABLE_LEN 65521
#define HASH_MULTIPLIER 641
#endif
