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
#include "rule.h"
#include "runner.h"

#define VERSION "V190225.1"

// This program runs a genetic search for rules that, when run in an instance
// of automaton C, produce a fitness statistic that exceeds 'target-fitness'.
//
// In summary, it:
// (1) Generates 'pool-size' random rules, creating an initial rule (or "chromosome")
//  pool,
// (2) Repeatedly fills a new pool by choosing pairs of parent rules from
//  the existing pool using a process that is biased toward higher-fitness rules
//  to a degree tunable using 'cum-fitness-exp'.
// (3) Mutating each parent with probability 'prob-mutation'.
// (4) "Crossing" the parents by exchanging a randomly selected rule-subpart.
//
// - Duplicate rules are never inserted in the nascent pool.
// - Random number generation can be seeded with the --randseed <seed> command
// line option.
// - Each newly generated rule is recorded along with its fitness. Evolution
// stops when either 'max-generations' pools have been processed or the number
// of rules exceeding 'target-fitness' exceeds 'stop-after'.
//
// Results are recorded in files with the root name specified with command
// line option --record <rootname>:
//
// - <rootname>_s.json contains parameter values and summary statistics.
// - <rootname>_g.txt contains the rules and fitnesses for each generation.
// - <rootname>_rp.txt contains a list of all generated rules ("rulepath")
//  and their fitnesses.
//
// This program also accepts command line parameters that configure the operation
// of the automata:
// - --nodes
// - --cycle-check-depth
// - --max-iterations
//
// Notes:
// - Duplicate rules never appear in the same pool. They may, however, appear
// in different generations of the pools. This results in some double-counting
// in the running tally 'nrFitRules'.
//

#define NR_SUBPARTS (NR_TRIAD_STATES * 3)

//---------------
// Global structures
//
// Set up the Mersenne Twister random number generator.
// 'pMersenne' will point to a seeded instance of the generator.
// 'rn05' is an instantiated wrapper that yields uniform [0, 5] when called with
// a generator.
static std::mt19937* pMersenne;
static std::uniform_int_distribution<int> rn05(0, 5);

static std::ofstream journalFS;
static std::ofstream snapFS;
static std::ofstream sumFS;
static std::ofstream genFS;
static std::ofstream rulepathFS;
static std::string snapName;
static std::string sumName;
static std::string genName;
static std::string rulepathName;
static int nrFitRules = 0;

//---------------
// Procedures
//
static void ChooseParents(Pool*, PickList*, Chromosome*&, Chromosome*&);
static double Compactness(Pool*);
static void Cross(rulenr_t&, rulenr_t&);
static double Distance(Pool*, int, int);
static void FillRandomPool(Pool*);
static rulenr_t GenRandRule();
static void Mutate(rulenr_t&);
static void MutateAndCross(int, Chromosome*, Chromosome*, Chromosome*&, Chromosome*&);
static Chromosome* NoteNewRule(int, Chromosome*);
static void ParseCommand(const int, char**);
static void RecordPool(int, Pool*);
static double SimulateGeneration(int, Pool*&);
static int Uniform(int, int);


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
    int maxGenerations;
    int stopAfter;
    int probMutation;
    unsigned int cycleCheckDepth;
    bool rulePresent;
    double cumFitnessExp;
    double targetFitness;
    int poolSize;
    std::string machineTypeName;
    std::string rootName;
    std::string journalName;
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
#define CO_RESUME 1006
#define CO_MAXGENERATIONS 1007
#define CO_CUMFITNESSEXP 1008
#define CO_POOLSIZE 1009
#define CO_PROBMUTATION 1010
#define CO_ROOTNAME 1011
#define CO_STOPAFTER 1012
#define CO_TARGET 1013

static struct option long_options[MAX_COMMAND_OPTIONS] = {
    {"no-console", no_argument, &cmdOpt.noConsole, 1},
    {"extend-id", no_argument, &cmdOpt.extendId, 1},

