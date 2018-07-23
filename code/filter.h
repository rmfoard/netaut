#ifndef FILTER_H
#define FILTER_H

class Filter {
public:
    RuleMask **m_any;
    RuleMask **m_none;
    int m_nrAny;
    int m_nrNone;

    Filter(RuleMask** any, RuleMask** none, int nrAny, int nrNone);
};
#endif