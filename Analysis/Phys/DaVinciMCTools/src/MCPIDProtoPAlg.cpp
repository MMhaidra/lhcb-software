// $Id: MCPIDProtoPAlg.cpp,v 1.3 2002-07-26 19:44:57 gcorti Exp $
// Include files 
#include <memory>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// from EventSys
#include "Event/TrStoredTrack.h"
#include "Event/RichPID.h"
#include "Event/MuonID.h"
#include "Event/ProtoParticle.h"

// local
#include "MCPIDProtoPAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCPIDProtoPAlg
//
// 2002-07-08 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<MCPIDProtoPAlg>          s_factory ;
const        IAlgFactory& MCPIDProtoPAlgFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCPIDProtoPAlg::MCPIDProtoPAlg( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
  , m_electronPath( "Rec/Calo/Electrons" )
  , m_photonMatchName( "PhotonMatch" )
  , m_electronMatchName( "ElectronMatch" )
  , m_bremMatchName( "BremMatch" )
  , m_lastChiSqMax( 100 )
  , m_upstream( false )
  , m_trackAsctName( "Track2MCParticleAsct" )
{

  // Inputs
  declareProperty("TrStoredTracksInput", 
                  m_tracksPath = TrStoredTrackLocation::Default );
  declareProperty("RichPIDsInput",
                  m_richPath = RichPIDLocation::Default );
  declareProperty("MuonPIDsInput",
                  m_muonPath = MuonIDLocation::Default );
  declareProperty("ElectronsInput", m_electronPath );
  declareProperty("PhotonMatching",    m_photonMatchName );
  declareProperty("ElectronMatching", m_electronMatchName );
  declareProperty("BremMatching",     m_bremMatchName );
  
  // Outputs
  declareProperty("OutputData",
                  m_protoPath = ProtoParticleLocation::Charged );

  // Selections
  declareProperty("MaxChiSquare", m_lastChiSqMax = 1000 );
  declareProperty("UpstreamsTracks", m_upstream );
  declareProperty("TrackAsct", m_trackAsctName );
  
}

//=============================================================================
// Destructor
//=============================================================================
MCPIDProtoPAlg::~MCPIDProtoPAlg() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MCPIDProtoPAlg::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  StatusCode sc;
  IParticlePropertySvc* ppSvc;
  sc = service("ParticlePropertySvc", ppSvc);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to locate Particle Property Service" 
        << endreq;
    return sc;
  }
  ParticleProperty* partProp;

  partProp = ppSvc->find( "e+" );
  m_idElectron = partProp->jetsetID();

  partProp = ppSvc->find( "mu+" );
  m_idMuon = partProp->jetsetID();

  partProp = ppSvc->find( "pi+" );
  m_idPion = partProp->jetsetID();

  partProp = ppSvc->find( "K+" );
  m_idKaon = partProp->jetsetID();

  partProp = ppSvc->find( "p+" );
  m_idProton = partProp->jetsetID();

  // Retrieve track associator
  sc = toolSvc()->retrieveTool(
       "AssociatorWeighted<TrStoredTrack,MCParticle,double>", 
       m_trackAsctName, m_track2MCParticleAsct);
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Tracks associator not found" << endreq;
    return sc;
  }

  // Associators for CaloTables
  std::string matchType="AssociatorWeighted<CaloCluster,TrStoredTrack,float>";
  sc = toolSvc()->retrieveTool( matchType, m_photonMatchName, m_photonMatch );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Unable to retrieve " << matchType << endreq;
    return sc;
  }
  matchType = "AssociatorWeighted<CaloHypo,TrStoredTrack,float>";
  sc = toolSvc()->retrieveTool( matchType, m_electronMatchName,
                                m_electronMatch );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Unable to retrieve " << matchType << endreq;
    return sc;
  }
  matchType = "AssociatorWeighted<CaloHypo,TrStoredTrack,float>";
  sc = toolSvc()->retrieveTool( matchType, m_bremMatchName, m_bremMatch );
  if( !sc.isSuccess() ) {
    log << MSG::ERROR << "Unable to retrieve " << matchType << endreq;
    return sc;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MCPIDProtoPAlg::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  // Load stored tracks
  SmartDataPtr<TrStoredTracks> tracks ( eventSvc(), m_tracksPath );
  if( !tracks || 0 == tracks->size() ) {
    log << MSG::DEBUG << "Unable to retrieve TrStoredTracks at "
        << m_tracksPath << endreq;
    return StatusCode::FAILURE;
  }
  else {   
    log << MSG::DEBUG << "Successfully retrieved " << tracks->size()
        << " TrStoredTracks at " << m_tracksPath << endreq;
  }

  // Load richPid results
  bool richData;
  SmartDataPtr<RichPIDs> richpids ( eventSvc(), m_richPath );
  if( !richpids || 0 == richpids->size() ) {
    log << MSG::INFO  << "Failed to locate RichPIDs at "
        << m_richPath << endreq;
  }
  else {   
    log << MSG::DEBUG << "Successfully located " << richpids->size()
        << " RichPIDs at " << m_richPath << endreq;
    richData = true;
  }
  
  // Load muonPid results
  bool muonData;
  SmartDataPtr<MuonIDs> muonpids ( eventSvc(), m_muonPath );
  if( !muonpids || 0 == muonpids->size() ) {
    log << MSG::INFO << "Failed to locate MuonIDs at "
        << m_muonPath << endreq;
  }
  else {
    log << MSG::DEBUG << "Successfully located " << muonpids->size()
        << " MuonIDs at " << m_muonPath << endreq;
    muonData = true;
  }

  // Load Electron results
  bool caloData = false;
  SmartDataPtr<CaloHypos> electrons ( eventSvc(), m_electronPath );
  if( !electrons || 0 == electrons->size() ) {
    log << MSG::INFO << "Failed to locate CaloHypos at "
        << m_electronPath << endreq;
  }
  else {
    log << MSG::DEBUG << "Successfully located " << electrons->size()
        << " CaloHypp at " << m_electronPath << endreq;
    caloData = true;
  }

  /// Check the tables for electronID
  const PhotonTable* phtable = m_photonMatch->inverse();
  if( 0 == phtable ) { 
    log << MSG::DEBUG << "Table from PhotonMatch points to NULL";
    caloData = false;
  }
  const ElectronTable* etable = m_electronMatch->inverse();
  if( 0 == etable ) { 
    log << MSG::DEBUG << "Table from PhotonMatch points to NULL";
    caloData = false;
  }
  const BremTable* brtable = m_bremMatch->inverse();
  if( 0 == brtable ) { 
    log << MSG::DEBUG << "Table from PhotonMatch points to NULL";
    caloData = false;
  }

  // Prepare output container
  ProtoParticles* chprotos = new ProtoParticles();
  StatusCode sc = eventSvc()->registerObject( m_protoPath, chprotos );
  if( !sc.isSuccess() ) {
    delete chprotos; 
    log << MSG::ERROR  
        << "Unable to register ProtoParticles container in " 
        << m_protoPath << endreq;
    return sc;
  }

  // ProtoParticles should only be "good tracks"
  //   keep only Forward, Upstream and Matched tracks, no clones 
  //   reject tracks with fit errors
  // Note that history does not accumulate:: it is only the nput container
  // and then a track is classifed as a clone but not respect to what
  
  int unforwr=0; int unmatch=0; int unupstr=0;

  int countTrackProto = 0;
  int countRichProto = 0;
  int countMuonProto = 0;
  int countElectronProto = 0;
  int countAssociated = 0;
  TrStoredTracks::const_iterator iTrack;
  for( iTrack = tracks->begin(); tracks->end() != iTrack; ++iTrack ) {
    
    // Track satisfy criteria to make a ProtoParticle ?
    if( (*iTrack)->unique() ) {
      if( (*iTrack)->forward() )  unforwr++;
      if( (*iTrack)->match() )    unmatch++;
      if( (*iTrack)->upstream() ) unupstr++;
    }
    if( !keepTrack( (*iTrack) ) ) continue;
    
    ProtoParticle* proto = new ProtoParticle();
    proto->setTrack( *iTrack );
    double tkcharge = 0.0;
    int pidmc = 0;
    // Retrieve the MCParticle associated with this ProtoParticle
    MCParticle* mcPart = m_track2MCParticleAsct->associatedFrom( *iTrack );
    if( 0 != mcPart ) {
      countAssociated++;
      pidmc = mcPart->particleID().pid();
      tkcharge = (mcPart->particleID().threeCharge())/3.0;
    }
    else {   
      const TrStateP* tkstate = proto->trStateP();
      if( tkstate ) {
        tkcharge = proto->charge();
        pidmc = m_idPion * (int)tkcharge;
        log << MSG::DEBUG << "tkstate q/p =" << tkstate->qDivP() << endreq;
      }
    }
    if( 0 == tkcharge ) {
      log << MSG::DEBUG << "track charge = " << tkcharge << endreq;
      delete proto;
      continue;
    }
    
    countTrackProto++;

    // Set combined particle ID as from MC truth when available
    ProtoParticle::PIDInfoVector idinfovec;
    ProtoParticle::PIDInfoPair idinfo;
    idinfo.first = pidmc;
    idinfo.second = 1.0;
    idinfovec.push_back(idinfo);
    proto->setPIDInfo(idinfovec);

    ProtoParticle::PIDDetVector iddetvec;
    proto->setPIDDetectors(iddetvec);

    // Add RichPID to this ProtoParticle
    if( richData ) {
      StatusCode sc = addRich( richpids, proto );
      if( !sc.isFailure() ) {
        countRichProto++;
      }  
    }
    
    // Add MuonID to this ProtoParticle
    if( muonData ) {
      MuonIDs::const_iterator iMuon;
      for( iMuon = muonpids->begin(); muonpids->end() != iMuon; ++iMuon ) {
        const TrStoredTrack* track = (*iMuon)->idTrack();
        if( track == (*iTrack) ) {
          proto->setMuonPID( *iMuon );
          if( (*iMuon)->InAcceptance() && (*iMuon)->PreSelMomentum() ) {
            countMuonProto++;
            ProtoParticle::PIDDetPair iddet;
            iddet.first = ProtoParticle::MuonMuon;
            iddet.second = (*iMuon)->MuProb();
            proto->pIDDetectors().push_back(iddet);
            proto->setMuonBit(1);
          }          
          // The muon pid for track has been added so exit from loop
          break;
        }
      } // break here
    }
    
    // Add CaloElectrons to this ProtoParticle   
    if( caloData ) {  
      log << MSG::DEBUG << "Processing CaloInfo" << endreq;
      // Add the Electron hypothesis when available (no cuts at the moment)
      ElectronRange erange = etable->relations( *iTrack );
      if( !erange.empty() ) {
        countElectronProto++;
        CaloHypo* hypo = erange.begin()->to();
        proto->addToCalo( hypo );

        double chi2 = erange.begin()->weight();
        ProtoParticle::PIDDetPair iddet;
        iddet.first = ProtoParticle::CaloEMatch;
        iddet.second = chi2;
        proto->pIDDetectors().push_back(iddet);
        proto->setCaloeBit(1);
       
        // Add the CaloCluster chi2 (only lowest)
        PhotonRange phrange = phtable->relations( *iTrack );
        if( !phrange.empty() ) {
          chi2 = phrange.begin()->weight();
          iddet.first = ProtoParticle::CaloTrMatch;
          iddet.second = chi2;
          proto->pIDDetectors().push_back(iddet);
        }
        
        // Add Brem hypothesis and chi2 (only lowest)
        BremRange brrange = brtable->relations( *iTrack );
        if( !brrange.empty() ) {
          hypo = brrange.begin()->to();
          proto -> addToCalo( hypo );
          chi2 = brrange.begin()->weight();
          iddet.first = ProtoParticle::BremMatch;
          iddet.second = chi2;
          proto->pIDDetectors().push_back(iddet);
        }
      }
    }
    // Check is at least one particleID is beeing added otherwise set NoPID
    if( (0 == proto->richBit()) && (0 == proto->muonBit()) &&
        (0 == proto->caloeBit()) ) {
      proto->setNoneBit(1);
    }

    //chprotos->insert(proto.release());
    chprotos->insert(proto);
    
  }

  log << MSG::DEBUG << "Found " << (unforwr+unmatch+unupstr)
      << " forward + upstream + matched unique tracks" << endreq;
  log << MSG::DEBUG << "Found " << countTrackProto
      << " tracks of quality to produce ProtoParticles" << endreq;
  log << MSG::DEBUG << "Made " << countRichProto 
      << " ProtoParticle with RichPID " << endreq;
  log << MSG::DEBUG << "Made " << countMuonProto 
      << " ProtoParticle with MuonID " << endreq;
  log << MSG::DEBUG << "Number of ProtoParticles in TES is " 
      << chprotos->size() << endreq;

  for( ProtoParticles::iterator ip = chprotos->begin(); chprotos->end() != ip;
       ++ip ) {
    log << MSG::VERBOSE << "track = " << (*ip)->track() << endreq;
    log << MSG::VERBOSE << "charge = " << (*ip)->charge() << endreq;
    log << MSG::VERBOSE << "richid = " << (*ip)->richPID() << endreq;
    log << MSG::VERBOSE << "muonid = " << (*ip)->muonPID() << endreq;
    log << MSG::VERBOSE << "richhistory = " << (*ip)->richBit() << endreq;
    log << MSG::VERBOSE << "muonhistory = " << (*ip)->muonBit() << endreq;
    log << MSG::VERBOSE << "bestPID = " << (*ip)->bestPID() << endreq;
    for( ProtoParticle::PIDInfoVector::iterator id = (*ip)->pIDInfo().begin(); 
         (*ip)->pIDInfo().end()!=id; ++id ) {
      log << MSG::VERBOSE << "id = " << (*id).first << " , prob = " 
          << (*id).second  << endreq;
    }
    for( ProtoParticle::PIDDetVector::iterator 
           idd = (*ip)->pIDDetectors().begin(); 
         (*ip)->pIDDetectors().end()!=idd; ++idd ) {
      log << MSG::VERBOSE << "det = " << (*idd).first << " , value = " 
          << (*idd).second  << endreq;
    }
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MCPIDProtoPAlg::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}; 

//=============================================================================
//  keepTrack because of good quality
//=============================================================================
bool MCPIDProtoPAlg::keepTrack( const TrStoredTrack* track ) {

  bool keep = false;
  if( 0 != track ) {
    if( 0 == track->errorFlag() ) {
      if( track->lastChiSq() <= m_lastChiSqMax ) {
        if( track->unique() && (track->forward() || track->match()) ) {
          keep = true;
        }
        if( m_upstream && (track->unique() && track->upstream()) ) {
          keep = true;
        }
      }
    }
  }
  return keep;
};

//=============================================================================
// Add rich info to ProtoParticle
//=============================================================================
StatusCode MCPIDProtoPAlg::addRich( SmartDataPtr<RichPIDs>& richpids, 
                                    ProtoParticle* proto) {
 
  MsgStream log(msgSvc(), name());
  StatusCode sc = StatusCode::FAILURE;

  RichPIDs::const_iterator iRich;
  for( iRich = richpids->begin(); richpids->end() != iRich; ++iRich ) {
    const TrStoredTrack* track = (*iRich)->recTrack();
    if( track == proto->track() ) {
      sc = StatusCode::SUCCESS;
      proto->setRichPID( *iRich );

      // store Raw probabilities for RICH as detector info
      ProtoParticle::PIDDetVector& iddetvec = proto->pIDDetectors();
      ProtoParticle::PIDDetPair iddet;
      iddet.first = ProtoParticle::RichElectron;
      iddet.second = (*iRich)->particleRawProb(Rich::Electron);
      iddetvec.push_back(iddet);
      log << MSG::VERBOSE
          << "Rich " << Rich::Electron << " = " 
          << (*iRich)->particleRawProb(Rich::Electron)
          << endreq;
      
      iddet.first = ProtoParticle::RichMuon;
      iddet.second = (*iRich)->particleRawProb(Rich::Muon);
      iddetvec.push_back(iddet);
      log << MSG:: VERBOSE 
          << "Rich " << Rich::Muon << " = " 
          << (*iRich)->particleRawProb(Rich::Muon)
          << endreq;
      
      iddet.first = ProtoParticle::RichPion;
      iddet.second = (*iRich)->particleRawProb(Rich::Pion);
      iddetvec.push_back(iddet);
      log << MSG::VERBOSE
          << "Rich " << Rich::Pion << " = "
          << (*iRich)->particleRawProb(Rich::Pion)
          << endreq;
      
      iddet.first = ProtoParticle::RichKaon;
      iddet.second = (*iRich)->particleRawProb(Rich::Kaon);
      iddetvec.push_back(iddet);
      log << MSG::VERBOSE
          << "Rich " << Rich::Kaon << " = "
          << (*iRich)->particleRawProb(Rich::Kaon)
          << endreq;
      
          
      iddet.first = ProtoParticle::RichProton;
      iddet.second = (*iRich)->particleRawProb(Rich::Proton);
      iddetvec.push_back(iddet);
      log << MSG::VERBOSE 
          << "Rich " << Rich::Proton << " = "
          << (*iRich)->particleRawProb(Rich::Proton)
          << endreq;
      
      proto->setRichBit(1);
        
      break; // the rich pid for track has been added so exit from loop
    }
  } // break here
  
  return sc;
}

//=============================================================================
