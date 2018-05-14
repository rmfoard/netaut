#include <assert.h>
#include <getopt.h>
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <vector>
#include "rules.h"

const std::string Rules::dst_names[] = { "L", "LL", "LR", "R", "RL", "RR" };

const std::string Rules::state_names[] = { "WHITE", "BLACK" };

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
// ParseRule
//
// Parses a rule expressed in text. Returns a rule number if successful,
// otherwise throws an exception.
//
// Syntax for rule text:
//   <rule part> (8 (nr triad states) times), separated by ';'s.
//
//   <rule part> = [L<]<dst>, [R<]<dst>, [N<]<node state>
//
//   <dst> = L | LL | LR | R | RL | RR
//
//   <node state> = WHITE | BLACK
//
//  (see rule.h for numeric values of 'dst' and 'node state' codes.)
//
// Rule part components may be omitted if the separating commas are present. Rule parts,
// likewise, may be omitted if separating semicolons are present. Defaults are:
// L<L, R<R, N< (existing node color for the triad state this rule part is associated with).
//
// For example, '...; L<LL,, N<WHITE;..a.' is equivalent to '...; L<LL, R<R, N<WHITE;...'
//
// ';;;;;;;' is equivalent to:
//
// 'L<L, R<R, N<WHITE; L<L, R<R, N<WHITE; L<L, R<R, N<BLACK; L<L, R<R, N<BLACK; ...
//    L<L, R<R, N<WHITE; L<L, R<R, N<WHITE; L<L, R<R, N<BLACK; L<L, R<R, N<BLACK'
//---------------
long long unsigned Rules::ParseRule(const char* text) {
}

//---------------
// RuleNr
//
// Given a rule expressed as a 'ruleParts' array, returns a rule number.
// The rule number is a radix 'NR_ACTIONS' number in which the nth digit is the
//---------------
long long unsigned Rules::RuleNr(const int nrTriadStates, const int nrActions, int* ruleParts) {
    assert(nrTriadStates > 1);
    assert(nrActions > 0);

    long long unsigned ruleNr = 0;
    long long unsigned increase;
    for (int partNr = 0; partNr < nrTriadStates; partNr += 1) {
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
// machine, returns an integer vector in which the nth entry is the rule part
// applicable to nodes in state 'n'.
//
// A rule part is:
//  ((<left dst> * 6 (nr dsts)) + <right dst>) * 2
//
//  <dst> =
//      0 L (left edge)
//      1 LL (left edge of left neighbor)
//      2 LR (right edge of left neighbor)
//      3 R (right edge)
//      4 RL (left edge of right neighbor)
//      5 RR (right edge of right neighbor)
//---------------
int* Rules::RuleParts(const int nrTriadStates, const int nrActions, const long long unsigned ruleNr) {
    assert(nrTriadStates > 1);
    assert(nrActions > 0);

    int* ruleParts = new int[NR_TRIAD_STATES];
    long long unsigned residue = ruleNr;

    for (int triadState = 0; triadState < nrTriadStates; triadState += 1) {
        *(ruleParts + triadState) = residue % nrActions;
        residue /= nrActions;
        assert(triadState < nrTriadStates - 1 || residue == 0);
    }

    return ruleParts;
}

//---------------
std::string Rules::RulePartText(const int rulePart) {
    return "{L<" + dst_names[(rulePart/2) / 6]
      + ", "
      + "R<" + dst_names[(rulePart/2) % 6]
      + ", "
      + "N<" + state_names[rulePart % 2]
      + "}";
}

//---------------
// TODO: Fix this mess.
std::string Rules::RuleText(unsigned long long ruleNr) {
    char scratch[4096]; // TODO: ick!
    Rules* r = new Rules();
    int* ruleParts = r->RuleParts(NR_TRIAD_STATES, NR_ACTIONS, ruleNr);

    std::string s = std::string();
    int i;
    for (i = 0; i < NR_TRIAD_STATES; i += 1) { // for each rule part

        // Insert the triad state number.
        sprintf(scratch, "[%d ", i);
        s += scratch;

        // Add the three-character triad state icon, e.g., --- -*- **-, ...
        sprintf(scratch, "%s", (i / 4 == 1) ? "*" : "-");
        s += scratch;
        sprintf(scratch, "%s", ((i / 2) % 2 == 1) ? "*" : "-");
        s += scratch;
        sprintf(scratch, "%s] ", (i % 2 == 1) ? "*" : "-");
        s += scratch;

        // Add the rule part text and terminate the line.
        sprintf(scratch, "%d = %s\n", ruleParts[i], r->RulePartText(ruleParts[i]).c_str());
        s += scratch;
    }
    delete ruleParts;
    delete r;
    return s;
}

//---------------
//int main(int argc, char* argv[]) {
//    Rules* r = new Rules();
//    printf("%s", r->RuleText((unsigned long long) 61699204822654).c_str());
//    delete r;
//}
