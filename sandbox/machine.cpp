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
MachineS::MachineS(rulenr_t ruleNr, int nrNodes, int cycleCheckDepth,
  std::string tapeStructure, std::string topoStructure) {
    m_rule = new Rule(ruleNr);
    m_ruleParts = m_rule->get_ruleParts();
    m_nrNodes = nrNodes;
    m_cycleCheckDepth = cycleCheckDepth;
    m_graph = TNGraph::New();
    m_nodeStates = new int[m_nrNodes];
    m_nextNodeStates = new int[m_nrNodes];
    m_stats.multiEdgesAvoided = 0;
    m_stats.selfEdgesAvoided = 0;
    for (int i = 0; i < NR_TRIAD_STATES; i += 1) m_stats.triadOccurrences[i] = 0;
    InitTape(tapeStructure);
    InitTopo(topoStructure);
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
PNGraph MachineS::get_graph() { return m_graph; }
int* MachineS::get_nodeStates() { return m_nodeStates; }
MachineS::Statistics* MachineS::get_stats() { return &m_stats; }

//---------------
// AdvanceNode
//
// Apply the loaded rule to the indicated node, adjusting "next"
// structures.  Every action must put appropriate edges in place
// for the node being advanced.
//---------------
void MachineS::AdvanceNode(TNGraph::TNodeI NIter, int selfEdges) {
    bool selfEdge = selfEdges;

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
    m_stats.triadOccurrences[triadState] += 1;

    // Gather info on neighbors' neighbors.
    TNGraph::TNodeI lNIter = m_graph->GetNI(lNId); // iterator for left neighbor
    TNGraph::TNodeI rNIter = m_graph->GetNI(rNId); // iterator for right neighbor
    int llNId = lNIter.GetOutNId(0);
    int lrNId = lNIter.GetOutNId(1);
    int rlNId = rNIter.GetOutNId(0);
    int rrNId = rNIter.GetOutNId(1);

    // Prepare an array of possible new destinations for edges
    int* newDsts = new int[NR_DSTS];
    newDsts[LEDGE] = lNId;
    newDsts[LLEDGE] = llNId;
    newDsts[LREDGE] = lrNId;
    newDsts[REDGE] = rNId;
    newDsts[RLEDGE] = rlNId;
    newDsts[RREDGE] = rrNId;

    assert(0 <= triadState && triadState < NR_TRIAD_STATES);
    const int rulePart = m_ruleParts[triadState];
    assert(0 <= rulePart && rulePart < NR_ACTIONS);

    // TODO: (minor opt) Maintain unpacked structures built when
    //   'ruleParts' is developed during initialization.
    // Unpack the rule part into left edge, right edge, and node actions
    const int lAction = (rulePart / 2) / NR_DSTS;
    const int rAction = (rulePart / 2) % NR_DSTS;
    const int nAction = rulePart % 2;

    // Confirm that topological invariants still hold.
    assert(lNId != rNId);
    assert((lNId != nNId && rNId != nNId) || selfEdge);

    // TODO: Keep running stats on action use.

    // Apply the node action unconditionally.
    m_nextNodeStates[nNId] = nAction;

    // Apply the topological action...
    int lNewDst = newDsts[lAction];
    int rNewDst = newDsts[rAction];

    // ...only if it preserves the topo invariants.
    if (((lNewDst != nNId && rNewDst != nNId) || selfEdge) && lNewDst != rNewDst) {
        m_nextGraph->AddEdge(nNId, lNewDst);
        m_nextGraph->AddEdge(nNId, rNewDst);
    }
    else { // Otherwise, re-create the previous edges.
        m_nextGraph->AddEdge(nNId, lNId);
        m_nextGraph->AddEdge(nNId, rNId);

        if (lNewDst == rNewDst) m_stats.multiEdgesAvoided += 1;
        if (lNewDst == nNId || rNewDst == nNId) m_stats.selfEdgesAvoided += 1;
    }

    delete newDsts;
}

//---------------
void MachineS::BuildRing() {
    assert(m_nrNodes > 1);

    for (int n = 0; n < m_nrNodes; n += 1) m_graph->AddNode(n);
    for (int n = 1; n < m_nrNodes; n += 1) m_graph->AddEdge(n, n - 1);
    m_graph->AddEdge(0, m_nrNodes - 1);
    for (int n = 0; n < m_nrNodes - 1; n += 1) m_graph->AddEdge(n, n + 1);
    m_graph->AddEdge(m_nrNodes - 1, 0);
}

//---------------
// Cycling
//
// Compare the current machine state (node states and graph topology) to
// determine whether it is identical to one of the preceding 'cycleCheckDepth'
// states. Return the cycle length if so, zero otherwise.
//---------------
int MachineS::Cycling() {
    MachineState eoq = {nullptr, nullptr};
    m_stateHistory.push(eoq);
    int cycleLength = m_stateHistory.size(); // invariant: <=
    MachineState candidate = m_stateHistory.front();
    m_stateHistory.pop();
    while (candidate.nodeStates != nullptr) {
        if (StateMatchesCurrent(candidate)) {
            return cycleLength;
        }
        m_stateHistory.push(candidate);
        candidate = m_stateHistory.front();
        m_stateHistory.pop();
        cycleLength -= 1;
    }
    return 0; // 0 indicates that no cycle was found.
}

//---------------
// InitNodeStates
//---------------
void MachineS::InitNodeStates() {
    for (int i = 0; i < m_nrNodes; i += 1) m_nodeStates[i] = 0;
    m_nodeStates[m_nrNodes / 2] = 1;
}

//---------------
// InitTape
//---------------
void MachineS::InitTape(std::string tapeStructure) {
    if (tapeStructure == "single")
        InitNodeStates();
    else {
        ; // TODO: throw exception
    }
}

//---------------
// InitTopo
//---------------
void MachineS::InitTopo(std::string topoStructure) {
    if (topoStructure == "ring")
        BuildRing();
    else {
        ; // TODO: throw exception
    }
}


//---------------
// IterateMachine
//
// Run one step of the loaded rule.
// Return the length of any detected state cycle (0 => no cycle found)
//---------------
int MachineS::IterateMachine(int selfEdges, int iterationNr) {

    // Show node states at the beginning of the cycle.
    /* temporarily hold aside
    if (CommandOpts::printTape) {
        for (int i = m_nrNodes/2 - std::min(64, m_nrNodes/2); i <= m_nrNodes/2 + std::min(64, m_nrNodes/2); i += 1)
            printf("%s", (m_nodeStates[i] == 1) ? "X" : " ");
        printf("\n");
        ShowDepthFirst(0);
    }
    */
    // Stop and report if the machine is cycling.
    int cycleLength = Cycling();
    if (cycleLength > 0) return cycleLength; // report early termination

    // Shift the current state into history.
    // Discard the head of the queue if it's full.
    MachineState discard;
    if (m_stateHistory.size() == m_cycleCheckDepth) {
        discard = m_stateHistory.front();
        delete discard.nodeStates;
        // (We leave discard.graph for Snap's reclamation scheme.)
        m_stateHistory.pop();
    }
    MachineState newEntry;
    newEntry.graph = m_graph;
    newEntry.nodeStates = new int[m_nrNodes];
    for (int i = 0; i < m_nrNodes; i += 1) newEntry.nodeStates[i] = m_nodeStates[i];
    m_stateHistory.push(newEntry);
    // TODO: Clean up the leftover queue entries in the destructor.

    // Create the seed of the graph's next generation.
    m_nextGraph = TNGraph::New();

    // Copy all nodes from current to next graph (added edges will "forward
    // reference" them). Rule actions in 'AdvanceNode' are responsible for
    // [re-]creating all edges in the new graph -- those that have been changed
    // and those that remain the same.
    for (TNGraph::TNodeI NIter = m_graph->BegNI(); NIter < m_graph->EndNI(); NIter++) {
        int nId = NIter.GetId();
        m_nextGraph->AddNode(nId);
    }

    // TODO: Extend this loop to also build by-nodeId neighbor vectors
    //   for use in 'AdvanceNode'. (middling opt)
    // Apply one generation of the loaded rule, creating the next generation
    // state in 'm_nextGraph' and 'm_nextNodeStates'.
    for (TNGraph::TNodeI NIter = m_graph->BegNI(); NIter < m_graph->EndNI(); NIter++) {
        AdvanceNode(NIter, selfEdges);
    }

    // Cycling finished, replace "current" structures with "next" counterparts.
    // (Abandoning m->graph to garbage collection.)
    m_graph = m_nextGraph;

    // Unlike with the graph, avoid de- and re-allocating state storage.
    // Instead, alternate "current" and "next" roles.
    int* swap = m_nodeStates;
    m_nodeStates = m_nextNodeStates;
    m_nextNodeStates = swap;
    return 0; // report no state cycle found
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
        TNGraph::TNodeI rootIter = m_graph->GetNI(rootNId);
        int lNId = rootIter.GetOutNId(0);
        int rNId = rootIter.GetOutNId(1);
        ShowDF(lNId, visited);
        ShowDF(rNId, visited);
    }
}

//---------------
// StateMatchesCurrent
//
// Return true if the machine state in the parameter is identical
// to the machine's current state.
//---------------
bool MachineS::StateMatchesCurrent(MachineState other) {
    // Compare node states.
    for (int i = 0; i < m_nrNodes; i += 1)
        if (m_nodeStates[i] != other.nodeStates[i]) return false;

    // Compare topologies.
    TNGraph::TNodeI graphNodeIter = m_graph->BegNI();
    TNGraph::TNodeI otherGraphNodeIter = other.graph->BegNI();
    while (graphNodeIter < m_graph->EndNI()) {
        if (graphNodeIter.GetId() != otherGraphNodeIter.GetId()) return false;
        if (graphNodeIter.GetOutNId(0) != otherGraphNodeIter.GetOutNId(0)) return false;
        if (graphNodeIter.GetOutNId(1) != otherGraphNodeIter.GetOutNId(1)) return false;
        graphNodeIter++;
        otherGraphNodeIter++;
    }

    return true;
}
