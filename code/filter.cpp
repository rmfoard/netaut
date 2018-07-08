#include "rulemask.h"
#include "filter.h"

//---------------
// Filter constructor: from RuleMask arrays
//---------------
Filter::Filter(RuleMask** any, int nrAny, RuleMask** none, int nrNone) :
    m_any(any), m_nrAny(nrAny), m_none(none), m_nrNone(nrNone) {}
