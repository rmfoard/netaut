#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdexcept>
#include <string>
#include "rule.h"
#include "rulemask.h"

const char* Rule::dstNames[] = { "L", "LL", "LR", "R", "RL", "RR" };

const char* Rule::nodeStateNames[] = { "W", "B" };

//---------------
// Raise (static helper)
//
// Performs integer exponentiation.
//---------------
static
long long unsigned Raise(const int base, const int exponent) {
    long long unsigned result = 1;
    for (int i = 0; i < exponent; i += 1) result *= base;
    return result;
}

//---------------
// MapMatchesMask (static helper)
//
// Determines whether a rulemap matches a rulemask.
// A map matches a mask if, for every 'true' entry in the
// map, the corresponding entry in the mask is 'true'.
// In Boolean terms, MasksMatch == ((map & mask) == map)
//---------------
static
bool MapMatchesMask(RuleMask rMap, RuleMask gMask) {
    bool* map = rMap.get_mask();
    bool* mask = gMask.get_mask();
    for (int i = 0; i < NR_RULEMASK_ELEMENTS; i += 1)
        if (map[i] && !mask[i]) return false;
    return true;
}

//===============
// class Rule methods
//===============

//---------------
// Rule constructor: from a ruleNr
//
//---------------
Rule::Rule(const rulenr_t ruleNr) {
    static_assert(sizeof(rulenr_t) == sizeof(uintmax_t), "faulty type size assumption");
    CheckRuleNr(ruleNr);
    m_ruleNr = ruleNr;
}

//---------------
// Rule constructor: from an int array of rule-parts or sub-parts
//
//---------------
Rule::Rule(const std::string partsType, int* parts) {
    rulenr_t ruleNr = 0;
    if (partsType == "parts") {
        rulenr_t increase;
        for (int partNr = 0; partNr < NR_TRIAD_STATES; partNr += 1) {
            increase = parts[partNr] * Raise(NR_ACTIONS, partNr);
            assert(parts[partNr] < NR_ACTIONS);
            assert(increase <= (RULENR_MAX - ruleNr));
            ruleNr += increase;
        }
    }
    else if (partsType == "subparts") {
        rulenr_t multiplier = 1;
        for (int ix = 0; ix < NR_TRIAD_STATES * 3; ix += 3) {
            int part = parts[ix+0] * NR_DSTS * 2
              + parts[ix+1] * 2
              + parts[ix+2];
            ruleNr += part * multiplier;
            multiplier *= NR_ACTIONS;
        }
    }
    else {
        assert(false);
    }

    Rule::CheckRuleNr(ruleNr);
    m_ruleNr = ruleNr;
}

//---------------
// Rule constructor: from a ruleText string
//
//---------------
Rule::Rule(const char* ruleText) {
    char* text = new char[strlen(ruleText) + 1];
    strcpy(text, ruleText);
    char* tok = strtok(text, " ,-;");
    m_ruleNr = 0;
    for (int partNr = 0; partNr < NR_TRIAD_STATES; partNr += 1) {
        int rulePart = 0;
        rulePart += dstIndex(tok) * NR_DSTS * 2; // left edge action specifier (* 2 * NR_DSTS)
        tok = strtok(NULL, " ,-;");
        rulePart += dstIndex(tok) * 2; // right edge action specifier (* 2)
        tok = strtok(NULL, " ,-;");
        if (strcmp(tok, "B") == 0) { // + node action specifier
            rulePart += 1;
        }
        else {
            if (strcmp(tok, "W") != 0) throw std::runtime_error("rule text 4");
        }

        // Build rule number from low order to high.
        m_ruleNr = m_ruleNr + (rulePart * Raise(NR_ACTIONS, partNr));
        tok = strtok(NULL, " ,-;");
    }
    CheckRuleNr(m_ruleNr);
    delete text;
}

//---------------
// get_ruleNr
//---------------
rulenr_t Rule::get_ruleNr() { return m_ruleNr; }

