#include "Snap.h"
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>

uintmax_t RuleNr(const int, const int, const std::vector<int>);
std::vector<int>* RuleParts(const int, const int, const uintmax_t);

template <class TGraph>
static TPt<TGraph> RingGraph(int nrNodes) {
    assert(nrNodes > 1);
    TPt<TGraph> net = TGraph::New();

    for (int n = 0; n < nrNodes; n += 1) net->AddNode(n);
    for (int n = 0; n < nrNodes - 1; n += 1) net->AddEdge(n, n + 1);
    net->AddEdge(nrNodes - 1, 0);
    net->AddEdge(0, nrNodes - 1);
    for (int n = 1; n < nrNodes; n += 1) net->AddEdge(n, n - 1);

    return net;
}

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
    PNEANet fred = RingGraph<TNEANet>(10);
    // pass it to the Machine constructor

    std::vector<int>* rulePartsBack = RuleParts(8, 20, ruleNr);
    for (int i = 0; i < 8; i += 1)
        printf("rulePart %d: action %d\n", i, (*rulePartsBack)[i]);

    Machine* m = new Machine(ruleNr);
}
