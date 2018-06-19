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
    };

    PNGraph m_graph;
    PNGraph m_nextGraph;
    int* m_nodeStates;
    int* m_nextNodeStates;
    unsigned int m_cycleCheckDepth;
    std::queue<MachineState> m_stateHistory;

    void AdvanceNode(TNGraph::TNodeI, int);
    void BuildRing();
    void BuildTree();
    int Cycling();
    void InitTape(std::string, int);
    void InitTopo(std::string);
    void RandomizeTapeState(int);
    bool StateMatchesCurrent(MachineState);
};
#endif
