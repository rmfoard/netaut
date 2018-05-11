#ifndef RULES_H
#define RULES_H
#include <string>

class Rules {

public:
    long long unsigned RuleNr(const int, const int, int*);
    int* RuleParts(const int, const int, const long long unsigned);
    std::string RulePartText(const int);

    static const std::string topo_action_component_names[];
    static const std::string node_action_names[];

private:
    long long unsigned Raise(const int, const int);
};

//---------------
// TODO: Explain the following.
//---------------

// Node states, node action components
#define NWHITE 0
#define NBLACK 1
#define NR_NODE_STATES 2

// Graph topology action components
#define LEDGE 0
#define LLEDGE 1
#define LREDGE 2
#define REDGE 3
#define RLEDGE 4
#define RREDGE 5
#define NR_TOPO_ACTION_COMPONENTS 6

#define NR_TRIAD_STATES (NR_NODE_STATES * NR_NODE_STATES * NR_NODE_STATES)
#define NR_ACTIONS (NR_TOPO_ACTION_COMPONENTS * NR_TOPO_ACTION_COMPONENTS * NR_NODE_STATES)

// Rule part (action) identifier assembly macro
#define RP(LACTION, RACTION, NACTION) (((LACTION*6) + RACTION) * 2 + NACTION)

// A rule number is a radix NR_TRIAD_STATES number, each "digit" of which is:
//  ((<replacement for left edge> * NR_TOPO_ACTION_COMPONENTS) + <replacement for right edge>) * 2 +
//    <replacement for node>
#endif
