#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>
#include "rules.h"


// Action names
const char* actionNames[] = {
    "white",
    "black",
    "none",
    "invert"
};

//---------------
// Raise
//
// Performs integer exponentiation.
//---------------
static
long long unsigned Raise(const int base, const int exponent) {
    long long unsigned result = 1;
    for (int i = 0; i < exponent; i += 1) result *= base;

    return result;
}

//---------------
// RuleNr
//
// Given a rule expressed as a 'ruleParts' vector, returns a rule number.
// The rule number is a radix 'nrActions' number in which the nth digit is the
// action number applicable to nodes in state 'n'.
//---------------
long long unsigned RuleNr(const int nrStates, const int nrActions, const std::vector<int> ruleParts) {
    assert(nrStates > 1);
    assert(nrActions > 0);

    long long unsigned ruleNr = 0;
    long long unsigned increase;
    for (int partNr = 0; partNr < nrStates; partNr += 1) {
        increase = ruleParts[partNr] * Raise(nrActions, partNr);
        assert(ruleParts[partNr] < nrActions);
        assert(increase <= (UINTMAX_MAX - ruleNr));
        ruleNr += increase;
    }

    return ruleNr;
}

//---------------
// RuleParts
//
// Given a rule number and the number of states and actions of its associated
// machine, returns an integer vector in which the nth entry is the action number
// applicable to nodes in state 'n'.
//---------------
std::vector<int>* RuleParts(const int nrStates, const int nrActions, const long long unsigned ruleNr) {
    assert(nrStates > 1);
    assert(nrActions > 0);

    std::vector<int>* ruleParts = new std::vector<int>(nrStates);
    long long unsigned residue = ruleNr;
    for (int state = 0; state < nrStates; state += 1) {
        (*ruleParts)[state] = residue % nrActions;
        residue /= nrActions;
        assert(state < nrStates - 1 || residue == 0);
    }

    return ruleParts;
}

#ifdef __rules_main__
int main(const int argc, const char* argv[]) {
    //std::vector<int> ruleParts = { 1, 3, 5, 7, 0, 11, 9, 2  };
    std::vector<int> ruleParts = { 19, 19, 19, 19, 19, 19, 19, 19  };

    long long unsigned ruleNr = RuleNr(8, 20, ruleParts);
    printf("ruleNr: %llu\n", ruleNr);

    std::vector<int>* rulePartsBack = RuleParts(8, 20, ruleNr);
    for (int i = 0; i < 8; i += 1)
        printf("rulePart %d: action %d\n", i, (*rulePartsBack)[i]);
}
#endif
