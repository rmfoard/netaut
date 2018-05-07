#include "Snap.h"
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>

// TODO: Move these to a header file for 'rules'.
uintmax_t RuleNr(const int, const int, const std::vector<int>);
std::vector<int>* RuleParts(const int, const int, const uintmax_t);

// TODO: Consider moving this to a 'graph_factory' module.
static void BuildRing(int nrNodes, PNEGraph graph) {
    assert(nrNodes > 1);

    for (int n = 0; n < nrNodes; n += 1) graph->AddNode(n);
    for (int n = 0; n < nrNodes - 1; n += 1) graph->AddEdge(n, n + 1);
    graph->AddEdge(nrNodes - 1, 0);
    graph->AddEdge(0, nrNodes - 1);
    for (int n = 1; n < nrNodes; n += 1) graph->AddEdge(n, n - 1);
}

class MachineS {

public:
    MachineS(uintmax_t, int);
    void Cycle();

private:
    uintmax_t m_ruleNr;
    int m_nrNodes;
    PNEGraph m_graph;
    int *m_color;
    std::vector<int>* m_ruleParts;
};

MachineS::MachineS(uintmax_t ruleNr, int nrNodes) {
    m_ruleNr = ruleNr;
    m_nrNodes = nrNodes;
    m_graph = TNEGraph::New();
    BuildRing(nrNodes, m_graph);
    m_color = new int[nrNodes];
    m_ruleParts = RuleParts(8, 20, ruleNr);
}

// Cycle: Run one step of the loaded rule.
void MachineS::Cycle() {
    PNEGraph nextGraph = TNEGraph::New();

    // Traverse the current graph building 'nextGraph'.
    for (TNEGraph::TNodeI NI = m_graph->BegNI(); NI < m_graph->EndNI(); NI++) ;
}

int main(const int argc, const char* argv[]) {
    std::vector<int> ruleParts = { 19, 19, 19, 19, 19, 19, 19, 19  };
    uintmax_t ruleNr = RuleNr(8, 20, ruleParts);
    printf("ruleNr: %lld\n", ruleNr);

    std::vector<int>* rulePartsBack = RuleParts(8, 20, ruleNr);
    for (int i = 0; i < 8; i += 1)
        printf("rulePart %d: action %d\n", i, (*rulePartsBack)[i]);

    MachineS* m = new MachineS(ruleNr, 10);
}
