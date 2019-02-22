#ifndef RUNNER_H
#define RUNNER_H
#include <string>
#include "netaut.h"
#include "machine.h"

//---------------
class Runner {

public:
    static bool s_defaultsSet;

    static int s_initNrNodes;
    static int s_maxIterations;
    static int s_cycleCheckDepth;
    static std::string s_tapeStructure;
    static int s_tapePctBlack;
    static std::string s_topoStructure;
    static int s_noChangeTopo;

    Runner(rulenr_t, int, int, int, std::string, int, std::string, int);
    Runner(rulenr_t);
    ~Runner();
    static void SetDefaults(int, int, int, std::string, int, std::string, int);

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
