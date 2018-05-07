#include "Snap.h"
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <vector>

// TODO: Move these to a header file for 'rules'.
uintmax_t RuleNr(const int, const int, const std::vector<int>);
std::vector<int>* RuleParts(const int, const int, const uintmax_t);

// TODO: Consider moving this to a 'net_factory' module.
template <class TGraph>
static TPt<TGraph> RingNet(int nrNodes) {
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
    Machine(uintmax_t, int, PNEANet);
    void Cycle();

private:
    uintmax_t m_ruleNr;
    int m_nrNodes;
    PNEANet m_net;
    std::vector<int>* m_ruleParts;
};

Machine::Machine(uintmax_t ruleNr, int nrNodes, PNEANet net) {
    m_ruleNr = ruleNr;
    m_nrNodes = nrNodes;
    m_net = net;
    m_ruleParts = RuleParts(8, 20, ruleNr);
}

// Cycle: Run one step of the loaded rule.
void Machine::Cycle() {
    PNEANet nextNet = TNEANet::New();

    // Traverse the current net building 'nextNet'.
    for (TNEANet::TNodeI NI = m_net->BegNI(); NI < m_net->EndNI(); NI++) ;
}

int main(const int argc, const char* argv[]) {
    std::vector<int> ruleParts = { 19, 19, 19, 19, 19, 19, 19, 19  };
    uintmax_t ruleNr = RuleNr(8, 20, ruleParts);
    printf("ruleNr: %lld\n", ruleNr);

    // build a ring network
    PNEANet net = RingNet<TNEANet>(10);

    std::vector<int>* rulePartsBack = RuleParts(8, 20, ruleNr);
    for (int i = 0; i < 8; i += 1)
        printf("rulePart %d: action %d\n", i, (*rulePartsBack)[i]);

    Machine* m = new Machine(ruleNr, 10, net);
}
