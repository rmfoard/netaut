#include "Snap.h"
#include <assert.h>

template <class TGraph>
static TPt<TGraph> RingGraph(int nrNodes) {
    assert(nrNodes > 1);
    TPt<TGraph> net = TGraph::New();

    for (int n = 0; n < nrNodes; n += 1) net->AddNode(n);
    for (int n = 0; n < nrNodes - 1; n += 1) net->AddEdge(n, n + 1);
    net->AddEdge(nrNodes - 1, 0);
    net->AddEdge(0, nrNodes - 1);
    for (int n = 1; n < nrNodes; n += 1) net->AddEdge(n, n - 1);

    return net;
}
