#include "Snap.h"
#include <assert.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <vector>
#include "rules.h"

#define NR_CYCLES 40
#define NR_NODES 100

// TODO: Make 'CommandOptions' a structure.
//---------------
// Command format:
//
//      machine --(t)ype <machine type> --(n)r-actions <n> --(r)ule <rulenum> ...
//          --(p)arts <rulepart list> --(c)onvert-only --write --iterations <n>
//
//      --type defaults to "S"
//
//      --nr-actions defaults to 20
//
//      --iterations defaults to 40
//
//      --rule <rule number>
//
//      --parts <actions string>
//
//      --self-edges [permitted] defaults to off/false
//
//      --rule xor --parts must be specified. (part list must be quoted)
//
//      The specified machine will be run for 'iterations' unless
//      --convert-only is given.
//
// TODO: Explain actions string
//---------------

//---------------
// TODO: Consider moving this to a 'graph_factory' module.
static void BuildRing(int nrNodes, PNEGraph graph) {
    assert(nrNodes > 1);

    for (int n = 0; n < nrNodes; n += 1) graph->AddNode(n);
    for (int n = 0; n < nrNodes - 1; n += 1) graph->AddEdge(n, n + 1);
    graph->AddEdge(nrNodes - 1, 0);
    graph->AddEdge(0, nrNodes - 1);
    for (int n = 1; n < nrNodes; n += 1) graph->AddEdge(n, n - 1);
}

static char* strAllocCpy(const char* src) {
    return strcpy(new char[strlen(src) + 1], src);
}

//---------------
class CommandOpts {

public:
    static int convertOnly;
    static int nrIterations;
    static int nrActions;
    static int selfEdges;
    static long long unsigned ruleNr;
    static bool rulePresent;
    static bool partsPresent;
    static bool writeDot;
    static char* outFile;
};
int CommandOpts::convertOnly;
int CommandOpts::nrIterations = 40;
int CommandOpts::nrActions = 20;
long long unsigned CommandOpts::ruleNr; // initial/default value is set at run-time
int CommandOpts::selfEdges = 0;
bool CommandOpts::rulePresent = false;
bool CommandOpts::partsPresent = false;
bool CommandOpts::writeDot = false;
char* CommandOpts::outFile;

//---------------
class MachineS {

public:
    MachineS(long long unsigned, int);
    PNEGraph get_m_graph();
    void Cycle();

private:
    long long unsigned m_ruleNr;
    int m_nrNodes;
    int m_nrTriadStates;
    int m_nrActions;
    PNEGraph m_graph;
    PNEGraph m_nextGraph;
    int* m_nodeStates;
    int* m_nextNodeStates;
    int* m_ruleParts;
    Rules* m_rules;

    void AdvanceNode(TNEGraph::TNodeI);
    void InitNodeStates();
};

//---------------
// TODO: Provide a destructor that releases resources.
//---------------
MachineS::MachineS(long long unsigned ruleNr, int nrNodes) {
    m_rules = new Rules();
    m_ruleNr = ruleNr;
    m_nrNodes = nrNodes;
    m_nrTriadStates = NR_TRIAD_STATES;
    m_nrActions = NR_ACTIONS;
    m_graph = TNEGraph::New();
    m_nodeStates = new int[nrNodes];
    m_nextNodeStates = new int[nrNodes];
    BuildRing(nrNodes, m_graph);
    InitNodeStates();
    m_ruleParts = m_rules->RuleParts(NR_TRIAD_STATES, NR_ACTIONS, ruleNr);
}

//---------------
// TODO: Use the proper form for "getters."
//---------------
PNEGraph MachineS::get_m_graph() { return m_graph; }

//---------------
void MachineS::InitNodeStates() {
    for (int i = 0; i < m_nrNodes; i += 1) {
        m_nodeStates[i] = (i == m_nrNodes / 2) ? 1 : 0;
    }
}

