// $Id: LoKi_MCMuon.cpp,v 1.1 2004-04-09 07:23:44 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/LoKi.h"
// ============================================================================

// ============================================================================
// ============================================================================


LOKI_ALGORITHM( LoKi_MCMuon ) 
{
  using namespace LoKi ;
  using namespace LoKi::Cuts ;
  using namespace LoKi::MCTypes ;
  
  // get all reconstructed muons
  Range muons = select( "Muons" , ABSID == 13 ) ;
  
  // get all MC particles from TES 
  typedef ::MCParticles MCPs ;
  const MCPs* mcps = get<MCPs>( MCParticleLocation::Default );

  // copy all MC muons into separate container 
  MCContainer mcmuons;
  LoKi::select( mcps->begin() , mcps->end() , 
                std::back_inserter( mcmuons ) , MCABSID == 13 ) ;

  // create the matching  object 
  MCMatch mc = mctruth();
  
  // check for matching with ALL muons 
  Cut mccut1 = MCTRUTH( mc , mcmuons.begin() , mcmuons.end() ) ;
  
  // check for matching with muon with highest PT 
  const MCParticle* mcmuon = LoKi::select_max( mcmuons, MCPT );
  Cut mccut2 = ( 0 != mcmuon ) ? 
    Cut( MCTRUTH( mc , mcmuon ) ) : Cut( PNONE ) ;
  
  Tuple tuple = nTuple ( "Muons" ) ;
  
  // loop over all recontructed muons 
  for ( Range::iterator imuon = muons.begin() ; 
        muons.end() != imuon ; ++imuon ) 
  {
    const Particle* muon = *imuon ;
    if( 0 == muon ) { continue ; }
    // ID variable 
    tuple -> column ( "DLLmu" , PIDmu ( muon )  ) ;
    tuple -> column ( "PT"    , PT    ( muon )  ) ;
    tuple -> column ( "RCM"   , muon->momentum() ) ; //4-momentum 
    tuple -> column ( "mc1"   , mccut1( muon )  ) ; // true muon?
    tuple -> column ( "mc2"   , mccut2( muon )  ) ; // true muon with highest PT
  
    // find MC muon which 'matches' with the recontructed muon
    MCContainer::iterator im = 
      mc->match( muon , mcmuons.begin() , mcmuons.end() ) ;
    if( mcmuons.end() != im &&  0 != *im ) 
    {
      tuple -> column ( "MCM" , (*im)->momentum() ) ; // 4-momentum of MC muon
    }
    else 
    {
      tuple -> column ( "MCM" , HepLorentzVector() ) ; // 4-momentum of MC muon
    }
    
    tuple -> write  () ;
  }
  
  
  return StatusCode::SUCCESS ;
  
};

// ============================================================================
// The END 
// ============================================================================
