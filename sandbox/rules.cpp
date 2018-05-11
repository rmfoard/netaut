#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <vector>
#include "rules.h"

//---------------
// Action names (indexed by action number)
//---------------
const std::string Rules::actionNames[] = {
    "white",
    "black",
    "none",
    "invert",

    "r_rr+white",
    "r_rr+black",
    "r_rr",
    "r_rr+invert",

    "l_ll+white",
    "l_ll+black",
    "l_ll",
    "l_ll+invert",

    "r_rl+white",
    "r_rl+black",
    "r_rl",
    "r_rl+invert",

    "l_lr+white",
    "l_lr+black",
    "l_lr",
    "l_lr+invert"
};

//---------------
// Raise
//
// Performs integer exponentiation.
//---------------
long long unsigned Rules::Raise(const int base, const int exponent) {
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
long long unsigned Rules::RuleNr(const int nrStates, const int nrActions, const std::vector<int> ruleParts) {
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
std::vector<int>* Rules::RuleParts(const int nrStates, const int nrActions, const long long unsigned ruleNr) {
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
