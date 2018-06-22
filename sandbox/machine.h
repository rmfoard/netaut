#ifndef MACHINE_H
#define MACHINE_H

#include <queue>

//---------------
class MachineS {

public:
    struct Statistics {
        long unsigned int multiEdgesAvoided;
        long unsigned int selfEdgesAvoided;
        long unsigned int triadOccurrences[NR_TRIAD_STATES];
    };

    MachineS(rulenr_t, int, int, std::string, int, std::string);
    ~MachineS();
    std::string get_machineType();
    PNGraph get_graph();
    int* get_nodeStates();
    Statistics* get_stats();
    int IterateMachine(int, int);
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
    unsigned char* m_stateHistoryHashTable;
    unsigned int m_cycleCheckDepth;
    std::queue<MachineState> m_stateHistory;

    void AdvanceNode(TNGraph::TNodeI, int);
    void BuildRing();
    void BuildTree();
    void BuildRandomGraph();
    int Cycling(unsigned int);
    void InitTape(std::string, int);
    void InitTopo(std::string);
    void RandomizeTapeState(int);
    unsigned int CurStateHash();
    bool StateMatchesCurrent(MachineState);
};

#define STATE_HISTORY_HASH_TABLE_LEN 65521
#define HASH_MULTIPLIER 641
#endif
