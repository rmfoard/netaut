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
#include <ctime>
#include <chrono>
#include <iostream>
#include <string>
#include <json/json.h>
#include "rule.h"
#include "machine.h"
#include "machine2D.h"

#define VERSION "V180720.0"

//---------------
// Command option settings
//
struct CommandOpts {
    rulenr_t ruleNr;
    int maxIterations;
    int randSeed;
    int allowSelfEdges;
    int noInfo;
    int printTape;
    int noWriteEndState;
    int nrNodes;
    int graphWriteStart;
    int graphWriteStride;
    unsigned int cycleCheckDepth;
    int tapePctBlack;
    bool rulePresent;
    bool ruletextPresent;
    std::string outFileSuffix;
    std::string writeAsName;
    char* ruleText;
    std::string tapeStructure;
    std::string topoStructure;
};

static CommandOpts cmdOpt;

//---------------
// Command line parsing structure
//
#define MAX_COMMAND_OPTIONS 128

#define CO_WRITE_START 1000
#define CO_WRITE_STRIDE 1001
#define CO_CYCLE_CHECK_DEPTH 1002
#define CO_HELP 1003
#define CO_INIT_TAPE 1004
#define CO_INIT_TOPO 1005
#define CO_TAPE_PCT_BLACK 1006
#define CO_WRITE_AS 1007
#define CO_NOOP 1008

static struct option long_options[MAX_COMMAND_OPTIONS] = {
    {"no-info", no_argument, &cmdOpt.noInfo, 1},
    {"no-write-end-state", no_argument, &cmdOpt.noWriteEndState, 1},
    {"print-tape", no_argument, &cmdOpt.printTape, 1},

    {"cycle-check-depth", required_argument, 0, CO_CYCLE_CHECK_DEPTH},
    {"init-tape", required_argument, 0, CO_INIT_TAPE},
    {"init-topo", required_argument, 0, CO_INIT_TOPO},
    {"max-iterations", required_argument, 0, 'i'},
    /*{"machine", required_argument, 0, 'm'},*/
    {"nodes", required_argument, 0, 'n'},
    {"noop", no_argument, 0, CO_NOOP},
    {"randseed", required_argument, 0, 'a'},
    {"rule", required_argument, 0, 'r'},
    {"ruletext", required_argument, 0, 't'},
    {"suffix", required_argument, 0, 's'},
    {"tape-pct-black", required_argument, 0, CO_TAPE_PCT_BLACK},
    {"graph-start", required_argument, 0, CO_WRITE_START},
    {"graph-stride", required_argument, 0, CO_WRITE_STRIDE},
    {"write-as", required_argument, 0, CO_WRITE_AS},

    {"help", no_argument, 0, CO_HELP},
    {0, 0, 0, 0}
};

//---------------
char* strAllocCpy(const char* src) { return strcpy(new char[strlen(src) + 1], src); }

//---------------
// RunId
//---------------
static
std::string RunId(std::string machineType, rulenr_t ruleNr) {
    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);
    char* nowStr = strAllocCpy("yymmddhhmmss");
    snprintf(nowStr, strlen(nowStr) + 1, "%02d%02d%02d%02d%02d%02d",
      now->tm_year % 100, now->tm_mon+1, now->tm_mday,
      now->tm_hour, now->tm_min, now->tm_sec);
    return machineType
      + "-" + std::to_string(ruleNr)
      + "-" + std::string(nowStr);
}

