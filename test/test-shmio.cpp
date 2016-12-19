#include <gtest/gtest.h>

#include "Snap.h"
#include "test-helper.h"

class SHMTest { };  // For gtest highlighting

using namespace TSnap;

template <class PGraph>
void CheckGraphCorrect(PGraph Saved, PGraph Loaded) {
  EXPECT_EQ(Saved->GetNodes(), Loaded->GetNodes());
  EXPECT_EQ(Saved->GetEdges(), Loaded->GetEdges());
  for (typename PGraph::TObj::TEdgeI EI = Saved->BegEI(); EI < Saved->EndEI(); EI++) {
    TInt Src = EI.GetSrcNId();
    TInt Dst = EI.GetDstNId();
    EXPECT_TRUE(Loaded->IsEdge(Src, Dst));
  }
}

template <class PGraph>
PGraph WriteGraph(TStr Filename) {
  int num_nodes = 100;
  PGraph Graph = PGraph::TObj::New();
  for (int i = 0; i < num_nodes; i++) {
    Graph->AddNode(i);
  }
  for (int i = 1; i < num_nodes - 1; i++) {
    Graph->AddEdge(i, i+1);
    Graph->AddEdge(i, i-1);
  }
  TFOut outstream(Filename);
  Graph->Save(outstream);
  return Graph;
}

template <class PGraph>
PGraph ReadGraph(TStr Filename) {
  TShMIn ShMIn(Filename);
  PGraph Graph = PGraph::TObj::LoadShM(ShMIn);
  return Graph;
}

template <class PGraph>
void CheckGraph() {
  TStr Filename("test.graph");
  PGraph g = WriteGraph<PGraph>(Filename);
  PGraph g2 = ReadGraph<PGraph>(Filename);
  CheckGraphCorrect<PGraph>(g, g2);
}

// Tests saving and loading of undirected and directed graphs
TEST(SHMTest, LoadGraphs) {
  // Undirected graph
  CheckGraph<PUNGraph>();
  // Directed graph
  CheckGraph<PNGraph>();
}

PNEANet writeTNEANet(TStr Filename) {
  int NNodes = 100;
  int NEdges = 100;
  PNEANet Graph = TNEANet::New();
  int i;
  for (i = NNodes - 1; i >= 0; i--) {
    Graph->AddNode(i);
  }
  // create the edges 
  int x, y;
  for (i = NEdges - 1; i >= 0; i--) {
    x = (long) (drand48() * NNodes);
    y = (long) (drand48() * NNodes);
    Graph->AddEdge(x, y, i);
  }

  // create attributes and fill all nodes
  TStr attr1 = "str";
  TStr attr2 = "int";
  TStr attr3 = "float";
  TStr attr4 = "default";

  Graph->AddIntAttrDatN(3, 3*2, attr2);
  Graph->AddIntAttrDatN(50, 50*2, attr2);

  Graph->AddFltAttrDatN(5, 3.41, attr3);
  Graph->AddFltAttrDatN(50, 2.718, attr3);

  Graph->AddStrAttrDatN(10, "abc", attr1);
  Graph->AddStrAttrDatN(20, "def", attr1);

  TFOut outstream(Filename);
  Graph->Save(outstream);
  return Graph;
}

void
checkTNEANetCorrect(PNEANet g, PNEANet g2) {
  EXPECT_EQ(g->GetNodes(), g2->GetNodes());
  EXPECT_EQ(g->GetEdges(), g2->GetEdges());
  TStr attr2 = "int";
  TStr attr3 = "float";
  TStr attr1 = "str";
  EXPECT_EQ(g->GetNAIntI(attr2, 3).GetDat(), g2->GetNAIntI(attr2, 3).GetDat());
  EXPECT_EQ(g->GetNAIntI(attr2, 50).GetDat(), g2->GetNAIntI(attr2, 50).GetDat());
  EXPECT_EQ(g->GetNAFltI(attr3, 5).GetDat(), g2->GetNAFltI(attr3, 5).GetDat());
  EXPECT_EQ(g->GetNAFltI(attr3, 50).GetDat(), g2->GetNAFltI(attr3, 50).GetDat());
  EXPECT_EQ(g->GetNAStrI(attr1, 10).GetDat().LastCh(), g2->GetNAStrI(attr1, 10).GetDat().LastCh());
  EXPECT_EQ(g->GetNAStrI(attr1, 20).GetDat().LastCh(), g2->GetNAStrI(attr1, 20).GetDat().LastCh());
}

// Tests saving and loading of undirected and directed graphs
TEST(SHMTest, LoadTNeanet) {
  TStr Filename("test.graph");
  PNEANet g = writeTNEANet(Filename);
  TShMIn ShMIn(Filename);
  PNEANet g2 = TNEANet::LoadShM(ShMIn);
  checkTNEANetCorrect(g, g2);
}

