#include "Snap.h"
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
#include "machine.h"

#define VERSION "25May2018"
#define NR_CYCLES 40

// TODO: Add --help
// TODO: Remove or update the following comment block.
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
static
char* strAllocCpy(const char* src) { return strcpy(new char[strlen(src) + 1], src); }

// TODO: Make 'CommandOptions' a structure.
//---------------
class CommandOpts {

public:
    static int convertOnly;
    static int nrIterations;
    static rulenr_t ruleNr;
    static int breadthFirstRoot;
    static int depthFirstRoot;
    static int randSeed;
    static int selfEdges;
    static int noMultiEdges;
    static int noWriteInfo;
    static int printTape;
    static int nrNodes;
    static bool rulePresent;
    static bool textPresent;
    static bool writeDot;
    static char* outFile;
    static char* ruleText;
};
int CommandOpts::convertOnly;
int CommandOpts::nrIterations = 128;
rulenr_t CommandOpts::ruleNr; // initial/default value is set at run-time
int CommandOpts::breadthFirstRoot = -1;
int CommandOpts::depthFirstRoot = -1;
int CommandOpts::randSeed = -1;
int CommandOpts::selfEdges = 0;
int CommandOpts::noMultiEdges = 0;
int CommandOpts::noWriteInfo = 0;
int CommandOpts::printTape = 0;
int CommandOpts::nrNodes = 256;
bool CommandOpts::rulePresent = false;
bool CommandOpts::textPresent = false;
bool CommandOpts::writeDot = false;
char* CommandOpts::outFile = NULL;
char* CommandOpts::ruleText = NULL;

