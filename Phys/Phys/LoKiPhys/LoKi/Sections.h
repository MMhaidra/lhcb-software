// $Id$
// ============================================================================
#ifndef LOKI_SECTIONS_H 
#define LOKI_SECTIONS_H 1
// ============================================================================
// Include files
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Child.h"
// ============================================================================
/** @file
 *  Collection of varios functions related to the "sections" for 
 *  the certain graph/tree
 *  @author Vanya  BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2008-04-29
 */
// ============================================================================
namespace Decays 
{
  // ========================================================================
  /// the actual type for the graph/tree section 
  typedef LHCb::Particle::ConstVector Section ;
  // ========================================================================
  /// the actual type for collection of sections  
  typedef std::vector<Section>        Sections ;
  // ========================================================================
  /** simple function to extarct all section from the graph/tree 
   *  @param particle the head of th egraph/tree 
   *  @param output the sections of the graph/tree 
   *  @return size of the section container 
   */
  size_t sections 
  ( const LHCb::Particle* particle , Sections& output ) ;
  // ========================================================================
  /** simple function to extract all section from the graph/tree 
   *  @param particle the head of th egraph/tree 
   *  @return sice of the section container 
   */
  inline Sections sections ( const LHCb::Particle* particle )
  {
    Sections result ;
    sections ( particle , result ) ;
    return result ;
  }
  // ========================================================================
  /** get the most trivial section (daughters) 
   *  @author Vanya  BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2008-05-25
   */ 
  inline size_t section
  ( const LHCb::Particle* particle , 
    Section&              output   )
  {
    return LoKi::Child::daughters ( particle , output ) ; 
  }
  // ========================================================================
  /** get the most trivial section (daughters) 
   *  @author Vanya  BELYAEV Ivan.Belyaev@nikhef.nl
   *  @date   2008-05-25
   */ 
  inline Section section ( const LHCb::Particle* particle )  
  {
    Section result ;
    section ( particle , result ) ;
    return result ;
  }
  // ========================================================================      
} // end of namespace LoKi::Decay 
// ============================================================================
#endif // LOKI_SECTIONS_H
// ============================================================================
