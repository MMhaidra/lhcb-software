// $Id: TrackOTProjector.cpp,v 1.11 2006-08-01 08:38:04 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// OTDet
#include "OTDet/DeOTModule.h"

// from GSL
#include "gsl/gsl_math.h"

// local
#include "TrackOTProjector.h"

// from Tr/TrackFitEvent
#include "Event/OTMeasurement.h"

using namespace Gaudi;
using namespace LHCb;

DECLARE_TOOL_FACTORY( TrackOTProjector );

//-----------------------------------------------------------------------------
/// Project a state onto a measurement.
/// It returns the chi squared of the projection
//-----------------------------------------------------------------------------
StatusCode TrackOTProjector::project( const State& state,
                                  Measurement& meas )
{
  // Set refVector in case it was not set before
  if ( !meas.refIsSet() ) meas.setRefVector( state.stateVector() );

  // Get the stereo angle
  OTChannelID OTChan = meas.lhcbID().otID();
  DeOTModule* module = m_det -> findModule( OTChan );
  double stereoAngle = module -> angle();

  // Get the required velocities
  double driftVelocity = m_det -> driftDelay();       // ns/mm
  double wireVelocity  = m_det -> propagationDelay(); // ns/mm

  XYZPoint vWirePos = module->centerOfStraw( OTChan.straw() );
  double wirePos =  vWirePos.x() * cos(stereoAngle) 
                  + vWirePos.y() * sin(stereoAngle);
  double wireLength = module -> wireLength();

  double x  = state.x();
  double y  = state.y();
  double cosA = cos( stereoAngle );
  double sinA = sin( stereoAngle );

  // Determine "tu" from the reference vector
  const TrackVector& refVector = meas.refVector();
  double tu = (refVector(2) * cosA + refVector(3) * sinA) ;

  double cosU     = 1./sqrt( 1.+ tu*tu );
  double du       = (x * cosA + y * sinA - wirePos) ;
  double wireDist = ( x * cosA + y * sinA - wirePos ) * cosU;
  double dDrift = meas.measure() - 
     (wireLength-fabs(y)) * wireVelocity / driftVelocity ;

  OTMeasurement& otmeas =  dynamic_cast<OTMeasurement&>(meas);
  m_H = TrackProjectionMatrix();  
  m_H(0,0) = cosA * cosU ;
  m_H(0,1) = sinA * cosU 
           - otmeas.ambiguity() * wireVelocity/driftVelocity * y/fabs(y);
  m_H(0,2) = -du * tu * gsl_pow_3( cosU ) * cosA;
  m_H(0,3) = -du * tu * gsl_pow_3( cosU ) * sinA;

  // this shouls be ~ equivalent to : computeResidual(state,meas);
  m_residual = otmeas.ambiguity() * dDrift - wireDist;

  computeErrorResidual( state, meas );  

  return StatusCode::SUCCESS; 
}

//-----------------------------------------------------------------------------
/// Initialize
//-----------------------------------------------------------------------------
StatusCode TrackOTProjector::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return Error( "Failed to initialize!", sc ); }
  
  m_det = getDet<DeOTDetector>( m_otTrackerPath );
  
  return StatusCode::SUCCESS;
}

//-----------------------------------------------------------------------------
/// Standard constructor, initializes variables
//-----------------------------------------------------------------------------
TrackOTProjector::TrackOTProjector( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : TrackProjector ( type, name , parent )
{
  declareInterface<ITrackProjector>(this);

  declareProperty( "OTGeometryPath",
                   m_otTrackerPath = DeOTDetectorLocation::Default );
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TrackOTProjector::~TrackOTProjector() {};
