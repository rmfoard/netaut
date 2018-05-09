#include "Snap.h"
#include <assert.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>

#define NR_CYCLES 40
#define NR_NODES 100
#define NR_STATES 8
#define NR_ACTIONS 20

// Command format:
//
//      machine --(t)ype <machine type> --(n)r-actions <n> --(r)ule <rulenum> ...
//          --(p)arts <rulepart list> --(c)onvert-only --iterations <n>
//
//      --type defaults to "S"
//
//      --nr-actions defaults to 20
//
//      --iterations defaults to 40
//
//      --rule xor --parts must be specified. (part list must be quoted)
//
//      The specified machine will be run for 'iterations' unless
//      --convert-only is given.
//  


// TODO: Move these to a header file for 'rules'.
uintmax_t RuleNr(const int, const int, const std::vector<int>);
std::vector<int>* RuleParts(const int, const int, const uintmax_t);

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

//---------------
class MachineS {

public:
    MachineS(uintmax_t, int);
    void Cycle();

private:
    uintmax_t m_ruleNr;
    int m_nrNodes;
    int m_nrStates;
    int m_nrActions;
    PNEGraph m_graph;
    PNEGraph m_nextGraph;
    int *m_nodeStates;
    int *m_nextNodeStates;
    std::vector<int>* m_ruleParts;

    void AdvanceNode(TNEGraph::TNodeI);
    void InitNodeStates();
};

//---------------
MachineS::MachineS(uintmax_t ruleNr, int nrNodes) {
    m_ruleNr = ruleNr;
    m_nrNodes = nrNodes;
    m_nrStates = NR_STATES;
    m_nrActions = NR_ACTIONS;
    m_graph = TNEGraph::New();
    m_nodeStates = new int[nrNodes];
    m_nextNodeStates = new int[nrNodes];
    BuildRing(nrNodes, m_graph);
    InitNodeStates();
    m_ruleParts = RuleParts(8, 20, ruleNr);
}

//---------------
void MachineS::InitNodeStates() {
    for (int i = 0; i < m_nrNodes; i += 1) {
        //m_nodeStates[i] = (i % 3) % 2;
        m_nodeStates[i] = (i == m_nrNodes / 2) ? 1 : 0;
    }
}

//---------------
// Cycle: Run one step of the loaded rule.
void MachineS::Cycle() {

    // Show node states at the beginning of the cycle.
    for (int i = m_nrNodes - 1; i >= 0; i -= 1) {
        printf("%s", (m_nodeStates[i] == 1) ? "X" : " ");
    }
    printf("\n");

    PNEGraph m_nextGraph = TNEGraph::New();

    // Apply one generation of the loaded rule, creating the next generation
    // state in 'm_nextGraph'.
    for (TNEGraph::TNodeI ni = m_graph->BegNI(); ni < m_graph->EndNI(); ni++) AdvanceNode(ni);

    // Temporarily copy m_nextGraph <- graph here.
    // It'll later be done piecemeal, when applying rules.
    for (TNEGraph::TNodeI ni = m_graph->BegNI(); ni < m_graph->EndNI(); ni++) {
        int n = ni.GetId();
        //printf("node Id: %d, state: %d\n", n, m_nodeStates[n]);
        m_nextGraph->AddNode(n);
    }

    for (TNEGraph::TNodeI ni = m_graph->BegNI(); ni < m_graph->EndNI(); ni++) {
        for (int e = 0; e < ni.GetOutDeg(); e++) {
            int orig = ni.GetId();
            int dest = ni.GetOutNId(e);
            m_nextGraph->AddEdge(orig, dest);
        }
    }

    // Cycling finished, replace "current" structures with "next" counterparts.
    // (Abandon m->graph to garbage collection.)
    m_graph = m_nextGraph;

    // Unlike with the net, avoid de- and re-allocating state storage.
    // Instead, alternate "current" and "next" roles.
    int* swap = m_nodeStates;
    m_nodeStates = m_nextNodeStates;
    m_nextNodeStates = swap;
}

