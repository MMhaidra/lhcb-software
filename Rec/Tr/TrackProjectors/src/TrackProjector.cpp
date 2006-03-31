// $Id: TrackProjector.cpp,v 1.10 2006-03-31 17:13:55 jvantilb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from TrackEvent
#include "Event/State.h"
#include "Event/Measurement.h"

// from stl
#include <numeric>

// local
#include "TrackProjector.h"

using namespace Gaudi;
using namespace LHCb;

// Declaration of the Tool Factory
static const  ToolFactory<TrackProjector>          s_factory ;
const        IToolFactory& TrackProjectorFactory = s_factory ;

//-----------------------------------------------------------------------------
/// Dummy implementation of method, to please Windows linker
//-----------------------------------------------------------------------------
StatusCode TrackProjector::project( const State&, Measurement& )
{
  return StatusCode::FAILURE;
}

//-----------------------------------------------------------------------------
// Retrieve the projection matrix H of the (last) projection
//-----------------------------------------------------------------------------
const TrackVector& TrackProjector::projectionMatrix() const
{
  return m_H;
}

//-----------------------------------------------------------------------------
/// Retrieve the chi squared of the (last) projection
//-----------------------------------------------------------------------------
double TrackProjector::chi2() const
{
  return ( m_errResidual > 0.0 ) ?
    ( m_residual / m_errResidual ) * ( m_residual / m_errResidual ) : 0.0 ;
}

//-----------------------------------------------------------------------------
/// Retrieve the residual of the (last) projection
//-----------------------------------------------------------------------------
double TrackProjector::residual() const
{
  return m_residual;
}

//-----------------------------------------------------------------------------
/// Retrieve the error on the residual of the (last) projection
//-----------------------------------------------------------------------------
double TrackProjector::errResidual() const
{
  return m_errResidual;
}

//-----------------------------------------------------------------------------
/// Standard constructor, initializes variables
//-----------------------------------------------------------------------------
TrackProjector::TrackProjector( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_residual(0.),
    m_errResidual(0.),
    m_H()
{
  declareInterface<ITrackProjector>( this );
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TrackProjector::~TrackProjector() {}; 

//-----------------------------------------------------------------------------
/// Compute the residual
//-----------------------------------------------------------------------------
void TrackProjector::computeResidual( const State& state,
                                      const Measurement& meas ) 
{
  m_residual = meas.measure() -
    std::inner_product(m_H.begin(), m_H.end(), state.stateVector().begin(), 0.0);
}

//-----------------------------------------------------------------------------
/// Compute the error on the residual (depreciated method)
//-----------------------------------------------------------------------------
void TrackProjector::computeErrorResidual( const State& state,
                                           const Measurement& meas )
{
  m_errResidual = sqrt( meas.resolution2( state.position(), state.slopes() ) + 
                        ROOT::Math::Similarity<double,5>
                        ( m_H, state.covariance() ) );
}
