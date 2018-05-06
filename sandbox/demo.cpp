#include "Snap.h"

// Print graph statistics
template <class PGraph>
void PrintGStats(const char s[], PGraph Graph) {

  printf("graph %s, nodes %d, edges %d, empty %s\n",
         s, Graph->GetNodes(), Graph->GetEdges(),
         Graph->Empty() ? "yes" : "no");

}

// AHA!! It's their pointer management scheme!
template <class TGraph>
TPt<TGraph> NewGraph() { // instantiates into equivalents of PUNGraph, PNGraph, PNEGraph, etc.
  return TPt<TGraph>::New();
}

// Save directed, undirected and multi-graphs in GraphVizp .DOT format
int main(int argc, char* argv[]) {
    
  const int NNodes = 50;
  const int NEdges = 200;
  
  const char *FName1 = "demo1.dot", *FName2 = "demo2.dot";
  const char *Desc = "Randomly generated GgraphVizp for input/output.";
  
  PNEGraph GOut = NewGraph<TNEGraph>();
  
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
