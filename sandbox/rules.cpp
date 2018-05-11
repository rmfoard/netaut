#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <vector>
#include "rules.h"

const std::string Rules::topo_action_component_names[] = {
    "L",
    "LL",
    "LR",
    "R",
    "RL",
    "RR"
};

const std::string Rules::node_action_names[] = {
    "WHITE",
    "BLACK"
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
long long unsigned Rules::RuleNr(const int nrStates, const int nrActions, int* ruleParts) {
    assert(nrStates > 1);
    assert(nrActions > 0);

    long long unsigned ruleNr = 0;
    long long unsigned increase;
    for (int partNr = 0; partNr < nrStates; partNr += 1) {
        increase = ruleParts[partNr] * Raise(nrActions, partNr);
        assert(ruleParts[partNr] < nrActions);
        // TODO: Replace UINTMAX_MAX, below.
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
int* Rules::RuleParts(const int nrStates, const int nrActions, const long long unsigned ruleNr) {
    assert(nrStates > 1);
    assert(nrActions > 0);

    int* ruleParts = new int[NR_TRIAD_STATES];
    long long unsigned residue = ruleNr;

    for (int state = 0; state < nrStates; state += 1) {
        *(ruleParts + state) = residue % nrActions;
        residue /= nrActions;
        assert(state < nrStates - 1 || residue == 0);
    }

    return ruleParts;
}

//---------------
std::string Rules::RulePartText(const int rulePart) {
    return "L<" + topo_action_component_names[(rulePart/2) / 6]
      + "+"
      + "R<" + topo_action_component_names[(rulePart/2) % 6]
      + "+"
      + "N<" + node_action_names[rulePart % 2];
}

//---------------
int main(int argc, char* argv[]) {
    Rules* r = new Rules();
    int* ruleParts = r->RuleParts(NR_TRIAD_STATES, NR_ACTIONS, (unsigned long long) 10658);
    for (int i = 0; i < NR_TRIAD_STATES; i += 1)
        printf("[%d] ___ %d = %s\n", i, ruleParts[i], r->RulePartText(ruleParts[i]).c_str());
    printf("finis.\n");
}
