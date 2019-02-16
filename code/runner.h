#ifndef RUNNER_H
#define RUNNER_H
#include "machine.h"

//---------------
class Runner {

public:
    Runner(int);
    ~Runner();

private:
    Machine *m_machine;

    void Placeholder(int);

};
#endif
