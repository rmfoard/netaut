#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>

static
uintmax_t Raise(const int base, const int exponent) {
    uintmax_t result = 1;
    for (int i = 0; i < exponent; i += 1) result *= base;

    return result;
}

uintmax_t RuleNr(const int nrStates, const int nrActions, const std::vector<int> ruleParts) {
    assert(nrStates > 1);
    assert(nrActions > 0);

    uintmax_t ruleNr = 0;
    uintmax_t increase;
    for (int partNr = 0; partNr < nrStates; partNr += 1) {
        increase = ruleParts[partNr] * Raise(nrActions, partNr);
        assert(ruleParts[partNr] < nrActions);
        assert(increase <= (UINTMAX_MAX - ruleNr));
        ruleNr += increase;
    }

    return ruleNr;
}

std::vector<int>* RuleParts(const int nrStates, const int nrActions, const uintmax_t ruleNr) {
    assert(nrStates > 1);
    assert(nrActions > 0);

    std::vector<int>* ruleParts = new std::vector<int>(nrStates);
    uintmax_t residue = ruleNr;
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

    uintmax_t ruleNr = RuleNr(8, 20, ruleParts);
    printf("ruleNr: %lld\n", ruleNr);

    std::vector<int>* rulePartsBack = RuleParts(8, 20, ruleNr);
    for (int i = 0; i < 8; i += 1)
        printf("rulePart %d: action %d\n", i, (*rulePartsBack)[i]);
}
#endif
