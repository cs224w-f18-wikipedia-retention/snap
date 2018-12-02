#include <string.h>
#include "stdafx.h"
#include "rolx.h"

int main(int argc, char* argv[]) {
  Env = TEnv(argc, argv, TNotify::StdNotify);
  Env.PrepArgs(TStr::Fmt("Rolx. build: %s, %s. Time: %s", __TIME__, __DATE__, TExeTm::GetCurTm()));
  TExeTm ExeTm;
  Try
  const TStr InFNm = Env.GetIfArgPrefixStr("-i:", "graph.txt", "Input graph (one edge per line, tab/space separated)");
  PUNGraph Graph = TSnap::LoadEdgeList<PUNGraph>(InFNm, 0, 1);
  printf("extracting features...\n");
  TIntFtrH Features = ExtractFeatures(Graph);
  TIntIntH NodeIdMtxIdH = CreateNodeIdMtxIdxHash(Features);
  TFltVV V = ConvertFeatureToMatrix(Features, NodeIdMtxIdH);
  printf("saving features...\n");
  FPrintMatrix(V, "v.txt");
  printf("feature matrix is saved in v.txt\n");
  printf("saving node mappings: feature line -> node ID...\n");
  FPrintNodeMappings(NodeIdMtxIdH, "mappings.txt");
  printf("node mappings are saved to mappings.txt\n");
  Catch
  printf("\nrun time: %s (%s)\n", ExeTm.GetTmStr(), TSecTm::GetCurTm().GetTmStr().CStr());
  return 0;
}
