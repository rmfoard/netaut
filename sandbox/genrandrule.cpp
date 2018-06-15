#include <assert.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <algorithm>
#include <fstream>
#include <iostream>
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
        //{"convert-only", no_argument, &cmdOpt.convertOnly, 1},

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
static
rulenr_t GenRandRule() {
    rulenr_t rr = 0;
    for (int i = 0; i < 8; i += 1) {
        int lAction = rand() % 6;
        int rAction;
        do {
            rAction = rand() % 6;
        } while (rAction == lAction);
        rr = (72 * rr) + ((6 * lAction + rAction) * 2) + rand() % 2;
    }
    return rr;
}

//---------------
int main(const int argc, char* argv[]) {

    ParseCommand(argc, argv);

    // Do it.
    srand(cmdOpt.randSeed);
    if (cmdOpt.acceptFile != std::string(""))
        ; //printf("process acceptFile: %s\n", cmdOpt.acceptFile.c_str());
    if (cmdOpt.rejectFile != std::string(""))
        ; //printf("process rejectFile: %s\n", cmdOpt.rejectFile.c_str());
    std::cout << std::to_string(GenRandRule());
    exit(0);
}

/* Reference:
int main () {
  string line;
  ifstream myfile ("example.txt");
  if (myfile.is_open())
  {
      while ( getline (myfile,line) )
      {
          cout << line << '\n';
      }
      myfile.close();
  }

  else cout << "Unable to open file"; 

  return 0;
}
*/
