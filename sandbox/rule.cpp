#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdexcept>
#include <string>
#include "rule.h"

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
// class Rule methods
//---------------

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
// Rule constructor: from an int array of rule part numbers
//
//---------------
Rule::Rule(const int* ruleParts) {
    rulenr_t ruleNr = 0;
    rulenr_t increase;
    for (int partNr = 0; partNr < NR_TRIAD_STATES; partNr += 1) {
        increase = ruleParts[partNr] * Raise(NR_ACTIONS, partNr);
        assert(ruleParts[partNr] < NR_ACTIONS);
        assert(increase <= (RULENR_MAX - ruleNr));
        ruleNr += increase;
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
        if (strcmp(tok, "L") != 0) throw std::runtime_error("rule text 1");
        tok = strtok(NULL, " ,-;");
        rulePart += dstIndex(tok) * NR_POSS_DSTS * 2;
        tok = strtok(NULL, " ,-;");
        if (strcmp(tok, "R") != 0) throw std::runtime_error("rule text 2");
        tok = strtok(NULL, " ,-;");
        rulePart += dstIndex(tok) * 2;
        tok = strtok(NULL, " ,-;");
        if (strcmp(tok, "N") != 0) throw std::runtime_error("rule text 3");
        tok = strtok(NULL, " ,-;");
        if (strcmp(tok, "B") == 0) {
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
const int* Rule::get_ruleParts() {
    int* rp = new int[NR_TRIAD_STATES];
    rulenr_t dividend = m_ruleNr;
    for (int triadState = 0; triadState < NR_TRIAD_STATES; triadState += 1) {
        rp[triadState] = dividend % NR_ACTIONS;
        dividend /= NR_ACTIONS;
    }
    return rp;
}


//---------------
// get_ruleText
//---------------
std::string Rule::get_ruleText() {
    std::string rt = std::string("");
    const int* rp = get_ruleParts();
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
    for (int i = 0; i < NR_POSS_DSTS; i += 1) {
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
    if (ruleNr >= Raise(NR_ACTIONS, NR_TRIAD_STATES))
        throw std::runtime_error("rule number overflow");
}

//---------------
// RulePartText
//
// Returns a rule part in text form.
//---------------
const std::string Rule::RulePartText(const int rulePart) {
    int lAction = (rulePart / 2) / NR_POSS_DSTS;
    int rAction = (rulePart / 2) % NR_POSS_DSTS;
    int nAction = rulePart % 2;
    return std::string("L-") + std::string(dstNames[lAction]) + std::string(",")
      + std::string("R-") + std::string(dstNames[rAction]) + std::string(",")
      + std::string("N-") + std::string(nodeStateNames[nAction]);
}

//---------------
// PassesFilter
//
// Determines whether the rule passes the filter. A rule passes if its
// mask matches any of the 'anyOf' masks and none of the 'butNoneOf' masks.
//---------------
bool Rule::PassesFilter(const RuleMask* anyOf, const RuleMask* butNoneOf) {
    return true; // placeholder only
}

//---------------
// SetTopoActionMask (static helper)
//
// TODO: Document.
//---------------
static
char* SetTopoActionMask(const char* lr, int baseIx, char* rmt, char* tok, bool* mask) {
    if (strcmp(tok, lr) != 0) throw std::runtime_error("rulemask text 3");
    tok = strtok(NULL, " ,-;");
    for (int dst = 0; dst < NR_POSS_DSTS; dst += 1) {
        bool element = false;
        if (strcmp(tok, "*") == 0
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
    return strtok(NULL, " ,-;");
}

//---------------
// class RuleMask methods
//---------------

//---------------
// RuleMask constructor: from rule number
//---------------
RuleMask::RuleMask(rulenr_t ruleNr) {
    // We can accomplish this by using Rule::get_ruleText to convert
    // the rule number to a string, then passing the string to the
    // other constructor's logic to generate the rulemask. (We'll need
    // to move the guts of the other constructor into a separately
    // callable method.)
}

char* strAllocCpy(char*); // TODO: The obvious
//---------------
// RuleMask constructor: from rulemask text
//---------------
RuleMask::RuleMask(char* ruleMaskText) { // TODO: Should be const.
    m_mask = new bool[NR_RULEMASK_ELEMENTS];
    char* rmt = strAllocCpy(ruleMaskText);
    for (int i = 0; i < NR_RULEMASK_ELEMENTS; i += 1) m_mask[i] = false;

    char* tok = strtok(rmt, " ,-;");
    for (int partNr = 0; partNr < NR_TRIAD_STATES; partNr += 1) {
        int partBaseIx = partNr * (NR_POSS_DSTS * 2 + NR_NODE_STATES);

        tok = SetTopoActionMask("L", partBaseIx, rmt, tok, m_mask);
        tok = SetTopoActionMask("R", partBaseIx + NR_POSS_DSTS, rmt, tok, m_mask);

        if (strcmp(tok, "N") != 0) throw std::runtime_error("rulemask text 1");
        tok = strtok(NULL, " ,-;");

        // TODO: Consider the invalid sequences this will pass.
        if (strcmp(tok, "W") == 0 || strcmp(tok, "*") == 0)
            m_mask[partBaseIx + NR_POSS_DSTS * 2] = true;
        if (strcmp(tok, "B") == 0 || strcmp(tok, "*") == 0)
            m_mask[partBaseIx + NR_POSS_DSTS * 2 + 1] = true;

        tok = strtok(NULL, " ,-;");
    }
}

//---------------
// get_mask
//---------------
bool* RuleMask::get_mask() {
    return m_mask;
}

int main() {
    RuleMask* rm = new RuleMask("L-L,R-L,N-W;L-L,R-L,N-W;L-*,R-LR,N-B;L-*,R-R,N-B;L-*,R-RL,N-B;L-*,R-RR,N-B;L-*,R-*,N-B;L-*,R-*,N-*");
    bool* mask = rm->get_mask();
    for (int i = 0; i < NR_RULEMASK_ELEMENTS; i += 1)
        printf("%s", (mask[i] ? "1" : "0"));
    printf("\n");
}
