#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdelete-non-virtual-dtor"
#include "Snap.h"
#pragma GCC diagnostic pop
#include <assert.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>
#include "netaut.h"
#include "chromosome.h"
#include "pool.h"
#include "picklist.h"
#include "runner.h"

#define VERSION "V190216.0"

#define POOLSIZE 12
#define MAXGENERATIONS 2

// Set up the Mersenne Twister random number generator.
// 'pMersenne' will point to a seeded instantiation of the generator.
// 'rn05' is an instantiated wrapper that yields uniform [0, 5] when called with
// a generator.
static std::mt19937* pMersenne;
static std::uniform_int_distribution<int> rn05(0, 5);

static std::ofstream journal;

static void ChooseParents(Pool*, Chromosome*&, Chromosome*&);
static void FillRandomPool(Pool*);
static rulenr_t GenRandRule();
static void MutateAndCross(Chromosome*, Chromosome*, Chromosome*&, Chromosome*&);
static void ParseCommand(const int, char**);
static double SimulateGeneration(int, Pool*&);


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
    std::string journalName;
    std::string snapName;
    std::string resumeFromName;
};
static CommandOpts cmdOpt;

//---------------
// Command line parsing structure
//
#define MAX_COMMAND_OPTIONS 128

#define CO_CYCLE_CHECK_DEPTH 1001
#define CO_HELP 1002
#define CO_JOURNAL 1003
#define CO_NOOP 1004
#define CO_SNAP 1005
#define CO_RESUME 1006

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
    {"log", required_argument, 0, CO_JOURNAL},
    {"snapshot", required_argument, 0, CO_SNAP},
    {"resume-from", required_argument, 0, CO_RESUME},

    {"help", no_argument, 0, CO_HELP},
    {0, 0, 0, 0}
};

//---------------
char* strAllocCpy(const char* src) { return strcpy(new char[strlen(src) + 1], src); }

//---------------
void ChooseParents(Pool* pool, Chromosome*& maC, Chromosome*& paC) {
    // TODO: Use an object copy instead.
    rulenr_t maRuleNr = pool->get_entry(0)->get_ruleNr();
    double maFitness = pool->get_entry(0)->get_fitness();
    maC = new Chromosome(maRuleNr, maFitness);

    rulenr_t paRuleNr = pool->get_entry(1)->get_ruleNr();
    double paFitness = pool->get_entry(1)->get_fitness();
    paC = new Chromosome(paRuleNr, paFitness);
}

//---------------
void FillRandomPool(Pool* p) {
    int size = p->get_size();
    for (int i = 0; i < size; i += 1) p->put_entry(new Chromosome(GenRandRule()), i);
}

//---------------
// GenRandRule
//
// Generate a random MachineS rule.
//---------------
rulenr_t GenRandRule() {

    rulenr_t rr = 0;

    // For each of the 8 triad-state rule parts...
    for (int i = 0; i < 8; i += 1) {
        int leftAction = rn05(*pMersenne);
        int rightAction;

        // Disallow identical left- and right-actions (that would
        // create multi-edges).
        do {
            rightAction = rn05(*pMersenne);
        } while (rightAction == leftAction);

        // Shift in the rule part, encoded as a mixed-radix (6, 6, 2) number,
        // to develop the radix 72 (6*6*2) rule number.
        int nodeAction = rn05(*pMersenne) % 2;
        rr = (72 * rr) + ((leftAction * 6 + rightAction) * 2) + nodeAction;
    }
    return rr;
}

//---------------
void MutateAndCross(Chromosome* maC, Chromosome* paC, Chromosome*& c1C, Chromosome*& c2C) {
    c1C = new Chromosome(paC->get_ruleNr() + 100 /*test*/, paC->get_fitness());
    c2C = new Chromosome(maC->get_ruleNr() + 100 /*test*/, maC->get_fitness());
}

