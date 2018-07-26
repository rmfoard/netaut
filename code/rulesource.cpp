#include <assert.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <fstream>
#include <iostream>
#include <string>
#include "rule.h"

#define VERSION "V180726.0"

//---------------
struct CommandOpts {
    std::string acceptFile;
    std::string rejectFile;
    std::string sourceFile;
};

static CommandOpts cmdOpt;

static rulenr_t nextRuleNr;
static int nextRandSeed;

//---------------
static
void ParseCommand(const int argc, char* argv[]) {

    // Set options to default values.
    cmdOpt.sourceFile = "";
    cmdOpt.acceptFile = "";
    cmdOpt.rejectFile = "";

    static struct option long_options[] = {
        // Boolean options, e.g.,
        //{"option-name", no_argument, &cmdOpt.optionVarName, 1},

        // Other options
        {"accept", required_argument, 0, 'a'},
        {"reject", required_argument, 0, 'r'},
        {"source", required_argument, 0, 's'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int c;
    bool errorFound = false;
    while (true) {

        int option_index = 0;
        c = getopt_long (argc, argv, "a:r:s:h:",
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

          case 's': // --source <filename>
            cmdOpt.sourceFile = std::string(optarg);
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
    if (cmdOpt.sourceFile == "") {
        std::cerr << "error: --source <filename> is required" << std::endl;
        exit(1);
    }

    if (optind < argc) {
        std::cerr << "warning: there are extraneous command arguments: " << std::endl;
        while (optind < argc) printf ("%s ", argv[optind++]);
        putchar ('\n');
    }
}

//---------------
// ProcessSourceFile
//
// Draw the next entry from the source file.
//---------------
static
void ProcessSourceFile() {

    // Open the source file.
    std::ifstream sfileIn;
    sfileIn.open(cmdOpt.sourceFile, std::ios::in);
    if (!sfileIn.is_open()) {
        std::cerr << "error: can't open source file" << std::endl;
        exit(1);
    }

    // Open the marker file. If it does not exist, create it and
    // mark position zero.
    std::ifstream mfstreamIn;
    mfstreamIn.open(cmdOpt.sourceFile + ".mkr", std::ios::in);
    if (!mfstreamIn.is_open()) {
        std::cerr << "advice: rewinding marker file" << std::endl;
        std::ofstream mfstreamOut;
        mfstreamOut.open(cmdOpt.sourceFile + ".mkr", std::ios::out);
        mfstreamOut << "0" << std::endl;
        mfstreamOut.close();
        mfstreamIn.open(cmdOpt.sourceFile + ".mkr", std::ios::in);
    }

    // Read the marker to learn the current position in the source file.
    unsigned int streamPos;
    mfstreamIn >> streamPos;
    mfstreamIn.close();

    // Note the source file's end position.
    sfileIn.seekg(0, sfileIn.end);
    unsigned int endPos = sfileIn.tellg();

    // Position to and read the next entry in the source file
    // unless we're at the end.
    assert(streamPos < endPos);
    if (streamPos  == endPos - 1) throw std::runtime_error("source file exhausted");
    sfileIn.seekg(streamPos, sfileIn.beg);

    // Read values into globals.
    sfileIn >> nextRuleNr;
    sfileIn >> nextRandSeed;

    // If we've just read the last entry, delete the marker file.
    streamPos = sfileIn.tellg();
    if (streamPos == endPos - 1) {
        std::remove((cmdOpt.sourceFile + ".mkr").c_str());
    }
    else { // Otherwise, replace the marker with the new file position.
        std::ofstream  mfstreamOut;
        mfstreamOut.open(cmdOpt.sourceFile + ".mkr", std::ios::out);
        streamPos = sfileIn.tellg();
        mfstreamOut << std::to_string(streamPos) << std::endl;
        mfstreamOut.close();
    }
    sfileIn.close();
}

//---------------
int main(const int argc, char* argv[]) {

    ParseCommand(argc, argv);

    // Process filter specification files if they're present.
    if (cmdOpt.acceptFile != "")
        ; //printf("process acceptFile: %s\n", cmdOpt.acceptFile.c_str());
    if (cmdOpt.rejectFile != std::string(""))
        ; //printf("process rejectFile: %s\n", cmdOpt.rejectFile.c_str());

    // Read and output the next entries from the source file..
    ProcessSourceFile();
    std::cout << "--rule " << nextRuleNr << " --randseed " << nextRandSeed;

    exit(0);
}
