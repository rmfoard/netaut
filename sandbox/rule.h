#ifndef RULES_H
#define RULES_H
#include <inttypes.h>
#include <string>

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
#define NR_POSS_DSTS 6

#define NR_TRIAD_STATES (NR_NODE_STATES * NR_NODE_STATES * NR_NODE_STATES)
#define NR_ACTIONS (NR_POSS_DSTS * NR_POSS_DSTS * NR_NODE_STATES)

typedef long long unsigned rulenr_t;

// The implicit assumption underlying the following is that
// sizeof(rulenr_t) == sizeof(uintmax_t). The assumption is
// checked in a static assertion in the 'Rule(rulenr_t)' constructor.
#define RULENR_MAX UINTMAX_MAX

//---------------
// Rule Object
//
// For each possible set of node states in a left neighbor/node/right neighbor
// triad, a rule encodes the action to be applied to yield the next
// left neighbor/node state/right neighbor configuration, i.e., it specifies
// replacement values for the node's left out-edge, state, and right out-edge.
// By convention, the first out-edge is the "left," the second the "right."
//
// Rules may specify that an edge be replaced by the node's left out-edge,
// its right out-edge, its left neighbor's left or right out-edges, or its
// right neighbor's left or right out-edges. These new edge destinations are denoted
// by prefixes L, R, RL, RR, LL, and LR, respectively.
//
// A rule is represented internally as a "rule number."
// A rule number is a radix NR_TRIAD_STATES number, each "digit" of which is
// a "rule part" value. The digit in the i-th power position gives the rule part
// to be applied for triads whose left neighbor/node/right neighbor node states
// equal i when interpreted as a binary number.
//
// A rule part value is the packed representation:
//
//  ((<new edge dst for left edge> * NR_POSS_DSTS) + <new edge dst for right edge>) * 2 +
//    <new state for node>
//
// in which the "new edge dst" codes are as defined above, under "Graph topology action
// components," and the "new state for node" codes are as defined under "Node states,i
// node action components."
//---------------

class Rule {

public:
    // Constructors, from "rule number," "rule parts," and "rule text."
    Rule(const rulenr_t);
    Rule(const int*);
    Rule(const char*);

    rulenr_t get_ruleNr();
    const int* get_ruleParts();
    const char* get_ruleText();

private:
    long long unsigned Raise(const int, const int);
    const std::string RulePartText(const int);
    void CheckRuleNr(const rulenr_t);
    int dstIndex(const char*);

    rulenr_t m_ruleNr;
    static const char* dstNames[];
    static const char* nodeStateNames[];
};
#endif
