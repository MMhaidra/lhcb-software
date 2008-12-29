// $Id: HltL0MuonPrepare.cpp,v 1.15 2008-12-29 16:36:13 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include <algorithm>
#include "boost/foreach.hpp"


// local
#include "HltL0MuonPrepare.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltL0MuonPrepare
//
// 2006-07-28 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltL0MuonPrepare );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltL0MuonPrepare::HltL0MuonPrepare( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator, false )
  , m_maker(0)
  , m_selections(*this)
{
  //@TODO: need to automatically pick up L0 threshold somehow instead of a property...
  declareProperty("MinPt", m_PtMin = 0.1);
  m_selections.declareProperties( boost::assign::map_list_of( 1,std::string("TES:")+L0MuonCandidateLocation::Default) );
}
//=============================================================================
// Destructor
//=============================================================================
HltL0MuonPrepare::~HltL0MuonPrepare() {
} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltL0MuonPrepare::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorith

  m_selections.retrieveSelections();
  m_selections.registerSelection();
  
  m_maker = tool<IMuonSeedTool>("MuonSeedTool");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltL0MuonPrepare::execute() {

  Tracks* muons = new Tracks();
  put(muons, "Hlt/Track/"+m_selections.output()->id().str());

  BOOST_FOREACH( L0MuonCandidate* l0muon, *m_selections.input<1>()) {
    debug() << "l0pt " << l0muon->pt()<< endmsg;
    if (fabs(l0muon->pt()) < m_PtMin) continue;
    if (checkClone(l0muon)) {
        debug() << "is clone " << endmsg;
        continue;
    }
    std::auto_ptr<Track> track( new Track() );
    StatusCode sc = m_maker->makeTrack(*l0muon,*track);
    if (sc.isFailure()) return sc;
    //TODO: pushes both into IHltDataSvc (m_outputTracks) and into TES (muons)
    //      this should be possible to do through the IHltDataSvc...
    //      triggered by 'setDecision'...
    m_selections.output()->push_back(track.get());
    muons->insert(track.release());
  }

  if (msgLevel(MSG::DEBUG)) {
     debug() << "# Input: " << m_selections.input<1>()->size() 
             << " -> # Output: " << m_selections.output()->size() << endreq;
  }

  return StatusCode::SUCCESS;
}

namespace {
    class isInMuonStation : public std::unary_function<bool,LHCb::LHCbID> {
    public:
       isInMuonStation(unsigned int station) : m_station (station) {}
       bool operator()(const LHCb::LHCbID& id) { return id.isMuon() && id.muonID().station()==m_station; }
    private:
       unsigned int m_station;
    };
}

//=============================================================================
bool HltL0MuonPrepare::checkClone(L0MuonCandidate* muon)
{
  const MuonTileID& tileM1 = muon->muonTileIDs(0).front();
  const MuonTileID& tileM2 = muon->muonTileIDs(1).front();

  BOOST_FOREACH( Track* t, *m_selections.output() ) {
    const std::vector< LHCb::LHCbID >& ids= t->lhcbIDs();

    std::vector<LHCb::LHCbID>::const_iterator oldTileM1 = 
        std::find_if( ids.begin(), ids.end(), isInMuonStation(0));
    if (oldTileM1 == ids.end() || !(*oldTileM1 == tileM1) ) continue; // not found, or no match...

    std::vector<LHCb::LHCbID>::const_iterator oldTileM2 =
        std::find_if( ids.begin(), ids.end(), isInMuonStation(1));
    if (oldTileM2 != ids.end() &&   *oldTileM2 == tileM2 ) return true; // found, and match...
  }
  return false;
}
