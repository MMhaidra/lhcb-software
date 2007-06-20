// $Id: MuonTConfWithT.cpp,v 1.2 2007-06-20 16:11:55 hernando Exp $
// Include files 

// from Gaudi

#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"
// local
#include "MuonTConfWithT.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonTConfWithT
//
// 2007-02-14 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuonTConfWithT );

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonTConfWithT::MuonTConfWithT( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
    declareProperty("OutputMuonTracksName"   ,
                  m_outputMuonTracksName = "Hlt/Tracks/ConfirmedTMuon");

  declareProperty("nSigma",m_nsigma=5);
  declareProperty("debugInfo", m_debugInfo = false);

}
//=============================================================================
// Destructor
//=============================================================================
MuonTConfWithT::~MuonTConfWithT() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MuonTConfWithT::initialize() {
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  
  m_prepareMuonSeed = tool<IPrepareMuonTSeed>("PrepareMuonTSeed");

  m_TsaConfirmTool=tool<ITrackConfirmTool>( "TsaConfirmTool" );
  m_outputMuonTracks =
    m_patDataStore->createTrackContainer( m_outputMuonTracksName, 20 );
 
  
 

  m_myState = new State();

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonTConfWithT::execute() {

  debug() << "==> Execute" << endmsg;
  setFilterPassed(true);

  m_countEvent++;

//info()<<" sono qui "<<endreq;
   std::vector<LHCb::Track*>* foundTracksTmp = new std::vector<LHCb::Track*>;
  foundTracksTmp->reserve(50);
//info()<<" e ora qui"<<endreq;
  
  m_inputL0Muons = get<L0MuonCandidates>(L0MuonCandidateLocation::Default); 

  int muonCounter=0;  

  std::vector<LHCb::Track*>::iterator iterTrack;
  m_totMuonInput=m_totMuonInput+m_patInputTracks->size();
int nummu=0;  
  for ( std::vector<Track*>::const_iterator itT = m_patInputTracks->begin();
        m_patInputTracks->end() != itT; itT++ ) {
    
    
    Track* pTrack = (*itT);




    std::vector<LHCbID> muonTiles=
      pTrack->lhcbIDs();
    MuonTileID tileM2=muonTiles[0].muonID();
    MuonTileID tileM3=muonTiles[1].muonID();
    
    //reject the muon tracks that share the M3 pad with one L0Muon    
    L0MuonCandidates::const_iterator itL0Mu;
    std::vector<LHCb::Track*>::iterator iterTrack;
    bool L0clone=false;
    //int nummu=0;
    for (itL0Mu = m_inputL0Muons->begin();
         itL0Mu!=m_inputL0Muons->end();
         itL0Mu++){
      //  std::vector<MuonTileID> list_of_tile= (*itL0Mu)->muonTileIDs(1);
      //MuonTileID tileM2=*(list_of_tile.begin());
      std::vector<MuonTileID> list_of_tileM3= (*itL0Mu)->muonTileIDs(2);
      MuonTileID L0tileM3=*(list_of_tileM3.begin());
      if(tileM3==L0tileM3)L0clone=true;
      
    }
//    if(L0clone)info()<<" reject "<<nummu<<endreq;
    nummu++;		
    if(L0clone)continue;
    m_totMuonAccepted++;  

    muonCounter++;

    if(m_debugInfo) info()<<"Processing muon number "<<muonCounter<<endmsg;
    
    LHCb::State myState;
    StatusCode sctmp = m_prepareMuonSeed->prepareSeed( *(pTrack) , myState );
    if( sctmp.isFailure() ){
      err()<<"Failed to prepare the seed"<<endmsg;
    }
  //  info()<<" dopo prepare "<<endreq;
    //play with seeding tool
    m_TsaConfirmTool->tracks( myState , *foundTracksTmp );
   // info()<<" qopo tsa "<<endreq;
    for( iterTrack = foundTracksTmp->begin();
       iterTrack != foundTracksTmp->end() ;
       iterTrack++ ) {
       //info()<<" track "<<endreq;
      LHCb::Track* fitTrack =  (*iterTrack)->clone();
      fitTrack->addToLhcbIDs(tileM2);
      fitTrack->addToLhcbIDs(tileM3);
 
      Track* outTr = m_outputMuonTracks->newEntry();
     // info()<<" muon "<<endreq;   
      outTr->copy(*fitTrack);
      outTr->setFlag(Track::L0Candidate,false);

      m_outputTracks->push_back(outTr);
     // info()<<" hlt container"<<endreq;
//      setFilterPassed(true);
      delete  (*iterTrack);
   
    }

    if(m_debugInfo)
 info()<<"Tool found : "<<foundTracksTmp->size()
                           <<" tracks for the seed number "<<muonCounter<<" !"<<endmsg;
     foundTracksTmp->clear();

    
  }//end loop muons


  delete foundTracksTmp;
  setFilterPassed(true);
//info()<<" output size "<< m_outputTracks->size()<<" "<< 
//m_outputMuonTracks->size()<<endreq;
 
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonTConfWithT::finalize() {

  debug() << "==> Finalize" << endmsg;
  float rate_input=m_totMuonInput/m_countEvent;
  float rate_accepted=m_totMuonAccepted/m_countEvent;
  info()<<" average muon tracks in input "<< rate_input<<endreq;
  info()<<" average muon tracks after clone rejection "<< 
  rate_accepted<<endreq;

  return HltAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
