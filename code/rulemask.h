#ifndef RULEMASK_H
#define RULEMASK_H
#include "netaut.h"
#include "rule.h"

// Rulemask objects:
//
// Rulemasks are objects that match classes of rules. They are typically constructed
// based on human-readable rulemask text strings.
//
// A rulemask text string is similar to a rule text string, but allows the use of
// wildcard characters to represent multiple possibilities for parts of rules. For
// example, where the rule text snippet containing three action specifiers:
//
//      ... ; R,LL,W; ...
//
// indicates that a node's left edge is to be replaced by its right edge (R), and its right edge
// is to be replaced by its left neighbor's left edge (LL), the rulemask snippet:
//
//      ... ; -,LL,W; ...
//
// will match any rule that has L, R, RL, RR, LL, or LR where the "-" appears in the mask.
//
// Similarly, if a rulemask has a "*" character in place of an entire rule part, as in:
//
//      ...; R,LL,W; *; RR,R,B; ...
//
// the mask will match rules with any combination of actions in the corresponding rule part.
// Note that "-" and "*" wildcards are not interchageable -- "-" matches any individual
// action specifier in a rule part; "*" matches any entire rule part.
//---------------

#define NR_RULEMASK_ELEMENTS (NR_TRIAD_STATES * (NR_DSTS * 2 + NR_NODE_STATES))

//---------------
// class RuleMask
//---------------
class RuleMask {

public:
    // Constructors
    RuleMask(const rulenr_t);
    RuleMask(const char*);

    // Getters
    bool* get_mask();

private:
    bool *m_mask;
};
#endif
