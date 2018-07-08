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

static std::mt19937::result_type seed = 641;
static auto r6 = std::bind(std::uniform_int_distribution<int>(0, 5), std::mt19937(seed));

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
int main() {

    for (int i = 1; i <= 10000; i++) std::cout << GenRandRule() << std::endl;
    exit(0);
}
