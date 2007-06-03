// $Id: LoopChild.cpp,v 1.1 2007-06-03 20:43:42 ibelyaev Exp $
// ============================================================================
// CVS tag $Nam:$ 
// ============================================================================
// $Log: not supported by cvs2svn $
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
#include "LoKi/Loop.h"
#include "LoKi/LoopChild.h"
// ============================================================================
/*  get the number of children 
 *  @param mother poiunter to mother particle 
 *  @return number of daugthter particles 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-06-02
 */   
// ============================================================================
std::size_t LoKi::Child::nChildren 
( const LoKi::Loop& particle )
{
  return !particle ? 0 : particle->dim() ;
}
// ============================================================================
/*  Trivial accessor to the dauthter particles for the given "particle"
 *
 *  @attention index starts with 1 , null index corresponds 
 *             to the original particle 
 *
 *  @param  particle (const) pointer to mother particle 
 *  @param  index    index   index of the child particle 
 *  @return daughter particle with given index 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-06-02
 */
// ============================================================================
LHCb::Particle* LoKi::Child::child
( const LoKi::Loop& particle , 
  const size_t      index    ) 
{
  const LHCb::Particle* p = particle.child( index ) ;
  if ( 0 == p ) { return  0 ; }                                   // RETURN 
  return const_cast<LHCb::Particle*> ( p ) ;
}
// ============================================================================
/*  Trivial accessor to the dauthter particles for the given "particle"
 *
 *  @attention index starts with 1 , null index corresponds 
 *             to the original particle 
 *
 *  @param  particle (const) pointer to mother particle 
 *  @param  index1   index   index of the child particle 
 *  @param  index2   index   index of the child particle 
 *  @return daughter particle with given index 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-06-02
 */    
// ============================================================================
LHCb::Particle* LoKi::Child::child
( const LoKi::Loop& particle , 
  const size_t      index1   , 
  const size_t      index2   )
{ return child ( child ( particle , index1 ) , index2 ) ; }    
// ============================================================================
/*  Trivial accessor to the dauthter particles for the given "particle"
 *
 *  @attention index starts with 1 , null index corresponds 
 *             to the original particle 
 *
 *  @param  particle (const) pointer to mother particle 
 *  @param  index1   index   index of the child particle 
 *  @param  index2   index   index of the child particle 
 *  @param  index3   index   index of the child particle 
 *  @return daughter particle with given index 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-06-02
 */    
// ============================================================================
LHCb::Particle* LoKi::Child::child
( const LoKi::Loop& particle , 
  const size_t      index1   , 
  const size_t      index2   ,
  const size_t      index3   )
{ return child ( child ( particle , index1 ) , index2 , index3 ) ; }
// ============================================================================
/*  Trivial accessor to the dauthter particles for the given "particle"
 *
 *  @attention index starts with 1 , null index corresponds 
 *             to the original particle 
 *
 *  @param  particle (const) pointer to mother particle 
 *  @param  index1   index   index of the child particle 
 *  @param  index2   index   index of the child particle 
 *  @param  index3   index   index of the child particle 
 *  @param  index4   index   index of the child particle 
 *  @return daughter particle with given index 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2007-06-02
 */    
// ============================================================================
LHCb::Particle* LoKi::Child::child
( const LoKi::Loop&  particle , 
  const size_t       index1   , 
  const size_t       index2   ,
  const size_t       index3   ,
  const size_t       index4   )
{ return child ( child ( particle , index1 ) , index2 , index3 , index4 ) ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
