
//-----------------------------------------------------------------------------
/** @file RichGeomEffFixedValue.cpp
 *
 *  Implementation file for tool : RichGeomEffFixedValue
 *
 *  CVS Log :-
 *  $Id: RichGeomEffFixedValue.cpp,v 1.11 2005-06-23 15:17:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichGeomEffFixedValue.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichGeomEffFixedValue>          s_factory ;
const        IToolFactory& RichGeomEffFixedValueFactory = s_factory ;

// Standard constructor
RichGeomEffFixedValue::RichGeomEffFixedValue ( const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent )
  : RichRecToolBase  ( type, name, parent ),
    m_ckAngle        ( 0 ),
    m_geomTool       ( 0 ),
    m_fixedValue     ( Rich::NRadiatorTypes, 0.7 ),
    m_fixedScatValue ( 0.7 )
{

  // interface
  declareInterface<IRichGeomEff>(this);

  // job options
  declareProperty( "FixedSignalEfficiency",  m_fixedValue            );
  declareProperty( "FixedScatterEfficiency", m_fixedScatValue = 0.73 );

}

StatusCode RichGeomEffFixedValue::initialize() 
{
  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  m_ckAngle  = cherenkovAngleTool();
  m_geomTool = geometryTool();

  return sc;
}

StatusCode RichGeomEffFixedValue::finalize() 
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

double RichGeomEffFixedValue::geomEfficiency ( RichRecSegment * segment,
                                               const Rich::ParticleIDType id ) const {

  if ( !segment->geomEfficiency().dataIsValid(id) ) 
  {
    double eff = 0;

    // Cherenkov theta
    const double ckTh = m_ckAngle->avgCherenkovTheta( segment, id );

    if ( ckTh > 0 ) 
    {
      // First get the HPD panel acceptance (edges)
      eff = m_geomTool->hpdPanelAcceptance(segment,id);
      // .. next, scale by the average panel acceptance (circular HPDs)
      eff *= m_fixedValue[segment->trackSegment().radiator()];
    }

    // Set the geom eff
    segment->setGeomEfficiency( id, eff );

    // Track impact point on HPD panel
    const HepPoint3D & tkPoint = segment->pdPanelHitPointLocal();

    // radius of ring for given hypothesis
    const double rSig = m_ckAngle->avCKRingRadiusLocal(segment,id);
    
    // flag where hits could be
    //segment->setPhotonsInYPlus(true);
    //segment->setPhotonsInYMinus(true);
    //segment->setPhotonsInXPlus(true);
    //segment->setPhotonsInXMinus(true);
    if ( tkPoint.y()+rSig > 0 ) segment->setPhotonsInYPlus(true);
    if ( tkPoint.y()-rSig < 0 ) segment->setPhotonsInYMinus(true);
    if ( tkPoint.x()+rSig > 0 ) segment->setPhotonsInXPlus(true);
    if ( tkPoint.x()-rSig < 0 ) segment->setPhotonsInXMinus(true);

  }

  return segment->geomEfficiency( id );
}

double RichGeomEffFixedValue::geomEfficiencyScat ( RichRecSegment * segment,
                                                   const Rich::ParticleIDType id ) const {

  if ( !segment->geomEfficiencyScat().dataIsValid(id) ) {

    double eff = 0;
    if ( segment->trackSegment().radiator() == Rich::Aerogel ) {
      eff = ( m_ckAngle->avgCherenkovTheta(segment,id) > 0 ? m_fixedScatValue : 0 );
    }

    // Assign this efficiency to all hypotheses
    segment->setGeomEfficiencyScat( Rich::Electron, eff );
    segment->setGeomEfficiencyScat( Rich::Muon, eff );
    segment->setGeomEfficiencyScat( Rich::Pion, eff );
    segment->setGeomEfficiencyScat( Rich::Kaon, eff );
    segment->setGeomEfficiencyScat( Rich::Proton, eff );

  }

  return segment->geomEfficiencyScat( id );
}