//---------------
// Cycle
//
// Run one step of the loaded rule.
//---------------
void MachineS::Cycle() {

    // Show node states at the beginning of the cycle.
    for (int i = m_nrNodes - 1; i >= 0; i -= 1) {
        printf("%s", (m_nodeStates[i] == 1) ? "X" : " ");
    }
    printf("\n");

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
    for (TNEGraph::TNodeI NIter = m_graph->BegNI(); NIter < m_graph->EndNI(); NIter++) AdvanceNode(NIter);

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
void MachineS::AdvanceNode(TNEGraph::TNodeI NIter) {
    bool selfEdge = CommandOpts::selfEdges;

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
    int rulePart = m_ruleParts[triadState];
    assert(0 <= rulePart && rulePart < NR_ACTIONS);

    // Unpack the rule part into left edge, right edge, and node actions
    int lAction = (rulePart / 2) / NR_POSS_DSTS;
    int rAction = (rulePart / 2) % NR_POSS_DSTS;
    int nAction = rulePart % 2;

    // Apply the actions
    // TODO: Keep running stats on action use.
    if (newDsts[lAction] != nNId || selfEdge)
        m_nextGraph->AddEdge(nNId, newDsts[lAction]);
    else
        m_nextGraph->AddEdge(nNId, lNId);

    if (newDsts[rAction] != nNId || selfEdge)
        m_nextGraph->AddEdge(nNId, newDsts[rAction]);
    else
        m_nextGraph->AddEdge(nNId, rNId);

    m_nextNodeStates[nNId] = nAction;

    delete newDsts;
}

//---------------
// TODO: Learn where the hell 'optind' came from.
static void ParseCommand(const int argc, char* argv[]) {
    int c;
    bool errorFound = false;

    while (true) {
        static struct option long_options[] = {
            {"convert-only", no_argument, &CommandOpts::convertOnly, 1},
            {"self-edges", no_argument, &CommandOpts::selfEdges, 1},

            {"type", required_argument, 0, 't'},
            {"nr-actions", required_argument, 0, 'n'},
            {"iterations", required_argument, 0, 'i'},
            {"rule", required_argument, 0, 'r'},
            {"parts", required_argument, 0, 'p'},
            {"write", required_argument, 0, 'w'},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        c = getopt_long (argc, argv, "i:n:p:r:t:w:", long_options, &option_index);

        if (c == -1) // end of options?
            break;

        switch (c) {
          case 0: // flag setting only, no further processing required
            if (long_options[option_index].flag != 0)
                break;

            printf ("option %s", long_options[option_index].name);
            if (optarg)
                printf (" with arg %s", optarg);
            printf ("\n");
            break;

          case 'i':
            CommandOpts::nrIterations = atoi(optarg);
            break;

          case 'n':
            printf("--nr-actions option is not yet supported.\n");
            errorFound = true;
            break;

          case 'p':
            CommandOpts::partsPresent = true;
            if (CommandOpts::rulePresent) {
                printf("error: can't specify both --parts and --rule\n");
                errorFound = true;
            } else {
                printf("--parts option is not yet supported.\n");
                errorFound = true;
            }
            break;

          case 'r':
            CommandOpts::rulePresent = true;
            if (CommandOpts::partsPresent) {
                printf("error: can't specify both --parts and --rule\n");
                errorFound = true;
            } else {
                printf("--rule %s\n", optarg);
                char* endPtr;
                CommandOpts::ruleNr = strtoumax(optarg, &endPtr, 10); // radix 10
                if (CommandOpts::ruleNr == 0) {
                    printf("error: invalid rule number\n");
                    errorFound = true;
                }
            }
            break;

          case 't':
            printf("--type option is not yet supported.\n");
            errorFound = true;
            break;

          case 'w':
            CommandOpts::outFile = strAllocCpy(optarg);
            CommandOpts::writeDot = true;
            printf("outFile: %s\n", CommandOpts::outFile);
            break;

          case '?':
            errorFound = true;
            break;

          default:
            abort();
       }
    }
    if (errorFound) exit(1);
    if (errorFound) exit(1);


    // Process any non-option command arguments.
    if (optind < argc) {
        printf ("non-option ARGV-elements: ");
        while (optind < argc) printf ("%s ", argv[optind++]);
        putchar ('\n');
    }
}

//---------------
// DoConversion
//
// Convert from rule number to rule parts or vice versa, depending on
// command line options present.
//---------------
static int DoConversion() {
    if (CommandOpts::partsPresent) {
        printf("parts -> rule conversion is not yet supported.\n");
        return 1; // failure
    }
    else { // rulePresent
        Rules* rules = new Rules();
        // TODO: Re-implement rule part display
//        std::vector<int>* ruleParts = rules->RuleParts(NR_TRIAD_STATES, NR_ACTIONS, CommandOpts::ruleNr);
//        printf("actions: ");
//        for (int part = 0; part < NR_TRIAD_STATES; part += 1) {
//            printf("%s, ", rules->actionNames[(*ruleParts)[part]].c_str());
//        }
//        printf("\n");
        delete rules;
        return 0; // success
    }
}

//---------------
int main(const int argc, char* argv[]) {

    CommandOpts::ruleNr = 
        (long long unsigned) 6
        + (long long unsigned) 6 *72
        + (long long unsigned)11 *72*72
        + (long long unsigned)10 *72*72*72
        + (long long unsigned)7  *72*72*72*72
        + (long long unsigned)11 *72*72*72*72*72
        + (long long unsigned)10 *72*72*72*72*72*72
        + (long long unsigned)6  *72*72*72*72*72*72*72;

    ParseCommand(argc, argv);

    // Was the --convert-only option present?
    // If so, then either --parts or --rule, but not both, must be present.
    if (CommandOpts::convertOnly) {
        if ((CommandOpts::partsPresent && !CommandOpts::rulePresent)
          || (!CommandOpts::partsPresent && CommandOpts::rulePresent)) {
            exit(DoConversion());
        } else {
            printf("error: must specify either --rule xor --parts\n");
            exit(1);
        }
    }

    // Convert-only was not selected, so build and run the machine.
    printf("ruleNr: %llu\n", CommandOpts::ruleNr);

    MachineS* m = new MachineS(CommandOpts::ruleNr, NR_NODES);
    for (int i = 1; i <= CommandOpts::nrIterations; i += 1) m->Cycle();

    // Write the end-state graph if --write was present.
    if (CommandOpts::writeDot) TSnap::SaveGViz(m->get_m_graph(), CommandOpts::outFile);

    delete m;
    exit(0);
}
