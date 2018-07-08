#include <assert.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include "rule.h"

#define VERSION "V180615.0"

// TODO: Add --help
//---------------
struct CommandOpts {
    int number;
    int text;
    int parts;
    int map;
    rulenr_t ruleNr;
    char* ruleText;
};

static CommandOpts cmdOpt;

//---------------
char* strAllocCpy(const char* src) { return strcpy(new char[strlen(src) + 1], src); }

//---------------
static
void ParseCommand(const int argc, char* argv[]) {

    // Set options to default values.
    cmdOpt.number = 0;
    cmdOpt.text = 0;
    cmdOpt.parts = 0;
    cmdOpt.map = 0;
    cmdOpt.ruleNr = 0;
    cmdOpt.ruleText = nullptr;

    static struct option long_options[] = {
        // Boolean options, e.g.,
        //{"option-name", no_argument, &cmdOpt.optionVarName, 1},
        {"number", no_argument, &cmdOpt.number, 1},
        {"text", no_argument, &cmdOpt.text, 1},
        {"parts", no_argument, &cmdOpt.parts, 1},
        {"map", no_argument, &cmdOpt.map, 1},
        {0, 0, 0, 0}
    };

    int c;
    bool errorFound = false;
    while (true) {

        int option_index = 0;
        c = getopt_long (argc, argv, "", long_options, &option_index);

        if (c == -1) // end of options?
            break;

        switch (c) {
          case 0: // flag setting only, no further processing required
            if (long_options[option_index].flag != 0)
                break;
            assert(false);

          case '?':
            errorFound = true;
            break;

          default:
            abort();
       }
    }

    // Check option consistency.
    if (cmdOpt.number + cmdOpt.text + cmdOpt.parts + cmdOpt.map != 1) {
        printf("error: please choose exactly one output type from:\n");
        printf("  --number\n");
        printf("  --text\n");
        printf("  --parts\n");
        printf("  --map\n");
        errorFound = true;
    }

    if (errorFound) exit(1);

    // Warn if any non-option command arguments are present.
    if (optind != argc - 1) {
        printf ("error: please provide a [single] input value to be converted\n");
        exit(1);
    }

    // Collect either a rule number or rule text from the command line.
    char telltale = argv[optind][0];
    if ('0' <= telltale && telltale <= '9') {
        char* endPtr;
        cmdOpt.ruleNr = strtoumax(argv[optind], &endPtr, 10); // radix 10
        if (cmdOpt.ruleNr == 0) {
            // TODO: invoke the rule number checker
            printf("error: invalid rule number\n");
            exit(1);
        }
    }
    else
        cmdOpt.ruleText = strAllocCpy(argv[optind]);
}

//---------------
int main(const int argc, char* argv[]) {

    ParseCommand(argc, argv);

    // If rule text was provided, compile it to a rule number.
    // In any case, establish a rule object in 'r'.
    Rule* r;
    if (cmdOpt.ruleText != nullptr) {
        r = new Rule(cmdOpt.ruleText);
        cmdOpt.ruleNr = r->get_ruleNr();
    }
    else
        r = new Rule(cmdOpt.ruleNr);

    // Do the selected conversion.
    if (cmdOpt.number)
        std::cout << cmdOpt.ruleNr;

    else if (cmdOpt.text)
        std::cout << r->get_ruleText();

    else if (cmdOpt.parts) {
        const int* ruleParts = r->get_ruleParts();
        for (int i = 0; i < NR_TRIAD_STATES; i += 1) {
            std::cout << ruleParts[i];
            if (i < NR_TRIAD_STATES - 1) std::cout << " ";
        }
        delete ruleParts;
    }

    else
        std::cout << "apology: conversion to a map is not yet implemented";

    delete r;
    exit(0);
}
