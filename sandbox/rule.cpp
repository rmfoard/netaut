#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <string>
#include "rule.h"

//---------------
Rule::Rule(const rulenr_t ruleNr) {
    static_assert(sizeof(rulenr_t) == sizeof(uintmax_t), "faulty type size assumption");
    CheckRuleNumber(ruleNr);
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
    Rule::CheckRuleNumber(ruleNr);
    m_ruleNr = ruleNr;
}

//---------------
Rule::Rule(const char* ruleText) {
    m_ruleNr = 0;
    CheckRuleNumber(m_ruleNr);
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
        if (partNr < NR_TRIAD_STATES - 1) rt += "; ";
    }
    return rt.c_str();
}

//---------------
// CheckRuleNumber
//
// Checks for overlarge rule number.
//---------------
static
void Rule::CheckRuleNumber(rulenr_t ruleNr) {
    if (ruleNr >= Raise(NR_ACTIONS, NR_TRIAD_STATES))
        throw std::exception("rule number overflow");
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
    return std::string("L,R,N");
}

//---------------
int main(const int argc, const char* argv[]) {
    Rule* r = new Rule((rulenr_t) 1234);
    printf("rule: %llu\n", r->get_ruleNr());
    delete r;

    const int ruleParts[] = { 71, 71, 71, 71, 71, 71, 71, 71 };
    r = new Rule(ruleParts);
    printf("rule: %llu\n", r->get_ruleNr());
    const int* rp = r->get_ruleParts();
    for (int i = 0; i < NR_TRIAD_STATES; i += 1) printf("part %d: %d\n", i, rp[i]);
    delete rp;
    printf("rule text: %s\n", r->get_ruleText());
    delete r;

    const int ruleParts2[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    r = new Rule(ruleParts2);
    printf("rule: %llu\n", r->get_ruleNr());
    const int* rp2 = r->get_ruleParts();
    for (int i = 0; i < NR_TRIAD_STATES; i += 1) printf("part %d: %d\n", i, rp2[i]);
    printf("rule text: %s\n", r->get_ruleText());
    delete rp2;
    delete r;
}
