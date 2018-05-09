#ifndef RULES_H
#define RULES_H
long long unsigned RuleNr(const int, const int, const std::vector<int>);
std::vector<int>* RuleParts(const int, const int, const long long unsigned);

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
