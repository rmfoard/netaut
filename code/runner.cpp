#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdelete-non-virtual-dtor"
#include "Snap.h"
#pragma GCC diagnostic pop
#include <assert.h>
//#include <getopt.h>
//#include <inttypes.h>
#include <stdio.h>
//#include <stdint.h>
//#include <algorithm>
//#include <ctime>
//#include <chrono>
//#include <fstream>
#include <iostream>
//#include <limits>
#include <string>
//#include <vector>
//#include <json/json.h>
#include "netaut.h"
#include "rule.h"
#include "machine.h"
#include "machine2D.h"
//#include "machineR.h"
#include "runner.h"

//===============
// class Runner methods
//===============

//---------------
// Runner constructor
//
//---------------
Runner::Runner(rulenr_t rulenr) {
    m_machine = new Machine2D("C");
    m_machine->BuildMachine(rulenr, 100, 200, "single-center", -1, "ring", 0);
    // -1 => tapePctBlack (ignored), 0 => noChangeTopo (change is permitted)
}

//---------------
Runner::~Runner() {
    delete m_machine;
}

//---------------
void Runner::Run() {
    int iter = 0;
    int cycleLength;
    for ( ; iter < 100; iter += 1) {
        //std::cout << "iteration: " << iter << std::endl;
        // Iterate once. Stop afterward if a state cycle or graph collapse was detected.
        cycleLength = m_machine->IterateMachine(iter);

        if (cycleLength > 0 || cycleLength < 0) { iter += 1; break; }
    } // The residual value of 'iter' is the actual number of iterations completed.
}

//---------------
void Runner::Placeholder(int placeholder) {
}
