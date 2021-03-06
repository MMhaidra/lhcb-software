
//-----------------------------------------------------------------------------
/** @file RichAdaptivePhotonReco.cpp
 *
 * Implementation file for class : Rich::Rec::AdaptivePhotonReco
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @author Antonis Papanestis
 * @date 2003-11-14
 */
//-----------------------------------------------------------------------------

// local
#include "RichAdaptivePhotonReco.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AdaptivePhotonReco::
AdaptivePhotonReco( const std::string& type,
                    const std::string& name,
                    const IInterface* parent )
  : PhotonRecoBase      ( type, name, parent          ),
    m_maxCKThetaForFast ( Rich::NRadiatorTypes, -1    ),
    m_fastRecoIsEnabled ( Rich::NRadiatorTypes, false )
{
  // Job options                                          Aero  R1gas R2gas
  declareProperty( "MaxFracCKTheta", m_maxFracCKtheta = { -1.0, 0.95, 0.95 } );

  //setProperty( "Outputlevel", 1 );
}

//=============================================================================
// Destructor
//=============================================================================
AdaptivePhotonReco::~AdaptivePhotonReco() { }

//=============================================================================
// Initialisation.
//=============================================================================
StatusCode AdaptivePhotonReco::initialize()
{
  // initialise base class
  const StatusCode sc = PhotonRecoBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Acquire instances of tools
  acquireTool( "RichCherenkovAngle",     m_ckAngle        );
  acquireTool( "AdaptiveSlowPhotonReco", m_recoSlow, this );
  acquireTool( "AdaptiveFastPhotonReco", m_recoFast, this );

  // Check if the fast reconstruction is enabled for each radiator
  for ( const auto rad : Rich::radiators() )
  {
    m_fastRecoIsEnabled[rad] = ( m_maxFracCKtheta[rad] > 0 );
  }
 
  return sc;
}

//-------------------------------------------------------------------------
//  reconstruct a photon from track segment and pixel
//-------------------------------------------------------------------------
StatusCode
AdaptivePhotonReco::
reconstructPhoton ( const LHCb::RichRecSegment * segment,
                    const LHCb::RichRecPixel * pixel,
                    LHCb::RichGeomPhoton& gPhoton ) const
{

  // which radiator ?
  const auto rad = segment->trackSegment().radiator();
  
  // Is the use of the fast tool enabled for this radiators ?
  if ( m_fastRecoIsEnabled[rad] )
  {
    
    // Expected Kaon CK theta
    const auto kaonCKTheta = m_ckAngle->avgCherenkovTheta(segment,Rich::Kaon);
    
    // First check to see if the fast or slow tool should be used based
    // on how close to saturation the track is
    if ( kaonCKTheta < maxCKThetaForFast(rad) )
    {
      // First try the fast tool. If this fails (for ambiguous photons for 
      // instance) try again with the slow tool.
      const auto sc = m_recoFast->reconstructPhoton(segment,pixel,gPhoton);
      if ( sc.isSuccess() ) return sc; 
    }
    
  }
  
  // If we get here, run the slow tool
  return m_recoSlow->reconstructPhoton(segment,pixel,gPhoton);
}

//=============================================================================
// Declaration of the Algorithm Factory
//=============================================================================

DECLARE_TOOL_FACTORY ( AdaptivePhotonReco )

//=============================================================================
