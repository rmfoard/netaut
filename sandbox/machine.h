#ifndef MACHINE_H
#define MACHINE_H
//---------------
class MachineS {

public:
    MachineS(rulenr_t, int);
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

    void AdvanceNode(TNGraph::TNodeI, int, int);
    bool Cycling();
    void InitNodeStates();
    void ShowDF(int, bool*);
};
#endif
