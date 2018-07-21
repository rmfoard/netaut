#include "rulemask.h"
#include "filter.h"

//---------------
// Filter constructor: from RuleMask arrays
//---------------
Filter::Filter(RuleMask** any, RuleMask** none, int nrAny, int nrNone) :
    m_any(any), m_none(none), m_nrAny(nrAny), m_nrNone(nrNone) {}
