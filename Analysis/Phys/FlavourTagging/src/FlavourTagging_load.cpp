//====================================================================
// FlavourTagging_load.cpp
//--------------------------------------------------------------------
//
//	Package   : FlavourTagging
//
//	Author    : Marco Musy
//====================================================================

#include "GaudiKernel/DeclareFactoryEntries.h"
DECLARE_FACTORY_ENTRIES(FlavourTagging) {

  DECLARE_ALGORITHM(BTagging);
  DECLARE_ALGORITHM(CheatedSelection);
  DECLARE_ALGORITHM(BTaggingMonitor);
  DECLARE_TOOL(BTaggingTool);
  DECLARE_TOOL(TaggerMuonTool);
  DECLARE_TOOL(TaggerElectronTool);
  DECLARE_TOOL(TaggerKaonOppositeTool);
  DECLARE_TOOL(TaggerKaonSameTool);
  DECLARE_TOOL(TaggerPionSameTool);
  DECLARE_TOOL(NNetTool);
  DECLARE_TOOL(SVertexTool);
  DECLARE_TOOL(SVertexNNTool);
  DECLARE_TOOL(SVertexABTool);
}