    {"cycle-check-depth", required_argument, 0, CO_CYCLE_CHECK_DEPTH},
    {"max-iterations", required_argument, 0, 'i'},
    {"machine", required_argument, 0, 'm'},
    {"nodes", required_argument, 0, 'n'},
    {"noop", no_argument, 0, CO_NOOP},
    {"randseed", required_argument, 0, 'a'},
    {"max-generations", required_argument, 0, CO_MAXGENERATIONS},
    {"cum-fitness-exp", required_argument, 0, CO_CUMFITNESSEXP},
    {"pool-size", required_argument, 0, CO_POOLSIZE},
    {"log", required_argument, 0, CO_JOURNAL},
    {"resume-from", required_argument, 0, CO_RESUME},
    {"prob-mutation", required_argument, 0, CO_PROBMUTATION},
    {"record", required_argument, 0, CO_ROOTNAME},
    {"stop-after", required_argument, 0, CO_STOPAFTER},
    {"target-fitness", required_argument, 0, CO_TARGET},

    {"help", no_argument, 0, CO_HELP},
    {0, 0, 0, 0}
};

//---------------
char* strAllocCpy(const char* src) { return strcpy(new char[strlen(src) + 1], src); }

//---------------
void ChooseParents(Pool* pool, PickList* pl, Chromosome*& maC, Chromosome*& paC) {
    // Select a target total fitness value randomly from [0.0, 1.0].
    // Scan down the list until reaching a cumulative fitness
    // greater than the target; use this location as the end of
    // a range of rules from which to select parent rules randomly.
    //
    // The intent is to bias the selection process strongly, but
    // not wholly, toward fitter rules.
    double targetCumFitness = Uniform(1, 100) / 100.0;

    int ix = 0;
    for ( ; (ix < pool->get_capacity()) && (pl->get_elt(ix).cumFitness <= targetCumFitness); ix += 1) ;
    // Note that ix can end up "off the end."
    // Ensure that the range has at least 2 elements.
    if (ix < 2) ix = 2;

    // Choose two different parents from the selected range.
    int maIx, paIx;
    do {
        maIx = Uniform(0, ix-1);
        paIx = Uniform(0, ix-1);
    } while (pl->get_elt(maIx).c->get_ruleNr() == pl->get_elt(paIx).c->get_ruleNr());

    // Create and return the parent chromosomes.
    Chromosome* prevMaC = pl->get_elt(maIx).c;
    Chromosome* prevPaC = pl->get_elt(paIx).c;

    // TODO: Use an object copy.
    maC = new Chromosome(prevMaC->get_ruleNr(), prevMaC->get_fitness());
    paC = new Chromosome(prevPaC->get_ruleNr(), prevPaC->get_fitness());
}

//---------------
double Compactness(Pool* p) {
    double totInterDistance = 0.0;
    for (int i = 0; i < p->get_capacity(); i += 1) {
        for (int j = 0; j < i; j += 1) {
            totInterDistance += Distance(p, i, j);
        }
    }
    return totInterDistance / p->get_capacity();
}

//---------------
void Cross(rulenr_t& rn1, rulenr_t& rn2) {
    // Get the rules' sub-parts.
    Rule* r1 = new Rule(rn1);
    Rule* r2 = new Rule(rn2);
    int* r1subParts = r1->get_ruleSubParts();
    int* r2subParts = r2->get_ruleSubParts();

    // Ensure that the swap changes both (and that the loop termination condition holds).
    assert(rn1 != rn2);
    int ix = Uniform(0, NR_SUBPARTS-1);
    while (r1subParts[ix] == r2subParts[ix]) ix = (ix + 1) % NR_SUBPARTS;

    int tmp = r1subParts[ix];
    r1subParts[ix] = r2subParts[ix];
    r2subParts[ix] = tmp;

    Rule* newR1 = new Rule("subparts", r1subParts);
    Rule* newR2 = new Rule("subparts", r2subParts);
    rn1 = newR1->get_ruleNr();
    rn2 = newR2->get_ruleNr();

    delete r1subParts;
    delete r2subParts;
    delete r1;
    delete r2;
    delete newR1;
    delete newR2;
}

//---------------
double Distance(Pool* p, int i, int j) {
    Rule* iRule = new Rule(p->get_entry(i)->get_ruleNr());
    Rule* jRule = new Rule(p->get_entry(j)->get_ruleNr());
    int* iSubParts = iRule->get_ruleSubParts();
    int* jSubParts = jRule->get_ruleSubParts();

    double sumsq = 0.0;
    for (int px = 0; px < NR_SUBPARTS; px += 1)
        sumsq += pow(iSubParts[px] - jSubParts[px], 2.0);

    delete iSubParts;
    delete jSubParts;
    delete iRule;
    delete jRule;

    return pow(sumsq, 0.5);
}

