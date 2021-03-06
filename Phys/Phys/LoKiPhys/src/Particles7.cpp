// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// LoKiCore
// ============================================================================
// LoKiPhys
// ============================================================================
#include "LoKi/Particles7.h"
// ============================================================================
/** @file
 *
 *  Implementation file for functions from namespace  LoKi::Particles
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-21 
 */
// ============================================================================
LoKi::Particles::VFunAsPFun::VFunAsPFun
( const LoKi::Types::VFunc& vfun , 
  const double              bad  ) 
  : LoKi::AuxFunBase ( std::tie ( vfun , bad ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_vfun ( vfun ) 
  , m_bad  ( bad  ) 
{}
// ===========================================================================
LoKi::Particles::VFunAsPFun::VFunAsPFun
( const LoKi::Particles::VFunAsPFun& right ) 
  : LoKi::AuxFunBase                      ( right ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function ( right ) 
  , m_vfun ( right.m_vfun ) 
  , m_bad  ( right.m_bad  ) 
{}
// ===========================================================================
LoKi::Particles::VFunAsPFun::~VFunAsPFun(){}
// ===========================================================================
LoKi::Particles::VFunAsPFun*
LoKi::Particles::VFunAsPFun::clone() const
{ return new VFunAsPFun(*this) ; }
// ===========================================================================
LoKi::Particles::VFunAsPFun::result_type
LoKi::Particles::VFunAsPFun::operator() 
  ( const LoKi::Particles::VFunAsPFun::argument p ) const 
{
  if ( 0 != p ) { return m_vfun ( p->endVertex() ) ; }      // RETURN 
  Error ( "Invalid Particle, return " + Gaudi::Utils::toString( m_bad ) ) ;
  return m_bad;                                              // RETURN 
}
// ===========================================================================
std::ostream& 
LoKi::Particles::VFunAsPFun::fillStream
( std::ostream& s ) const 
{ return s << "VFASPF(" << m_vfun << "," << m_bad << ")" ; }
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

