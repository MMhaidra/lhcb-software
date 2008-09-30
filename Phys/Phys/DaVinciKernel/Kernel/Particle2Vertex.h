// $Id: Particle2Vertex.h,v 1.4 2008-09-30 14:18:41 jpalac Exp $
#ifndef KERNEL_PARTICLE2VERTEX_H 
#define KERNEL_PARTICLE2VERTEX_H 1

// Include files
#include "Event/VertexBase.h"
#include "Event/Particle.h"
#include "Relations/RelationWeighted1D.h"
/** @namespace Particle2Vertex Particle2Vertex.h Kernel/Particle2Vertex.h
 *  
 * Namespace containing types corresponding to the Particle -> VertexBase 
 * uni-directional weighted relationship.
 *
 *  @author Juan PALACIOS
 *  @date   2006-08-17
 */
namespace Particle2Vertex {
  typedef double                                          WeightType ;
  typedef LHCb::RelationWeighted1D<LHCb::Particle, 
                                   LHCb::VertexBase, 
                                   WeightType>            Table      ;
  typedef Table::Range                                    Range      ;
  typedef Table::IBase::TypeTraits::Entry                 Relation   ;
  typedef Table::IBase::TypeTraits::Entries               Relations  ;
  typedef Table::To                                       To         ;
  typedef Table::From                                     From       ;
  typedef WeightType                                      Weight     ;


}
#endif // KERNEL_PARTICLE2VERTEX_H
