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
    int nrNodes;
    unsigned int cycleCheckDepth;
    bool rulePresent;
    std::string machineTypeName;
    std::string recordName;
};
static CommandOpts cmdOpt;

//---------------
// Command line parsing structure
//
#define MAX_COMMAND_OPTIONS 128

#define CO_CYCLE_CHECK_DEPTH 1001
#define CO_HELP 1002
#define CO_RECORD 1003
#define CO_NOOP 1004

static struct option long_options[MAX_COMMAND_OPTIONS] = {
    {"no-console", no_argument, &cmdOpt.noConsole, 1},
    {"extend-id", no_argument, &cmdOpt.extendId, 1},

    {"cycle-check-depth", required_argument, 0, CO_CYCLE_CHECK_DEPTH},
    {"max-iterations", required_argument, 0, 'i'},
    {"machine", required_argument, 0, 'm'},
    {"nodes", required_argument, 0, 'n'},
    {"noop", no_argument, 0, CO_NOOP},
    {"randseed", required_argument, 0, 'a'},
    {"rule", required_argument, 0, 'r'},
    {"record", required_argument, 0, CO_RECORD},

    {"help", no_argument, 0, CO_HELP},
    {0, 0, 0, 0}
};

//---------------
char* strAllocCpy(const char* src) { return strcpy(new char[strlen(src) + 1], src); }

//---------------
static
void ParseCommand(const int argc, char* argv[]) {
    int c;
    bool errorFound = false;

    // Set command options to default values.
    // TODO: Need we initialize the "flag" option vars?
    cmdOpt.ruleNr = 641;
    cmdOpt.maxIterations = 1031;
    cmdOpt.randSeed = 1;
    cmdOpt.noConsole = 0;
    cmdOpt.extendId = 0;
    cmdOpt.nrNodes = 1031;
    cmdOpt.cycleCheckDepth = 1031;
    cmdOpt.rulePresent = false;
    cmdOpt.machineTypeName = "C";
    cmdOpt.recordName = "";

    while (true) {

        int option_index = 0;
        c = getopt_long(argc, argv, "m:i:n:r:a:t:s:", // disused
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

          case 'r':
            cmdOpt.rulePresent = true;
            char* endPtr;
            cmdOpt.ruleNr = strtoumax(optarg, &endPtr, 10); // radix 10
            if (cmdOpt.ruleNr == 0) {
                std::cerr << "error: invalid rule number" << std::endl;
                errorFound = true;
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
    if (errorFound) exit(1);

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
int main(const int argc, char* argv[]) {

    // Parse the command.
    ParseCommand(argc, argv);

    // Set Runner/Machine default parameters.
    Runner::SetRunnerDefaults(cmdOpt.nrNodes, cmdOpt.maxIterations, cmdOpt.cycleCheckDepth, "single-center", -1, "ring", 0);
    // -1 => (unused) tapePctBlack, 0 => noChangeTopo

    // Create a machine-runner.
    Runner* r = new Runner((rulenr_t) cmdOpt.ruleNr);

    // Run the machine.
    r->Run();

    // Show the outcome data.
    std::cout << "nrIterations: " << r->m_nrIterations << std::endl;
    std::cout << "cycleLength: " << r->m_cycleLength << std::endl;
    std::cout << "nrCcSizes: " << r->m_nrCcSizes << std::endl;
    std::cout << "nrCcs: " << r->m_nrCcs << std::endl;
    std::cout << "nrNodes: " << r->m_nrNodes << std::endl;
    std::cout << "avgClustCoef: " << r->m_avgClustCoef << std::endl;
    std::cout << "nrClosedTriads: " << r->m_nrClosedTriads << std::endl;
    std::cout << "nrOpenTriads: " << r->m_nrOpenTriads << std::endl;
    std::cout << "diameter: " << r->m_diameter << std::endl;
    std::cout << "effDiameter90Pctl: " << r->m_effDiameter90Pctl << std::endl;
    std::cout << "nrInDegrees: " << r->m_nrInDegrees << std::endl;
    std::cout << "maxInDegree: " << r->m_maxInDegree << std::endl;
    std::cout << "inDegreeEntropy: " << r->m_inDegreeEntropy << std::endl;

    // Dispose of the machine-runner.
    delete r;

    std::cout << "finis." << std::endl;
    exit(0);
/*
    pool <- POOLSIZE random rules
    do
        note the average fitness in the pool // (fitness is computed on demand)
        newpoolsize <- 0
        while newpoolsize < POOLSIZE
            newpool += 2 parent chromosomes selected with fitness bias from pool
            mutate the 2
            recombine the 2
            newpool += the resulting 2 mutined chromosomes
        end
        pool <- newpool
    until pool is deemed finished
    finis.
*/
}
