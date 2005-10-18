
//-------------------------------------------------------------------------------------
/** @file RichTrackCreatorFromTrStoredTracks.cpp
 *
 *  Implementation file for tool : RichTrackCreatorFromTrStoredTracks
 *
 *  CVS Log :-
 *  $Id: RichTrackCreatorFromTrStoredTracks.cpp,v 1.31 2005-10-18 13:03:51 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-------------------------------------------------------------------------------------

// local
#include "RichTrackCreatorFromTrStoredTracks.h"

//-------------------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichTrackCreatorFromTrStoredTracks>          s_factory ;
const        IToolFactory& RichTrackCreatorFromTrStoredTracksFactory = s_factory ;

// Standard constructor
RichTrackCreatorFromTrStoredTracks::
RichTrackCreatorFromTrStoredTracks( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : RichTrackCreatorBase   ( type, name, parent ),
    m_trTracks             ( 0 ),
    m_rayTrace             ( 0 ),
    m_smartIDTool          ( 0 ),
    m_massHypoRings        ( 0 ),
    m_segMaker             ( 0 ),
    m_signal               ( 0 ),
    m_trTracksLocation     ( TrStoredTrackLocation::Default ),
    m_trSegToolNickName    ( "RichTrSegMakerFromTrStoredTracks" ),
    m_allDone              ( false ),
    m_buildHypoRings       ( false )
{

  // declare interface for this tool
  declareInterface<IRichTrackCreator>(this);

  // job options
  declareProperty( "TrStoredTracksLocation",   m_trTracksLocation   );
  declareProperty( "BuildMassHypothesisRings", m_buildHypoRings     );
  declareProperty( "TrackSegmentTool",         m_trSegToolNickName  );

}

StatusCode RichTrackCreatorFromTrStoredTracks::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichTrackCreatorBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRayTracing",          m_rayTrace    );
  acquireTool( "RichExpectedTrackSignal", m_signal      );
  acquireTool( "RichSmartIDTool",         m_smartIDTool, 0, true );
  acquireTool( m_trSegToolNickName,       m_segMaker    );
  if ( m_buildHypoRings ) acquireTool( "RichMassHypoRings", m_massHypoRings );

  // Configure the ray-tracing mode
  m_traceMode.setDetPrecision      ( RichTraceMode::circle );
  m_traceMode.setDetPlaneBound     ( RichTraceMode::loose  );
  m_traceMode.setForcedSide        ( false                 );
  m_traceMode.setOutMirrorBoundary ( false                 );
  m_traceMode.setMirrorSegBoundary ( false                 );

  return sc;
}

StatusCode RichTrackCreatorFromTrStoredTracks::finalize()
{
  // Execute base class method
  return RichTrackCreatorBase::finalize();
}

const StatusCode RichTrackCreatorFromTrStoredTracks::newTracks() const
{

  if ( !m_allDone )
  {
    m_allDone = true;

    // Iterate over all reco tracks, and create new RichRecTracks
    richTracks()->reserve( nInputTracks() );
    for ( TrStoredTracks::const_iterator track = trStoredTracks()->begin();
          track != trStoredTracks()->end(); ++track )
    {
      newTrack( *track );
    }

  }

  return StatusCode::SUCCESS;
}

const long RichTrackCreatorFromTrStoredTracks::nInputTracks() const
{
  const TrStoredTracks * tracks = trStoredTracks();
  return ( tracks ? tracks->size() : 0 );
}

const TrStoredTracks *
RichTrackCreatorFromTrStoredTracks::trStoredTracks() const
{
  if ( !m_trTracks )
  {
    // Obtain smart data pointer to TrStoredTracks
    m_trTracks = get<TrStoredTracks>( m_trTracksLocation );
    if ( msgLevel(MSG::DEBUG) )
    {
      debug() << "located " << m_trTracks->size() << " TrStoredTracks at "
              << m_trTracksLocation << endreq;
    }
  }

  return m_trTracks;
}

// Forms a new RichRecTrack object from a TrStoredTrack
RichRecTrack *
RichTrackCreatorFromTrStoredTracks::newTrack ( const ContainedObject * obj ) const
{

  // flag the tool as having been used this event
  m_hasBeenCalled = true;

  // Is this a TrStoredTrack ?
  const TrStoredTrack * trTrack = dynamic_cast<const TrStoredTrack*>(obj);
  if ( !trTrack )
  {
    Warning( "Input data object is not of type TrStoredTrack" );
    return NULL;
  }

  // track type
  const Rich::Track::Type trType = Rich::Track::type(trTrack);

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "Trying TrStoredTrack " << trTrack->key()
              << " type " << trType << ", unique=" << trTrack->unique()
              << ", charge=" << trTrack->charge()
              << endreq;
  }

  // Is track a usable type
  if ( !Rich::Track::isUsable(trType) ) return NULL;

  // Track selection
  if ( !trackSelector().trackSelected(trTrack) ) return NULL;

  // Get reference to track stats object
  TrackCount & tkCount = trackStats().trackStats(trType,trTrack->unique());

  // See if this RichRecTrack already exists
  if ( bookKeep() && m_trackDone[trTrack->key()] )
  {
    return richTracks()->object(trTrack->key());
  }
  else
  {

    // count tried tracks
    ++tkCount.triedTracks;

    RichRecTrack * newTrack = NULL;

    // Form the RichRecSegments for this track
    std::vector<RichTrackSegment> segments;
    const int Nsegs = m_segMaker->constructSegments( trTrack, segments );
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << " Found " << Nsegs << " radiator segments" << endreq;
    }
    if ( 0 < Nsegs )
    {

      // Find TrStoredTrack current state and momentum
      SmartRef<TrState> trackState = trTrack->closestState(-999999.);
      TrStateP * trackPState = dynamic_cast<TrStateP*>( static_cast<TrState*>(trackState) );
      if ( trackPState )
      {

        if ( msgLevel(MSG::VERBOSE) )
        {
          verbose() << " Ptot = " << trackPState->p()/GeV << " passed cut" << endreq;
        }

        // Form a new RichRecTrack
        newTrack = new RichRecTrack();

        // Configure TrackID for this TrStoredTrack
        newTrack->trackID().initialiseFor( trTrack );

        bool keepTrack = false;
        for ( std::vector<RichTrackSegment>::iterator iSeg = segments.begin();
              iSeg != segments.end(); ++iSeg )
        {

          // make a new RichRecSegment from this RichTrackSegment
          RichRecSegment * newSegment = segmentCreator()->newSegment( *iSeg, newTrack );

          // Get PD panel impact point
          HepPoint3D & hitPoint = newSegment->pdPanelHitPoint();
          const HepVector3D & trackDir = (*iSeg).bestMomentum();
          if ( m_rayTrace->traceToDetectorWithoutEff( (*iSeg).rich(),
                                                      (*iSeg).bestPoint(),
                                                      trackDir,
                                                      hitPoint,
                                                      m_traceMode ) )
          {

            // Get PD panel hit point in local coordinates
            // need to do before test below, since potentially needed by geom eff tool
            // need to make this data "on-demand" to avoid this sort of thing
            newSegment->pdPanelHitPointLocal() = m_smartIDTool->globalToPDPanel(hitPoint);

            // test if this segment has valid information
            if ( m_signal->hasRichInfo(newSegment) )
            {

              if ( msgLevel(MSG::VERBOSE) )
                verbose() << " TrackSegment in " << (*iSeg).radiator() << " selected" << endreq;

              // keep track
              keepTrack = true;

              // Save this segment
              segmentCreator()->saveSegment( newSegment );

              // Add to the SmartRefVector of RichSegments for this RichRecTrack
              newTrack->addToRichRecSegments( newSegment );

              // set radiator info
              const Rich::RadiatorType rad = (*iSeg).radiator();
              setDetInfo( newTrack, rad );

              // Set the average photon energy (for default hypothesis)
              newSegment->trackSegment()
                .setAvPhotonEnergy( m_signal->avgSignalPhotEnergy(newSegment,
                                                                  newTrack->currentHypothesis()) );

              // make RichRecRings for the mass hypotheses if requested
              if ( m_buildHypoRings ) m_massHypoRings->newMassHypoRings( newSegment );

              // Count radiator segments
              tkCount.countRadiator( rad );

            }
            else
            {
              if ( msgLevel(MSG::VERBOSE) )
                verbose() << " TrackSegment in " << (*iSeg).radiator() << " rejected" << endreq;
              delete newSegment;
              newSegment = NULL;
            }

          }
          else
          {
            if ( msgLevel(MSG::VERBOSE) )
              verbose() << " TrackSegment in " << (*iSeg).radiator() << " rejected" << endreq;
            delete newSegment;
            newSegment = NULL;
          }

        } // end loop over RichTrackSegments

        if ( keepTrack )
        {

          // give to container
          richTracks()->insert( newTrack, trTrack->key() );

          // Set vertex momentum
          newTrack->setVertexMomentum( trackPState->p() );

          // track charge
          newTrack->setCharge( trTrack->charge() );

          // Set parent information
          newTrack->setParentTrack( trTrack );

          // Count selected tracks
          ++tkCount.selectedTracks;

        }
        else
        {
          delete newTrack;
          newTrack = NULL;
        }

      } // end track state and momentum if
 
    } // end segments if

    // Add to reference map
    if ( bookKeep() ) m_trackDone[trTrack->key()] = true;

    return newTrack;
  }

}

void RichTrackCreatorFromTrStoredTracks::InitNewEvent()
{
  RichTrackCreatorBase::InitNewEvent();
  m_allDone  = false;
  m_trTracks = 0;
}
