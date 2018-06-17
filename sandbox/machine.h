#ifndef MACHINE_H
#define MACHINE_H

#include <queue>

//---------------
class MachineS {

struct MachineState {
    int *nodeStates;
    PNGraph graph;
};

public:
    struct Statistics {
        long unsigned int multiEdgesAvoided;
        long unsigned int selfEdgesAvoided;
        long unsigned int triadOccurrences[NR_TRIAD_STATES];
    };

    MachineS(rulenr_t, int, int);
    ~MachineS();
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
    PNGraph m_graph;
    PNGraph m_nextGraph;
    int* m_nodeStates;
    int* m_nextNodeStates;
    int m_cycleCheckDepth;
    std::queue<MachineState> m_stateHistory;

    void AdvanceNode(TNGraph::TNodeI, int);
    int Cycling();
    void InitNodeStates();
    void ShowDF(int, bool*);
    bool StateMatchesCurrent(MachineState);
};
#endif