//---------------
// TODO: Learn where the hell 'optind' came from.
static
void ParseCommand(const int argc, char* argv[]) {
    int c;
    bool errorFound = false;

    // Set command options to default values.
    cmdOpt.maxIterations = 128;
    cmdOpt.randSeed = -1;
    cmdOpt.allowSelfEdges = 1;
    cmdOpt.noInfo = 0;
    cmdOpt.printTape = 0;
    cmdOpt.nrNodes = 256;
    cmdOpt.graphWriteStart = -1;
    cmdOpt.graphWriteStride = -1;
    cmdOpt.cycleCheckDepth = 0;
    cmdOpt.tapePctBlack = 50;
    cmdOpt.rulePresent = false;
    cmdOpt.ruletextPresent = false;
    cmdOpt.noWriteEndState = false;
    cmdOpt.outFileSuffix = "";
    cmdOpt.writeAsName = "";
    cmdOpt.ruleText = NULL;
    cmdOpt.tapeStructure = "single-center";
    cmdOpt.topoStructure = "ring";

    bool tapePctBlackSpecified = false;

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
            cmdOpt.maxIterations = atoi(optarg);
            break;

          case 'n':
            cmdOpt.nrNodes = atoi(optarg);
            break;

          case 'a':
            cmdOpt.randSeed = atoi(optarg);
            srand(cmdOpt.randSeed); // plant seed at this first opportunity
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

          /*case 'm':
            printf("--machine option is not yet supported.\n");
            errorFound = true;
            break;*/

          case 's':
            cmdOpt.outFileSuffix = optarg;
            break;

          case 'w':
            cmdOpt.noWriteEndState = 1;
            break;

          case CO_WRITE_START:
            cmdOpt.graphWriteStart = atoi(optarg);
            break;

          case CO_WRITE_STRIDE:
            cmdOpt.graphWriteStride = atoi(optarg);
            break;

          case CO_CYCLE_CHECK_DEPTH:
            cmdOpt.cycleCheckDepth = atoi(optarg);
            if (cmdOpt.cycleCheckDepth == 0)
                std::cerr << "warning: 0 cycle-check-depth is replaced with nrNodes" << std::endl;
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
            cmdOpt.tapeStructure = optarg;
            break;

          case CO_INIT_TOPO:
            cmdOpt.topoStructure = optarg;
            break;

          case CO_TAPE_PCT_BLACK:
            cmdOpt.tapePctBlack = atoi(optarg);
            tapePctBlackSpecified = true;
            break;

          case CO_WRITE_AS:
            cmdOpt.writeAsName = optarg;
            break;

          case CO_NOOP:
            break;

          case '?':
            errorFound = true;
            break;

          default:
            /*abort()*/;
       }
    }

    // Check option consistency.
    if ((cmdOpt.graphWriteStart >= 0 && cmdOpt.graphWriteStride < 0)
      || (cmdOpt.graphWriteStart < 0 && cmdOpt.graphWriteStride >= 0)) {
        printf("error: --graph-start and --graph-stride must both be specified\n");
        errorFound = true;
    }

    if (errorFound) exit(1);

    // Warn of odd selections.
    if (tapePctBlackSpecified && cmdOpt.tapeStructure != "random")
        std::cerr << "warning: --tape-pct-black with non-random init-tape structure has no effect" << std::endl;

    // Warn if any non-option command arguments are present.
    if (optind < argc) {
        printf ("warning: there are extraneous command arguments: ");
        while (optind < argc) printf ("%s ", argv[optind++]);
        putchar ('\n');
    }

    // Check 'nrNodes' generations for cycles unless depth was set explicitly.
    if (cmdOpt.cycleCheckDepth == 0) cmdOpt.cycleCheckDepth = cmdOpt.nrNodes;
}

//---------------
// WriteGraph
//
// Write the current machine state to a file.
//---------------
static
void WriteGraph(const std::string runId, Machine* m, const std::string outFileSuffix,
  const int numTag, int actualNrIterations) {
    TIntStrH nodeColorHash = THash<TInt, TStr>();
    int* nodeStates = m->get_nodeStates();
    for (TNGraph::TNodeI NIter = m->get_graph()->BegNI(); NIter < m->get_graph()->EndNI(); NIter++) {
        int nId = NIter.GetId();
        nodeColorHash.AddDat(nId, (nodeStates[nId] == NBLACK) ? "black" : "white");
    }

    // Compose the file name.
    std::string suffix = "";
    if (outFileSuffix != "") suffix = "_" + outFileSuffix;

    std::string stateFName = "";
    std::string baseName = "";
    if (cmdOpt.writeAsName == "")
        baseName = runId;
    else
        baseName = cmdOpt.writeAsName;

    if (numTag < 0)
        stateFName = baseName + suffix;
    else
        stateFName = baseName + suffix + "." + std::to_string(numTag);

    stateFName += ".dot";

    // Compose the description string.
    std::string description = runId + " @"
      + (numTag < 0 ? std::to_string(actualNrIterations) : std::to_string(numTag));

    // Write state to the file (false => no labels provided).
    TSnap::SaveGViz(m->get_graph(), stateFName.c_str(), TStr(description.c_str()), false, nodeColorHash);
}

