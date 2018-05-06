//#include "Snap.h"
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>

uintmax_t Raise(const int base, const int exponent) {
    uintmax_t result = 1;
    for (int i = 0; i < exponent; i += 1) result *= base;

    return result;
}

uintmax_t RuleNr(const int nrStates, const int nrActions, const std::vector<int> ruleParts) {
    uintmax_t ruleNr = 0;
    for (int partNr = 0; partNr < nrStates; partNr += 1) {
        assert(ruleParts[partNr] < nrActions);
        assert(ruleParts[partNr] * Raise(nrActions,partNr) <= (UINTMAX_MAX - ruleNr));
        ruleNr += ruleParts[partNr] * Raise(nrActions, partNr);
    }

    return ruleNr;
}

std::vector<int>* RuleParts(const int nrStates, const int nrActions, const uintmax_t ruleNr) {
    std::vector<int>* ruleParts = new std::vector<int>(nrStates);
    uintmax_t residue = ruleNr;
    for (int state = 0; state < nrStates; state += 1) {
        (*ruleParts)[state] = residue % nrActions;
        residue /= nrActions;
        assert(state < nrStates - 1 || residue == 0);
    }

    return ruleParts;
}

int main(const int argc, const char* argv[]) {
    std::vector<int> ruleParts = { 1, 3, 5, 7, 0, 11, 9, 2  };

    uintmax_t ruleNr = RuleNr(8, 20, ruleParts);
    printf("ruleNr: %lld\n", ruleNr);

    std::vector<int>* rulePartsBack = RuleParts(8, 20, ruleNr);
    for (int i = 0; i < 8; i += 1)
        printf("rulePart %d: action %d\n", i, (*rulePartsBack)[i]);
}
