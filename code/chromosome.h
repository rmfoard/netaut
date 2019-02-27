#ifndef CHROMOSOME_H
#define CHROMOSOME_H
#include "netaut.h"

//---------------
class Chromosome {

public:
    static bool s_paramsSet;

    static std::string s_statName;
    static double s_statMin;
    static double s_statMax;

    Chromosome(rulenr_t);
    Chromosome(rulenr_t, double);
    ~Chromosome();

    static void SetParameters(std::string, double, double);

    rulenr_t get_ruleNr();
    double get_fitness();
    void put_fitness(double);

private:
    void Placeholder(int);

    rulenr_t m_ruleNr;
    double m_fitness;
};
#endif
