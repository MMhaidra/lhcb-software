// $Id: Particles16.cpp,v 1.1 2006-12-16 13:22:31 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
// ============================================================================
// LoKi 
// ============================================================================
#include "LoKi/Math.h"
#include "LoKi/Particles16.h"
// ============================================================================
/** @file
 *
 *  Implementation file for class LoKi::Particles::CosineDirectionAngle
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-20 
 */
// ============================================================================
/// constructor from the vertex  
// ============================================================================
LoKi::Particles::CosineDirectionAngle::CosineDirectionAngle
( const LHCb::VertexBase* vertex ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , LoKi::Vertices::VertexHolder ( vertex ) 
{}
// ============================================================================
/// constructor from the point 
// ============================================================================
LoKi::Particles::CosineDirectionAngle::CosineDirectionAngle
( const LoKi::Point3D& point ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , LoKi::Vertices::VertexHolder ( point ) 
{}
// ============================================================================
/// constructor from the holder 
// ============================================================================
LoKi::Particles::CosineDirectionAngle::CosineDirectionAngle
( const LoKi::Vertices::VertexHolder& holder ) 
  : LoKi::Function<const LHCb::Particle*> ()
  , LoKi::Vertices::VertexHolder ( holder ) 
{}
// ============================================================================
/// copy constructor
// ============================================================================
LoKi::Particles::CosineDirectionAngle::CosineDirectionAngle
( const LoKi::Particles::CosineDirectionAngle& right ) 
  : LoKi::Function<const LHCb::Particle*> ( right )
  , LoKi::Vertices::VertexHolder          ( right ) 
{}
// ============================================================================
/// destructor 
// ============================================================================
LoKi::Particles::CosineDirectionAngle::~CosineDirectionAngle(){} ;
// ============================================================================
/// MANDATORY: clone method ("virtual destructor")
// ============================================================================
LoKi::Particles::CosineDirectionAngle*
LoKi::Particles::CosineDirectionAngle::clone() const 
{ return new LoKi::Particles::CosineDirectionAngle(*this) ; }
// ============================================================================
LoKi::Particles::CosineDirectionAngle::result_type 
LoKi::Particles::CosineDirectionAngle::operator() 
  ( LoKi::Particles::CosineDirectionAngle::argument p ) const 
{
  if ( 0 == p ) 
    {
      Error ( "LHCb::Particle* points to NULL, return -1000");
      return -1000 ;
    }
  const LHCb::VertexBase* v = p->endVertex() ;
  if ( 0 == v ) 
    {
      Error ( "LHCb::Particle::endVertex points to NULL, return -2000");
      return -2000 ;
    }
  if ( !valid() ) 
    {
      Error ( "VertexHolder base is invalid, return -3000");
      return -3000 ;
    }
  const LoKi::ThreeVector m = p->momentum().Vect() ;
  const LoKi::ThreeVector d = v->position()-position() ;
  const double dot  = m.Dot(d) ;
  // orthogonal vectors ? 
  if ( 0 == dot ) { return 0  ;}
  const double norm = m.Mag2()*d.Mag2() ;
  if ( 0 >= norm ) 
    {
      Warning("Vectors of illegan length, return -4000") ;
      return -4000 ;
    }
  return  dot / ::sqrt( norm ) ;
} ;
// ============================================================================
// OPTIONAL: printout 
// ============================================================================
std::ostream& 
LoKi::Particles::CosineDirectionAngle::fillStream ( std::ostream& s ) const
{ return s << "DIRA" ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
