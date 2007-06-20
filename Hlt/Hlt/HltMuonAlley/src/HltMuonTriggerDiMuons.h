// $Id: HltMuonTriggerDiMuons.h,v 1.4 2007-06-20 16:12:57 hernando Exp $
#ifndef HLTMUONTRIGGERDIMUONS_H 
#define HLTMUONTRIGGERDIMUONS_H 1

// Include files
#include "HltBase/HltAlgorithm.h"
#include "HltBase/HltFunctions.h"

/** @class HltMuonAlleyTrigger HltMuonAlleyTrigger.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2006-09-07
 */
class HltMuonTriggerDiMuons : public HltAlgorithm {
public: 
  /// Standard constructor
  HltMuonTriggerDiMuons( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltMuonTriggerDiMuons( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  void makeDiMuonPair(const Hlt::TrackContainer& tcon1,
                      const Hlt::TrackContainer& tcon2,
                      // Estd::bifunction<Track,Track>& fun,
                      // Estd::filter<double>& cut,
                      Hlt::VertexContainer& vcon);
  
protected:

  //  double m_ptMin;

  //  HltHisto m_histoPt;
  // HltHisto m_histoPt1;

protected:

  

  Estd::filter<LHCb::Track>* _negMuonFil;
  Estd::filter<LHCb::Track>* _posMuonFil;
  Estd::filter<LHCb::RecVertex>*  _massAndDOCAAndIPCut;
  Estd::filter<LHCb::RecVertex>*  _massAndDOCACut;
  Estd::function<LHCb::Track>* _chargeFun;
  Hlt::TrackVertexBiFunction*  _ipFun;
  //Estd::filter<LHCb::Track>*   _ptCut;
  Hlt::TrackBiFunction*  _docaFun;
  

  Hlt::SortTrackByPt _sortByPT;
  int m_IPKey;
  int m_massKey;
  int m_DOCAKey;
  double m_minMassWithIP;
  double m_minMassNoIP;  
  double m_minIP;
  double m_maxDOCA;
  
  Hlt::TrackContainer m_otrack;
  Hlt::TrackContainer m_otrack2;

  Hlt::VertexContainer m_overtices;  
  Hlt::VertexContainer m_selevertices;
  Hlt::VertexCreator _vertexCreator;
  HltHisto h_mass;
  HltHisto h_DOCA;
  HltHisto h_IP;
  PatVertexContainer* m_patVertexBank;

  std::string m_selection2SummaryName;  
  int m_selection2SummaryID;
  //HltHisto h_patOutTrack;
};
#endif // HLTMUONTRIGGERDIMUONS_H
