#ifndef RULES_H
#define RULES_H

class Rules {

public:
    long long unsigned RuleNr(const int, const int, const std::vector<int>);
    std::vector<int>* RuleParts(const int, const int, const long long unsigned);

    static const char* actionNames[];

private:
    long long unsigned Raise(const int, const int);
};

//---------------
// Action code = graph state op * 4 + node state op
//
// Node state operations
//---------------
#define NWHITE 0
#define NBLACK 1
#define NNONE 2
#define NINVERT 3

//---------------
// Graph state operations
//---------------
#define GNONE 0
#define GR_RR 1 // replace redge with redge[redge]
#define GL_LL 2 // replace ledge with ledge[ledge]
#define GR_RL 3 // replace redge with redge[ledge]
#define GL_LR 4 // replace ledge with ledge[redge]
//---------------
#endif
