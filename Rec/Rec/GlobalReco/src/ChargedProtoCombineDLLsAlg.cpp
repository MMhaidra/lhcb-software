
//-----------------------------------------------------------------------------
/** @file ChargedProtoCombineDLLsAlg.cpp
 *
 * Implementation file for algorithm ChargedProtoCombineDLLsAlg
 *
 * CVS Log :-
 * $Id: ChargedProtoCombineDLLsAlg.cpp,v 1.4 2006-12-22 10:50:45 odescham Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 15/11/2006
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "ChargedProtoCombineDLLsAlg.h"

//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedProtoCombineDLLsAlg );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoCombineDLLsAlg::ChargedProtoCombineDLLsAlg( const std::string& name,
                                                        ISvcLocator* pSvcLocator )
  : GaudiAlgorithm ( name , pSvcLocator )
    ,m_elCombDll(0xFFFF)
    ,m_muCombDll(0xFFFF)
    ,m_prCombDll(0xFFFF)
    ,m_piCombDll(0xFFFF)
    ,m_kaCombDll(0xFFFF){
      
  // Job Options
  declareProperty( "ProtoParticleLocation",
                   m_protoPath = LHCb::ProtoParticleLocation::Charged );
  declareProperty("ElectronDllDisable"  , m_elDisable);
  declareProperty("MuonDllDisable"      , m_muDisable);
  declareProperty("KaonDllDisable"      , m_kaDisable);
  declareProperty("ProtonDllDisable"    , m_piDisable);
  declareProperty("PionllDisable"       , m_prDisable);

  m_maskTechnique["Rich"] = 0x1;
  m_maskTechnique["Muon"] = 0x2;
  m_maskTechnique["Ecal"] = 0x4;
  m_maskTechnique["Hcal"] = 0xF;
  m_maskTechnique["Prs"]  = 0x10;
  m_maskTechnique["Spd"]  = 0x20;
  m_maskTechnique["Brem"] = 0x40;
  m_maskTechnique["Calo"] = 0x7C;
}

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoCombineDLLsAlg::~ChargedProtoCombineDLLsAlg() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ChargedProtoCombineDLLsAlg::initialize()
{
  const StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;
  
  for(std::vector<std::string>::iterator itech = m_elDisable.begin(); itech != m_elDisable.end(); ++itech){
    m_elCombDll &= ~m_maskTechnique[(*itech)] ;
  }
  for(std::vector<std::string>::iterator itech = m_muDisable.begin(); itech != m_muDisable.end(); ++itech){
    m_muCombDll &= ~m_maskTechnique[(*itech)] ;
  }
  for(std::vector<std::string>::iterator itech = m_prDisable.begin(); itech != m_prDisable.end(); ++itech){
    m_prCombDll &= ~m_maskTechnique[(*itech)] ;
  }
  for(std::vector<std::string>::iterator itech = m_piDisable.begin(); itech != m_piDisable.end(); ++itech){
    m_piCombDll &= ~m_maskTechnique[(*itech)] ;
  }
  for(std::vector<std::string>::iterator itech = m_kaDisable.begin(); itech != m_kaDisable.end(); ++itech){
    m_kaCombDll &= ~m_maskTechnique[(*itech)] ;
  }
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoCombineDLLsAlg::execute()
{
  // Load the charged ProtoParticles
  LHCb::ProtoParticles * protos = get<LHCb::ProtoParticles>( m_protoPath );

  // Loop over the protos
  for ( LHCb::ProtoParticles::iterator iP = protos->begin();
        iP != protos->end(); ++iP )
  {

    // Does this ProtoParticle have any PID information ?
    if ( !(*iP)->hasInfo(LHCb::ProtoParticle::NoPID) )
    {

      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "ProtoParticle before : " << **iP << endreq;
      }

      // Combined DLL data object for this proto
      CombinedLL combDLL(0);

      // Add any RICH info
      addRich(*iP,combDLL);

      // Add any MUON info
      addMuon(*iP,combDLL);

      // Add any CALO info
      addCalo(*iP,combDLL);

      // Store the final combined DLL values into the ProtoParticle
      (*iP)->addInfo( LHCb::ProtoParticle::CombDLLe,  combDLL.elDLL-combDLL.piDLL );
      (*iP)->addInfo( LHCb::ProtoParticle::CombDLLmu, combDLL.muDLL-combDLL.piDLL );
      (*iP)->addInfo( LHCb::ProtoParticle::CombDLLpi, 0 ); // by definition
      (*iP)->addInfo( LHCb::ProtoParticle::CombDLLk,  combDLL.kaDLL-combDLL.piDLL );
      (*iP)->addInfo( LHCb::ProtoParticle::CombDLLp,  combDLL.prDLL-combDLL.piDLL );

      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "ProtoParticle after  : " << **iP << endreq;
      }

    } // Has PID info

  } // loop over protos

  return StatusCode::SUCCESS;
}

void
ChargedProtoCombineDLLsAlg::addRich( LHCb::ProtoParticle * proto, CombinedLL & combDLL ){
  // Add RICH Dll information. Default, for when not available is 0
  int richMask = m_maskTechnique["Rich"];
  if( 0 !=  m_elCombDll & richMask )combDLL.elDLL += proto->info ( LHCb::ProtoParticle::RichDLLe  , 0 );
  if( 0 !=  m_muCombDll & richMask )combDLL.muDLL += proto->info ( LHCb::ProtoParticle::RichDLLmu , 0 );
  if( 0 !=  m_piCombDll & richMask )combDLL.piDLL += proto->info ( LHCb::ProtoParticle::RichDLLpi , 0 );
  if( 0 !=  m_kaCombDll & richMask )combDLL.kaDLL += proto->info ( LHCb::ProtoParticle::RichDLLk  , 0 );
  if( 0 !=  m_prCombDll & richMask )combDLL.prDLL += proto->info ( LHCb::ProtoParticle::RichDLLp  , 0 );
}

void
ChargedProtoCombineDLLsAlg::addMuon( LHCb::ProtoParticle * proto, CombinedLL & combDLL ){
  int muonMask = m_maskTechnique["Muon"];
  if( 0 != m_elCombDll  & muonMask )combDLL.elDLL += proto->info ( LHCb::ProtoParticle::MuonBkgLL,0 );
  if( 0 != m_muCombDll  & muonMask )combDLL.muDLL += proto->info ( LHCb::ProtoParticle::MuonMuLL, 0 );
  if( 0 != m_piCombDll  & muonMask )combDLL.piDLL += proto->info ( LHCb::ProtoParticle::MuonBkgLL,0 );
  if( 0 != m_kaCombDll  & muonMask )combDLL.kaDLL += proto->info ( LHCb::ProtoParticle::MuonBkgLL,0 );
  if( 0 != m_prCombDll  & muonMask )combDLL.prDLL += proto->info ( LHCb::ProtoParticle::MuonBkgLL,0 );
}

void
ChargedProtoCombineDLLsAlg::addCalo( LHCb::ProtoParticle * proto, CombinedLL & combDLL ){
  int ecalMask = m_maskTechnique["Ecal"];
  int hcalMask = m_maskTechnique["Hcal"];
  int prsMask  = m_maskTechnique["Prs"];
  int bremMask = m_maskTechnique["Brem"];
  // DLL(el)
  if( 0 != m_elCombDll & ecalMask )combDLL.elDLL += proto->info ( LHCb::ProtoParticle::EcalPIDe, 0  );
  if( 0 != m_elCombDll & hcalMask )combDLL.elDLL += proto->info ( LHCb::ProtoParticle::HcalPIDe, 0  );
  if( 0 != m_elCombDll & prsMask  )combDLL.elDLL += proto->info ( LHCb::ProtoParticle::PrsPIDe , 0  );
  if( 0 != m_elCombDll & bremMask )combDLL.elDLL += proto->info ( LHCb::ProtoParticle::BremPIDe, 0  );
  // DLL(mu)
  if( 0 != m_muCombDll & ecalMask )combDLL.muDLL += proto->info ( LHCb::ProtoParticle::EcalPIDmu, 0 );
  if( 0 != m_muCombDll & hcalMask )combDLL.muDLL += proto->info ( LHCb::ProtoParticle::HcalPIDmu, 0 );
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ChargedProtoCombineDLLsAlg::finalize()
{
  // execute base class finalise and return
  return GaudiAlgorithm::finalize();
}

//=============================================================================
