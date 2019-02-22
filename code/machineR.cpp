#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdelete-non-virtual-dtor"
#include "Snap.h"
#pragma GCC diagnostic pop
#include <assert.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <json/json.h>
#include "rule.h"
#include "machineR.h"
#include "machine.h"

//---------------
void MachineR::BuildMachine(rulenr_t ruleNr, int nrNodes, int cycleCheckDepth,
  std::string tapeStructure, int tapePctBlack, std::string topoStructure, int noChangeTopo) {
    assert(m_machineType == "R" || m_machineType == "RM");

    m_rule = new Rule(ruleNr);
    m_ruleParts = m_rule->get_ruleParts();
    m_nrNodes = nrNodes;
    m_cycleCheckDepth = cycleCheckDepth;
    m_graph = TNEGraph::New();
    m_snapRnd = new TRnd(rand()); // unused BUT rand() call must remain in place
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
MachineR::~MachineR() {
    delete m_ruleParts;
    delete m_nodeStates;
    delete m_nextNodeStates;
    delete m_stateHistoryHashTable;
    delete m_nextL;
    delete m_nextR;

    // Clear any remaining storage in the state history queue.
    int queueLength = m_stateHistory.size(); // invariant: <=
    for (int i = 1; i <= queueLength; i += 1) {
        Machine::MachineState entry = m_stateHistory.front();
        delete entry.nodeStates;
        m_stateHistory.pop();
    }
}

//---------------
// TODO: Use the proper form for "getters."
//---------------
PNEGraph MachineR::get_graph() { return m_graph; }
int* MachineR::get_nodeStates() { return m_nodeStates; }

//---------------
// AdvanceNode
//
// Apply the loaded rule to the indicated node, recording new edge
// destinations in the scratchpad. At this step, it's okay to create
// multi-edges; they'll be removed in post-processing.
//---------------
void MachineR::AdvanceNode(TNEGraph::TNodeI NIter) {

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
    TNEGraph::TNodeI lNIter = m_graph->GetNI(lNId); // iterator for left neighbor
    TNEGraph::TNodeI rNIter = m_graph->GetNI(rNId); // iterator for right neighbor
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

    // If applicable, confirm that the multi-edge invariant still holds.
    if (m_machineType == "R") assert(lNId != rNId);

    // Apply the node action and note the provisional topological action
    // in the scratchpad.
    assert(m_nextL[nNId] == -1 && m_nextR[nNId] == -1);
    m_nextNodeStates[nNId] = m_nodeStates[nNId]; // (node states are left unchanged, not randomized)
    m_nextL[nNId] = newDsts[rand() % NR_DSTS];
    m_nextR[nNId] = newDsts[rand() % NR_DSTS];

    delete newDsts;
}

//---------------
void MachineR::BuildRing() {
    assert(m_nrNodes > 1);

    // Create all the nodes.
    for (int n = 0; n < m_nrNodes; n += 1) m_graph->AddNode(n);

    for (int n = 1; n < m_nrNodes; n += 1) m_graph->AddEdge(n, n - 1);
    m_graph->AddEdge(0, m_nrNodes - 1);
    for (int n = 0; n < m_nrNodes - 1; n += 1) m_graph->AddEdge(n, n + 1);
    m_graph->AddEdge(m_nrNodes - 1, 0);
}

//---------------
void MachineR::BuildTree() {
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
void MachineR::BuildRandomGraph() {
    assert(m_nrNodes > 1);

    // Create all the nodes.
    for (int i = 0; i < m_nrNodes; i += 1) m_graph->AddNode(i);

    // Create two random edges on each node.
    for (int i = 0; i < m_nrNodes; i += 1) {
        int j = rand() % m_nrNodes;
        m_graph->AddEdge(i, j);

        int k;
        do k = rand() % m_nrNodes; while (k == j);
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
int MachineR::Cycling(unsigned int curStateHash) {

    // If the current state's hash is not in the table,
    // no cycle is detected. (The check is not perfect, though,
    // unless we're keeping the entire history of the run.
    if (m_stateHistoryHashTable[curStateHash] == 0) return 0;

    // There was a hit in the hash table, so an exhaustive
    // comparison is necessary. Spin through the entire history
    // queue.
    Machine::MachineState eoq = {nullptr, nullptr, 0};
    m_stateHistory.push(eoq);
    int cycleLength = m_stateHistory.size(); // invariant: <=
    Machine::MachineState candidate = m_stateHistory.front();
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
void MachineR::InitTape(std::string tapeStructure, int tapePctBlack) {
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
void MachineR::InitTopo(std::string topoStructure) {
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
int MachineR::IterateMachine(int iterationNr) {

    // Stop and report if the machine is cycling.
    unsigned int curStateHash = CurStateHash();
    int cycleLength = Cycling(curStateHash);
    if (cycleLength > 0) return cycleLength; // report early termination

    // Shift the current state into history and mark the hash table.
    // Discard the head of the queue if the queue is full, releasing
    // resources and removing from the history hash.
    Machine::MachineState discard;
    if (m_stateHistory.size() == m_cycleCheckDepth) {
        discard = m_stateHistory.front();
        delete discard.nodeStates;
        m_stateHistoryHashTable[discard.stateHash] -= 1;
        assert(m_stateHistoryHashTable[discard.stateHash] >= 0);
        // (We leave discard.graph for Snap's reclamation scheme.)
        m_stateHistory.pop();
    }

    // Add the current state to the history queue and mark the hash table.
    Machine::MachineState newEntry;
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
    for (TNEGraph::TNodeI NIter = m_graph->BegNI(); NIter < m_graph->EndNI(); NIter++)
        AdvanceNode(NIter);

    // If they're disallowed, post-process the scratchpad to eliminate multi-edges.
    if (m_machineType == "R") {
        int termIndicator = EliminateMultiEdges();
        if (termIndicator < 0) return termIndicator;
    }
    else
        assert(m_machineType == "RM");

    // Create the next generation's graph from the scratchpad; initialize an empty graph
    // and create all the next generation's surviving nodes.
    m_nextGraph = TNEGraph::New();
    for (int nId = 0; nId < m_nrNodes; nId += 1) if (m_nextL[nId] != -1)
        m_nextGraph->AddNode(nId);

    // Add the edges.
    for (int nId = 0; nId < m_nrNodes; nId += 1) if (m_nextL[nId] != -1) {
            m_nextGraph->AddEdge(nId, m_nextL[nId]);
            m_nextGraph->AddEdge(nId, m_nextR[nId]);
    }

    // Replace "current" structures with "next" counterparts.
    // (Abandoning m_graph to garbage collection.)
    m_graph = m_nextGraph;

    // Unlike with the graph, avoid de- and re-allocating state storage.
    // Instead, alternate "current" and "next" roles.
    int* swap = m_nodeStates;
    m_nodeStates = m_nextNodeStates;
    m_nextNodeStates = swap;

    // Report no terminal condition found.
    return 0;
}

//---------------
// EliminateNode
//---------------
void MachineR::EliminateNode(int node) {
    assert(m_nextL[node] != -1);
    m_nextL[node] = -1;
    m_nextR[node] = -1;

    //printf("eliminating node %d\n", node);
    assert(node != -1);
    for (int in = 0; in < m_nrNodes; in += 1) {

        if (m_nextL[in] == node && m_nextR[in] == node)
            EliminateNode(in);

        else if (m_nextL[in] == node)
            m_nextL[in] = m_nextR[in];

        else if (m_nextR[in] == node)
            m_nextR[in] = m_nextL[in];
    }
}

//---------------
// EliminateMultiEdges
//
// Transform the scratchpad encoding of the provisional next generation topology
// into a proper state with no multi-edges.
//
// TODO: Correct this comment.
// Make multiple passes over the scratchpad arrays until a pass is completed
// without encountering a multi-edge or a termination condition arises. Termina-
// tion conditions are (-1) collapse to an empty graph, or (-2) occurrence of
// "multi-self-edges." Return 0 in the normal case in which the scratchpad
// encodes a non-empty graph with no multi-edges.
//
// When a multi-edge is encountered, eliminate the node by setting its scratchpad
// entries to -1's, then pass the entire scratchpad redirecting any edge termi-
// nating at the eliminated node to instead terminate at the node to which its
// multi-edges both linked.
//---------------
int MachineR::EliminateMultiEdges() {
    bool makingChanges = true;
    while (makingChanges) {
        makingChanges = false;

        int newDst = -1;
        int src;
        bool nodesPresent = false;
        for (src = 0; src < m_nrNodes; src += 1) if (m_nextL[src] != -1) {
            nodesPresent = true;
            if (m_nextL[src] == m_nextR[src] && m_nextL[src] != src) {
                //printf("bypassing and eliminating node %d\n", src);
                // This is a simple multi-edge, eliminate and bypass it.
                makingChanges = true;
                m_stats.multiEdgesAvoided += 1;
                newDst = m_nextL[src];
                m_nextL[src] = -1;
                m_nextR[src] = -1;

                // We found and eliminated a simple multi-edge node; pass the scratchpad,
                // redirecting its inbound edges to newDst.
                for (int i = 0; i < m_nrNodes; i += 1) {
                    if (m_nextL[i] == src) m_nextL[i] = newDst;
                    if (m_nextR[i] == src) m_nextR[i] = newDst;
                }
                break;
            }

            else if (m_nextL[src] == m_nextR[src] && m_nextL[src] == src) {
                // This is a multi-self-edge; eliminate it and all its in-linking nodes.
                makingChanges = true;
                EliminateNode(src);
                break;
            }
        }
        if (!nodesPresent) return -1; // graph collapsed -> 0 nodes?
        if (!makingChanges) return 0; // normal case
    }
    assert(false);
}

//---------------
// RandomizeTapeState
//---------------
void MachineR::RandomizeTapeState(int tapePctBlack) {
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
unsigned int MachineR::CurStateHash() {
    unsigned int hash = 0;

    // Incorporate node states...
    for (int i = 0; i < m_nrNodes; i += 1) {
        hash = (hash * HASH_MULTIPLIER + m_nodeStates[i])
          % STATE_HISTORY_HASH_TABLE_LEN;
    }

    // ...and topology.
    TNEGraph::TNodeI graphNodeIter = m_graph->BegNI();
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
bool MachineR::StateMatchesCurrent(Machine::MachineState other) {

    // Compare node states.
    for (int i = 0; i < m_nrNodes; i += 1)
        if (m_nodeStates[i] != other.nodeStates[i]) return false;

    // Compare topologies.
    TNEGraph::TNodeI graphNodeIter = m_graph->BegNI();
    TNEGraph::TNodeI otherGraphNodeIter = other.graph->BegNI();
    while (graphNodeIter < m_graph->EndNI()) {
        if (graphNodeIter.GetId() != otherGraphNodeIter.GetId()) return false;
        if (graphNodeIter.GetOutNId(0) != otherGraphNodeIter.GetOutNId(0)) return false;
        if (graphNodeIter.GetOutNId(1) != otherGraphNodeIter.GetOutNId(1)) return false;
        graphNodeIter++;
        otherGraphNodeIter++;
    }

    return true;
}

//---------------
// AddSummaryInfo
//---------------
void MachineR::AddSummaryInfo(Json::Value& info) {
    info["hashCollisions"] = (Json::Value::UInt64) m_stats.hashCollisions;

    Json::Value triadOccurrences;
    for (int i = 0; i < NR_TRIAD_STATES; i += 1) {
        auto occurrences = m_stats.triadOccurrences[i];
        triadOccurrences.append((Json::Value::UInt64) occurrences);
    }
    info["triadOccurrences"] = triadOccurrences;
}
