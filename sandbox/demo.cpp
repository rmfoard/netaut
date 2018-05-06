#include "Snap.h"
#include <assert.h>

// Print graph statistics
template <class PGraph>
void PrintGStats(const char s[], PGraph Graph) {

  printf("graph %s, nodes %d, edges %d, empty %s\n",
         s, Graph->GetNodes(), Graph->GetEdges(),
         Graph->Empty() ? "yes" : "no");

}

// AHA!! It's their pointer management scheme!
//template <class TGraph>
//TPt<TGraph> NewGraph() { // instantiates into equivalents of PUNGraph, PNGraph, PNEGraph, etc.
//  return TPt<TGraph>::New();
//}
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

// Save directed, undirected and multi-graphs in GraphVizp .DOT format
int main(int argc, char* argv[]) {
    
  const char *FName1 = "demo1.dot", *FName2 = "demo2.dot";
  const char *Desc = "Randomly generated GgraphVizp for input/output.";
  
  PNEGraph GOut = RingGraph<TNEGraph>(10);
  
  TSnap::SaveGViz(GOut, FName1);
  
  // Output node IDs as numbers
  TIntStrH NIdLabelH;
  
  // Generate labels for random graph
  for (TNEGraph::TNodeI NI = GOut->BegNI(); NI < GOut->EndNI(); NI++) {
    NIdLabelH.AddDat(NI.GetId(), TStr::Fmt("Node%d", NI.GetId()));
    
  }
  TSnap::SaveGViz(GOut, FName2, Desc, NIdLabelH);
  
  PrintGStats("IOGViz - In", GOut);
}
