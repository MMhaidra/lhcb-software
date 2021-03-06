
//--------------------------------------------------------------------------
/** @file RichRecSummaryAlg.cpp
 *
 *  Implementation file for algorithm class : RichRecSummaryAlg
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   17/04/2002
 */
//--------------------------------------------------------------------------

// local
#include "RichRecSummaryAlg.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( SummaryAlg )

// Standard constructor, initializes variables
SummaryAlg::SummaryAlg( const std::string& name,
                        ISvcLocator* pSvcLocator )
  : Rich::Rec::AlgBase ( name, pSvcLocator ),
    m_summaryLoc       ( LHCb::RichSummaryTrackLocation::Default ),
    m_nSigma           ( Rich::NRadiatorTypes, 1 )
{
  // job opts
  declareProperty( "SummaryLocation", 
                   m_summaryLoc = contextSpecificTES(LHCb::RichSummaryTrackLocation::Default) );
  declareProperty( "PhotonNSigma",    m_nSigma );
}

// Destructor
SummaryAlg::~SummaryAlg() {}

// Initialize
StatusCode SummaryAlg::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::AlgBase::initialize();
  if ( sc.isFailure() )
  { return Error( "Failed to initialize base class", sc ); }

  // get tools
  acquireTool( "RichCherenkovAngle",      m_ckAngle     );
  acquireTool( "RichCherenkovResolution", m_ckAngleRes  );
  acquireTool( "RichExpectedTrackSignal", m_tkSignal    );
  acquireTool( "TrackSelector",           m_trSelector, this );

  info() << "Will select photons within (aero/R1Gas/R2Gas) " << m_nSigma
         << " sigma of any mass hypothesis" << endmsg;

  return sc;
}

StatusCode SummaryAlg::execute()
{

  // Make sure all tracks and segments have been formed
  if ( trackCreator()->newTracks().isFailure() )
    return Error( "Problem creating RichRecTracks" );
  // make sure RichrecPixels are ready
  if ( pixelCreator()->newPixels().isFailure() )
    return Error( "Problem creating RichRecPixels" );
  // make sure photons are available
  if ( photonCreator()->reconstructPhotons().isFailure() )
    return Error( "Problem creating RichRecPhotons" );

  // Create a new container for RICH reconstruction summary Tracks
  auto * sumTracks = new LHCb::RichSummaryTracks();
  put( sumTracks, m_summaryLoc );

  // All OK, load the RichRecTracks
  for ( const auto track : *richTracks() )
  {

    // apply track selection
    if ( !m_trSelector->trackSelected(track) ) continue;

    // get the reco track
    const auto * trtrack = dynamic_cast<const LHCb::Track *>(track->parentTrack());
    if ( !trtrack )
    {
      Warning( "Input track type is not Track -> RichRecTrack skipped" ).ignore();
      continue;
    }

    // Make a new summary track
    LHCb::RichSummaryTrack * sumTrack = new LHCb::RichSummaryTrack();
    // give to Gaudi
    sumTracks->insert( sumTrack, track->key() );

    // store track reference
    sumTrack->setTrack( trtrack );

    // vector of summary segments
    LHCb::RichSummaryRadSegment::Vector sumSegs;

    // loop over radiator segments
    for ( const auto seg : track->richRecSegments() )
    {
      // Add a summary segment for this reconstructed segment
      sumSegs.push_back( LHCb::RichSummaryRadSegment() );
      // get reference to this segment
      auto & sumSeg = sumSegs.back();

      // which radiator
      const auto rad = seg->trackSegment().radiator();

      // set the radiator type
      sumSeg.setRadiator( rad );

      // Loop over all particle codes.
      for ( const auto hypo : Rich::particles() )
      {
        // Set expected CK theta angles
        sumSeg.setExpectedCkTheta( hypo, (float)m_ckAngle->avgCherenkovTheta(seg,hypo) );
        // set expected CK resolutions (errors)
        sumSeg.setExpectedCkThetaError( hypo, (float)m_ckAngleRes->ckThetaResolution(seg,hypo) );
        // Expected number of observable signal photons
        sumSeg.setExpectedNumPhotons( hypo, (float)m_tkSignal->nObservableSignalPhotons(seg,hypo) );
      }

      // vector of summary photons
      LHCb::RichSummaryPhoton::Vector sumPhots;

      // Loop over photons for this segment and add info on those passing the selection
      const auto & photons = photonCreator()->reconstructPhotons( seg );
      for ( const auto * phot : photons )
      {
        // get Cherenkov angles
        const auto ckTheta ( phot->geomPhoton().CherenkovTheta() );
        const auto ckPhi   ( phot->geomPhoton().CherenkovPhi()   );

        // does this photon come within n-sigma of any of the expected CK angles for this segment ?
        bool OK ( false );
        for ( const auto hypo : Rich::particles() )
        {
          const auto ckDiff = fabs( sumSeg.expectedCkTheta(hypo) - ckTheta );
          if ( ckDiff < m_nSigma[rad] * sumSeg.expectedCkThetaError(hypo) )
          {
            OK = true; break;
          }
        }

        // select all photons for testing
        //OK = true;

        // is this photon selected
        if ( OK )
        {
          // Add a new summary photon
          sumPhots.push_back( LHCb::RichSummaryPhoton() );
          auto & sumPhot = sumPhots.back();
          // set angles
          sumPhot.setCherenkovTheta((float)ckTheta);
          sumPhot.setCherenkovPhi((float)ckPhi);
          // channel ID (only store primary ID)
          sumPhot.setSmartID( phot->richRecPixel()->hpdPixelCluster().primaryID() );
        }

      } // loop over reco photons

      // add vector of photons to segment
      sumSeg.setPhotons( sumPhots );

    } // loop over reco segments

    // add vector of segments to track
    sumTrack->setRadSegments( sumSegs );

  } // loop over reco tracks

  return StatusCode::SUCCESS;
}
