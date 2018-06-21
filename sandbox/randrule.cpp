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


// Set up the Mersenne Twister random number generator.
// 'pMersenne' will point to a seeded instantiation of the generator.
// 'rn05' is an instantiated wrapper that yields uniform [0, 5] when called with
// a generator.
static std::mt19937* pMersenne;
static std::uniform_int_distribution<int> rn05(0, 5);

//---------------
struct CommandOpts {
    int randSeed;
    int reserve;
    std::string acceptFile;
    std::string rejectFile;
    std::string cacheFile;
};

static CommandOpts cmdOpt;

//---------------
// TODO: Learn where the hell 'optind' came from.
static
void ParseCommand(const int argc, char* argv[]) {

    // Set options to default values.
    cmdOpt.randSeed = -1;
    cmdOpt.reserve = 0;
    cmdOpt.cacheFile = "";
    cmdOpt.acceptFile = "";
    cmdOpt.rejectFile = "";

    static struct option long_options[] = {
        // Boolean options, e.g.,
        //{"option-name", no_argument, &cmdOpt.optionVarName, 1},

        // Other options
        {"accept", required_argument, 0, 'a'},
        {"reject", required_argument, 0, 'r'},
        {"randseed", required_argument, 0, 's'},
        {"reserve", required_argument, 0, 'e'},
        {"cache", required_argument, 0, 'c'},
        {"reset", no_argument, 0, 't'},
        {"help", no_argument, 0, 'h'},
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

          case 'a': // --accept <filename>
            cmdOpt.acceptFile = std::string(optarg);
            break;

          case 'r': // --reject <filename>
            cmdOpt.rejectFile = std::string(optarg);
            break;

          case 'c': // --cache <filename>
            cmdOpt.cacheFile = std::string(optarg);
            break;

          case 's': // --randseed <numeric_seed>
            // Instantiate the seeded random number generator.
            cmdOpt.randSeed = atoi(optarg);
            pMersenne = new std::mt19937((std::mt19937::result_type) cmdOpt.randSeed);
            break;

          case 'e': // --reserve <cache_size>
            cmdOpt.reserve = atoi(optarg);
            break;

          case 't': // --reset
            // Eradicate the cache
            break;

          case 'h': // --help
            // Show help.
            printf("Command options:\n");
            for (auto entry : long_options) if (entry.name != NULL) {
                printf("  --%s", entry.name);
                if (entry.has_arg) printf(" <value>");
                printf("\n");
            }
            exit(0);

          case '?':
            errorFound = true;
            break;

          default:
            abort();
       }
    }

    // Check option consistency.
    if ((cmdOpt.cacheFile != "" && cmdOpt.reserve == 0)
      || (cmdOpt.cacheFile == "" && cmdOpt.reserve > 0)) {
        std::cerr << "error: --reserve and --cacheFile must both be specified" << std::endl;
        errorFound = true;
    }

    if (errorFound) exit(1);

    // Warn if any non-option command arguments are present.
    if (optind < argc) {
        std::cerr << "warning: there are extraneous command arguments: " << std::endl;
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
// ProcessCacheFile
//
// Draw next entry from the cache file, first generating it if necessary.
//---------------
void ProcessCacheFile() {

    // Create and fill the cache file if we're just starting.
    std::ifstream cfileIn;
    cfileIn.open(cmdOpt.cacheFile, std::ios::in);
    if (!cfileIn.is_open()) { // if there is no cache file
        std::ofstream cfileOut;
        cfileOut.open(cmdOpt.cacheFile, std::ios::out);
        if (!cfileOut.is_open()) {
            std::cerr << "error: can't create cache file" << std::endl;
            exit(1);
        }

        // Fill the cache file.
        for (int i = 0; i < cmdOpt.reserve; i += 1) {
            cfileOut << GenRandRule() << std::endl;
        }
        cfileOut.close();

        // Mark position zero.
        std::ofstream mfstream;
        mfstream.open(cmdOpt.cacheFile + ".mkr", std::ios::out);
        if (!mfstream.is_open()) {
            std::cerr << "error: can't create marker file" << std::endl;
            exit(1);
        }
        mfstream << "0" << std::endl;
        mfstream.close();
    }

    // Read the marker to learn the current position in the cache file.
    std::ifstream mfstream;
    mfstream.open(cmdOpt.cacheFile + ".mkr", std::ios::in);
    if (!mfstream.is_open()) {
        std::cerr << "error: can't open marker file" << std::endl;
        exit(1);
    }
    unsigned int pos << mfstream;

    // Open the cache, set the position, and read the next entry.
    cfileIn.open(cmdOpt.cacheFile, std::ios::in);
    if (!cfileIn.is_open()) {
        std::cerr << "error: can't open cache file" << std::endl;
        exit(1);
    }

    cfileIn.close();
}

//---------------
int main(const int argc, char* argv[]) {

    ParseCommand(argc, argv);

    // Process filter specification files if they're present.
    if (cmdOpt.acceptFile != "")
        ; //printf("process acceptFile: %s\n", cmdOpt.acceptFile.c_str());
    if (cmdOpt.rejectFile != std::string(""))
        ; //printf("process rejectFile: %s\n", cmdOpt.rejectFile.c_str());

    // Generate and draw from a cache file if specified.
    if (cmdOpt.cacheFile != "") { // if a cache file is specified
        ProcessCacheFile();
        // if it does not exist
            // create and fill it, set position to the beginning
        // endif
        // read the next position
        // if it's not at the end
            // read the next entry
            // advance and record the position
            // return the next entry
            // exit(0)
        // else
            // show a warning (and fall to generate a "singleton")
        // endif
    }
    
    //std::cout << GenRandRule();
    exit(0);
}
