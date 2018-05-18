#include "Snap.h"
#include <assert.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <vector>
#include "rule.h"

#define NR_CYCLES 40
#define NR_NODES 256

// TODO: Make 'CommandOptions' a structure.
//---------------
// Command format:
//
//      machine --(m)achine <machine type> --(r)ule <rule number> ...
//          --(t)ext <rule text> --(c)onvert-only --write --iterations <nr>
//
//      --machine type defaults to "S"
//
//      --iterations defaults to 40
//
//      --rule <rule number>
//
//      --text <rule text>
//
//      --self-edges [permitted] defaults to off/false
//
//      --rule xor --text must be specified.
//
//      Runs the specified machine for 'iterations' unless --convert-only
//      is present.
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
    static int selfEdges;
    static rulenr_t ruleNr;
    static bool rulePresent;
    static bool textPresent;
    static bool writeDot;
    static char* outFile;
    static char* ruleText;
};
int CommandOpts::convertOnly;
int CommandOpts::nrIterations = 40;
rulenr_t CommandOpts::ruleNr; // initial/default value is set at run-time
int CommandOpts::selfEdges = 0;
bool CommandOpts::rulePresent = false;
bool CommandOpts::textPresent = false;
bool CommandOpts::writeDot = false;
char* CommandOpts::outFile = NULL;
char* CommandOpts::ruleText = NULL;

//---------------
class MachineS {

public:
    MachineS(rulenr_t, int);
    PNEGraph get_m_graph();
    void Cycle();

private:
    Rule* m_rule;
    int m_nrNodes;
    PNEGraph m_graph;
    PNEGraph m_nextGraph;
    int* m_nodeStates;
    int* m_nextNodeStates;

    void AdvanceNode(TNEGraph::TNodeI);
    void InitNodeStates();
};

//---------------
// TODO: Provide a destructor that releases resources.
//---------------
MachineS::MachineS(rulenr_t ruleNr, const int nrNodes) {
    m_rule = new Rule(ruleNr);
    m_nrNodes = nrNodes;
    m_graph = TNEGraph::New();
    m_nodeStates = new int[m_nrNodes];
    m_nextNodeStates = new int[m_nrNodes];
    BuildRing(m_nrNodes, m_graph);
    InitNodeStates();
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
    assert(m_nrNodes > 2 * 64);
    for (int i = m_nrNodes/2 - 64; i <= m_nrNodes/2 + 64; i += 1) {
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
    // TODO: Eliminate call to get_ruleParts
    int rulePart = m_rule->get_ruleParts()[triadState];
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

            {"machine", required_argument, 0, 'm'},
            {"iterations", required_argument, 0, 'i'},
            {"rule", required_argument, 0, 'r'},
            {"text", required_argument, 0, 't'},
            {"write", required_argument, 0, 'w'},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        c = getopt_long (argc, argv, "m:i:r:t:w:", long_options, &option_index);

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

          case 't':
            CommandOpts::textPresent = true;
            if (CommandOpts::rulePresent) {
                printf("error: can't specify both --text and --rule\n");
                errorFound = true;
            } else {
                CommandOpts::ruleText = strAllocCpy(optarg);
            }
            break;

          case 'r':
            CommandOpts::rulePresent = true;
            if (CommandOpts::textPresent) {
                printf("error: can't specify both --text and --rule\n");
                errorFound = true;
            } else {
                char* endPtr;
                CommandOpts::ruleNr = strtoumax(optarg, &endPtr, 10); // radix 10
                if (CommandOpts::ruleNr == 0) {
                    printf("error: invalid rule number\n");
                    errorFound = true;
                }
            }
            break;

          case 'm':
            printf("--machine option is not yet supported.\n");
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
    if (CommandOpts::textPresent) {
        Rule* rule = new Rule(CommandOpts::ruleText);
        printf("%llu\n", rule->get_ruleNr());
        delete rule;
        return 0; // failure
    }
    else { // rulePresent
        Rule* rule = new Rule(CommandOpts::ruleNr);
        printf("%s\n", rule->get_ruleText());
        delete rule;
        return 0; // success
    }
}

//---------------
int main(const int argc, char* argv[]) {

    CommandOpts::ruleNr = 
        (rulenr_t) 6
        + (rulenr_t) 6 *72
        + (rulenr_t)11 *72*72
        + (rulenr_t)10 *72*72*72
        + (rulenr_t)7  *72*72*72*72
        + (rulenr_t)11 *72*72*72*72*72
        + (rulenr_t)10 *72*72*72*72*72*72
        + (rulenr_t)6  *72*72*72*72*72*72*72;

    ParseCommand(argc, argv);

    // Was the --convert-only option present?
    // If so, then either --text or --rule, but not both, must be present.
    if (CommandOpts::convertOnly) {
        if ((CommandOpts::textPresent && !CommandOpts::rulePresent)
          || (!CommandOpts::textPresent && CommandOpts::rulePresent)) {
            exit(DoConversion());
        } else {
            printf("error: must specify either --rule xor --text\n");
            exit(1);
        }
    }

    // Convert-only was not selected, so build and run the machine.
    printf("ruleNr: %llu\n", CommandOpts::ruleNr);

    if (CommandOpts::textPresent) {
        Rule* tmpRule = new Rule(CommandOpts::ruleText);
        CommandOpts::ruleNr = tmpRule->get_ruleNr();
        delete tmpRule;
    }

    MachineS* m = new MachineS(CommandOpts::ruleNr, NR_NODES);

    for (int i = 1; i <= CommandOpts::nrIterations; i += 1) m->Cycle();

    // Write the end-state graph if --write was present.
    if (CommandOpts::writeDot) TSnap::SaveGViz(m->get_m_graph(), CommandOpts::outFile);

    // Reiterate the machine number.
    printf("ruleNr: %llu\n", CommandOpts::ruleNr);

    // Show graph characteristis.
    // get distribution of connected components (component size, count)
    TVec<TPair<TInt, TInt> > CntV; // vector of pairs of integers (size, count)
    TSnap::GetWccSzCnt(m->get_m_graph(), CntV); // get degree distribution pairs (degree, count)

    for (int i = 0; i < CntV.Len(); i += 1) {
        printf("%d: %d %d\n", i, CntV[i].Val1, CntV[i].Val2);
    }

    delete m;
    exit(0);
}
