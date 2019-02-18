#include <assert.h>
//#include <inttypes.h>
#include <stdio.h>
//#include <stdint.h>
//#include <algorithm>
#include <iostream>
//#include <limits>
#include <string>
#include <vector>
#include "netaut.h"
#include "chromosome.h"
#include "pool.h"
#include "runner.h"

//===============
// class Pool methods
//===============

//---------------
// Pool constructor
//
//---------------
Pool::Pool() {
    m_pool = new std::vector<Chromosome*>;
    for (int i = 1; i <= 10; i += 1) m_pool[i] = new Chromosome((rulenr_t) i);
}

//---------------
Pool::~Pool() {
    for (int i = 1; i <= 10; i += 1) delete m_pool[i];
}

//---------------
void Pool::Placeholder(int placeholder) {
}
