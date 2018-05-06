//#include "Snap.h"
#include <assert.h>
#include <stdio.h>
#include <vector>

int Raise(const int base, const int exponent) {
    int result = 1;
    for (int i = 0; i < exponent; i += 1) result *= base;

    return result;
}

int MachineNr(const int nrStates, const int nrActions, const std::vector<int> rules) {
    int machineNr = 0;
    for (int ruleNr = 0; ruleNr < nrStates; ruleNr += 1) {
        assert(rules[ruleNr] < nrActions);
        machineNr += rules[ruleNr] * Raise(nrActions, ruleNr);
    }

    return machineNr;
}

std::vector<int>* Rules(const int nrStates, const int nrActions, const int machineNr) {
    std::vector<int>* rules = new std::vector<int>(nrStates);
    for (int state = 0, residue = machineNr; state < nrStates; state += 1) {
        (*rules)[state] = residue % nrActions;
        residue /= nrActions;
        assert(state < nrStates - 1 || residue == 0);
    }

    return rules;
}

int main(const int argc, const char* argv[]) {
    std::vector<int> rules = { 1, 3, 5, 7, 0, 11, 9, 2  };

    int machineNr = MachineNr(8, 20, rules);
    printf("machineNr: %d\n", machineNr);

    std::vector<int>* rulesBack = Rules(8, 20, machineNr);
    for (int i = 0; i < 8; i += 1)
        printf("rule %d: action %d\n", i, (*rulesBack)[i]);
}