template <class PNet>
void checkNetworkCorrect(PNet Saved, PNet Loaded) {
  EXPECT_EQ(Saved->GetNodes(), Loaded->GetNodes());
  EXPECT_EQ(Saved->GetEdges(), Loaded->GetEdges());
  TAttrType AttrTypeS;
  TAttrType AttrTypeL;
  TInt AttrIdS;
  TInt AttrIdL;

  Saved->GetSAttrIdN(TStr("TestInt"), AttrIdS, AttrTypeS);
  Loaded->GetSAttrIdN(TStr("TestInt"), AttrIdL, AttrTypeL);
  EXPECT_EQ(AttrTypeS, AttrTypeL);
  EXPECT_EQ(AttrIdS, AttrIdL);

  Saved->GetSAttrIdN(TStr("TestFlt"), AttrIdS, AttrTypeS);
  Loaded->GetSAttrIdN(TStr("TestFlt"), AttrIdL, AttrTypeL);
  EXPECT_EQ(AttrTypeS, AttrTypeL);
  EXPECT_EQ(AttrIdS, AttrIdL);

  Saved->GetSAttrIdN(TStr("TestStr"), AttrIdS, AttrTypeS);
  Loaded->GetSAttrIdN(TStr("TestStr"), AttrIdL, AttrTypeL);
  EXPECT_EQ(AttrTypeS, AttrTypeL);
  EXPECT_EQ(AttrIdS, AttrIdL);
}


template <class PNet>
PNet writeNetwork(TStr Filename) {
  int NNodes = 1000;
  int NEdges = 1000;
  PNet Graph = PNet::TObj::New();
  int i;
  for (i = 0; i < NNodes; i++) {
    Graph->AddNode(i);
  }
  int NCount = NEdges;
  int x,y;
  while (NCount > 0) {
    x = (long) (drand48() * NNodes);
    y = (long) (drand48() * NNodes);
    if (x != y  &&  !Graph->IsEdge(x,y)) {
      Graph->AddEdge(x, y);
      NCount--;
    }
  }
  TInt AttrId;
  Graph->AddSAttrN("TestInt", atInt, AttrId);
  Graph->AddSAttrN("TestFlt", atFlt, AttrId);
  Graph->AddSAttrN("TestStr", atStr, AttrId);
  TFOut outstream(Filename);
  Graph->Save(outstream);
  return Graph;
}

template <class PNet>
PNet readNetwork(TStr Filename) {
  TShMIn ShMIn(Filename);
  PNet g = PNet::TObj::LoadShM(ShMIn);
  return g;
}

template <class PNet>
void checkNetwork() {
  TStr Filename("test.graph");
  PNet g = writeNetwork<PNet>(Filename);
  PNet g2 = readNetwork<PNet>(Filename);
  checkNetworkCorrect<PNet>(g, g2);
}

// Tests saving and loading of undirected and directed graphs
TEST(SHMTest, LoadNetworks) {
  // Undirected network
  checkNetwork<PUndirNet>();
  // Directed network
  checkNetwork<PDirNet>();
  // only check graph functionality for the following networks
  CheckGraph< TPt<TNodeNet<TInt> > >();
  CheckGraph< TPt<TNodeEdgeNet<TInt, TInt> > >();
  CheckGraph< TPt<TNodeEDatNet<TInt, TInt> > >();
}

TEST(SHMTest, LoadTables) {
  TStr Filename("test.graph");

  TTableContext Context;
  // Create schema.
  Schema GradeS;
  GradeS.Add(TPair<TStr,TAttrType>("A", atStr));
  GradeS.Add(TPair<TStr,TAttrType>("B", atStr));
  GradeS.Add(TPair<TStr,TAttrType>("Quarter", atStr));
  GradeS.Add(TPair<TStr,TAttrType>("Grade 2011", atInt));
  GradeS.Add(TPair<TStr,TAttrType>("Grade 2012", atInt));
  GradeS.Add(TPair<TStr,TAttrType>("Grade 2013", atInt));
  TIntV RelevantCols;
  RelevantCols.Add(0); RelevantCols.Add(1); RelevantCols.Add(2);
  RelevantCols.Add(3); RelevantCols.Add(4); RelevantCols.Add(5);
  PTable p1 = TTable::LoadSS(GradeS, "table/grades.txt", &Context, RelevantCols);
  TFOut outstream(Filename);
  p1->Save(outstream);

  TShMIn Shmin(Filename);
  PTable p2 = TTable::LoadShM(Shmin, &Context);

  EXPECT_EQ(p1->GetNumRows().Val, p2->GetNumRows().Val);
  EXPECT_EQ(p1->GetNumValidRows().Val, p2->GetNumValidRows().Val); 
  EXPECT_EQ(p1->GetIntVal("Grade 2011", 0).Val, p2->GetIntVal("Grade 2011", 0).Val);
  EXPECT_EQ(p1->GetIntVal("Grade 2013", 4).Val, p2->GetIntVal("Grade 2013", 4).Val);
}