//---------------
// RemoveNewLines
//---------------
static
std::string Compress(std::string in) {
    std::string out = "";
    for (char c : in) if (c != '\n' and c != '\t' and c != ' ') out += c;
    return out;
}

//---------------
// WriteSummaryInfo
//
// Write a file containing JSON-encoded run parameters and outcome statistics.
//---------------
static
void WriteSummaryInfo(std::string runId, Machine* machine, int nrActualIterations, int cycleLength, int runTimeMs) {
    // Capture the run parameters.
    Json::Value info;

    info["runId"] = runId;
    info["machineType"] = machine->get_machineType();
    info["version"] = VERSION;
    info["ruleNr"] = (Json::UInt64) machine->m_rule->get_ruleNr();
    info["nrNodes"] = machine->m_nrNodes;
    info["maxIterations"] = cmdOpt.maxIterations;
    info["allowSelfEdges"] = cmdOpt.allowSelfEdges;
    info["cycleCheckDepth"] = cmdOpt.cycleCheckDepth;
    info["tapeStructure"] = cmdOpt.tapeStructure;
    info["topoStructure"] = cmdOpt.topoStructure;
    info["randseed"] = cmdOpt.randSeed;

    if (cmdOpt.tapeStructure == "random")
        info["tapePctBlack"] = cmdOpt.tapePctBlack;
    else
        info["tapePctBlack"] = -1;

    // Capture outcome measures.
    info["nrActualIterations"] = nrActualIterations;
    info["cycleLength"] = cycleLength;
    info["runTimeMs"] = runTimeMs;

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

    // Add machine-specific summary information.
    machine->AddSummaryInfo(info);

    Json::StreamWriterBuilder wBuilder;
    std::string infoString = Compress(Json::writeString(wBuilder, info));
    assert(!infoString.empty());
    if (infoString[infoString.length() - 1] == '\n')
        infoString.erase(infoString.length() - 1);

    std::cout << infoString;
}

//---------------
int main(const int argc, char* argv[]) {

    cmdOpt.ruleNr = 15;

    // Instantiate the "2D" (2 degree) machine.
    Machine* m = new Machine2D();

    // Augment the command parsing structure with options specific to
    // the current machine.
    m->AddCommandOptions(long_options, MAX_COMMAND_OPTIONS);

    // Parse the base command and machine-specific options.
    ParseCommand(argc, argv);
    m->ParseCommand(argc, argv);

    // Translate ruletext if it is provided in lieu of a rule number.
    if (cmdOpt.ruletextPresent) {
        Rule* tmpRule = new Rule(cmdOpt.ruleText);
        cmdOpt.ruleNr = tmpRule->get_ruleNr();
        delete tmpRule;
    }

    // Create the machine.
    m->BuildMachine(cmdOpt.ruleNr, cmdOpt.nrNodes, cmdOpt.cycleCheckDepth,
      cmdOpt.tapeStructure, cmdOpt.tapePctBlack,cmdOpt.topoStructure);

    // Fabricate a run identifier.
    std::string runId = RunId(m->get_machineType(), cmdOpt.ruleNr);

    // Run it, saving state periodically if specified.
    auto start_time = std::chrono::high_resolution_clock::now();
    int iter;
    int cycleLength = 0;
    for (iter = 0; iter < cmdOpt.maxIterations; iter += 1) {
        if (cmdOpt.graphWriteStart >= 0) {
            if (iter >= cmdOpt.graphWriteStart && (iter - cmdOpt.graphWriteStart) % cmdOpt.graphWriteStride == 0) {
                WriteGraph(runId, m, cmdOpt.outFileSuffix, iter, iter);
            }
        }

        // Stop iteration if 'IterateMachine' reported a state cycle.
        cycleLength = m->IterateMachine(iter);
        if (cycleLength > 0 || cycleLength < 0) break;
    } // The residual value of 'iter' is the actual number of iterations performed.
    auto stop_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_secs = stop_time - start_time;
    int runTimeMs = elapsed_secs.count() * 1000;

    // Write the end-state machine unless --no-write-end-state was present.
    //   (-1 => no numeric tag for inclusion in file name)
    if (!cmdOpt.noWriteEndState) WriteGraph(runId, m, cmdOpt.outFileSuffix, -1, iter);

    // Write run information unless --no-write-info was present.
    if (!cmdOpt.noInfo) WriteSummaryInfo(runId, m, iter, cycleLength, runTimeMs);

    delete m;
    exit(0);
}