//---------------
// get_maxRuleNr
//
// Return the largest possible rule number. NR_ACTIONS**NR_TRIAD_STATES - 1
//---------------
rulenr_t Rule::get_maxRuleNr() { return Raise(NR_ACTIONS, NR_TRIAD_STATES) - 1; }

//---------------
// get_ruleParts
//---------------
int* Rule::get_ruleParts() {
    int* rp = new int[NR_TRIAD_STATES];
    rulenr_t dividend = m_ruleNr;
    for (int triadState = 0; triadState < NR_TRIAD_STATES; triadState += 1) {
        rp[triadState] = dividend % NR_ACTIONS;
        dividend /= NR_ACTIONS;
    }
    return rp;
}

//---------------
// get_ruleSubParts
//---------------
int* Rule::get_ruleSubParts() {
    int* rsp = new int[NR_TRIAD_STATES * 3];
    rulenr_t dividend = m_ruleNr;
    for (int triadState = 0; triadState < NR_TRIAD_STATES; triadState += 1) {
        int part = dividend % NR_ACTIONS;
        rsp[triadState * 3 + 0] = part / (NR_DSTS * 2);
        rsp[triadState * 3 + 1] = (part % (NR_DSTS * 2)) / 2;
        rsp[triadState * 3 + 2] = part % 2;
        dividend /= NR_ACTIONS;
    }
    return rsp;
}

//---------------
// get_ruleText
//---------------
std::string Rule::get_ruleText() {
    std::string rt = std::string("");
    int* rp = get_ruleParts();
    for (int partNr = 0; partNr < NR_TRIAD_STATES; partNr += 1) {
        rt += RulePartText(rp[partNr]);
        if (partNr < NR_TRIAD_STATES - 1) rt += ";";
    }
    return rt;
}

//---------------
// dstIndex
//---------------
int Rule::dstIndex(const char* dstStr) {
    for (int i = 0; i < NR_DSTS; i += 1) {
        if (strcmp(dstStr, dstNames[i]) == 0) return i;
    }
    throw std::runtime_error("rule text 5");
    assert(false);
    return 0; // appease compiler
}

//---------------
// CheckRuleNr
//
// Checks for overlarge rule number.
//---------------
void Rule::CheckRuleNr(rulenr_t ruleNr) {
    if (ruleNr > get_maxRuleNr())
        throw std::runtime_error(std::string("rule number overflow") + std::to_string(ruleNr));
}

//---------------
// RulePartText
//
// Returns a rule part in text form.
//---------------
const std::string Rule::RulePartText(const int rulePart) {
    int lAction = (rulePart / 2) / NR_DSTS;
    int rAction = (rulePart / 2) % NR_DSTS;
    int nAction = rulePart % 2;
    return std::string(dstNames[lAction]) + std::string(",")
      + std::string(dstNames[rAction]) + std::string(",")
      + std::string(nodeStateNames[nAction]);
}

//---------------
// PassesFilter
//
// Determines whether the rule passes the filter. A rule passes if its
// map matches any of the 'anyOf' masks and none of the 'butNoneOf' masks.
//---------------
bool Rule::PassesFilter(const Filter& filter) {
    RuleMask ruleMap = RuleMask(m_ruleNr);

    // TODO: Review the following. I think the "any" comment is wrong,
    // and that we should move on to check the "none of"s as soon as
    // the first "any" match occurs. (Any==disjunction, None=conjunction)
    // The rulemap must match every "any" mask ...
    for (int i = 0; i < filter.m_nrAny; i += 1)
        if (!MapMatchesMask(ruleMap, *filter.m_any[i])) return false;

    // ... and must not match any "none" mask.
    for (int i = 0; i < filter.m_nrNone; i += 1)
        if (MapMatchesMask(ruleMap, *filter.m_none[i])) return false;

    return true;
}

