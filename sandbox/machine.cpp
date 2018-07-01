#include "Snap.h"
#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <algorithm>
#include <iostream>
#include <string>
#include "rule.h"
#include "machine.h"

//---------------
MachineS::MachineS(rulenr_t ruleNr, int nrNodes, int cycleCheckDepth,
  std::string tapeStructure, int tapePctBlack, std::string topoStructure) {
    m_machineType = std::string("S");
    m_rule = new Rule(ruleNr);
    m_ruleParts = m_rule->get_ruleParts();
    m_nrNodes = nrNodes;
    m_cycleCheckDepth = cycleCheckDepth;
    m_graph = TNGraph::New();
    m_nodeStates = new int[m_nrNodes];
    m_nextNodeStates = new int[m_nrNodes];
    m_nextL = new int[m_nrNodes];
    m_nextR = new int[m_nrNodes];
    m_stateHistoryHashTable = new unsigned char[STATE_HISTORY_HASH_TABLE_LEN]();
    m_stats.multiEdgesAvoided = 0;
    m_stats.selfEdgesAvoided = 0;
    m_stats.hashCollisions = 0;
    for (int i = 0; i < NR_TRIAD_STATES; i += 1) m_stats.triadOccurrences[i] = 0;
    InitTape(tapeStructure, tapePctBlack);
    InitTopo(topoStructure);
}

//---------------
MachineS::~MachineS() {
    delete m_ruleParts;
    delete m_nodeStates;
    delete m_nextNodeStates;
    delete m_stateHistoryHashTable;

    // Clear any remaining storage in the state history queue.
    int queueLength = m_stateHistory.size(); // invariant: <=
    for (int i = 1; i <= queueLength; i += 1) {
        MachineState entry = m_stateHistory.front();
        delete entry.nodeStates;
        m_stateHistory.pop();
    }
}

//---------------
// TODO: Use the proper form for "getters."
//---------------
std::string MachineS::get_machineType() { return m_machineType; }
PNGraph MachineS::get_graph() { return m_graph; }
int* MachineS::get_nodeStates() { return m_nodeStates; }
MachineS::Statistics* MachineS::get_stats() { return &m_stats; }

//---------------
// AdvanceNode
//
// Apply the loaded rule to the indicated node, recording new edge
// destinations in the scratchpad. At this step, it's okay to create
// multi-edges; they'll be removed in post-processing.
//---------------
void MachineS::AdvanceNode(TNGraph::TNodeI NIter) {

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

    // Confirm that the multi-edge invariant still holds.
    assert(lNId != rNId);

    // Apply the node action.
    m_nextNodeStates[nNId] = nAction;

    // Note the provisional topological action in the scratchpad.
    assert(m_nextL[nNId] == -1 && m_nextR[nNId] == -1);
    m_nextL[nNId] = newDsts[lAction];
    m_nextR[nNId] = newDsts[rAction];

    delete newDsts;
}

//---------------
void MachineS::BuildRing() {
    assert(m_nrNodes > 1);

    // Create all the nodes.
    for (int n = 0; n < m_nrNodes; n += 1) m_graph->AddNode(n);

    for (int n = 1; n < m_nrNodes; n += 1) m_graph->AddEdge(n, n - 1);
    m_graph->AddEdge(0, m_nrNodes - 1);
    for (int n = 0; n < m_nrNodes - 1; n += 1) m_graph->AddEdge(n, n + 1);
    m_graph->AddEdge(m_nrNodes - 1, 0);
}

//---------------
void MachineS::BuildTree() {
    assert(m_nrNodes > 1);

    // Create all the nodes.
    for (int n = 0; n < m_nrNodes; n += 1) m_graph->AddNode(n);

    // Add edges to create a regular binary tree.
    for (int n = 0; n < m_nrNodes; n += 1) {
        // Set left child; leaves link to root.
        if (n*2 + 1 < m_nrNodes)
            m_graph->AddEdge(n, n*2 + 1);
        else
            m_graph->AddEdge(n, 0);

        // Set right child; leaves link to right "uncle."
        if ((n+1) * 2 < m_nrNodes)
            m_graph->AddEdge(n, (n+1) * 2);
        else
            m_graph->AddEdge(n, n/2 + 1);
    }
}

