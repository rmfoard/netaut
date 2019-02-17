#ifndef RUNNER_H
#define RUNNER_H
#include "netaut.h"
#include "machine.h"

//---------------
class Runner {

public:
    Runner(rulenr_t, int, int, int, std::string, int, std::string, int);
    ~Runner();

    void Run();

    // Outcome data
    double m_nrIterations;
    double m_cycleLength;
    double m_nrCcSizes;
    double m_nrCcs;
    double m_nrNodes;
    double m_avgClustCoef;
    double m_nrClosedTriads;
    double m_nrOpenTriads;
    double m_diameter;
    double m_effDiameter90Pctl;
    double m_nrInDegrees;
    double m_maxInDegree;
    double m_inDegreeEntropy;

private:
    Machine *m_machine;
    int m_maxIterations;

    void Placeholder(int);

};
#endif
