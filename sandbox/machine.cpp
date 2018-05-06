#include "Snap.h"
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>

uintmax_t RuleNr(const int, const int, const std::vector<int>);
std::vector<int>* RuleParts(const int, const int, const uintmax_t);

class Machine {

public:
    Machine(uintmax_t);

private:
    uintmax_t m_ruleNr;
};

Machine::Machine(uintmax_t ruleNr) {
    m_ruleNr = ruleNr;
    // accept a network parameter
    // unpack the rule's ruleparts
    // build a 'next' network
}

int main(const int argc, const char* argv[]) {
    std::vector<int> ruleParts = { 19, 19, 19, 19, 19, 19, 19, 19  };
    uintmax_t ruleNr = RuleNr(8, 20, ruleParts);
    printf("ruleNr: %lld\n", ruleNr);

    // build a ring network
    // pass it to the Machine constructor

    std::vector<int>* rulePartsBack = RuleParts(8, 20, ruleNr);
    for (int i = 0; i < 8; i += 1)
        printf("rulePart %d: action %d\n", i, (*rulePartsBack)[i]);

    Machine* m = new Machine(ruleNr);
}
