#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdelete-non-virtual-dtor"
#include "Snap.h"
#pragma GCC diagnostic pop
#include <assert.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
//#include <algorithm>
//#include <ctime>
//#include <chrono>
//#include <fstream>
#include <iostream>
#include <limits>
#include <string>
#include <vector>
//#include <json/json.h>
#include "netaut.h"
//#include "rule.h"
//#include "machine.h"
//#include "machine2D.h"
//#include "machineR.h"
#include "runner.h"

#define VERSION "V190216.0"

//---------------
// Command option settings
//
struct CommandOpts {
    rulenr_t ruleNr;
    int maxIterations;
    int randSeed;
    int noConsole;
    int extendId;
    int printTape;
    int noWriteEndGraph;
    int noChangeTopo;
    int tapeX;
    int nrNodes;
    int graphWriteStart;
    int graphWriteStride;
    int graphWriteStop;
    int statWriteStart;
    int statWriteStride;
    int statWriteStop;
    unsigned int cycleCheckDepth;
    int tapePctBlack;
    bool rulePresent;
    bool ruletextPresent;
    std::string machineTypeName;
    std::string recordName;
    std::string graphFileSuffix;
    std::string writeAsName;
    std::string writeTapeName;
    char* ruleText;
    std::string tapeStructure;
    std::string topoStructure;
};
static CommandOpts cmdOpt;

//---------------
// Command line parsing structure
//
#define MAX_COMMAND_OPTIONS 128

#define CO_GRAPH_START 1000
#define CO_GRAPH_STRIDE 1001
#define CO_GRAPH_STOP 1009
#define CO_STAT_START 1010
#define CO_STAT_STRIDE 1011
#define CO_STAT_STOP 1012
#define CO_CYCLE_CHECK_DEPTH 1002
#define CO_HELP 1003
#define CO_INIT_TAPE 1004
#define CO_INIT_TOPO 1005
#define CO_TAPE_PCT_BLACK 1006
#define CO_WRITE_GRAPH_AS 1007
#define CO_RECORD 1013
#define CO_NOOP 1008

static struct option long_options[MAX_COMMAND_OPTIONS] = {
    {"no-console", no_argument, &cmdOpt.noConsole, 1},
    {"extend-id", no_argument, &cmdOpt.extendId, 1},
    {"no-write-end-graph", no_argument, &cmdOpt.noWriteEndGraph, 1},
    {"no-change-topo", no_argument, &cmdOpt.noChangeTopo, 1},
    {"print-tape", no_argument, &cmdOpt.printTape, 1},
    {"tape-x", no_argument, &cmdOpt.tapeX, 1},

    {"cycle-check-depth", required_argument, 0, CO_CYCLE_CHECK_DEPTH},
    {"init-tape", required_argument, 0, CO_INIT_TAPE},
    {"init-topo", required_argument, 0, CO_INIT_TOPO},
    {"max-iterations", required_argument, 0, 'i'},
    {"machine", required_argument, 0, 'm'},
    {"nodes", required_argument, 0, 'n'},
    {"noop", no_argument, 0, CO_NOOP},
    {"randseed", required_argument, 0, 'a'},
    {"rule", required_argument, 0, 'r'},
    {"ruletext", required_argument, 0, 't'},
    {"suffix", required_argument, 0, 's'},
    {"tape-pct-black", required_argument, 0, CO_TAPE_PCT_BLACK},
    {"graph-start", required_argument, 0, CO_GRAPH_START},
    {"graph-stride", required_argument, 0, CO_GRAPH_STRIDE},
    {"graph-stop", required_argument, 0, CO_GRAPH_STOP},
    {"stat-start", required_argument, 0, CO_STAT_START},
    {"stat-stride", required_argument, 0, CO_STAT_STRIDE},
    {"stat-stop", required_argument, 0, CO_STAT_STOP},
    {"write-graph-as", required_argument, 0, CO_WRITE_GRAPH_AS},
    {"record", required_argument, 0, CO_RECORD},

    {"help", no_argument, 0, CO_HELP},
    {0, 0, 0, 0}
};

//---------------
char* strAllocCpy(const char* src) { return strcpy(new char[strlen(src) + 1], src); }

