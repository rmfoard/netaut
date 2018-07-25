#include <assert.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include "rule.h"

#define VERSION "V180614.0"


// Set up the Mersenne Twister random number generator.
// 'pMersenne' will point to a seeded instantiation of the generator.
// 'rn05' is an instantiated wrapper that yields uniform [0, 5] when called with
// a generator.
static std::mt19937* pMersenne = nullptr;
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
        {"genrand", required_argument, 0, 's'},
        {"reserve", required_argument, 0, 'e'},
        {"cache", required_argument, 0, 'c'},
        {"reset", no_argument, 0, 't'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int c;
    bool errorFound = false;
    bool resetting = false;
    while (true) {

        int option_index = 0;
        c = getopt_long (argc, argv, "a:r:s:e:c:t:h:",
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

          case 's': // --genrand <numeric_seed>
            cmdOpt.randSeed = atoi(optarg);
            break;

          case 'e': // --reserve <cache_size>
            cmdOpt.reserve = atoi(optarg);
            break;

          case 't': // --reset
            resetting = true;
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
    if (errorFound) exit(1);

    // Check option consistency.
    if (cmdOpt.cacheFile == ""
      && (resetting || cmdOpt.reserve > 0)) {
        std::cerr << "error: --reserve and --reset require --cache" << std::endl;
        exit(1);
    }

    if ((cmdOpt.reserve > 0 || resetting) && cmdOpt.randSeed == -1) {
        std::cerr << "error: --reserve and --reset require --genrand" << std::endl;
        exit(1);
    }

    if (resetting) {
        // Remove the cache file and its companion marker file.
        int s1 = std::remove(cmdOpt.cacheFile.c_str());
        int s2 = std::remove((cmdOpt.cacheFile + ".mkr").c_str());
        if (s1 != 0 || s2 != 0)
            std::cerr << "warning: all cache file deletions were not successful" << std::endl;
        exit(0);
    }

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

    // Instantiate a seeded Mersenne random number generator if necessary.
    if (!pMersenne) {
        if (cmdOpt.randSeed == -1) {
            std::cerr << "error: --genrand <integer> must be in the command" << std::endl;
            exit(1);
        }
        pMersenne = new std::mt19937((std::mt19937::result_type) cmdOpt.randSeed);
    }

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
static
rulenr_t ProcessCacheFile() {

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
        for (int i = 0; i < cmdOpt.reserve; i += 1)
            cfileOut << GenRandRule() << std::endl;

        // Close it, then reopen it for downstream use.
        cfileOut.close();
        cfileIn.open(cmdOpt.cacheFile, std::ios::in);

        // Mark position zero.
        std::ofstream mfstreamOut;
        mfstreamOut.open(cmdOpt.cacheFile + ".mkr", std::ios::out);
        mfstreamOut << "0" << std::endl;
        mfstreamOut.close();
    }

    // Open the marker file. If it does not exist, this must be the first use of
    // an existing cache file, so create and open a marker file at position zero.
    std::ifstream mfstreamIn;
    mfstreamIn.open(cmdOpt.cacheFile + ".mkr", std::ios::in);
    if (!mfstreamIn.is_open()) {
        std::ofstream mfstreamOut;
        mfstreamOut.open(cmdOpt.cacheFile + ".mkr", std::ios::out);
        mfstreamOut << "0" << std::endl;
        mfstreamOut.close();
        mfstreamIn.open(cmdOpt.cacheFile + ".mkr", std::ios::in);
    }

    // Read the marker to learn the current position in the cache file.
    unsigned int streamPos;
    mfstreamIn >> streamPos;
    mfstreamIn.close();

    // Note the cache's end position.
    cfileIn.seekg(0, cfileIn.end);
    unsigned int endPos = cfileIn.tellg();

    // Position to and read the next entry in the cache
    // unless we're at the end.
    assert(streamPos < endPos);
    if (streamPos  == endPos - 1) throw std::runtime_error("rule number cache exhausted");
    cfileIn.seekg(streamPos, cfileIn.beg);
    rulenr_t nextRuleNr;
    cfileIn >> nextRuleNr;

    // If we've just read the last entry, delete the marker file.
    streamPos = cfileIn.tellg();
    if (streamPos == endPos - 1) {
        std::remove((cmdOpt.cacheFile + ".mkr").c_str());
    }
    else { // Otherwise, replace the marker with the new file position.
        // Replace the marker with the new file position.
        std::ofstream  mfstreamOut;
        mfstreamOut.open(cmdOpt.cacheFile + ".mkr", std::ios::out);
        streamPos = cfileIn.tellg();
        mfstreamOut << std::to_string(streamPos) << std::endl;
        mfstreamOut.close();
        cfileIn.close();
    }

    return nextRuleNr;
}

//---------------
int main(const int argc, char* argv[]) {

    ParseCommand(argc, argv);

    // Process filter specification files if they're present.
    if (cmdOpt.acceptFile != "")
        ; //printf("process acceptFile: %s\n", cmdOpt.acceptFile.c_str());
    if (cmdOpt.rejectFile != std::string(""))
        ; //printf("process rejectFile: %s\n", cmdOpt.rejectFile.c_str());

    // Generate and/or draw from a cache file if specified.
    if (cmdOpt.cacheFile != "")
        std::cout << ProcessCacheFile();
    else
        std::cout << GenRandRule();

    exit(0);
}
