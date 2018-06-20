#include <assert.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <algorithm>
#include <fstream>
#include <functional>
#include <iostream>
#include <random>
#include <string>
#include "rule.h"

#define VERSION "V180614.0"


// TODO: Add --help
//---------------
struct CommandOpts {
    int randSeed;
    std::string acceptFile;
    std::string rejectFile;
};

static CommandOpts cmdOpt;

//---------------
// TODO: Learn where the hell 'optind' came from.
static
void ParseCommand(const int argc, char* argv[]) {

    // Set options to default values.
    cmdOpt.randSeed = -1;
    cmdOpt.acceptFile = std::string("");
    cmdOpt.rejectFile = std::string("");

    static struct option long_options[] = {
        // Boolean options, e.g.,
        //{"option-name", no_argument, &cmdOpt.optionVarName, 1},

        // Other options
        {"accept", required_argument, 0, 'a'},
        {"reject", required_argument, 0, 'r'},
        {"randseed", required_argument, 0, 's'},
        {0, 0, 0, 0}
    };

    int c;
    bool errorFound = false;
    while (true) {

        int option_index = 0;
        c = getopt_long (argc, argv, "a:r:",
          long_options, &option_index);

        if (c == -1) // end of options?
            break;

        switch (c) {
          case 0: // flag setting only, no further processing required
            if (long_options[option_index].flag != 0)
                break;
            assert(false);

          case 'a':
            cmdOpt.acceptFile = std::string(optarg);
            break;

          case 'r':
            cmdOpt.rejectFile = std::string(optarg);
            break;

          case 's':
            cmdOpt.randSeed = atoi(optarg);
            break;

          case '?':
            errorFound = true;
            break;

          default:
            abort();
       }
    }

    // Check option consistency.
    if (errorFound) exit(1);

    // Warn if any non-option command arguments are present.
    if (optind < argc) {
        printf ("warning: there are extraneous command arguments: ");
        while (optind < argc) printf ("%s ", argv[optind++]);
        putchar ('\n');
    }
}

//---------------
// GenRandRule
//
// Generate a random MachineS rule.
//---------------
static
rulenr_t GenRandRule() {

    // Set up the Mersenne Twister random number generator.
    std::mt19937::result_type seed = cmdOpt.randSeed;
    auto r6 = std::bind(std::uniform_int_distribution<int>(0, 5), std::mt19937(seed));

    rulenr_t rr = 0;

    // For each of the 8 triad-state rule parts...
    for (int i = 0; i < 8; i += 1) {
        int leftAction = r6();
        int rightAction;

        // Disallow identical left- and right-actions (that would
        // create multi-edges).
        do {
            rightAction = r6();
        } while (rightAction == leftAction);

        // Shift in the rule part, encoded as a mixed-radix (6, 6, 2) number,
        // to develop the radix 72 (6*6*2) rule number.
        int nodeAction = r6() % 2;
        rr = (72 * rr) + ((leftAction * 6 + rightAction) * 2) + nodeAction;
    }
    return rr;
}

//---------------
int main(const int argc, char* argv[]) {

    ParseCommand(argc, argv);

    if (cmdOpt.acceptFile != "")
        ; //printf("process acceptFile: %s\n", cmdOpt.acceptFile.c_str());
    if (cmdOpt.rejectFile != std::string(""))
        ; //printf("process rejectFile: %s\n", cmdOpt.rejectFile.c_str());
    std::cout << GenRandRule();
    exit(0);
}
