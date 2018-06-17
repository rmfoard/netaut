#include "Snap.h"
#include <assert.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <string>
#include <json/json.h>
#include "rule.h"
#include "machine.h"

#define VERSION "V180614.0"
#define NR_CYCLES 40

//---------------
struct CommandOpts {
    rulenr_t ruleNr;
    int convertOnly;
    int nrIterations;
    int selfEdges;
    int noInfo;
    int printTape;
    int noWriteEndState;
    int nrNodes;
    int writeStart;
    int writeStride;
    int cycleCheckDepth;
    bool rulePresent;
    bool ruletextPresent;
    std::string outFileSuffix;
    char* ruleText;
    std::string tapeStructure;
    std::string topoStructure;
};

static CommandOpts cmdOpt;

//---------------
char* strAllocCpy(const char* src) { return strcpy(new char[strlen(src) + 1], src); }

//---------------
// TODO: Learn where the hell 'optind' came from.
static
void ParseCommand(const int argc, char* argv[]) {
    int c;
    bool errorFound = false;

    // Set command options to default values.
    cmdOpt.convertOnly = 0;
    cmdOpt.nrIterations = 128;
    cmdOpt.selfEdges = 0;
    cmdOpt.noInfo = 0;
    cmdOpt.printTape = 0;
    cmdOpt.nrNodes = 256;
    cmdOpt.writeStart = -1;
    cmdOpt.writeStride = -1;
    cmdOpt.cycleCheckDepth = 20;
    cmdOpt.rulePresent = false;
    cmdOpt.ruletextPresent = false;
    cmdOpt.noWriteEndState = false;
    cmdOpt.outFileSuffix = std::string("");
    cmdOpt.ruleText = NULL;
    cmdOpt.tapeStructure = std::string("single");
    cmdOpt.topoStructure = std::string("ring");

#define CO_WRITE_START 1000
#define CO_WRITE_STRIDE 1001
#define CO_CYCLE_CHECK_DEPTH 1002
#define CO_HELP 1003
#define CO_INIT_TAPE 1004
#define CO_INIT_TOPO 1005

    static struct option long_options[] = {
        {"allow-self-edges", no_argument, &cmdOpt.selfEdges, 1},
        {"convert-only", no_argument, &cmdOpt.convertOnly, 1},
        {"no-info", no_argument, &cmdOpt.noInfo, 1},
        {"no-write-end-state", no_argument, &cmdOpt.noWriteEndState, 1},
        {"print", no_argument, &cmdOpt.printTape, 1},

        {"cycle-check-depth", required_argument, 0, CO_CYCLE_CHECK_DEPTH},
        {"init-tape", required_argument, 0, CO_INIT_TAPE},
        {"init-topo", required_argument, 0, CO_INIT_TOPO},
        {"iterations", required_argument, 0, 'i'},
        {"machine", required_argument, 0, 'm'},
        {"nodes", required_argument, 0, 'n'},
        {"rule", required_argument, 0, 'r'},
        {"ruletext", required_argument, 0, 't'},
        {"suffix", required_argument, 0, 's'},
        {"write-start", required_argument, 0, CO_WRITE_START},
        {"write-stride", required_argument, 0, CO_WRITE_STRIDE},

        {"help", no_argument, 0, CO_HELP},
        {0, 0, 0, 0}
    };

    while (true) {

        int option_index = 0;
        c = getopt_long(argc, argv, "m:i:n:r:a:t:s:",
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

          case 't':
            cmdOpt.ruletextPresent = true;
            if (cmdOpt.rulePresent) {
                printf("error: can't specify both --text and --rule\n");
                errorFound = true;
            } else {
                cmdOpt.ruleText = strAllocCpy(optarg);
            }
            break;

          case 'r':
            cmdOpt.rulePresent = true;
            if (cmdOpt.ruletextPresent) {
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

          case 's':
            cmdOpt.outFileSuffix = std::string(optarg);
            break;

          case 'w':
            cmdOpt.noWriteEndState = 1;
            break;

          case CO_WRITE_START:
            cmdOpt.writeStart = atoi(optarg);
            break;

          case CO_WRITE_STRIDE:
            cmdOpt.writeStride = atoi(optarg);
            break;

          case CO_CYCLE_CHECK_DEPTH:
            cmdOpt.cycleCheckDepth = atoi(optarg);
            break;

          case CO_HELP:
            printf("Command options:\n");
            for (auto entry : long_options) if (entry.name != NULL) {
                printf("  --%s", entry.name);
                if (entry.has_arg) printf(" <value>");
                printf("\n");
            }
            exit(0);

          case CO_INIT_TAPE:
            cmdOpt.tapeStructure = std::string(optarg);
            break;

          case CO_INIT_TOPO:
            cmdOpt.topoStructure = std::string(optarg);
            break;

          case '?':
            errorFound = true;
            break;

          default:
            abort();
       }
    }

    // Check option consistency.

    if ((cmdOpt.writeStart >= 0 && cmdOpt.writeStride < 0)
      || (cmdOpt.writeStart < 0 && cmdOpt.writeStride >= 0)) {
        printf("error: --write-start and --write-stride must both be specified\n");
        errorFound = true;
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
    if (cmdOpt.ruletextPresent) {
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
// WriteState
//
// Write the current machine state to a file.
//---------------
static
void WriteState(const std::string runId, MachineS* m, const std::string outFileSuffix, const int numTag) {
    TIntStrH nodeColorHash = THash<TInt, TStr>();
    int* nodeStates = m->get_nodeStates();
    for (TNGraph::TNodeI NIter = m->get_graph()->BegNI(); NIter < m->get_graph()->EndNI(); NIter++) {
        int nId = NIter.GetId();
        nodeColorHash.AddDat(nId, (nodeStates[nId] == NBLACK) ? "black" : "white");
    }

    // Compose the file name.
    std::string suffix = std::string("");
    if (outFileSuffix != std::string("")) suffix = std::string("_") + outFileSuffix;

    std::string stateFName = std::string("");
    if (numTag < 0)
        stateFName = runId + suffix;
    else
        stateFName = runId + suffix + std::string(".") + std::to_string(numTag);
    stateFName += std::string(".dot");

    // Compose the description string.
    std::string description = runId + std::string(" @")
      + (numTag < 0 ? std::to_string(cmdOpt.nrIterations) : std::to_string(numTag));

    // Write state to the file (false => no labels provided).
    TSnap::SaveGViz(m->get_graph(), stateFName.c_str(), TStr(description.c_str()), false, nodeColorHash);
}

//---------------
// WriteInfo
//
// Write a file containing JSON-encoded run parameters and outcome statistics.
//---------------
static
void WriteInfo(std::string runId, MachineS* machine, int nrActualIterations, int cycleLength, int runTimeMs) {
    // Capture the run parameters.
    Json::Value info;

    info["runId"] = runId;
    info["version"] = VERSION;
    info["ruleNr"] = (Json::UInt64) machine->m_rule->get_ruleNr();
    info["nrNodes"] = machine->m_nrNodes;
    info["nrIterations"] = cmdOpt.nrIterations;
    info["selfEdges"] = cmdOpt.selfEdges;
    info["cycleCheckDepth"] = cmdOpt.cycleCheckDepth;
    info["tapeStructure"] = cmdOpt.tapeStructure;
    info["topoStructure"] = cmdOpt.topoStructure;
    info["nrActualIterations"] = nrActualIterations;
    info["cycleLength"] = cycleLength;
    info["runTimeMs"] = runTimeMs;
    MachineS::Statistics* stats = machine->get_stats();
    info["multiEdgesAvoided"] = (Json::Value::UInt64) stats->multiEdgesAvoided;
    info["selfEdgesAvoided"] = (Json::Value::UInt64) stats->selfEdgesAvoided;
    Json::Value triadOccurrences;
    for (int i = 0; i < NR_TRIAD_STATES; i += 1) {
        auto occurrences = stats->triadOccurrences[i];
        triadOccurrences.append((Json::Value::UInt64) occurrences);
    }
    info["triadOccurrences"] = triadOccurrences;

    // Develop and capture outcome measures.
    Json::Value ccSizeCount;
    TVec<TPair<TInt, TInt> > sizeCount;
    TSnap::GetWccSzCnt(machine->get_graph(), sizeCount);
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
    const double CCF = TSnap::GetClustCf(machine->get_graph(), DegCCfV, ClosedTriads, OpenTriads);
    info["avgClustCoef"] = CCF;
    info["nrClosedTriads"] = (uint64_t) TUInt64(ClosedTriads);
    info["nrOpenTriads"] = (uint64_t) TUInt64(OpenTriads);

    int FullDiam;
    double EffDiam;
    TSnap::GetBfsEffDiam(machine->get_graph(), 1000, false, EffDiam, FullDiam);
    info["diameter"] = FullDiam;
    info["effDiameter90Pctl"] = EffDiam;

    Json::Value inDegreeCount;
    TVec<TPair<TInt, TInt> > inDegCnt;
    TSnap::GetInDegCnt(machine->get_graph(), inDegCnt); 
    for (int i = 0; i < inDegCnt.Len(); i += 1) {
        Json::Value inDegreeCountPair;
        inDegreeCountPair.append((int) inDegCnt[i].Val1);
        inDegreeCountPair.append((int) inDegCnt[i].Val2);
        inDegreeCount.append(inDegreeCountPair);
    }
    info["inDegreeCount"] = inDegreeCount;
    info["nrInDegrees"] = inDegCnt.Len();

    // TODO: Replace with non-deprecated equivalent.
    Json::FastWriter stringWriter;
    std::string infoString = stringWriter.write(info);
    assert(!infoString.empty());
    if (infoString[infoString.length() - 1] == '\n')
        infoString.erase(infoString.length() - 1);

    std::cout << infoString;
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
        if ((cmdOpt.ruletextPresent && !cmdOpt.rulePresent)
          || (!cmdOpt.ruletextPresent && cmdOpt.rulePresent)) {
            exit(DoConversion());
        } else {
            printf("error: must specify either --rule xor --text\n");
            exit(1);
        }
    }

    // Convert-only was not selected, so build and run the machine.
    if (cmdOpt.ruletextPresent) {
        Rule* tmpRule = new Rule(cmdOpt.ruleText);
        cmdOpt.ruleNr = tmpRule->get_ruleNr();
        delete tmpRule;
    }

    // Fabricate a run identifier. (TODO: Enrich runId composition.)
    std::string runId = std::string("R") + std::string(std::to_string(cmdOpt.ruleNr));

    // Create the machine.
    MachineS* m = new MachineS(cmdOpt.ruleNr, cmdOpt.nrNodes, cmdOpt.cycleCheckDepth,
      cmdOpt.tapeStructure, cmdOpt.topoStructure);

    // Run it, saving state periodically if specified.
    auto start_time = std::chrono::high_resolution_clock::now();
    int iter, cycleLength;
    for (iter = 0; iter < cmdOpt.nrIterations; iter += 1) {
        if (cmdOpt.writeStart >= 0) {
            if (iter >= cmdOpt.writeStart && (iter - cmdOpt.writeStart) % cmdOpt.writeStride == 0) {
                WriteState(runId, m, cmdOpt.outFileSuffix, iter);
            }
        }

        // Stop iteration if 'IterateMachine' reported a state cycle.
        cycleLength = m->IterateMachine(cmdOpt.selfEdges, iter);
        if (cycleLength > 0) break;
    } // The residual value of 'iter' is the actual number of iterations performed.
    auto stop_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_secs = stop_time - start_time;
    int runTimeMs = elapsed_secs.count() * 1000;

    // Write the end-state machine unless --no-write-end-state was present.
    //   (-1 => no numeric tag for inclusion in file name)
    if (!cmdOpt.noWriteEndState) WriteState(runId, m, cmdOpt.outFileSuffix, -1);

    // Write run information unless --no-write-info was present.
    if (!cmdOpt.noInfo) WriteInfo(runId, m, iter, cycleLength, runTimeMs);

    delete m;
    exit(0);
}
