#ifndef MACHINE_H
#define MACHINE_H
//---------------
class MachineS {

public:
    MachineS(rulenr_t, int);
    ~MachineS();
    // TODO: Remove 'm_' from the following getter.
    PNGraph get_graph();
    int* get_nodeStates();
    void Cycle(int, int, int);
    void ShowDepthFirst(int);

    Rule* m_rule;
    int m_nrNodes;
    const int* m_ruleParts;

private:
    PNGraph m_graph;
    PNGraph m_nextGraph;
    int* m_nodeStates;
    int* m_nextNodeStates;

    void AdvanceNode(TNGraph::TNodeI, int, int);
    void InitNodeStates();
    void ShowDF(int, bool*);
};
#endif
