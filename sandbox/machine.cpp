#include "Snap.h"
#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <algorithm>
#include <string>
#include "rule.h"
#include "machine.h"

//---------------
// TODO: Consider moving this to a 'graph_factory' module.
static
void BuildRing(int nrNodes, PNEGraph graph) {
    assert(nrNodes > 1);

    for (int n = 0; n < nrNodes; n += 1) graph->AddNode(n);
    for (int n = 1; n < nrNodes; n += 1) graph->AddEdge(n, n - 1);
    graph->AddEdge(0, nrNodes - 1);
    for (int n = 0; n < nrNodes - 1; n += 1) graph->AddEdge(n, n + 1);
    graph->AddEdge(nrNodes - 1, 0);
}

//---------------
MachineS::MachineS(rulenr_t ruleNr, int nrNodes) {
    m_rule = new Rule(ruleNr);
    m_ruleParts = m_rule->get_ruleParts();
    m_nrNodes = nrNodes;
    m_graph = TNEGraph::New();
    m_nodeStates = new int[m_nrNodes];
    m_nextNodeStates = new int[m_nrNodes];
    BuildRing(m_nrNodes, m_graph);
    InitNodeStates();
}

//---------------
MachineS::~MachineS() {
    delete m_ruleParts;
    delete m_nodeStates;
    delete m_nextNodeStates;
}

//---------------
// TODO: Use the proper form for "getters."
//---------------
PNEGraph MachineS::get_m_graph() { return m_graph; }

//---------------
void MachineS::InitNodeStates() {
    for (int i = 0; i < m_nrNodes; i += 1) m_nodeStates[i] = 0;
    m_nodeStates[m_nrNodes / 2] = 1;
    //m_nodeStates[m_nrNodes / 2 + 2] = 1;
}

//---------------
// Cycle
//
// Run one step of the loaded rule.
//---------------
void MachineS::Cycle(int selfEdges, int noMultiEdges) {

    // Show node states at the beginning of the cycle.
    /* temporarily hold aside
    if (CommandOpts::printTape) {
        for (int i = m_nrNodes/2 - std::min(64, m_nrNodes/2); i <= m_nrNodes/2 + std::min(64, m_nrNodes/2); i += 1)
            printf("%s", (m_nodeStates[i] == 1) ? "X" : " ");
        printf("\n");
        ShowDepthFirst(0);
    }
    */

    // Create the seed of the graph's next generation.
    m_nextGraph = TNEGraph::New();

    // Copy all nodes from current to next graph (added edges will "forward
    // reference" them). Rule actions in 'AdvanceNode' are responsible for
    // [re-]creating all edges in the new graph -- those that have been changed
    // and those that remain the same.
    for (TNEGraph::TNodeI NIter = m_graph->BegNI(); NIter < m_graph->EndNI(); NIter++) {
        int nId = NIter.GetId();
        m_nextGraph->AddNode(nId);
    }

    // Apply one generation of the loaded rule, creating the next generation
    // state in 'm_nextGraph' and 'm_nextNodeStates'.
    for (TNEGraph::TNodeI NIter = m_graph->BegNI(); NIter < m_graph->EndNI(); NIter++)
        AdvanceNode(NIter, selfEdges, noMultiEdges);

    // Cycling finished, replace "current" structures with "next" counterparts.
    // (Abandoning m->graph to garbage collection.)
    m_graph = m_nextGraph;

    // Unlike with the graph, avoid de- and re-allocating state storage.
    // Instead, alternate "current" and "next" roles.
    int* swap = m_nodeStates;
    m_nodeStates = m_nextNodeStates;
    m_nextNodeStates = swap;
}

//---------------
// AdvanceNode
//
// Apply the loaded rule to the indicated node, adjusting "next"
// structures.  Every action must put appropriate edges in place
// for the node being advanced.
//---------------
void MachineS::AdvanceNode(TNEGraph::TNodeI NIter, int selfEdges, int noMultiEdges) {
    bool selfEdge = selfEdges;
    bool multiEdge = !noMultiEdges;

    // Get node ids of neighbors.
    int nNId = NIter.GetId();
    int lNId = NIter.GetOutNId(0);
    int rNId = NIter.GetOutNId(1);

    // TODO: Remove unnecessary intermediate variables.
    // Set state variables for convenience.
    int nState = m_nodeStates[nNId]; 
    int lState = m_nodeStates[lNId];
    int rState = m_nodeStates[rNId];

    int triadState = lState * 4 + nState * 2 + rState;

    // Gather info on neighbors' neighbors.
    TNEGraph::TNodeI lNIter = m_graph->GetNI(lNId); // iterator for left neighbor
    TNEGraph::TNodeI rNIter = m_graph->GetNI(rNId); // iterator for right neighbor
    int llNId = lNIter.GetOutNId(0);
    int lrNId = lNIter.GetOutNId(1);
    int rlNId = rNIter.GetOutNId(0);
    int rrNId = rNIter.GetOutNId(1);

    // Prepare an array of possible new destinations for edges
    int* newDsts = new int[NR_POSS_DSTS];
    newDsts[LEDGE] = lNId;
    newDsts[LLEDGE] = llNId;
    newDsts[LREDGE] = lrNId;
    newDsts[REDGE] = rNId;
    newDsts[RLEDGE] = rlNId;
    newDsts[RREDGE] = rrNId;

    assert(0 <= triadState && triadState < NR_TRIAD_STATES);
    const int rulePart = m_ruleParts[triadState];
    assert(0 <= rulePart && rulePart < NR_ACTIONS);

    // Unpack the rule part into left edge, right edge, and node actions
    const int lAction = (rulePart / 2) / NR_POSS_DSTS;
    const int rAction = (rulePart / 2) % NR_POSS_DSTS;
    const int nAction = rulePart % 2;

    // Confirm that topological invariants still hold.
    assert(lNId != rNId || multiEdge);
    assert((lNId != nNId && rNId != nNId) || selfEdge);

    // TODO: Keep running stats on action use.

    // Apply the node action unconditionally.
    m_nextNodeStates[nNId] = nAction;

    // Apply the topological actions...
    int lNewDst = newDsts[lAction];
    int rNewDst = newDsts[rAction];

    // ...only if they preserve the topo invariants.
    if (((lNewDst != nNId && rNewDst != nNId) || selfEdge)
      && ((lNewDst != rNewDst) || multiEdge)) {
        m_nextGraph->AddEdge(nNId, lNewDst);
        m_nextGraph->AddEdge(nNId, rNewDst);
    }
    else { // Otherwise, re-create the previous edges.
        m_nextGraph->AddEdge(nNId, lNId);
        m_nextGraph->AddEdge(nNId, rNId);
    }

    delete newDsts;
}

//---------------
// ShowDepthFirst
//
// Print a string representing node values traversed in depth-first order from 'root'.
//---------------
void MachineS::ShowDepthFirst(int rootNId) {
    bool *visited = new bool[m_nrNodes];
    for (int i = 0; i < m_nrNodes; i += 1) visited[i] = false;

    ShowDF(rootNId, visited);
    printf("\n");

    delete visited;
}

//---------------
void MachineS::ShowDF(int rootNId, bool *visited) {
    if (!visited[rootNId]) {
        visited[rootNId] = true;
        printf("%s", (m_nodeStates[rootNId] == NBLACK) ? "+" : " ");
        TNEGraph::TNodeI rootIter = m_graph->GetNI(rootNId);
        int lNId = rootIter.GetOutNId(0);
        int rNId = rootIter.GetOutNId(1);
        ShowDF(lNId, visited);
        ShowDF(rNId, visited);
    }
}
