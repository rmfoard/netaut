#ifndef RULES_H
#define RULES_H
uintmax_t RuleNr(const int, const int, const std::vector<int>);
std::vector<int>* RuleParts(const int, const int, const uintmax_t);

// Action numbers
#define AWHITE 0
#define ABLACK 1
#define ANONE 2
#define AINVERT 3

// Action names
const char* actionNames[] = {
    "white",
    "black",
    "none",
    "invert"
};
#endif