//---------------
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
    cmdOpt.journalName = "log";
    cmdOpt.snapName = "searcher_snapshot";
    cmdOpt.resumeFromName = "";

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

          case CO_JOURNAL:
            cmdOpt.journalName = optarg;
            break;

          case CO_SNAP:
            cmdOpt.snapName = optarg;
            break;

          case CO_RESUME:
            cmdOpt.resumeFromName = optarg;
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
double SimulateGeneration(int generationNr, Pool*& pool) {
    Pool* newPool = new Pool(pool->get_size());
    int newPoolSize = 0;
    assert(POOLSIZE % 4 == 0); // TODO: static_assert?

    while (newPoolSize < POOLSIZE) {
        Chromosome* maC;
        Chromosome* paC;

        // Choose 2 parents (copies) from pool, insert in new pool.
        ChooseParents(pool, maC, paC);
        newPool->put_entry(maC, newPoolSize++);
        newPool->put_entry(paC, newPoolSize++);

        // Create 2 children, insert in new pool.
        Chromosome* c1C;
        Chromosome* c2C;
        MutateAndCross(maC, paC, c1C, c2C);
        newPool->put_entry(c1C, newPoolSize++);
        newPool->put_entry(c2C, newPoolSize++);
    }
    delete pool;
    pool = newPool;
    return pool->AvgFitness();
}

//---------------
int main(const int argc, char* argv[]) {

    // Parse the command.
    ParseCommand(argc, argv);

    // Set Runner/Machine default parameters.
    Runner::SetDefaults(cmdOpt.nrNodes, cmdOpt.maxIterations, cmdOpt.cycleCheckDepth, "single-center", -1, "ring", 0);
    // -1 => (unused) tapePctBlack, 0 => noChangeTopo

    // Instantiate a seeded Mersenne random number generator.
    // Use seed 1 unless a seed was specified in the command.
    if (cmdOpt.randSeed == -1) cmdOpt.randSeed = 1;
    pMersenne = new std::mt19937((std::mt19937::result_type) cmdOpt.randSeed);

    // Open the journal.
    journal.open(cmdOpt.journalName + ".log", std::ios::app);
    if (!journal.is_open()) {
        std::cerr << "error: can't open the log file" << std::endl;
        exit(1);
    }
    journal << "start" << std::endl;

    // Prepare the snapshot file name.
    cmdOpt.snapName = cmdOpt.snapName + "_" + std::to_string(getpid()) + ".txt";

    // Create a random pool or read it if we're resuming from a file.
    Pool* pool = new Pool(POOLSIZE);
    if (cmdOpt.resumeFromName != "") {
        std::cerr << "Resuming using pool from: " << cmdOpt.resumeFromName << std::endl;
        journal << "Resuming using pool from: " << cmdOpt.resumeFromName << std::endl;
        if (!pool->Read(cmdOpt.resumeFromName)) {
            std::cerr << "error: unable to read pool from: " << cmdOpt.resumeFromName << std::endl;
            journal << "error: unable to read pool from: " << cmdOpt.resumeFromName << std::endl;
            journal.close();
            exit(1);
        }
    }
    else {
        FillRandomPool(pool);
    }
    /*PickList* pl = new PickList(pool);
    for (int ix = 0; ix < pl->get_basePool()->get_size(); ix += 1) {
        PickElt pe = pl->get_elt(ix);
        std::cout << pe.normFitness << " " << pe.cumFitness << std::endl;
    }*/

    // Simulate reproduction until...
    int generationNr = 0;
    double statistic = 0.0;
    while (generationNr < MAXGENERATIONS && statistic < 100000.0) { // TODO: Replace the test.
        assert(pool->Write(cmdOpt.snapName));
        statistic = SimulateGeneration(generationNr, pool); // Replaces pool
        generationNr += 1;
    }
    assert(pool->Write(cmdOpt.snapName));

    delete pool;
    /*
    delete pl;
    */
    journal << "stop" << std::endl;
    journal.close();

    std::cout << "finis." << std::endl;
    exit(0);

/*
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
*/

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
