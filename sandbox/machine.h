#ifndef MACHINE_H
#define MACHINE_H

#include <queue>

//---------------
class MachineS {

struct MachineState {
    int* nodeStates;
    PNGraph graph;
};

public:
    MachineS(rulenr_t, int, int);
    ~MachineS();
    PNGraph get_graph();
    int* get_nodeStates();
    bool IterateMachine(int, int, int);
    void ShowDepthFirst(int);

    Rule* m_rule;
    int m_nrNodes;
    const int* m_ruleParts;

private:
    PNGraph m_graph;
    PNGraph m_nextGraph;
    int* m_nodeStates;
    int* m_nextNodeStates;
    int m_cycleCheckDepth;
    std::queue<MachineState> m_stateHistory;

    void AdvanceNode(TNGraph::TNodeI, int, int);
    bool Cycling();
    void InitNodeStates();
    void ShowDF(int, bool*);
};
#endif
