// $Id: RichGlobalPIDFinalize.cpp,v 1.9 2004-04-19 23:04:00 jonesc Exp $
// Include files

// local
#include "RichGlobalPIDFinalize.h"

//--------------------------------------------------------------------------
// Implementation file for class : RichGlobalPIDFinalize
//
// 017/04/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//--------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichGlobalPIDFinalize>          s_factory ;
const        IAlgFactory& RichGlobalPIDFinalizeFactory = s_factory ;

// Standard constructor, initializes variables
RichGlobalPIDFinalize::RichGlobalPIDFinalize( const std::string& name,
                                              ISvcLocator* pSvcLocator )
  : RichGlobalPIDAlgBase ( name, pSvcLocator ) {

  declareProperty( "ProcStatusLocation",
                   m_procStatLocation = ProcStatusLocation::Default );

}

// Destructor
RichGlobalPIDFinalize::~RichGlobalPIDFinalize() {}

// Initialize
StatusCode RichGlobalPIDFinalize::initialize() 
{
  // Sets up various tools and services
  StatusCode sc = RichRecAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  return StatusCode::SUCCESS;
}

StatusCode RichGlobalPIDFinalize::execute() {

  debug() << "Execute" << endreq;

  // Event Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // update pointers to global PID working objects
  if ( !gpidPIDs() || !gpidTracks() ) return StatusCode::FAILURE;

  // Iterate over working tracks and keep/delete PID results
  for ( RichGlobalPIDTracks::iterator track = m_GPIDtracks->begin();
        track != m_GPIDtracks->end();
        ++track ) {
    RichRecTrack * rRTrack = (*track)->richRecTrack();

    if ( msgLevel(MSG::VERBOSE) ) {
      verbose() << "PID'ed Track "
                << (*track)->key() << " (" << (*track)->trQuality()
                << "), as " << rRTrack->currentHypothesis() << endreq;
    }

    RichGlobalPID * pid = (*track)->globalPID();

    // Only store results for physics quality tracks
    if ( (*track)->trQuality() != Rich::GlobalPID::Physics ) {
      m_GPIDs->erase( pid );
      continue;
    }

    // Set best PID
    pid->setBestParticleID( rRTrack->currentHypothesis() );

    // Finalise delta LL and probability values
    std::vector<float> & deltaLLs = pid->particleLLValues();
    if ( deltaLLs[pid->bestParticleID()] > 1e-10 ) {
      warning() << "PID " << pid->key() << " best ID " << pid->bestParticleID()
                << " has non-zero deltaLL value! " << deltaLLs[pid->bestParticleID()] << endreq;
    }
    for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
      if ( deltaLLs[iHypo] < 0 ) { deltaLLs[iHypo] = 0; }
      const double prob = 1.0 - gsl_sf_erf( sqrt(deltaLLs[iHypo]) );
      pid->setParticleRawProb( static_cast<Rich::ParticleIDType>(iHypo), prob );
    }

  }

  // All OK - Update ProcStatus with number of PIDs
  ProcStatus * procStat = get<ProcStatus>( m_procStatLocation );
  procStat->addAlgorithmStatus( m_richGPIDName, m_GPIDs->size() );

  return StatusCode::SUCCESS;
}

//  Finalize
StatusCode RichGlobalPIDFinalize::finalize() 
{
  debug() << "Finalize" << endreq;

  // Execute base class method
  return RichRecAlgBase::finalize();
}
