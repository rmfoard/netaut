#ifndef RULES_H
#define RULES_H
#include <inttypes.h>
#include <string>
#include "netaut.h"
#include "rulemask.h"
#include "filter.h"

// Node states, node action specifiers
#define NWHITE 0
#define NBLACK 1
#define NR_NODE_STATES 2

// Graph topology action specifiers
#define LEDGE 0
#define LLEDGE 1
#define LREDGE 2
#define REDGE 3
#define RLEDGE 4
#define RREDGE 5
#define NR_DSTS 6

#define NR_TRIAD_STATES (NR_NODE_STATES * NR_NODE_STATES * NR_NODE_STATES)
#define NR_ACTIONS (NR_DSTS * NR_DSTS * NR_NODE_STATES)

// The implicit assumption underlying the following is that
// sizeof(rulenr_t) == sizeof(uintmax_t). The assumption is
// checked in a static assertion in the 'Rule(rulenr_t)' constructor.
#define RULENR_MAX UINTMAX_MAX

//---------------
// Rule Objects:
//
// For each possible set of node states in a left neighbor/node/right neighbor
// triad, a rule encodes the action to be applied to yield the node's next
// left neighbor/node state/right neighbor values, i.e., it specifies
// replacement values for the node's left out-edge, state, and right out-edge.
// By convention, the first out-edge is the "left," the second the "right."
//
// Rules may specify that an edge be replaced by the node's left out-edge,
// its right out-edge, its left neighbor's left or right out-edges, or its
// right neighbor's left or right out-edges. These new edge destinations are denoted
// by the action specifiers L, R, RL, RR, LL, and LR, respectively. New node
// state values are denoted by the specifiers W (white) and B (black).
//
// A rule is represented internally as a "rule number."
// A rule number is a radix NR_TRIAD_STATES number, each "digit" of which is
// a "rule part" value. The digit in the i-th power position gives the rule part
// to be applied for triads whose left neighbor/node/right neighbor node states
// equal i when interpreted as a binary number (the "triad state").
//
// A rule part value is the packed representation:
//
//  ((<new edge dst for left edge> * NR_DSTS) + <new edge dst for right edge>) * 2 +
//    <new state for node>
//
// in which the "new edge dst" codes are the specifiers defined above, under
// "Graph topology action specifiers," and the "new state for node" codes are
// as defined under "Node states, node action specifiers."
//
// Rule text strings:
//
// A rule text string is a human-readable rule specification. It comprises
// a sequence of rule part specifications separated by semicolons. Each rule
// part specification is a string of the form:
//
//      <new edge dst for left edge>,<new edge dst for right edge>,<new value for node state>
//
// An edge destination is specified as one of L, R, RL, RR, LL, or LR. Node
// state is either W or B (white or black). Example substring of a rule text string
// showing the first two rule parts:
//
//      L,RL,B; LL,LR,W; ...
//
// Order of action specifications in rules:
//
// The leftmost part specification in a ruleText string corresponds to the low-order "digit"
// in the corresponding ruleNr integer; both encode the action for triad state 0, i.e.,
//
// ruleText: <action for triad state 0>; ... ;<action for triad state NR_TRIAD_STATES-1>
//
// ruleNr: <"digit" for triad state NR_TRIAD_STATES-1> ... <"digit" for triad state 0>
//
//
//---------------
class Rule {

public:
    // Constructors, from "rule number," "rule parts," and "rule text."
    Rule(rulenr_t);
    Rule(const std::string, int*);
    Rule(const char*);
    // TODO: Understand destructor declaration.

    const std::string RulePartText(const int);

    rulenr_t get_ruleNr();
    rulenr_t get_maxRuleNr();
    int* get_ruleParts();
    int* get_ruleSubParts();
    std::string get_ruleText();
    bool PassesFilter(const Filter& filter);

private:
    void CheckRuleNr(const rulenr_t);
    int dstIndex(const char*);

    rulenr_t m_ruleNr;
    static const char* dstNames[];
    static const char* nodeStateNames[];
};
#endif
