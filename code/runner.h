#ifndef RUNNER_H
#define RUNNER_H
#include "netaut.h"
#include "machine.h"

//---------------
class Runner {

public:
    Runner(rulenr_t);
    ~Runner();

    void Run();

private:
    Machine *m_machine;

    void Placeholder(int);

};
#endif
