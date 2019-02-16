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
//#include <iostream>
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
Runner::Runner(int placeholder) {
    m_machine = new Machine2D("C");
}

//---------------
Runner::~Runner() {
    delete m_machine;
}

//---------------
void Runner::Placeholder(int placeholder) {
}
