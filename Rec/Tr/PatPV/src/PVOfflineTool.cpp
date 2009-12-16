// $Id: PVOfflineTool.cpp,v 1.6 2009-12-16 11:51:52 witekma Exp $
// Include files:
// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/RecVertex.h"
// Local
#include "PVOfflineTool.h"

DECLARE_TOOL_FACTORY(PVOfflineTool);

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PVOfflineTool::PVOfflineTool(const std::string& type,
                             const std::string& name,
                             const IInterface* parent)
  : GaudiTool(type,name,parent) {
  declareInterface<IPVOfflineTool>(this);
  declareProperty("RequireVelo",   m_requireVelo   = true);
  declareProperty("SaveSeedsAsPV", m_saveSeedsAsPV = false);
  declareProperty("InputTracks",   m_inputTracks);
  declareProperty("PVFitterName",  m_pvFitterName = "LSAdaptPVFitter");
  declareProperty("PVSeedingName", m_pvSeedingName = "PVSeedTool");
}

//=========================================================================
// Initialize
//=========================================================================
StatusCode PVOfflineTool::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;
  // Access PVFitterTool
  m_pvfit = tool<IPVFitter>(m_pvFitterName,this);
  if(!m_pvfit) {
    err() << "Unable to retrieve the PVFitterTool" << endmsg;
    return  StatusCode::FAILURE;
  }
  // Access PVSeedTool
  m_pvSeedTool = tool<IPVSeeding>(m_pvSeedingName,this);
  if(!m_pvSeedTool) {
    err() << "Unable to retrieve the PV seeding tool " << m_pvSeedingName << endmsg;
    return  StatusCode::FAILURE;
  }
  // Input tracks
  if(m_inputTracks.size() == 0) { 
     m_inputTracks.push_back(LHCb::TrackLocation::Default);
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
// Destructor
//=============================================================================
PVOfflineTool::~PVOfflineTool() {}

//=============================================================================
// reconstruct PV for a given seed
//=============================================================================
StatusCode PVOfflineTool::reconstructSinglePV(const Gaudi::XYZPoint xyzseed,
					      LHCb::RecVertex& outvtx)  
{
  std::vector<const LHCb::Track*> rtracks;
  readTracks(rtracks); 

  StatusCode scvfit = m_pvfit->fitVertex( xyzseed, rtracks, outvtx );
  return scvfit;
}
//=============================================================================
// reconstruct PV for a given seed using tracks from the list
//=============================================================================
StatusCode PVOfflineTool::reconstructSinglePVFromTracks(const Gaudi::XYZPoint xyzseed,
				        std::vector<const LHCb::Track*>& tracks2use,
                                        LHCb::RecVertex& outvtx)  
{
  StatusCode scvfit = m_pvfit->fitVertex( xyzseed, tracks2use, outvtx );
  return scvfit;
}

//=============================================================================
// reconstruct PV for a given seed with a list of tracks to be excluded
//=============================================================================
StatusCode PVOfflineTool::reDoSinglePV(const Gaudi::XYZPoint xyzseed,
				 std::vector<const LHCb::Track*>& tracks2exclude,
				 LHCb::RecVertex& outvtx)
{

  std::vector<const LHCb::Track*> rtracks;
  readTracks(rtracks); 
  if(tracks2exclude.size()>0) {
    removeTracksByLHCbIDs(rtracks, tracks2exclude);
  }
  StatusCode scvfit = m_pvfit->fitVertex( xyzseed, rtracks, outvtx );
  return scvfit;

}


//=============================================================================
// multi vtx search and fit. Return new vtx after track removal
//=============================================================================
StatusCode PVOfflineTool::reDoMultiPV(const LHCb::RecVertex& invtx, 
				      std::vector<const LHCb::Track*>& tracks2exclude, 
				      LHCb::RecVertex& outvtx)
{
  std::vector<LHCb::RecVertex> outvtxVec;

  std::vector<const LHCb::Track*> rtracks;
  readTracks(rtracks); 

  if(tracks2exclude.size()>0) {
    removeTracksByLHCbIDs(rtracks, tracks2exclude);
  }

  StatusCode scvfit = reconstructMultiPVFromTracks(rtracks,outvtxVec); 

  if(scvfit != StatusCode::SUCCESS) return scvfit;
  // check which vtx corresponds to input vtx
  StatusCode scmatch = matchVtxByTracks(invtx, outvtxVec, outvtx);
  return scmatch;
}

//=============================================================================
// multi vtx search and fit with tracks from default location
//=============================================================================
StatusCode PVOfflineTool::reconstructMultiPV(
				std::vector<LHCb::RecVertex>& outvtxvec)  
{
  std::vector<const LHCb::Track*> rtracks;
  readTracks(rtracks); 
  StatusCode scvfit = reconstructMultiPVFromTracks(rtracks,outvtxvec); 
  return scvfit;
}

//=============================================================================
// multi vtx search and fit with tracks specified
//=============================================================================
StatusCode PVOfflineTool::reconstructMultiPVFromTracks(
                                std::vector<const LHCb::Track*>& tracks2use,
				std::vector<LHCb::RecVertex>& outvtxvec)  
{

  std::vector<const LHCb::Track*> rtracks;
  rtracks = tracks2use;

  outvtxvec.clear();

  // monitor quality of zseeds. Save them as fake PVs.
//-//  if(m_saveSeedsAsPV) {
//-//    storeDummyVertices(seeds, rtracks, outvtxvec);
//-//    return StatusCode::SUCCESS;
//-//  }


  int nvtx_before = -1;
  int nvtx_after  =  0;
  while ( nvtx_after > nvtx_before ) {
    nvtx_before = outvtxvec.size();
    // reconstruct vertices
    std::vector<Gaudi::XYZPoint>::iterator is;

    std::vector<Gaudi::XYZPoint> seeds;
    m_pvSeedTool->getSeeds(rtracks, seeds);  

    for (is = seeds.begin(); is != seeds.end(); is++) {
      LHCb::RecVertex recvtx;
      //    Gaudi::XYZPoint *pxyz = &is;
      StatusCode scvfit = m_pvfit->fitVertex( *is, rtracks, recvtx );
      removeTracksUsedByVertex(rtracks, recvtx);
      if(scvfit == StatusCode::SUCCESS) {
        bool isSepar = separatedVertex(recvtx,outvtxvec);
        if ( isSepar ) {
          outvtxvec.push_back(recvtx);
        }
      }
    }
    nvtx_after = outvtxvec.size();   
  }

  return StatusCode::SUCCESS;

}


bool PVOfflineTool::separatedVertex( LHCb::RecVertex& rvtx, std::vector<LHCb::RecVertex>& outvtxvec) {

  if(outvtxvec.size() < 1 ) return true;

  bool vsepar = true;
  double sig2min = 1e10;
  std::vector<LHCb::RecVertex>::iterator itv, itvclosest;
  for (itv = outvtxvec.begin(); itv != outvtxvec.end(); itv++) {
    double dist2v = (rvtx.position() - (*itv).position()).Mag2();
    double sigma2z_1 =   rvtx.covMatrix()(2,2); 
    double sigma2z_2 = (*itv).covMatrix()(2,2); 
    double sig2 = dist2v/(sigma2z_1+sigma2z_2);
    if ( sig2 < sig2min ) {
      itvclosest = itv;  
      sig2min = sig2;
    }
  }

  if ( sig2min < 9. ) vsepar = false;
  if ( sig2min < 81. && rvtx.tracks().size() < 9 ) vsepar = false;
  return vsepar;

}

//=============================================================================
// Read tracks
//=============================================================================
void PVOfflineTool::readTracks(std::vector<const LHCb::Track*>& rtracks) 
{

  if(msgLevel(MSG::VERBOSE)) {
    verbose() << "readTracks method" << endmsg;
  }

  unsigned int ntrall = 0;
  std::vector<std::string>::iterator itTrName;
  for(itTrName = m_inputTracks.begin(); itTrName != m_inputTracks.end(); 
      itTrName++) {
    // Retrieving tracks
    LHCb::Tracks* stracks = get<LHCb::Tracks>(*itTrName);
    if(!stracks) {
      warning() << "Tracks not found at location: " << *itTrName << endmsg; 
      continue;
    }
    ntrall += stracks->size();
    // Read parameters
    for(LHCb::Tracks::iterator istrack = stracks->begin(); 
        stracks->end() != istrack; istrack++) {
      const LHCb::Track* str = (*istrack);
      if ( str->hasVelo() ) {
     	  rtracks.push_back(*istrack);
      } else if ( ! m_requireVelo ) {
        rtracks.push_back(*istrack);
      }
    }
  }
  
  if(msgLevel(MSG::DEBUG)) {
    debug() << "readTracks: " << rtracks.size() << endmsg;
  }
  
}

//=============================================================================
// removeTracks
//=============================================================================
void PVOfflineTool::removeTracks(std::vector<const LHCb::Track*>& tracks, 
                                 std::vector<const LHCb::Track*>& tracks2remove)
{
  if(msgLevel(MSG::VERBOSE)) {
    verbose() << " removeTracks method" << endmsg;
  }

  if(msgLevel(MSG::DEBUG)) {
    debug() << "removeTracks. Input number of tracks: " << tracks.size() 
            << "  To remove:" << tracks2remove.size() << endmsg;
  }

  std::vector<const LHCb::Track*>::iterator itr;
  for(itr = tracks2remove.begin(); itr != tracks2remove.end(); itr++) {
    const LHCb::Track* ptr = (*itr);
    std::vector<const LHCb::Track*>::iterator itf = std::find(tracks.begin(),tracks.end(), ptr); 
    if(itf != tracks.end()) {
      tracks.erase(itf);
    }
  }

  if(msgLevel(MSG::DEBUG)) {
    debug() << "removeTracks. Output number of tracks: " << tracks.size() << endmsg;
  }
}

//=============================================================================
// removeTracks
//=============================================================================
void PVOfflineTool::removeTracksByLHCbIDs(std::vector<const LHCb::Track*>& tracks, 
                                  std::vector<const LHCb::Track*>& tracks2remove)
{
  if(msgLevel(MSG::VERBOSE)) {
    verbose() << " removeTracks method" << endmsg;
  }

  if(msgLevel(MSG::DEBUG)) {
    debug() << "removeTracksByLHCbIDs. Input number of tracks: " << tracks.size() << endmsg;
  }

  std::vector<const LHCb::Track*>::iterator itr1,itr2;
  for(itr1 = tracks2remove.begin(); itr1 != tracks2remove.end(); itr1++) {
    const LHCb::Track* ptr1 = (*itr1);

    for (itr2 = tracks.begin(); itr2 != tracks.end(); itr2++) {
      const LHCb::Track* ptr2 = (*itr2);

      // Calculate the number of common LHCbIDs
      const std::vector< LHCb::LHCbID > & ids1 = ptr1->lhcbIDs();
      const std::vector< LHCb::LHCbID > & ids2 = ptr2->lhcbIDs();

      std::vector< LHCb::LHCbID >::const_iterator ih1,ih2;

      int nhit2 = 0;
      int nCommon =0;
      for ( ih2 = ids2.begin(); ih2 != ids2.end(); ++ih2 ) {
        if ( !ih2->isVelo()) continue;
        nhit2++;
        for ( ih1 = ids1.begin(); ih1 != ids1.end(); ++ih1 ) {
          if ( !ih1->isVelo()) continue;
	  //          if ( ids1[i1].channelID() == ids2[i2].channelID() ) {
          if ( *ih1 == *ih2 ) {
            ++nCommon;
            break;
          }
        }
      }

      double rate = 0.;
      if(nhit2>0) rate = 1.0*nCommon/nhit2;
      if(rate>0.99) {
        tracks.erase(itr2);
        break;          
      }
    } // over tracks
  } // over tracks2remove

  if(msgLevel(MSG::DEBUG)) {
    debug() << "removeTracksByLHCbIDs. Output number of tracks: " << tracks.size() << endmsg;
  }
}

//=============================================================================
// removeTracksUsedByVertex
//=============================================================================
void PVOfflineTool::removeTracksUsedByVertex(std::vector<const LHCb::Track*>& tracks,
                                             LHCb::RecVertex& rvtx)

{
  if(msgLevel(MSG::VERBOSE)) {
    verbose() << " removeTracksUsedByVertex method" << endmsg;
  }

  std::vector<const LHCb::Track*> tracks2remove;
  //  std::vector<LHCb::Track*>::iterator it;
  LHCb::Tracks::const_iterator it;
  SmartRefVector< LHCb::Track >  vtx_tracks = rvtx.tracks();
  for (unsigned int it = 0; it < vtx_tracks.size(); it++ ) {
    const LHCb::Track* ptr =  vtx_tracks[it];
    tracks2remove.push_back(ptr); 
  }
  removeTracks(tracks, tracks2remove);
}

//=============================================================================
// Store dummy vertices
//=============================================================================
void PVOfflineTool::storeDummyVertices(std::vector<Gaudi::XYZPoint>& seeds,
                                       std::vector<const LHCb::Track*> rtracks, 
                                       std::vector<LHCb::RecVertex>& outvtxvec)
{
  std::vector<Gaudi::XYZPoint>::iterator itseed;
  for (itseed = seeds.begin(); itseed != seeds.end(); itseed++) {
    LHCb::RecVertex tVertex;
    tVertex.setPosition( *itseed );
    Gaudi::SymMatrix3x3 errMat;
    for(int i = 0; i < 3; i++) {
      for(int j = 0; j < 3; j++) {
        errMat(i,j) = 1.0;
      }
    }
    tVertex.setCovMatrix(errMat);
    tVertex.setNDoF(1);
    tVertex.setChi2(99999.0);
    // Fill close tracks
    std::vector<const LHCb::Track*>::iterator it;
    for(it = rtracks.begin(); rtracks.end() != it; it++) {
      const LHCb::Track * ptr = (*it);
      double zc = zCloseBeam(ptr);
      if(fabs(zc - itseed->Z()) < 3.0 * Gaudi::Units::mm) {
        tVertex. addToTracks(ptr);
      }
    }
    outvtxvec.push_back(tVertex);
  }
}

double PVOfflineTool::zCloseBeam(const LHCb::Track* track){
   

  Gaudi::XYZVector unitVect;
  unitVect = track->firstState().slopes().Unit();
  LHCb::State& stateG = track->firstState(); 

  double zclose = stateG.z() - unitVect.z() * 
         (unitVect.x() * stateG.x() + unitVect.y() * stateG.y()) /
         (1.0 - pow(unitVect.z(),2)); 

  return zclose;

}



//=============================================================================
// Match vtx in vector of vtx by matching tracks
//=============================================================================
StatusCode PVOfflineTool::matchVtxByTracks(const LHCb::RecVertex& invtx,  
					   std::vector<LHCb::RecVertex>& outvtxvec, 
					   LHCb::RecVertex& outvtx) 
{
  typedef const SmartRefVector<LHCb::Track>      PVTRACKS ;
  PVTRACKS& tracksIn =  invtx.tracks();
  PVTRACKS::const_iterator itin,itout;
  int ntrin = tracksIn.size();

  if( ntrin == 0 ) return StatusCode::FAILURE;

  double lastrate = 0.;
  std::vector<LHCb::RecVertex>::iterator itv, itbest;
  itbest = outvtxvec.end();

  for (itv = outvtxvec.begin(); itv != outvtxvec.end(); itv++) {
    int imatch = 0;
    PVTRACKS& tracksOut =  (*itv).tracks();
    for ( itin = tracksIn.begin(); itin !=tracksIn.end(); itin++) {
      itout = std::find(tracksOut.begin(),tracksOut.end(), *itin );
      if(itout != tracksOut.end() ) imatch++;
    }
    double rate = 1.0*imatch/ntrin;
    if (rate > lastrate ) {
      lastrate = rate;
      itbest = itv;
    }
  }

  if ( itbest == outvtxvec.end() && lastrate < 0.3 ) {
    return StatusCode::FAILURE;
  }

  outvtx = *itbest;
  debug() << " vtx succesfully matched at tracks rate: " << lastrate << endmsg;
  return StatusCode::SUCCESS;
}