//---------------
void FillRandomPool(Pool* p) {
    // Makes the shady assumption that no duplicates are generated.
    for (int i = 0; i < p->get_capacity(); i += 1) p->put_entry(new Chromosome(GenRandRule()), i);
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
void Mutate(rulenr_t& rn) {
    // Get the rule's sub-parts.
    Rule* origRule = new Rule(rn);
    int* subParts = origRule->get_ruleSubParts();

    // Randomly alter a randomly chosen sub-part.
    int spIx = Uniform(0, NR_SUBPARTS-1);
    if ((spIx % 3) == 2) { // If it's a node state [0, 1], invert it.
        subParts[spIx] = 1 - subParts[spIx];
    }
    else { // It's an edge destination specifier [0, 5], select a different one.
        int newDst;
        do {
            newDst = Uniform(0, 5);
        } while (newDst == subParts[spIx]);
        subParts[spIx] = newDst;
    }

    Rule* newRule = new Rule("subparts", subParts);
    rn = newRule->get_ruleNr();
    delete subParts;
    delete origRule;
    delete newRule;
}

//---------------
void MutateAndCross(int generationNr, Chromosome* maC, Chromosome* paC, Chromosome*& c1C, Chromosome*& c2C) {
    rulenr_t c1rn = maC->get_ruleNr();
    rulenr_t c2rn = paC->get_ruleNr();
    rulenr_t orig1rn = c1rn;
    rulenr_t orig2rn = c2rn;

    // Possibly mutate exactly one member of the pair.
    assert(c1rn != c2rn);
    if (Uniform(0, 99) < cmdOpt.probMutation) {
        do Mutate(c1rn); while(Uniform(0, 99) < cmdOpt.probMutation);
    }
    else if (Uniform(0, 99) < cmdOpt.probMutation) {
        do Mutate(c2rn); while(Uniform(0, 99) < cmdOpt.probMutation);
    }

    // Undo if a mutation left the rules equal.
    if (c1rn == c2rn) {
        c1rn = orig1rn;
        c2rn = orig2rn;
    }
    assert(c1rn != c2rn);

    // Pick a rule-part and exchange it, changing both arguments.
    Cross(c1rn, c2rn);

    c1C = new Chromosome(c1rn, -1); // -1 => absent fitness
    c2C = new Chromosome(c2rn, -1);
}

//---------------
Chromosome* NoteNewRule(int generationNr, Chromosome* c) {
    rulepathFS << generationNr << " " << c->get_ruleNr() << " " << c->get_fitness() << std::endl;
    if (c->get_fitness() > cmdOpt.targetFitness) nrFitRules += 1;
    return c;
}

//---------------
void RecordPool(int generationNr, Pool* p) {
    for (int ix = 0; ix < p->get_capacity(); ix += 1)
        genFS << generationNr << " "
          << p->get_entry(ix)->get_ruleNr() << " "
          << p->get_entry(ix)->get_fitness() << std::endl;
}

//---------------
double SimulateGeneration(int generationNr, Pool*& pool) {
    // Create a "pick list" of all rules in the pool, arranged from
    // most to least fit, with each accompanied by a cumulative (from
    // the top of the list) fitness figure. A rule's associated cumu-
    // lative fitness represents, in a sense, the total fitness of
    // the rule and all more-fit rules. Fitness values are normalized.
    PickList* pickList = new PickList(pool, cmdOpt.cumFitnessExp);

    // Create and fill the next generation pool.
    Pool* newPool = new Pool(pool->get_capacity());
    int newPoolNrElts = 0;
    while (newPoolNrElts < cmdOpt.poolSize) {
        Chromosome* maC;
        Chromosome* paC;

        // Choose 2 distinct parents (copies thereof) from the existing pool.
        ChooseParents(pool, pickList, maC, paC);
        //assert(maC->get_ruleNr() != paC->get_ruleNr());

        // Create 2 children. If they're not already in the new pool, insert
        // them and record them in the rulepath.
        Chromosome* c1C;
        Chromosome* c2C;
        MutateAndCross(generationNr, maC, paC, c1C, c2C);

        if (newPool->Contains(c1C->get_ruleNr()))
            delete c1C;
        else {
            newPool->put_entry(c1C, newPoolNrElts);
            NoteNewRule(generationNr, c1C);
            if (++newPoolNrElts == cmdOpt.poolSize) {
                delete c2C;
                break;
            }
        }

        if (newPool->Contains(c2C->get_ruleNr()))
            delete c2C;
        else {
            newPool->put_entry(c2C, newPoolNrElts);
            NoteNewRule(generationNr, c2C);
            if (++newPoolNrElts == cmdOpt.poolSize) break;
        }

        // Insert the parents in the new pool if they're not already in.
        // (note that if the kids fill it, the parents don't make it)
        if (newPool->Contains(maC->get_ruleNr()))
            delete maC;
        else {
            newPool->put_entry(maC, newPoolNrElts);
            if (++newPoolNrElts == cmdOpt.poolSize) {
                delete paC;
                break;
            }
        }

        if (newPool->Contains(paC->get_ruleNr()))
            delete paC;
        else {
            newPool->put_entry(paC, newPoolNrElts);
            ++newPoolNrElts;
        }
    }
    delete pool;
    delete pickList;
    pool = newPool;
    return pool->AvgFitness();
}

//---------------
int Uniform(int lo, int hi) {
    assert(0 <= lo && lo <= hi);
    if (hi == 0 && lo == 0) return 0;
    return lo + (rand() % (hi - lo + 1));
}

//---------------
void ParseCommand(const int argc, char* argv[]) {
    int c;
    bool errorFound = false;

    // Set command options to default values.
    cmdOpt.ruleNr = 641;
    cmdOpt.maxIterations = 1031;
    cmdOpt.randSeed = 1;
    cmdOpt.noConsole = 0;
    cmdOpt.extendId = 0;
    cmdOpt.nrNodes = 1031;
    cmdOpt.maxGenerations = 100;
    cmdOpt.cycleCheckDepth = 16;
    cmdOpt.rulePresent = false;
    cmdOpt.cumFitnessExp = 1.0;
    cmdOpt.poolSize = 40;
    cmdOpt.stopAfter = 0;
    cmdOpt.targetFitness = 1.0;
    cmdOpt.probMutation = 20;
    cmdOpt.machineTypeName = "C";
    cmdOpt.journalName = "searcher";
    cmdOpt.rootName = "";
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

          case CO_MAXGENERATIONS:
            cmdOpt.maxGenerations = atoi(optarg);
            break;

          case CO_CUMFITNESSEXP:
            cmdOpt.cumFitnessExp = atof(optarg);
            if (cmdOpt.cumFitnessExp <= 0)
                std::cerr << "warning: nonpositive exponent" << std::endl;
            break;

          case CO_POOLSIZE:
            cmdOpt.poolSize = atoi(optarg);
            if (cmdOpt.poolSize < 2) {
                std::cerr << "error: pool-size must be >= 2" << std::endl;
                errorFound = true;
            }
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

          case CO_PROBMUTATION:
            cmdOpt.probMutation = atoi(optarg);
            if (cmdOpt.probMutation < 0 || cmdOpt.probMutation > 100) {
                std::cerr << "error: 0 <= prob-mutation <= 100" << std::endl;
                exit(1);
            }
            break;

          case CO_ROOTNAME:
            cmdOpt.rootName = optarg;
            break;

          case CO_STOPAFTER:
            cmdOpt.stopAfter = atoi(optarg);
            break;

          case CO_TARGET:
            cmdOpt.targetFitness = atof(optarg);
            if (cmdOpt.targetFitness < 0.0 || cmdOpt.targetFitness > 1.00) {
                std::cerr << "error: 0.0 <= target-fitness <= 1.0" << std::endl;
                exit(1);
            }
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

    // Seed the simpler random number generator.
    srand(cmdOpt.randSeed);

    // Create output file names.
    if (cmdOpt.rootName == "") {
        std::cerr << "error: --record <root file name> must be specified" << std::endl;
        errorFound = true;
    }
    snapName = cmdOpt.rootName + "_snap.txt";
    sumName = cmdOpt.rootName + "_s.json";
    genName = cmdOpt.rootName + "_g.txt";
    rulepathName = cmdOpt.rootName + "_r.txt";

    // Check option consistency.
    if (errorFound) exit(1);

    // Warn if any non-option command arguments are present.
    if (optind < argc) {
        std::cerr << "warning: there are extraneous command arguments" << std::endl;
        // TODO: Direct the following to cerr.
        while (optind < argc) printf ("%s ", argv[optind++]);
        putchar ('\n');
    }
}

//---------------
int main(const int argc, char* argv[]) {

    // Parse the command.
    ParseCommand(argc, argv);

    // Set Runner/Machine default parameters.
    Runner::SetDefaults(cmdOpt.nrNodes, cmdOpt.maxIterations, cmdOpt.cycleCheckDepth, "single-center", -1, "ring", 0);
    // -1 => (unused) tapePctBlack, 0 => noChangeTopo

    // Instantiate a seeded Mersenne random number generator.
    pMersenne = new std::mt19937((std::mt19937::result_type) cmdOpt.randSeed);

    // Open the journal and log parameters.
    journalFS.open(cmdOpt.journalName + ".log", std::ios::app);
    if (!journalFS.is_open()) {
        std::cerr << "error: can't open the log file" << std::endl;
        exit(1);
    }
    // Open the output files.
    sumFS.open(sumName, std::ios::app);
    if (!sumFS.is_open()) {
        std::cerr << "error: can't open the summary file " << sumName << std::endl;
        exit(1);
    }
    genFS.open(genName, std::ios::trunc);
    if (!genFS.is_open()) {
        std::cerr << "error: can't open the generation file " << genName << std::endl;
        exit(1);
    }
    rulepathFS.open(rulepathName, std::ios::trunc);
    if (!rulepathFS.is_open()) {
        std::cerr << "error: can't open the rulepath file " << rulepathName << std::endl;
        exit(1);
    }

    // Create a random pool or read it if we're resuming from a file.
    Pool* pool = new Pool(cmdOpt.poolSize);
    if (cmdOpt.resumeFromName != "") {
        std::cerr << "info: resuming using pool from: " << cmdOpt.resumeFromName << std::endl;
        journalFS << "info: resuming using pool from: " << cmdOpt.resumeFromName << std::endl;
        if (!pool->Read(cmdOpt.resumeFromName)) {
            std::cerr << "error: unable to read pool from: " << cmdOpt.resumeFromName << std::endl;
            journalFS << "error: unable to read pool from: " << cmdOpt.resumeFromName << std::endl;
            journalFS.close();
            exit(1);
        }
    }
    else {
        FillRandomPool(pool);
    }

    // Write the initial rulepath entries.
    for (int ix = 0; ix < pool->get_capacity(); ix += 1) NoteNewRule(0, pool->get_entry(ix));

    // Write summary information.
    sumFS << "info: starting, poolcapacity: "
      << cmdOpt.poolSize << " randseed: "
      << cmdOpt.randSeed << " snapshot: "
      << snapName << " max-generations: " << cmdOpt.maxGenerations
      << std::endl;
    sumFS.close();

    // Simulate reproduction until reaching maxGenerations or nrFitRules > stopAfter.
    // Write a snapshot of the pool state before each generation advance.
    int generationNr = 0;
    double statistic = pool->AvgFitness();
    std::cerr << "gen avgFit maxFit compact cumFitRules" << std::endl;
    do {
        std::cerr << generationNr << " " << statistic << " " << pool->MaxFitness()
          << " " << Compactness(pool) << " " << nrFitRules << std::endl;
        journalFS << generationNr << " " << statistic << " " << pool->MaxFitness()
          << " " << Compactness(pool) << " " << nrFitRules << std::endl;
        assert(pool->Write(snapName));
        RecordPool(generationNr, pool);
        statistic = SimulateGeneration(generationNr, pool); // Replaces pool
        generationNr += 1;
    } while (generationNr < cmdOpt.maxGenerations
            && (cmdOpt.stopAfter == 0 || nrFitRules <= cmdOpt.stopAfter));

    // Snapshot and record/report the final pool state.
    assert(pool->Write(snapName));
    RecordPool(generationNr, pool);
    std::cerr << generationNr << " " << statistic << " " << pool->MaxFitness() << std::endl;
    journalFS << generationNr << " " << statistic << " " << pool->MaxFitness() << std::endl;

    delete pool;
    genFS.close();
    rulepathFS.close();
    journalFS << "info: stopping" << std::endl;
    journalFS.close();
    exit(0);
}