//---------------
// TODO: Learn where the hell 'optind' came from.
static
void ParseCommand(const int argc, char* argv[]) {
    int c;
    bool errorFound = false;

    // Set command options to default values.
    // TODO: Need we initialize the "flag" option vars?
    cmdOpt.maxIterations = 128;
    cmdOpt.randSeed = -1;
    cmdOpt.noConsole = 0;
    cmdOpt.extendId = 0;
    cmdOpt.printTape = 0;
    cmdOpt.tapeX = 0;
    cmdOpt.nrNodes = 256;
    cmdOpt.graphWriteStart = -1;
    cmdOpt.graphWriteStride = -1;
    cmdOpt.graphWriteStop = std::numeric_limits<int>::max();
    cmdOpt.statWriteStart = -1;
    cmdOpt.statWriteStride = -1;
    cmdOpt.statWriteStop = std::numeric_limits<int>::max();
    cmdOpt.cycleCheckDepth = 0;
    cmdOpt.tapePctBlack = 50;
    cmdOpt.rulePresent = false;
    cmdOpt.ruletextPresent = false;
    cmdOpt.noWriteEndGraph = false;
    cmdOpt.machineTypeName = "";
    cmdOpt.graphFileSuffix = "";
    cmdOpt.writeAsName = "";
    cmdOpt.recordName = "";
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
                std::cerr << "error: can't specify both --text and --rule" << std::endl;
                errorFound = true;
            } else {
                cmdOpt.ruleText = strAllocCpy(optarg);
            }
            break;

          case 'r':
            cmdOpt.rulePresent = true;
            if (cmdOpt.ruletextPresent) {
                std::cerr << "error: can't specify both --text and --rule" << std::endl;
                errorFound = true;
            } else {
                char* endPtr;
                cmdOpt.ruleNr = strtoumax(optarg, &endPtr, 10); // radix 10
                if (cmdOpt.ruleNr == 0) {
                    std::cerr << "error: invalid rule number" << std::endl;
                    errorFound = true;
                }
            }
            break;

          case 'm':
            cmdOpt.machineTypeName = std::string(optarg);
            if (cmdOpt.machineTypeName != "C"
              && cmdOpt.machineTypeName != "R"
              && cmdOpt.machineTypeName != "CM"
              && cmdOpt.machineTypeName != "RM") {
                std::cerr << "error: machine type '" << cmdOpt.machineTypeName << "' is not recognized."  << std::endl;
                errorFound = true;
            }
            break;

          case 's':
            cmdOpt.graphFileSuffix = optarg;
            break;

          case 'w':
            cmdOpt.noWriteEndGraph = 1;
            break;

          case CO_GRAPH_START:
            cmdOpt.graphWriteStart = atoi(optarg);
            break;

          case CO_GRAPH_STRIDE:
            cmdOpt.graphWriteStride = atoi(optarg);
            break;

          case CO_GRAPH_STOP:
            cmdOpt.graphWriteStop = atoi(optarg);
            break;

          case CO_STAT_START:
            cmdOpt.statWriteStart = atoi(optarg);
            break;

          case CO_STAT_STRIDE:
            cmdOpt.statWriteStride = atoi(optarg);
            break;

          case CO_STAT_STOP:
            cmdOpt.statWriteStop = atoi(optarg);
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

          case CO_WRITE_GRAPH_AS:
            cmdOpt.writeAsName = optarg;
            break;

          case CO_RECORD:
            cmdOpt.recordName = optarg;
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
        std::cerr << "error: --graph-start and --graph-stride must both be specified" << std::endl;
        errorFound = true;
    }
    if (cmdOpt.graphWriteStart >= 0 && cmdOpt.graphWriteStop < cmdOpt.graphWriteStart) {
        std::cerr << "error: --graph-stop must be >= --graph-st" << std::endl;
        errorFound = true;
    }

    if ((cmdOpt.statWriteStart >= 0 && cmdOpt.statWriteStride < 0)
      || (cmdOpt.statWriteStart < 0 && cmdOpt.statWriteStride >= 0)) {
        std::cerr << "error: --stat-start and --stat-stride must both be specified" << std::endl;
        errorFound = true;
    }
    if (cmdOpt.statWriteStart >= 0 && cmdOpt.statWriteStop < cmdOpt.statWriteStart) {
        std::cerr << "error: --stat-stop must be >= --stat-stop" << std::endl;
        errorFound = true;
    }

    if (cmdOpt.recordName == "") {
        std::cerr << "error: --record must be specified" << std::endl;
        errorFound = true;
    }

    if (cmdOpt.machineTypeName != "C"
      && cmdOpt.machineTypeName != "R"
      && cmdOpt.machineTypeName != "CM"
      && cmdOpt.machineTypeName != "RM") {
        std::cerr << "error: --machine must be specified as C, R, CM, or RM" << std::endl;
        errorFound = true;
    }

    if ((cmdOpt.machineTypeName == "R"|| cmdOpt.machineTypeName == "RM")
      && cmdOpt.noChangeTopo) {
        std::cerr << "error: --no-change-topo is not sensible for R* machine types" << std::endl;
        errorFound = true;
    }

    if (errorFound) exit(1);

    // Adjust --stat-start if user specified zero (0th is written unconditionally)
    if (cmdOpt.statWriteStart == 0) {
        if (cmdOpt.statWriteStop == 0)
            cmdOpt.statWriteStart = -1;
        else
            cmdOpt.statWriteStart = 1;
    }

    // Warn of odd selections.
    if (tapePctBlackSpecified && cmdOpt.tapeStructure != "random")
        std::cerr << "warning: --tape-pct-black with non-random init-tape structure has no effect" << std::endl;

    // Warn if any non-option command arguments are present.
    if (optind < argc) {
        std::cerr << "warning: there are extraneous command arguments" << std::endl;
        // TODO: Direct the following to cerr.
        while (optind < argc) printf ("%s ", argv[optind++]);
        putchar ('\n');
    }

    // Check 'nrNodes' generations for cycles unless depth was set explicitly.
    if (cmdOpt.cycleCheckDepth == 0) cmdOpt.cycleCheckDepth = cmdOpt.nrNodes;
}

//---------------
// Compress
//
// Return a string with whitespace removed.
//---------------
static
std::string Compress(std::string in) {
    std::string out = "";
    for (char c : in) if (c != '\n' and c != '\t' and c != ' ') out += c;
    return out;
}

//---------------
int main(const int argc, char* argv[]) {

    cmdOpt.ruleNr = 15;

    // Parse the command.
    ParseCommand(argc, argv);
    exit(0);
}
