
//-----------------------------------------------------------------------------
/** @file RichPhotonCreator.cpp
 *
 *  Implementation file for tool : Rich::Rec::PhotonCreator
 *
 *  CVS Log :-
 *  $Id: RichPhotonCreator.cpp,v 1.36 2007-02-02 10:10:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichPhotonCreator.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PhotonCreator );

// Standard constructor
PhotonCreator::PhotonCreator( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : RichPhotonCreatorBase ( type, name, parent ),
    m_photonReco          ( NULL )
{
  // job options
  declareProperty( "PhotonRecoTool", m_photonRecoName = "RichDetPhotonReco" );
}

StatusCode PhotonCreator::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichPhotonCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( m_photonRecoName, m_photonReco, this );

  return sc;
}

StatusCode PhotonCreator::finalize()
{
  // Execute base class method
  return RichPhotonCreatorBase::finalize();
}

LHCb::RichRecPhoton *
PhotonCreator::buildPhoton( LHCb::RichRecSegment * segment,
                            LHCb::RichRecPixel * pixel,
                            const RichRecPhotonKey key ) const
{

  LHCb::RichRecPhoton * newPhoton = NULL;

  // Reconstruct the geometrical photon
  LHCb::RichGeomPhoton * geomPhoton = new LHCb::RichGeomPhoton();
  if ( ( m_photonReco->reconstructPhoton( segment->trackSegment(),
                                          pixel->globalPosition(),
                                          *geomPhoton,
                                          pixel->smartID() ).isSuccess() ) &&
       // Check photon is OK
       ( ( geomPhoton->CherenkovTheta() > 0. ||
           geomPhoton->CherenkovPhi()   > 0. ) &&
         checkAngleInRange( segment, geomPhoton->CherenkovTheta() ) ) )
  {

    // make new RichRecPhoton ( NB will own geomPhoton )
    newPhoton = new LHCb::RichRecPhoton( geomPhoton, segment,
                                         segment->richRecTrack(), pixel );

    // check photon signal probability
    if ( checkPhotonProb( newPhoton ) )
    {
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "  -> RichRecPhoton selected" << endreq;
      }
      // save this photon to TES
      savePhoton( newPhoton, key );

      // Build cross-references between objects
      buildCrossReferences( newPhoton );
    }
    else
    {
      if ( msgLevel(MSG::VERBOSE) )
      {
        verbose() << "  -> RichRecPhoton FAILED probability checks" << endreq;
      }
      delete newPhoton; // also deletes geomPhoton
      newPhoton = NULL;
    }

  }
  else
  {
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "   -> RichGeomPhoton reconstruction FAILED" << endreq;
    }
    delete geomPhoton;
  }

  // Add to reference map
  if ( bookKeep() ) m_photonDone[ key ] = true;

  // Return pointer to this photon
  return newPhoton;

}
