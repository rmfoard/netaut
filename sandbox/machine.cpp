//#include "Snap.h"
#include <assert.h>
#include <stdio.h>
#include <vector>

int Raise(const int base, const int exponent) {
    int result = 1;
    for (int i = 0; i < exponent; i += 1) result *= base;

    return result;
}

int RuleNr(const int nrStates, const int nrActions, const std::vector<int> ruleParts) {
    int ruleNr = 0;
    for (int partNr = 0; partNr < nrStates; partNr += 1) {
        assert(ruleParts[partNr] < nrActions);
        ruleNr += ruleParts[partNr] * Raise(nrActions, partNr);
    }

    return ruleNr;
}

std::vector<int>* RuleParts(const int nrStates, const int nrActions, const int ruleNr) {
    std::vector<int>* ruleParts = new std::vector<int>(nrStates);
    for (int state = 0, residue = ruleNr; state < nrStates; state += 1) {
        (*ruleParts)[state] = residue % nrActions;
        residue /= nrActions;
        assert(state < nrStates - 1 || residue == 0);
    }

    return ruleParts;
}

int main(const int argc, const char* argv[]) {
    std::vector<int> ruleParts = { 1, 3, 5, 7, 0, 11, 9, 2  };

    int ruleNr = RuleNr(8, 20, ruleParts);
    printf("ruleNr: %d\n", ruleNr);

    std::vector<int>* rulePartsBack = RuleParts(8, 20, ruleNr);
    for (int i = 0; i < 8; i += 1)
        printf("rule %d: action %d\n", i, (*rulePartsBack)[i]);
}
