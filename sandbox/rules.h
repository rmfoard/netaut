#ifndef RULES_H
#define RULES_H
#include <string>

class Rules {

public:
    long long unsigned RuleNr(const int, const int, int*);
    long long unsigned ParseRule(const char* text);
    int* RuleParts(const int, const int, const long long unsigned);
    std::string RulePartText(const int);
    std::string RuleText(unsigned long long);

    static const std::string dst_names[];
    static const std::string state_names[];

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
#define NR_POSS_DSTS 6

#define NR_TRIAD_STATES (NR_NODE_STATES * NR_NODE_STATES * NR_NODE_STATES)
#define NR_ACTIONS (NR_POSS_DSTS * NR_POSS_DSTS * NR_NODE_STATES)

// Rule part (action) identifier assembly macro
#define RP(LACTION, RACTION, NACTION) (((LACTION*6) + RACTION) * 2 + NACTION)

// A rule number is a radix NR_TRIAD_STATES number, each "digit" of which is:
//  ((<replacement for left edge> * NR_POSS_DSTS) + <replacement for right edge>) * 2 +
//    <replacement for node>
#endif
