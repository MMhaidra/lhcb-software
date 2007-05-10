// $Id: RelatedPVFinder.cpp,v 1.4 2007-05-10 10:01:17 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "Kernel/IContextTool.h"            // Interface
#include "Kernel/Particle2Vertex.h"
#include "Kernel/IGeomDispCalculator.h"

// local
#include "RelatedPVFinder.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RelatedPVFinder
//
// 2006-10-22 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RelatedPVFinder );

typedef LHCb::RecVertices::const_iterator rv_iter;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RelatedPVFinder::RelatedPVFinder( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
    , m_geom(0)
{
  declareInterface<IRelatedPVFinder>(this);

  declareProperty("SelectByClosestZ", m_closestZ = false );
  declareProperty("SelectClosest", m_closest = false );
  declareProperty("SelectBySmallestIP", m_smallestIP = true );
  declareProperty("SelectBySignificance", m_significance = true );
}
//=============================================================================
// Destructor
//=============================================================================
RelatedPVFinder::~RelatedPVFinder() {} 
//=============================================================================
// initialize
//=============================================================================
StatusCode RelatedPVFinder::initialize(){

  StatusCode sc = GaudiTool::initialize() ;
  if (!sc) return sc;

  if ( m_closest+m_closestZ+m_smallestIP != 1 ){
    err() << "You must select only one of SelectByClosestZ: " << m_closestZ
          << ", SelectClosest: " <<  m_closest
          << ", SelectBySmallestIP: " << m_smallestIP << endmsg ;
    return StatusCode::FAILURE ;
  }
  if ( m_significance ){
    if ( m_closestZ ) debug() << "The PV closest in Z/err will be chosen." << endmsg ;
    else if ( m_closest ) debug() << "The PV closest in 3D/err will be chosen." << endmsg ;
    else if ( m_smallestIP ) debug() << "The PV with smallest IP/err will be chosen." << endmsg ;
  } else {  
    if ( m_closestZ ) debug() << "The PV closest in Z will be chosen." << endmsg ;
    else if ( m_closest ) debug() << "The PV closest in 3D will be chosen." << endmsg ;
    else if ( m_smallestIP ) debug() << "The PV with smallest IP will be chosen." << endmsg ;
  }
  if ( m_closest || m_closestZ ) {
    warning() << "You have chosen spacial separation." << endmsg ;
    warning() << "This only works with particles with an endVertex." << endmsg ;
    warning() << "Use (default) IP distance for final state particles" << endmsg ;
  }  

  m_geom = NULL ;

  return StatusCode::SUCCESS;
}
//=============================================================================
/// Set defaults
//===========================================================================
StatusCode RelatedPVFinder::setDefaults(std::string PVloc, std::string geomTool){
  if (NULL!=m_geom){
    debug() << "Already initialised" << endmsg;
    return StatusCode::SUCCESS ;
  }
  
  m_geom = tool<IGeomDispCalculator>(geomTool,this);
  m_pvLocation = PVloc ;
  return StatusCode::SUCCESS ;
}
//=============================================================================
/// Build relations table and store it in desktop
//============================================================================
StatusCode RelatedPVFinder::relatedPVs(const LHCb::Particle* p,
                                      Particle2Vertex::Table* table) const {
  

  if (NULL==p) return Error("Particle is NULL") ;
  if (NULL==m_geom) return Error("RelatedPVFinder has not been initialsied with setDefaults");
  
  debug() << "Building PV relations for " << p->particleID().pid() << endmsg ;
  // sanity check
  const LHCb::Vertex* v = p->endVertex() ;
  if ( (m_closestZ || m_closest) && (NULL==v)) 
    return Error("Cannot measure distances without vertex. You have been warned at initialisation!");
  
  LHCb::RecVertices* pvs = get<LHCb::RecVertices>( m_pvLocation );

  double fom = 0;
  double err = 0 ;
  StatusCode sc = StatusCode::SUCCESS ;
  verbose() << "Looping over " << pvs->size() << " PVs" << endmsg ;
  for ( rv_iter i = pvs->begin() ; i!=pvs->end() ; ++i){
    if ( m_closestZ ) {
      fom = fabs(v->position().z()-(*i)->position().z());
      if ( m_significance ) fom = fom/sqrt((*i)->covMatrix()(2,2)*(*i)->covMatrix()(2,2)
                                           + v->covMatrix()(2,2)*v->covMatrix()(2,2));
      verbose() << "Closest Z PV at " << (*i)->position() << " fom " << fom << endmsg ;
    } else if ( m_closest ) {
      sc = m_geom->calcVertexDis(*v,*(*i),fom,err);
      if ( m_significance ) fom = fom/err ;
      verbose() << "Closest PV at " << (*i)->position() << " fom " << fom << endmsg ;
    } else if ( m_smallestIP ) {
      sc = m_geom->calcImpactPar(*p,*(*i),fom,err);
      if ( m_significance ) fom = fom/err ;
      verbose() << "Smallest IP PV at " << (*i)->position() << " fom " << fom << endmsg ;
    } else {
      Exception("None of all options") ;
    }
    if (sc) sc = table->relate(p,*i,1./fom) ;
    if (!sc) return sc;

    debug() << "... PV at " << (*i)->position() << " gets weight 1/" << fom << endmsg ;
  }
  verbose() << "Done relations for " << pvs->size() << " PVs" << endmsg ;
  return sc ;
}

