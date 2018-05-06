//#include "Snap.h"
#include <assert.h>
#include <stdio.h>
#include <vector>

int Raise(int base, int exponent) {
    int result = 1;
    for (int i = 0; i < exponent; i += 1) result *= base;

    return result;
}

int MachineNr(int nrStates, int nrActions, std::vector<int> rules) {
    int machineNr = 0;
    for (int ruleNr = 0; ruleNr < nrStates; ruleNr += 1)
        machineNr += rules[ruleNr] * Raise(nrActions, ruleNr);

    return machineNr;
}

std::vector<int>* Rules(int nrStates, int nrActions, int machineNr) {
    std::vector<int>* rules = new std::vector<int>(nrStates);
    for (int state = 0, residue = machineNr; state < nrStates; state += 1) {
        (*rules)[state] = residue % nrActions;
        residue /= nrActions;
    }

    return rules;
}

int main(int argc, char* argv[]) {
    std::vector<int> rules = { 1, 3, 5, 0, 0, 0, 0, 0  };

    int machineNr = MachineNr(8, 20, rules);
    printf("machineNr: %d\n", machineNr);

    std::vector<int>* rulesBack = Rules(8, 20, machineNr);
    for (int i = 0; i < 8; i += 1)
        printf("rule %d: action %d\n", i, (*rulesBack)[i]);
}