//---------------
// SetTopoActionMask (static helper)
//
// TODO: Document.
//---------------
static
char* SetTopoActionMask(int baseIx, char* rmt, char* tok, bool* mask) {
    for (int dst = 0; dst < NR_DSTS; dst += 1) {
        bool element = false;
        if (strcmp(tok, "-") == 0
          || (strcmp(tok, "L") == 0 && dst == LEDGE)
          || (strcmp(tok, "LL") == 0 && dst == LLEDGE)
          || (strcmp(tok, "LR") == 0 && dst == LREDGE)
          || (strcmp(tok, "R") == 0 && dst == REDGE)
          || (strcmp(tok, "RL") == 0 && dst == RLEDGE)
          || (strcmp(tok, "RR") == 0 && dst == RREDGE)) {
            element = true;
        }
        mask[baseIx + dst] = element;
    }
    return strtok(NULL, " ,;");
}

char* strAllocCpy(const char*); // TODO: The obvious
//---------------
// CompileRuleMask (static helper)
//
// Accepts a rulemask text string and returns a boolean array
// in which each entry indicates whether the topo or node state
// action corresponding to the entry's position is included
// in the mask. (Rules that do not contain actions matching
// the 'true' entries in the array do not match the mask.)
//---------------
static
bool* CompileRuleMask(const char* ruleMaskText) {
    bool* mask = new bool[NR_RULEMASK_ELEMENTS];
    char* rmt = strAllocCpy(ruleMaskText);
    for (int i = 0; i < NR_RULEMASK_ELEMENTS; i += 1) mask[i] = false;

    char* tok = strtok(rmt, " ,;");
    for (int partNr = 0; partNr < NR_TRIAD_STATES; partNr += 1) {
        int partBaseIx = partNr * (NR_DSTS * 2 + NR_NODE_STATES);

        if (strcmp(tok, "*") == 0) {
            for (int i = 0; i < (NR_DSTS * 2 + NR_NODE_STATES); i += 1)
                mask[partBaseIx + i] = true;
        }
        else {
            tok = SetTopoActionMask(partBaseIx, rmt, tok, mask);
            tok = SetTopoActionMask(partBaseIx + NR_DSTS, rmt, tok, mask);

            // TODO: Consider the invalid sequences this will pass.
            if (strcmp(tok, "W") == 0 || strcmp(tok, "-") == 0)
                mask[partBaseIx + NR_DSTS * 2] = true;
            if (strcmp(tok, "B") == 0 || strcmp(tok, "-") == 0)
                mask[partBaseIx + NR_DSTS * 2 + 1] = true;
        }
        tok = strtok(NULL, " ,;");
    }
    delete rmt;
    return mask;
}

//===============
// class RuleMask methods
//===============

//---------------
// RuleMask constructor: from rule number
//---------------
RuleMask::RuleMask(const rulenr_t ruleNr) {
    Rule* r = new Rule(ruleNr);
    const char* ruleText = strAllocCpy(r->get_ruleText().c_str());
    delete r;

    // The following works because rule text syntax is a subset of
    // rule mask syntax.
    m_mask = CompileRuleMask(ruleText);
    delete ruleText;
}

//---------------
// RuleMask constructor: from rulemask text
//---------------
RuleMask::RuleMask(const char* ruleMaskText) : m_mask(CompileRuleMask(ruleMaskText)) {}

//---------------
// get_mask
//---------------
bool* RuleMask::get_mask() {
    return m_mask;
}

/*
int main() {
    Rule r1 = Rule("L,L,W; LL,LL,W; LR,LR,W; R,R,B; RL,RL,B; RR,RR,B; L,R,W; L,R,B");
    Rule r2 = Rule("L,L,W; LL,LL,W; LR,LR,W; R,R,B; RL,RL,B; RR,RR,B; L,R,W; L,R,W");

    RuleMask m1 = RuleMask(r1.get_ruleNr());
    RuleMask m2 = RuleMask(r2.get_ruleNr());

    RuleMask** any = new RuleMask*[1];
    RuleMask** none = new RuleMask*[1];

    any[0] = &m1;
    none[0] = &m2;

    Filter f = Filter(any, 1, none, 1);

    assert(r1.PassesFilter(f));
    assert(!r2.PassesFilter(f));
}
*/
