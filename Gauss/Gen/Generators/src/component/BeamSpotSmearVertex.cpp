// $Id: BeamSpotSmearVertex.cpp,v 1.8 2007-02-08 17:46:06 gcorti Exp $
// Include files 

// local
#include "BeamSpotSmearVertex.h"

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/IRndmGenSvc.h" 

// from Event
#include "Event/HepMCEvent.h"

#include "GaudiKernel/SystemOfUnits.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LHCbAcceptance
//
// 2005-08-17 : Patrick Robbe
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( BeamSpotSmearVertex );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BeamSpotSmearVertex::BeamSpotSmearVertex( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : GaudiTool ( type, name , parent ) {
    declareInterface< IVertexSmearingTool >( this ) ;
    declareProperty( "SigmaX" , m_sigmaX = 0.07 * Gaudi::Units::mm ) ;
    declareProperty( "SigmaY" , m_sigmaY = 0.07 * Gaudi::Units::mm ) ;
    declareProperty( "SigmaZ" , m_sigmaZ = 50. * Gaudi::Units::mm ) ;
    
    declareProperty( "Xcut" , m_xcut = 4. ) ; // times SigmaX 
    declareProperty( "Ycut" , m_ycut = 4. ) ; // times SigmaY
    declareProperty( "Zcut" , m_zcut = 4. ) ; // times SigmaZ    
}

//=============================================================================
// Destructor 
//=============================================================================
BeamSpotSmearVertex::~BeamSpotSmearVertex( ) { ; }

//=============================================================================
// Initialize 
//=============================================================================
StatusCode BeamSpotSmearVertex::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;
  
  IRndmGenSvc * randSvc = svc< IRndmGenSvc >( "RndmGenSvc" , true ) ;
  sc = m_gaussDist.initialize( randSvc , Rndm::Gauss( 0. , 1. ) ) ;
  if ( ! sc.isSuccess() ) 
    return Error( "Could not initialize gaussian random number generator" ) ;

  info() << "Smearing of interaction point with Gaussian distribution "
         << endmsg;
  if( msgLevel(MSG::DEBUG) ) {
    debug() << " with sigma(X) = " << m_sigmaX / Gaudi::Units::mm 
            << " mm troncated at " << m_xcut << " sigma(X)";
    debug() << " with sigma(Y) = " << m_sigmaY / Gaudi::Units::mm 
            << " mm troncated at " 
            << m_ycut << " sigma(Y)" << endmsg ;
    debug() << " with sigma(Z) = " << m_sigmaZ / Gaudi::Units::mm 
            << " mm troncated at " 
            << m_zcut << " sigma(Z)" << endmsg ;
  } else {
    info() << " with sigma(X,Y,Z) = " << m_sigmaX / Gaudi::Units::mm << " mm, "
           << m_sigmaY / Gaudi::Units::mm << " mm, " 
           << m_sigmaZ / Gaudi::Units::mm << " mm" << endmsg;
  }
    
  release( randSvc ) ;
 
  return sc ;
}

//=============================================================================
// Smearing function
//=============================================================================
StatusCode BeamSpotSmearVertex::smearVertex( LHCb::HepMCEvent * theEvent ) {
  double dx , dy , dz ;
  
  do { dx = m_gaussDist( ) ; } while ( fabs( dx ) > m_xcut ) ;
  dx = dx * m_sigmaX ;
  do { dy = m_gaussDist( ) ; } while ( fabs( dy ) > m_ycut ) ;
  dy = dy * m_sigmaY ;
  do { dz = m_gaussDist( ) ; } while ( fabs( dz ) > m_zcut ) ;
  dz = dz * m_sigmaZ ;

  HepLorentzVector dpos( dx , dy , dz , 0. ) ;
  
  HepMC::GenEvent::vertex_iterator vit ;
  HepMC::GenEvent * pEvt = theEvent -> pGenEvt() ;
  for ( vit = pEvt -> vertices_begin() ; vit != pEvt -> vertices_end() ; 
        ++vit ) {
    HepLorentzVector pos = (*vit) -> position() ;
    (*vit) -> set_position( pos + dpos ) ;
  }

  return StatusCode::SUCCESS ;      
}

