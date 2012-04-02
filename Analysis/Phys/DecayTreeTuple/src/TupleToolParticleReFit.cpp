// $Id: TupleToolParticleReFit.cpp,v 1.7 2010-04-23 09:34:39 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SmartIF.h"

#include <Kernel/GetDVAlgorithm.h>
#include <Kernel/DVAlgorithm.h>
#include <Kernel/ILifetimeFitter.h>

// local
#include "TupleToolParticleReFit.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/Particle.h"
#include "Event/Vertex.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolParticleReFit
//
// 2010-11-02 : Yuehong Xie
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolParticleReFit )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  TupleToolParticleReFit::TupleToolParticleReFit( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
    : TupleToolBase ( type, name , parent )
    , m_dva(0)
    , m_timefitter(0)
    , m_vtxfitter(0)
{
  declareInterface<IParticleTupleTool>(this);

  declareProperty("PropertimeFitterName", m_timefitterName = "PropertimeFitter" );
  declareProperty("VertexFitterName",     m_vertexfitterName = "OfflineVertexFitter" );

}

//=============================================================================

StatusCode TupleToolParticleReFit::initialize()
{
  const StatusCode sc = TupleToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  m_dva = Gaudi::Utils::getDVAlgorithm ( contextSvc() ) ;
  if (0==m_dva) return Error("Couldn't get parent DVAlgorithm",
                             StatusCode::FAILURE);

  m_timefitter = tool<ILifetimeFitter>( m_timefitterName, this );
  if( !m_timefitter ){
    Error("Unable to retrieve the ILifetimeFitter tool");
    return StatusCode::FAILURE;
  }

  m_vtxfitter = tool<IVertexFit>( m_vertexfitterName, this );
  if( !m_vtxfitter ){
    Error("Unable to retrieve the IVertexFit tool");
    return StatusCode::FAILURE;
  }

  return sc;
}

//=============================================================================

StatusCode TupleToolParticleReFit::fill( const Particle* mother
                                         , const Particle* P
                                         , const std::string& head
                                         , Tuples::Tuple& tuple )
{

  std::string prefix=fullName(head);
  if (m_extraName=="") prefix  =prefix +  "_OVrefit";

  Assert( m_timefitter && m_vtxfitter  && P,
          "Should not happen, you are inside TupleToolParticleReFit.cpp" );


  // no proper-time for basic parts.
  if( P->isBasicParticle() ) return StatusCode::SUCCESS;

  //only head of a decay chain
  if( P != mother ) return StatusCode::SUCCESS;


  const VertexBase* originVtx = m_dva->bestPV( mother );

  if( originVtx ){} // I'm happy
  else {
    Error("Can't get the origin vertex");
    return StatusCode::FAILURE;
  }

  int refit_status = 0;
  int refit_ndof = -100;
  double refit_chi2 = -100;

  double refit_mass =-100;
  double refit_massErr =-100;

  double refit_p =-100;
  double refit_pt =-100;

  double time   = -100;
  double timeErr  = -100;
  double timeChi2 = -100;

  LHCb::Particle O(*P);
  StatusCode sc = m_vtxfitter->reFit(O);

  if(sc.isSuccess()) {
    refit_status =1;
    refit_ndof = O.endVertex()->nDoF();
    refit_chi2 = O.endVertex()->chi2();
    refit_p = O.p();
    refit_pt = O.pt();
    refit_mass = O.measuredMass();
    refit_massErr = O.measuredMassErr();


    sc = m_timefitter->fit ( *originVtx, O , time, timeErr, timeChi2 );
    if( !sc ){
      Warning("The propertime fit failed in TupleToolParticleReFit").ignore();
      time   = -100;
      timeErr  = -100;
      timeChi2 = -100;
    }

  }

  bool test = true;
  test &= tuple->column( prefix+"_status" , refit_status );
  test &= tuple->column( prefix+"_ENDVERTEX_NDOF", refit_ndof );
  test &= tuple->column( prefix+"_ENDVERTEX_CHI2", refit_chi2 );
  test &= tuple->column( prefix+"_MM", refit_mass );
  test &= tuple->column( prefix+"_MMERR", refit_massErr );
  test &= tuple->column( prefix+"_P", refit_p );
  test &= tuple->column( prefix+"_PT", refit_pt );

  test &= tuple->column( prefix+"_P", O.momentum() );

  test &= tuple->column( prefix+"_TAU" , time ); // nanoseconds
  test &= tuple->column( prefix+"_TAUERR" , timeErr );
  test &= tuple->column( prefix+"_TAUCHI2" , timeChi2 );

  return StatusCode(test);

}