//---------------
void MachineS::AdvanceNode(TNEGraph::TNodeI NI) {
    int nodeId = NI.GetId();
    int lState = m_nodeStates[NI.GetOutNId(0)];
    int nState = m_nodeStates[nodeId];
    int rState = m_nodeStates[NI.GetOutNId(1)];

    int triadState = lState * 4 + nState * 2 + rState;

    assert(0 <= triadState && triadState <= 7);
    int action = (*m_ruleParts)[triadState];
    //printf("triadState %d => action %d\n", triadState, action);
    assert(0 <= action && action <= 19);

    // TODO: Keep running stats on action use.
    switch (action) {
        case 0:
            m_nextNodeStates[nodeId] = 0;
            break;
        case 1:
            m_nextNodeStates[nodeId] = 1;
            break;
        case 2:
            break;
        case 3:
            break;
        case 4:
            break;
        case 5:
            break;
        case 6:
            break;
        case 7:
            break;
        case 8:
            break;
        case 9:
            break;
        case 10:
            break;
        case 11:
            break;
        case 12:
            break;
        case 13:
            break;
        case 14:
            break;
        case 15:
            break;
        case 16:
            break;
        case 17:
            break;
        case 18:
            break;
        case 19:
            break;
    }
}

//---------------
class CommandOpts {

public:
    static int convertOnly;
    static int nrIterations;
    static int nrActions;
    static uintmax_t ruleNr;
    static bool rulePresent;
    static bool partsPresent;
};
int CommandOpts::convertOnly;
int CommandOpts::nrIterations = 40;
int CommandOpts::nrActions = 20;
// TODO: Use an array instead of a vector.
std::vector<int> ruleParts = { 0, 1, 1, 1, 0, 1, 1, 0  }; // Wolfram 110 equivalent
uintmax_t CommandOpts::ruleNr = RuleNr(8, 20, ruleParts);
bool CommandOpts::rulePresent = false;
bool CommandOpts::partsPresent = false;

//---------------
// TODO: Learn where the hell 'optind' came from.
static void ParseCommand(const int argc, char* argv[]) {
    int c;
    bool errorFound = false;

    while (true) {
        static struct option long_options[] = {
            {"convert-only", no_argument, &CommandOpts::convertOnly, 1},

            {"type", required_argument, 0, 't'},
            {"nr-actions", required_argument, 0, 'n'},
            {"iterations", required_argument, 0, 'i'},
            {"rule", required_argument, 0, 'r'},
            {"parts", required_argument, 0, 'p'},
            {0, 0, 0, 0}
        };

        int option_index = 0;
        c = getopt_long (argc, argv, "i:n:p:r:t:", long_options, &option_index);

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
            printf("--iterations option is not yet supported.\n");
            errorFound = true;
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

          case '?':
            errorFound = true;
            break;

          default:
            abort();
       }
    }
    if (errorFound) exit(1);

    // TODO: Lower this into a subroutine.
    // Convert-only?
    if (CommandOpts::convertOnly) {
        if ((CommandOpts::partsPresent && !CommandOpts::rulePresent)
          || (!CommandOpts::partsPresent && CommandOpts::rulePresent)) {
            if (CommandOpts::partsPresent) {
                printf("parts -> rule conversion is not yet supported.\n");
                errorFound = true;
            } else { // rulePresent
                std::vector<int>* ruleParts = RuleParts(NR_STATES, NR_ACTIONS, CommandOpts::ruleNr);
                printf("actions: ");
                for (int part = 0; part < NR_STATES; part += 1) {
                    printf("%d ", (*ruleParts)[part]);
                }
                printf("\n");
                exit(0);
            }
        } else {
            printf("error: must specify either --rule xor --parts\n");
            errorFound = true;
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
int main(const int argc, char* argv[]) {
    ParseCommand(argc, argv);

    //uintmax_t ruleNr = RuleNr(8, 20, ruleParts);
    printf("ruleNr: %lld\n", CommandOpts::ruleNr);

    //MachineS* m = new MachineS(ruleNr, NR_NODES);
    MachineS* m = new MachineS(CommandOpts::ruleNr, NR_NODES);
    for (int i = 1; i <= NR_CYCLES; i += 1) m->Cycle();
}
