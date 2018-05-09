#ifndef RULES_H
#define RULES_H

class Rules {

public:
    long long unsigned RuleNr(const int, const int, const std::vector<int>);
    std::vector<int>* RuleParts(const int, const int, const long long unsigned);

    // Action names
    const char* actionNames[4] = {
        "white",
        "black",
        "none",
        "invert"
    };

private:
    long long unsigned Raise(const int, const int);
};

// Action numbers
#define AWHITE 0
#define ABLACK 1
#define ANONE 2
#define AINVERT 3
#endif
