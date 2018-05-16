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
Rule::Rule(const rulenr_t ruleNr) {
    static_assert(sizeof(rulenr_t) == sizeof(uintmax_t), "faulty type size assumption");
    CheckRuleNr(ruleNr);
    m_ruleNr = ruleNr;
}

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
Rule::Rule(const char* ruleText) {
    char* text = new char[strlen(ruleText) + 1];
    strcpy(text, ruleText);
    char* tok = strtok(text, " ,-;");
    m_ruleNr = 0;
    for (int partNr = 0; partNr < NR_TRIAD_STATES; partNr += 1) {
        int rulePart = 0;
        if (strcmp(tok, "L") != 0) throw std::runtime_error("rule text error 1");
        tok = strtok(NULL, " ,-;");
        rulePart += dstIndex(tok) * NR_POSS_DSTS * 2;
        tok = strtok(NULL, " ,-;");
        if (strcmp(tok, "R") != 0) throw std::runtime_error("rule text error 2");
        tok = strtok(NULL, " ,-;");
        rulePart += dstIndex(tok) * 2;
        tok = strtok(NULL, " ,-;");
        if (strcmp(tok, "N") != 0) throw std::runtime_error("rule text error 3");
        tok = strtok(NULL, " ,-;");
        if (strcmp(tok, "B") == 0)
            rulePart += 1;
        else
            if (strcmp(tok, "W") != 0) throw std::runtime_error("rule text error 4");

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
const char* Rule::get_ruleText() {
    std::string rt = std::string("");
    const int* rp = get_ruleParts();
    for (int partNr = 0; partNr < NR_TRIAD_STATES; partNr += 1) {
        rt += RulePartText(rp[partNr]);
        if (partNr < NR_TRIAD_STATES - 1) rt += ";";
    }
    return rt.c_str();
}

//---------------
// dstIndex
//---------------
int Rule::dstIndex(const char* dstStr) {
    for (int i = 0; i < NR_POSS_DSTS; i += 1) {
        if (strcmp(dstStr, dstNames[i]) == 0) return i;
    }
    throw std::runtime_error("rule text error 5");
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
// Raise
//
// Performs integer exponentiation.
//---------------
long long unsigned Rule::Raise(const int base, const int exponent) {
    long long unsigned result = 1;
    for (int i = 0; i < exponent; i += 1) result *= base;
    return result;
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