//---------------
// TODO: Learn where the hell 'optind' came from.
static
void ParseCommand(const int argc, char* argv[]) {
    int c;
    bool errorFound = false;

    static struct option long_options[] = {
        {"convert-only", no_argument, &CommandOpts::convertOnly, 1},
        {"self-edges", no_argument, &CommandOpts::selfEdges, 1},
        {"no-multi-edges", no_argument, &CommandOpts::noMultiEdges, 1},
        {"no-write-info", no_argument, &CommandOpts::noWriteInfo, 1},
        {"print", no_argument, &CommandOpts::printTape, 1},

        {"machine", required_argument, 0, 'm'},
        {"iterations", required_argument, 0, 'i'},
        {"breadth-first-root", required_argument, 0, 'b'},
        {"depth-first-root", required_argument, 0, 'd'},
        {"nodes", required_argument, 0, 'n'},
        {"rule", required_argument, 0, 'r'},
        {"random", required_argument, 0, 'a'},
        {"text", required_argument, 0, 't'},
        {"write", required_argument, 0, 'w'},
        {0, 0, 0, 0}
    };

    while (true) {

        int option_index = 0;
        c = getopt_long (argc, argv, "m:i:b:d:n:r:a:t:w:",
          long_options, &option_index);

        if (c == -1) // end of options?
            break;

        switch (c) {
          case 0: // flag setting only, no further processing required
            if (long_options[option_index].flag != 0)
                break;
            assert(false);

          case 'i':
            CommandOpts::nrIterations = atoi(optarg);
            break;

          case 'b':
            CommandOpts::breadthFirstRoot = atoi(optarg);
            break;

          case 'd':
            CommandOpts::depthFirstRoot = atoi(optarg);
            break;

          case 'n':
            CommandOpts::nrNodes = atoi(optarg);
            break;

          case 'a':
            CommandOpts::randSeed = atoi(optarg);
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

    // Check assembled options.
    if (CommandOpts::breadthFirstRoot >= 0 && CommandOpts::depthFirstRoot >= 0) {
        printf("error: only one of --breadth-first-root and --depth-first-root may be present\n");
        errorFound = true;
    }
    else if (CommandOpts::breadthFirstRoot >= CommandOpts::nrNodes) {
        printf("breadth-first-root is too large\n");
        errorFound = true;
    }
    else if (CommandOpts::depthFirstRoot >= CommandOpts::nrNodes) {
        printf("depth-first-root is too large\n");
        errorFound = true;
    }

    // Create a random rule number if called for.
    if (CommandOpts::randSeed >= 0) {
        Rule* r  = new Rule((rulenr_t) 0);
        srand(CommandOpts::randSeed);
        CommandOpts::ruleNr = ((unsigned long long) rand() * RAND_MAX + rand()) % r->get_maxRuleNr();
        delete r;
    }

    if (errorFound) exit(1);


    // Warn if any non-option command arguments are present.
    if (optind < argc) {
        printf ("warning: there are extraneous command arguments: ");
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
static
int DoConversion() {
    if (CommandOpts::textPresent) {
        Rule* rule = new Rule(CommandOpts::ruleText);
        printf("%llu\n", rule->get_ruleNr());
        delete rule;
        return 0; // failure
    }
    else { // rulePresent
        Rule* rule = new Rule(CommandOpts::ruleNr);
        printf("%s\n", rule->get_ruleText().c_str());
        delete rule;
        return 0; // success
    }
}

//---------------
// WriteInfo
//
// Write a file containing JSON-encoded run parameters and outcome statistics.
//---------------
void WriteInfo(MachineS* machine) {
    Json::Value params;
    Json::Value ruleParts;
    Json::Value rulePartsText;

    params["version"] = VERSION;
    params["ruleNr"] = (Json::UInt64) machine->m_rule->get_ruleNr();
    for (int i = 0; i < NR_TRIAD_STATES; i += 1) {
        ruleParts.append(machine->m_ruleParts[i]);
        rulePartsText.append(machine->m_rule->RulePartText(machine->m_ruleParts[i]));
    }
    params["ruleParts"] = ruleParts;
    params["ruleText"] = machine->m_rule->get_ruleText();
    params["rulePartsText"] = rulePartsText;
    params["nrNodes"] = machine->m_nrNodes;
    params["iterations"] = CommandOpts::nrIterations;
    params["selfEdges"] = CommandOpts::selfEdges;
    params["noMultiEdges"] = CommandOpts::noMultiEdges;
    std::cout << params << std::endl;
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
    if (CommandOpts::textPresent) {
        Rule* tmpRule = new Rule(CommandOpts::ruleText);
        CommandOpts::ruleNr = tmpRule->get_ruleNr();
        delete tmpRule;
    }

    MachineS* m = new MachineS(CommandOpts::ruleNr, CommandOpts::nrNodes);

    for (int i = 1; i <= CommandOpts::nrIterations; i += 1)
        m->Cycle(CommandOpts::selfEdges, CommandOpts::noMultiEdges);

    // Show graph characteristics.
    // get distribution of connected components (component size, count)
    TVec<TPair<TInt, TInt> > CntV; // vector of pairs of integers (size, count)
    TSnap::GetWccSzCnt(m->get_m_graph(), CntV);
    printf("size: count, connected components\n");
    for (int i = 0; i < CntV.Len(); i += 1) printf("%d: %d\n", CntV[i].Val1, CntV[i].Val2);

    TFltPrV DegCCfV;
    int64 ClosedTriads, OpenTriads;
    int FullDiam;
    double EffDiam;
    const double CCF = TSnap::GetClustCf(m->get_m_graph(), DegCCfV, ClosedTriads, OpenTriads);
    printf("Average clustering coefficient\t%.4f\n", CCF);
    printf("Number of triangles\t%s\n", TUInt64(ClosedTriads).GetStr().CStr());
    printf("Fraction of closed triangles\t%.4g\n", ClosedTriads/double(ClosedTriads+OpenTriads));
    TSnap::GetBfsEffDiam(m->get_m_graph(), 1000, false, EffDiam, FullDiam);
    printf("Diameter (longest shortest path)\t%d\n", FullDiam);
    printf("90-percentile effective diameter\t%.2g\n", EffDiam);

    // Show node values in tree order if requested.
    if (CommandOpts::depthFirstRoot >= 0) {
        m->ShowDepthFirst(CommandOpts::depthFirstRoot);
    }
    else if (CommandOpts::breadthFirstRoot >= 0) {
        printf("error: breadth-first-root is not yet implemented.\n");
    }

    // Write the end-state graph if --write was present.
    if (CommandOpts::writeDot) TSnap::SaveGViz(m->get_m_graph(), CommandOpts::outFile);

    // Write run statistics unless --no-write-info was present.
    if (!CommandOpts::noWriteInfo) WriteInfo(m);

    delete m;
    exit(0);
}
