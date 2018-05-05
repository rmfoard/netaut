#include "Snap.h"

// Print graph statistics
template <class PGraph>
void PrintGStats(const char s[], PGraph Graph) {

  printf("graph %s, nodes %d, edges %d, empty %s\n",
         s, Graph->GetNodes(), Graph->GetEdges(),
         Graph->Empty() ? "yes" : "no");

}

// Print bipartite graph statistics
void PrintGStats(const char s[], PBPGraph Graph) {

  printf("graph %s, right nodes %d, left nodes %d, edges %d, empty %s\n",
         s, Graph->GetRNodes(), Graph->GetLNodes(),
         Graph->GetEdges(), Graph->Empty() ? "yes" : "no");
}

// Save directed, undirected and multi-graphs in GraphVizp .DOT format
int main(int argc, char* argv[]) {
    //****
    PNEANet net = TNEANet::New();
    net->AddNode(0);
    net->AddNode(2);
    net->AddEdge(0, 2);
    
  const int NNodes = 50;
  const int NEdges = 200;
  
  const char *FName1 = "demo1.dot", *FName2 = "demo2.dot";
  const char *Desc = "Randomly generated GgraphVizp for input/output.";
  
  PNGraph GOut;     // Can be PNEGraph or PUNGraph
  GOut = TSnap::GenRndGnm<PNGraph>(NNodes, NEdges);
  
  TSnap::SaveGViz(GOut, FName1);
  
  // Output node IDs as numbers
  TIntStrH NIdLabelH;
  
  // Generate labels for random graph
  for (TNGraph::TNodeI NI = GOut->BegNI(); NI < GOut->EndNI(); NI++) {
    NIdLabelH.AddDat(NI.GetId(), TStr::Fmt("Node%d", NI.GetId()));
    
  }
  TSnap::SaveGViz(GOut, FName2, Desc, NIdLabelH);
  
  PrintGStats("IOGViz - In", GOut);
}
