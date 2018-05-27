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

#define VERSION "180525.1"
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
struct CommandOpts {
    rulenr_t ruleNr;
    int convertOnly;
    int nrIterations;
    int randSeed;
    int selfEdges;
    int noMultiEdges;
    int noInfo;
    int shortInfo;
    int printTape;
    int nrNodes;
    bool rulePresent;
    bool textPresent;
    bool writeDot;
    char* outFile;
    char* ruleText;
};

static CommandOpts cmdOpt;

//---------------
static
char* strAllocCpy(const char* src) { return strcpy(new char[strlen(src) + 1], src); }

//---------------
// TODO: Learn where the hell 'optind' came from.
static
void ParseCommand(const int argc, char* argv[]) {
    int c;
    bool errorFound = false;

    cmdOpt.convertOnly = 0;
    cmdOpt.nrIterations = 128;
    cmdOpt.randSeed = -1;
    cmdOpt.selfEdges = 0;
    cmdOpt.noMultiEdges = 0;
    cmdOpt.noInfo = 0;
    cmdOpt.shortInfo = 0;
    cmdOpt.printTape = 0;
    cmdOpt.nrNodes = 256;
    cmdOpt.rulePresent = false;
    cmdOpt.textPresent = false;
    cmdOpt.writeDot = false;
    cmdOpt.outFile = NULL;
    cmdOpt.ruleText = NULL;

    static struct option long_options[] = {
        {"convert-only", no_argument, &cmdOpt.convertOnly, 1},
        {"self-edges", no_argument, &cmdOpt.selfEdges, 1},
        {"no-multi-edges", no_argument, &cmdOpt.noMultiEdges, 1},
        {"no-info", no_argument, &cmdOpt.noInfo, 1},
        {"short-info", no_argument, &cmdOpt.shortInfo, 1},
        {"print", no_argument, &cmdOpt.printTape, 1},

        {"machine", required_argument, 0, 'm'},
        {"iterations", required_argument, 0, 'i'},
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
            cmdOpt.nrIterations = atoi(optarg);
            break;

          case 'n':
            cmdOpt.nrNodes = atoi(optarg);
            break;

          case 'a':
            cmdOpt.randSeed = atoi(optarg);
            break;

          case 't':
            cmdOpt.textPresent = true;
            if (cmdOpt.rulePresent) {
                printf("error: can't specify both --text and --rule\n");
                errorFound = true;
            } else {
                cmdOpt.ruleText = strAllocCpy(optarg);
            }
            break;

          case 'r':
            cmdOpt.rulePresent = true;
            if (cmdOpt.textPresent) {
                printf("error: can't specify both --text and --rule\n");
                errorFound = true;
            } else {
                char* endPtr;
                cmdOpt.ruleNr = strtoumax(optarg, &endPtr, 10); // radix 10
                if (cmdOpt.ruleNr == 0) {
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
            cmdOpt.outFile = strAllocCpy(optarg);
            cmdOpt.writeDot = true;
            break;

          case '?':
            errorFound = true;
            break;

          default:
            abort();
       }
    }

    // Check option consistency.

    // Create a random rule number if called for.
    if (cmdOpt.randSeed >= 0) {
        Rule* r  = new Rule((rulenr_t) 0);
        srand(cmdOpt.randSeed);
        cmdOpt.ruleNr = ((unsigned long long) rand() * RAND_MAX + rand()) % r->get_maxRuleNr();
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
    if (cmdOpt.textPresent) {
        Rule* rule = new Rule(cmdOpt.ruleText);
        printf("%llu\n", rule->get_ruleNr());
        delete rule;
        return 0; // failure
    }
    else { // rulePresent
        Rule* rule = new Rule(cmdOpt.ruleNr);
        printf("%s\n", rule->get_ruleText().c_str());
        delete rule;
        return 0; // success
    }
}

//---------------
// SaveMachine
//
// Write the current machine state to a file.
//---------------
static
void SaveMachine(const std::string runId, MachineS* m, const char* outFile) {
    TSnap::SaveGViz(m->get_m_graph(), outFile, TStr(runId.c_str()), false);
    // (above, false => no node labels are provided)
}

//---------------
// WriteInfo
//
// Write a file containing JSON-encoded run parameters and outcome statistics.
//---------------
static
void WriteInfo(std::string runId, MachineS* machine) {
    // Capture the run parameters.
    Json::Value info;
    Json::Value ruleParts;
    Json::Value rulePartsText;

    info["runId"] = runId;
    info["version"] = VERSION;
    info["ruleNr"] = (Json::UInt64) machine->m_rule->get_ruleNr();
    info["nrNodes"] = machine->m_nrNodes;
    info["nrIterations"] = cmdOpt.nrIterations;
    info["selfEdges"] = cmdOpt.selfEdges;
    info["noMultiEdges"] = cmdOpt.noMultiEdges;
    if (!cmdOpt.shortInfo) {
        for (int i = 0; i < NR_TRIAD_STATES; i += 1) {
            ruleParts.append(machine->m_ruleParts[i]);
            rulePartsText.append(machine->m_rule->RulePartText(machine->m_ruleParts[i]));
        }
        info["ruleParts"] = ruleParts;
        info["ruleText"] = machine->m_rule->get_ruleText();
        info["rulePartsText"] = rulePartsText;

        // Develop and capture outcome measures.
        Json::Value ccSizeCount;
        TVec<TPair<TInt, TInt> > sizeCount;
        TSnap::GetWccSzCnt(machine->get_m_graph(), sizeCount);
        int nrCcs = 0;
        for (int i = 0; i < sizeCount.Len(); i += 1) {
            Json::Value sizeCountPair;
            sizeCountPair.append((int) sizeCount[i].Val1);
            sizeCountPair.append((int) sizeCount[i].Val2);
            ccSizeCount.append(sizeCountPair);
            nrCcs += (int) sizeCount[i].Val2;
        }
        info["ccSizeCount"] = ccSizeCount;
        info["nrCcSizes"] = sizeCount.Len();
        info["nrCcs"] = nrCcs;

        // TODO: Learn the meaning of DegCCfV, below.
        TFltPrV DegCCfV;
        int64 ClosedTriads, OpenTriads;
        const double CCF = TSnap::GetClustCf(machine->get_m_graph(), DegCCfV, ClosedTriads, OpenTriads);
        info["avgClustCoef"] = CCF;
        info["nrClosedTriads"] = (uint64_t) TUInt64(ClosedTriads);
        info["nrOpenTriads"] = (uint64_t) TUInt64(OpenTriads);

        int FullDiam;
        double EffDiam;
        TSnap::GetBfsEffDiam(machine->get_m_graph(), 1000, false, EffDiam, FullDiam);
        info["diameter"] = FullDiam;
        info["effDiameter90Pctl"] = EffDiam;

        Json::Value inDegreeCount;
        TVec<TPair<TInt, TInt> > inDegCnt;
        TSnap::GetInDegCnt(machine->get_m_graph(), inDegCnt); 
        for (int i = 0; i < inDegCnt.Len(); i += 1) {
            Json::Value inDegreeCountPair;
            inDegreeCountPair.append((int) inDegCnt[i].Val1);
            inDegreeCountPair.append((int) inDegCnt[i].Val2);
            inDegreeCount.append(inDegreeCountPair);
        }
        info["inDegreeCount"] = inDegreeCount;
        info["nrInDegrees"] = inDegCnt.Len();
    }

    Json::FastWriter stringWriter;
    std::string infoString = stringWriter.write(info);
    std::cout << infoString << std::endl;
}

//---------------
int main(const int argc, char* argv[]) {

    cmdOpt.ruleNr = 
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
    if (cmdOpt.convertOnly) {
        if ((cmdOpt.textPresent && !cmdOpt.rulePresent)
          || (!cmdOpt.textPresent && cmdOpt.rulePresent)) {
            exit(DoConversion());
        } else {
            printf("error: must specify either --rule xor --text\n");
            exit(1);
        }
    }

    // Convert-only was not selected, so build and run the machine.
    if (cmdOpt.textPresent) {
        Rule* tmpRule = new Rule(cmdOpt.ruleText);
        cmdOpt.ruleNr = tmpRule->get_ruleNr();
        delete tmpRule;
    }

    // Fabricate a run identifier. (TODO: Enrich runId composition.)
    std::string runId = std::string(std::to_string(cmdOpt.ruleNr));

    MachineS* m = new MachineS(cmdOpt.ruleNr, cmdOpt.nrNodes);

    for (int i = 1; i <= cmdOpt.nrIterations; i += 1)
        m->Cycle(cmdOpt.selfEdges, cmdOpt.noMultiEdges);

    // Write the end-state machine if --write was present.
    //if (cmdOpt.writeDot) TSnap::SaveGViz(m->get_m_graph(), cmdOpt.outFile);
    if (cmdOpt.writeDot) SaveMachine(runId, m, cmdOpt.outFile);

    // Write run information unless --no-write-info was present.
    if (!cmdOpt.noInfo) WriteInfo(runId, m);

    delete m;
    exit(0);
}