//---------------
void MachineS::BuildRandomGraph() {
    assert(m_nrNodes > 1);

    // Create all the nodes.
    for (int i = 0; i < m_nrNodes; i += 1) m_graph->AddNode(i);

    // Create two random edges on each node.
    for (int i = 0; i < m_nrNodes; i += 1) {
        int j;
        do j = rand() % m_nrNodes; while (j == i);
        m_graph->AddEdge(i, j);

        int k;
        do k = rand() % m_nrNodes; while (k == i || k == j);
        m_graph->AddEdge(i, k);
    }
}

//---------------
// Cycling
//
// Compare the current machine state (node states and graph topology) to
// determine whether it is identical to one of the preceding 'cycleCheckDepth'
// states. Return the cycle length if so, zero otherwise.
//---------------
int MachineS::Cycling(unsigned int curStateHash) {

    // If the current state's hash is not in the table,
    // no cycle is detected. (The check is not perfect, though,
    // unless we're keeping the entire history of the run.
    if (m_stateHistoryHashTable[curStateHash] == 0) return 0;

    // There was a hit in the hash table, so an exhaustive
    // comparison is necessary. Spin through the entire history
    // queue.
    MachineState eoq = {nullptr, nullptr, 0};
    m_stateHistory.push(eoq);
    int cycleLength = m_stateHistory.size(); // invariant: <=
    MachineState candidate = m_stateHistory.front();
    m_stateHistory.pop();

    while (candidate.nodeStates != nullptr) { // while not <end marker>
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
// InitTape
//---------------
void MachineS::InitTape(std::string tapeStructure, int tapePctBlack) {
    for (int i = 0; i < m_nrNodes; i += 1) m_nodeStates[i] = NWHITE;
    if (tapeStructure == "single-center")
        m_nodeStates[m_nrNodes/2] = NBLACK;

    else if (tapeStructure == "single-end")
        m_nodeStates[0] = NBLACK;

    else if (tapeStructure == "random")
        RandomizeTapeState(tapePctBlack);

    else
        throw std::runtime_error("--init-tape type is not recognized");
}

//---------------
// InitTopo
//---------------
void MachineS::InitTopo(std::string topoStructure) {
    if (topoStructure == "ring")
        BuildRing();
    else if (topoStructure == "tree")
        BuildTree();
    else if (topoStructure == "random")
        BuildRandomGraph();
    else
        throw std::runtime_error("--init-topo type is not recognized");
}


//---------------
// IterateMachine
//
// Run one step of the loaded rule.
// Return the length of any detected state cycle (0 => no cycle found)
//---------------
int MachineS::IterateMachine(int iterationNr) {

    // Stop and report if the machine is cycling.
    unsigned int curStateHash = CurStateHash();
    int cycleLength = Cycling(curStateHash);
    if (cycleLength > 0) return cycleLength; // report early termination

    // Shift the current state into history and mark the hash table.
    // Discard the head of the queue if the queue is full, releasing
    // resources and removing from the history hash.
    MachineState discard;
    if (m_stateHistory.size() == m_cycleCheckDepth) {
        discard = m_stateHistory.front();
        delete discard.nodeStates;
        m_stateHistoryHashTable[discard.stateHash] -= 1;
        assert(m_stateHistoryHashTable[discard.stateHash] >= 0);
        // (We leave discard.graph for Snap's reclamation scheme.)
        m_stateHistory.pop();
    }

    // Add the current state to the history queue and mark the hash table.
    MachineState newEntry;
    newEntry.graph = m_graph;
    newEntry.nodeStates = new int[m_nrNodes];
    newEntry.stateHash = curStateHash;
    m_stateHistoryHashTable[curStateHash] += 1;
    if (m_stateHistoryHashTable[curStateHash] > 1) m_stats.hashCollisions += 1;
    for (int i = 0; i < m_nrNodes; i += 1) newEntry.nodeStates[i] = m_nodeStates[i];
    m_stateHistory.push(newEntry);

    // Initialize the next generation's "scratchpad" arrays.
    for (int i = 0; i < m_nrNodes; i += 1) {
        m_nextL[i] = -1;
        m_nextR[i] = -1;
    }

    // Apply one iteration of the loaded rule, creating the next generation's
    // provisional state in the scratchpad. This step also creates the next
    // generation's node states.
    for (TNGraph::TNodeI NIter = m_graph->BegNI(); NIter < m_graph->EndNI(); NIter++)
        AdvanceNode(NIter);

    // Post-process the scratchpad to eliminate multi-edges.
    EliminateMultiEdges();

    // Create the next generation's graph from the scratchpad.
    // Make an empty graph.
    m_nextGraph = TNGraph::New();

    // Copy all nodes from current to next graph (added edges will "forward
    // reference" them).
    for (TNGraph::TNodeI NIter = m_graph->BegNI(); NIter < m_graph->EndNI(); NIter++) {
        int nId = NIter.GetId();
        m_nextGraph->AddNode(nId);
    }

    // <Here's where the graph is built from the scratchpad.>

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
// EliminateMultiEdges
//
// Transform the scratchpad encoding of the provisional next generation topology
// into a proper state with no multi-edges.
//
// Make multiple passes over the scratchpad arrays until a pass is completed
// without encountering a multi-edge.
//
// When a multi-edge is encountered, eliminate the node by setting its scratchpad
// entries to -1's, then pass the entire scratchpad redirecting any edge termi-
// nating at the eliminated node to instead terminate at the node to which its
// multi-edges both linked.
//---------------
void MachineS::EliminateMultiEdges() {
    bool makingChanges = true;
    while (makingChanges) {
        makingChanges = false;

        bool nodesPresent = false;
        int newDst = -1; // properly placed?
        int src;
        for (src = 0; src < m_nrNodes; src += 1) {
            if (m_nextL[src] != -1) {
                nodesPresent = true;
                if (m_nextL[src] == m_nextR[src]) {
                    // This is a multi-edge, eliminate it.
                    makingChanges = true;
                    if (m_nextL[src] == src) {
                        return 0000; // handle...
                    }
                    newDst = m_nextL[src];
                    m_nextL[src] = -1;
                    m_nextR[src] = -1;
                    break;
                }
            }
        }
        if (!nodesPresent) return 0000; // handle...
        if (!makingChanges) return 0000; // handle...
    }
}

//---------------
// RandomizeTapeState
//---------------
void MachineS::RandomizeTapeState(int tapePctBlack) {
    for (int i = 0; i < m_nrNodes; i += 1)
        if (rand() % 100 <= tapePctBlack)
            m_nodeStates[i] = NBLACK;
        else
            m_nodeStates[i] = NWHITE;
}

//---------------
// CurStateHash
//
// Return a hash of the current machine state.
//---------------
unsigned int MachineS::CurStateHash() {
    unsigned int hash = 0;

    // Incorporate node states...
    for (int i = 0; i < m_nrNodes; i += 1) {
        hash = (hash * HASH_MULTIPLIER + m_nodeStates[i])
          % STATE_HISTORY_HASH_TABLE_LEN;
    }

    // ...and topology.
    TNGraph::TNodeI graphNodeIter = m_graph->BegNI();
    while (graphNodeIter < m_graph->EndNI()) {
        hash = (hash * HASH_MULTIPLIER + graphNodeIter.GetOutNId(0))
          % STATE_HISTORY_HASH_TABLE_LEN;
        hash = (hash * HASH_MULTIPLIER + graphNodeIter.GetOutNId(1))
          % STATE_HISTORY_HASH_TABLE_LEN;
        graphNodeIter++;
    }
    return hash;
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
