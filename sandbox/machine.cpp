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
    int *m_nodeStates;
    int *m_nextNodeStates;
    std::vector<int>* m_ruleParts;

    void AdvanceNode(TNEGraph::TNodeI);
    void InitNodeStates();
};

MachineS::MachineS(uintmax_t ruleNr, int nrNodes) {
    m_ruleNr = ruleNr;
    m_nrNodes = nrNodes;
    m_graph = TNEGraph::New();
    m_nodeStates = new int[nrNodes];
    m_nextNodeStates = new int[nrNodes];
    BuildRing(nrNodes, m_graph);
    InitNodeStates();
    m_ruleParts = RuleParts(8, 20, ruleNr);
}

void MachineS::InitNodeStates() {
    for (int i = 0; i < m_nrNodes; i += 1) m_nodeStates[i] = 0;
    m_nodeStates[m_nrNodes / 2] = 1;
}

// Cycle: Run one step of the loaded rule.
void MachineS::Cycle() {
    PNEGraph nextGraph = TNEGraph::New();
/*
    // Traverse the current graph, applying the rule.
    // (Right now, we just copy.)
    for (TNEGraph::TNodeI ni = m_graph->BegNI(); ni < m_graph->EndNI(); ni++) {
        int n = ni.GetId();
        //printf("node Id: %d, state: %d\n", n, m_nodeStates[n]);
        nextGraph->AddNode(n);
        m_nextNodeStates[n] = m_nodeStates[n];
    }

    for (TNEGraph::TNodeI ni = m_graph->BegNI(); ni < m_graph->EndNI(); ni++) {
        for (int e = 0; e < ni.GetOutDeg(); e++) {
            int orig = ni.GetId();
            int dest = ni.GetOutNId(e);
            //printf("edge: %d --> %d\n", orig, dest);
            nextGraph->AddEdge(orig, dest);
        }
    }
*/

    // Apply one generation of the loaded rule, creating the next generation
    // state in 'm_nextGraph'.
    for (TNEGraph::TNodeI ni = m_graph->BegNI(); ni < m_graph->EndNI(); ni++) AdvanceNode(ni);

    // Cycling finished, replace "current" structures with "next" counterparts.
    // (Abandon m->graph to garbage collection.)
    m_graph = nextGraph;

    // Unlike with the net, avoid de- and re-allocating state storage.
    // Instead, alternate "current" and "next" roles.
    int* swap = m_nodeStates;
    m_nodeStates = m_nextNodeStates;
    m_nextNodeStates = swap;
}

void MachineS::AdvanceNode(TNEGraph::TNodeI NI) {
    int nodeId = NI.GetId();
    int lState = m_nodeStates[NI.GetOutNId(0)];
    int nState = m_nodeStates[nodeId];
    int rState = m_nodeStates[NI.GetOutNId(1)];
    int triadState = lState * 4 + nState * 2 + rState;

    assert(0 <= triadState && triadState <= 7);
    int action = (*m_ruleParts)[triadState];
    assert(0 <= action && action <= 19);
    switch (action) {
        case 0:
            m_nextNodeStates[nodeId] = 0;
            break;
        case 1:
            m_nextNodeStates[nodeId] = 1;
            break;
        case 2:
            printf("apply action %d\n", action);
            break;
        case 3:
            printf("apply action %d\n", action);
            break;
        case 4:
            printf("apply action %d\n", action);
            break;
        case 5:
            printf("apply action %d\n", action);
            break;
        case 6:
            printf("apply action %d\n", action);
            break;
        case 7:
            printf("apply action %d\n", action);
            break;
        case 8:
            printf("apply action %d\n", action);
            break;
        case 9:
            printf("apply action %d\n", action);
            break;
        case 10:
            printf("apply action %d\n", action);
            break;
        case 11:
            printf("apply action %d\n", action);
            break;
        case 12:
            printf("apply action %d\n", action);
            break;
        case 13:
            printf("apply action %d\n", action);
            break;
        case 14:
            printf("apply action %d\n", action);
            break;
        case 15:
            printf("apply action %d\n", action);
            break;
        case 16:
            printf("apply action %d\n", action);
            break;
        case 17:
            printf("apply action %d\n", action);
            break;
        case 18:
            printf("apply action %d\n", action);
            break;
        case 19:
            printf("apply action %d\n", action);
            break;
    }
}

int main(const int argc, const char* argv[]) {
    std::vector<int> ruleParts = { 0, 1, 1, 1, 0, 1, 1, 0  };
    uintmax_t ruleNr = RuleNr(8, 20, ruleParts);
    printf("ruleNr: %lld\n", ruleNr);

    std::vector<int>* rulePartsBack = RuleParts(8, 20, ruleNr);
    for (int i = 0; i < 8; i += 1)
        printf("rulePart %d: action %d\n", i, (*rulePartsBack)[i]);

    MachineS* m = new MachineS(ruleNr, 10);
    for (int i = 1; i <= 1; i += 1) m->Cycle();
}
