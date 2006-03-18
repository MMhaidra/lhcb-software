// $Id: MC2Collision.h,v 1.2 2006-03-18 19:15:10 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2006/03/18 18:26:43  ibelyaev
//  add (primary)MCVErtex->GenCollision links
//
// ============================================================================
#ifndef KERNEL_MC2COLLISION_H 
#define KERNEL_MC2COLLISION_H 1
// ============================================================================
// Include files
// ============================================================================
// Event
// ============================================================================
#include "Event/MCVertex.h"
#include "Event/GenCollision.h"
// ============================================================================
// forward declarations 
// ============================================================================
namespace LHCb
{
  class MCParticle   ;
  class MCVertex     ;
  class MCHit        ;
  class MCCaloHit    ;
  class MCRichHit    ;
  class MCOTDeposit  ;
  class MCSTDeposit  ;
};
template <class FROM, class TO> class IRelation;
// ============================================================================

// ============================================================================
/** @file
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
 *  @date 2006-03-18 
 */
// ============================================================================

namespace LHCb
{
  namespace Relations_
  {
    /// the actual type for MCVertex  --> GenCollision relations 
    typedef IRelation<LHCb::MCVertex,LHCb::GenCollision> MCVertex2Collision ;
  } ;
  /// the actual type for MCVertex  --> GenCollision relations 
  typedef   LHCb::Relations_::MCVertex2Collision          MCVertex2Collision ;
  ///
  namespace MC2CollisionLocation
  {
    const std::string Default = 
    "Relations/" + LHCb::MCVertexLocation::Default + "2Collision";
  };
} ;

// ============================================================================
/** @namespace MC2Collision MC2Collision.h Kernel/MC2Collision.h
 *  
 *  Collection of helper utilities to determine the collision for 
 *  MC objects 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-03-18
 */
// ==========================================================================
namespace MC2Collision 
{
  /** @fn collision 
   *  get the GenCollision object, associated with the given vertex
   *
   *  @code 
   *
   *  // get the relation table
   *  const LHCb::MCVertex2Collision* table = ... ;
   * 
   *  const LHCb::MCVertex* vertex = ... ;
   *  
   *  // get the GenCollision object:
   *  const LHCb::GenCollision* c = collision ( vertex , table ) ;
   * 
   *  @endcode
   * 
   *  @see LHCb::MCVertex
   *  @see LHCb::GenCollision
   *  @see LHCb::MCVertex2Collision
   *
   *  @param vertex vertex to be inspected 
   *  @param table  information provider
   *  @return the associated collision object 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-18
   */
  const LHCb::GenCollision* 
  collision 
  ( const LHCb::MCVertex*           vertex , 
    const LHCb::MCVertex2Collision* table  ) ;

  /** @fn collision 
   *  get the GenCollision object, associated with the given particle
   *
   *  @code 
   *
   *  // get the relation table
   *  const LHCb::MCVertex2Collision* table = ... ;
   * 
   *  const LHCb::MCParticle* particle = ... ;
   *  
   *  // get the GenCollision object:
   *  const LHCb::GenCollision* c = collision ( particle , table ) ;
   * 
   *  @endcode
   * 
   *  @see LHCb::MCParticle
   *  @see LHCb::GenCollision
   *  @see LHCb::MCVertex2Collision
   *
   *  @param particle particle to be inspected 
   *  @param table  information provider
   *  @return the associated collision object 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-18
   */
  const LHCb::GenCollision* 
  collision 
  ( const LHCb::MCParticle*         particle , 
    const LHCb::MCVertex2Collision* table    ) ;

  /** @fn collision 
   *  get the GenCollision object, associated with the given hit
   *
   *  @code 
   *
   *  // get the relation table
   *  const LHCb::MCVertex2Collision* table = ... ;
   * 
   *  const LHCb::MCHit* hit = ... ;
   *  
   *  // get the GenCollision object:
   *  const LHCb::GenCollision* c = collision ( hit , table ) ;
   * 
   *  @endcode
   * 
   *  @see LHCb::MCHit
   *  @see LHCb::GenCollision
   *  @see LHCb::MCVertex2Collision
   *
   *  @param hit    hit to be inspected 
   *  @param table  information provider
   *  @return the associated collision object 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-18
   */
  const LHCb::GenCollision* 
  collision 
  ( const LHCb::MCHit*              hit    , 
    const LHCb::MCVertex2Collision* table  ) ;

  /** @fn collision 
   *  get the GenCollision object, associated with the given hit
   *
   *  @code 
   *
   *  // get the relation table
   *  const LHCb::MCVertex2Collision* table = ... ;
   * 
   *  const LHCb::MCHit* hit = ... ;
   *  
   *  // get the GenCollision object:
   *  const LHCb::GenCollision* c = collision ( hit , table ) ;
   * 
   *  @endcode
   * 
   *  @see LHCb::MCHit
   *  @see LHCb::GenCollision
   *  @see LHCb::MCVertex2Collision
   *
   *  @param hit    hit to be inspected 
   *  @param table  information provider
   *  @return the associated collision object 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-18
   */
  const LHCb::GenCollision* 
  collision 
  ( const LHCb::MCHit*              hit    , 
    const LHCb::MCVertex2Collision* table  ) ;

  /** @fn collision 
   *  get the GenCollision object, associated with the given hit
   *
   *  @code 
   *
   *  // get the relation table
   *  const LHCb::MCVertex2Collision* table = ... ;
   * 
   *  const LHCb::MCCaloHit* hit = ... ;
   *  
   *  // get the GenCollision object:
   *  const LHCb::GenCollision* c = collision ( hit , table ) ;
   * 
   *  @endcode
   * 
   *  @see LHCb::MCCaloHit
   *  @see LHCb::GenCollision
   *  @see LHCb::MCVertex2Collision
   *
   *  @param hit    hit to be inspected 
   *  @param table  information provider
   *  @return the associated collision object 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-18
   */
  const LHCb::GenCollision* 
  collision 
  ( const LHCb::MCCaloHit*          hit    , 
    const LHCb::MCVertex2Collision* table  ) ;

  /** @fn collision 
   *  get the GenCollision object, associated with the given deposit
   *
   *  @code 
   *
   *  // get the relation table
   *  const LHCb::MCVertex2Collision* table = ... ;
   * 
   *  const LHCb::MCOTDeposit* hit = ... ;
   *  
   *  // get the GenCollision object:
   *  const LHCb::GenCollision* c = collision ( hit , table ) ;
   * 
   *  @endcode
   * 
   *  @see LHCb::MCOTDeposit
   *  @see LHCb::GenCollision
   *  @see LHCb::MCVertex2Collision
   *
   *  @param hit    hit to be inspected 
   *  @param table  information provider
   *  @return the associated collision object 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-18
   */
  const LHCb::GenCollision* 
  collision 
  ( const LHCb::MCOTDeposit*        hit    , 
    const LHCb::MCVertex2Collision* table  ) ;

  /** @fn collision 
   *  get the GenCollision object, associated with the given deposit
   *
   *  @code 
   *
   *  // get the relation table
   *  const LHCb::MCVertex2Collision* table = ... ;
   * 
   *  const LHCb::MCSTDeposit* hit = ... ;
   *  
   *  // get the GenCollision object:
   *  const LHCb::GenCollision* c = collision ( hit , table ) ;
   * 
   *  @endcode
   * 
   *  @see LHCb::MCSTDeposit
   *  @see LHCb::GenCollision
   *  @see LHCb::MCVertex2Collision
   *
   *  @param hit    hit to be inspected 
   *  @param table  information provider
   *  @return the associated collision object 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-18
   */
  const LHCb::GenCollision* 
  collision 
  ( const LHCb::MCSTDeposit*        hit    , 
    const LHCb::MCVertex2Collision* table  ) ;

  /** @fn collision 
   *  get the GenCollision object, associated with the given Rich hit
   *
   *  @code 
   *
   *  // get the relation table
   *  const LHCb::MCVertex2Collision* table = ... ;
   * 
   *  const LHCb::MCRichHit* hit = ... ;
   *  
   *  // get the GenCollision object:
   *  const LHCb::GenCollision* c = collision ( hit , table ) ;
   * 
   *  @endcode
   * 
   *  @see LHCb::MCRichHit
   *  @see LHCb::GenCollision
   *  @see LHCb::MCVertex2Collision
   *
   *  @param hit    hit to be inspected 
   *  @param table  information provider
   *  @return the associated collision object 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-18
   */
  const LHCb::GenCollision* 
  collision 
  ( const LHCb::MCRichHit*          hit    , 
    const LHCb::MCVertex2Collision* table  ) ;
  
  /** @fn primaryVertex
   *  Simple function wich returns the primary vertex for the given vertex
   *  
   *  @param vertex vertex
   *  @return the primary vertex 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-18
   */
  const LHCb::MCVertex* 
  primaryVertex ( const LHCb::MCVertex* vertex ) ;
  /** @fn primaryVertex
   *  Simple function wich returns the primary vertex for the given particle 
   *  
   *  @param particle particle
   *  @return the primary vertex 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-18
   */
  const LHCb::MCVertex* 
  primaryVertex ( const LHCb::MCParticle* hit ) ;
  /** @fn primaryVertex
   *  Simple function wich returns the primary vertex for the given hit 
   *  
   *  @param hit hit 
   *  @return the primary vertex 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-18
   */
  const LHCb::MCVertex* 
  primaryVertex ( const LHCb::MCHit* hit ) ;
  /** @fn primaryVertex
   *  Simple function wich returns the primary vertex for the given hit 
   *  
   *  @param hit hit 
   *  @return the primary vertex 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-18
   */
  const LHCb::MCVertex* 
  primaryVertex ( const LHCb::MCCaloHit* hit ) ;
  /** @fn primaryVertex
   *  Simple function wich returns the primary vertex for the given hit 
   *  
   *  @param hit hit 
   *  @return the primary vertex 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-18
   */
  const LHCb::MCVertex* 
  primaryVertex ( const LHCb::MCRichHit* hit ) ;
  /** @fn primaryVertex
   *  Simple function wich returns the primary vertex for the given hit 
   *  
   *  @param hit hit 
   *  @return the primary vertex 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-18
   */
  const LHCb::MCVertex* 
  primaryVertex ( const LHCb::MCOTDeposit* hit ) ;
  /** @fn primaryVertex
   *  Simple function wich returns the primary vertex for the given hit 
   *  
   *  @param hit hit 
   *  @return the primary vertex 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-18
   */
  const LHCb::MCVertex* 
  primaryVertex ( const LHCb::MCSTDeposit* hit ) ;
  
  /** @fn fromCollision
   *  Helper function which checs, is the given (primary) vertex 
   *  has as an origin the given collision
   *  @param vertex the vertex 
   *  @param collision the collision
   *  @param table the information source
   *  @return true if the vertex comes from the collision
   */
  bool fromCollision 
  ( const LHCb::MCVertex*            vertex    , 
    const LHCb::GenCollision*        collision , 
    const LHCb::MCVertex2Collision*  table     ) ;
  
  /** @class FromCollision
   *
   *  Helper class(functor) to find check if the MC-objects 
   *  originates form a given GenCollision
   *  
   *  @code
   *
   *  // get the relation table  
   *  const LHCb::MCVertex2Collision* table = ... ;
   *  
   *  // the collision 
   *  const LHCb::GenCollision* c = ... ; 
   *
   *  // A) count MC-hits from the given collision:
   *
   *  // get MCHits form TES
   *  const LHCb::MCHits* hits = get<LHCb::MCHits>( ... ) ;
   * 
   *  const size_t nHits = 
   *   std::count_if ( hits->begin()                           , 
   *                   hits->end()                             ,
   *                   FromCollision<LHCb::MCHit>( c , table ) ) ;
   * 
   *  // B) get all MC-particles form the given collison 
   *
   *  // get MCParticles form TES
   *  const LHCb::MCParticles* mcps = get<LHCb::MCParticles>( ... ) ;
   *  // prepare the output container
   *  LHCb::Particle::ConstVector result ;
   *  // put all particles from the given collisinig into the output container:
   *  LoKi::Algs::copy_if ( mcps->begin() , 
   *                        mcps->end  () , 
   *                        FromCollision<LHCb::MCParticle>( c , table ) ) ;
   *
   *
   *  @endcode
   *  
   *  @see LHCb::GenCollision  
   *  @see LHCb::MCVertex2Collision  
   *  @see MC2Collision 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-18
   */
  template <class TYPE> 
  class FromCollision
    : public std::unary_function<const TYPE*,bool>
  {
  public:
    /// constructor from the collision and the information provider 
    FromCollision
    ( const LHCb::GenCollision*       collision , 
      const LHCb::MCVertex2Collision* table     ) 
      : m_collision ( collision )
      , m_table ( table )
    {};
    /// the only one essential method 
    bool operator () ( const TYPE* object ) const 
    { return fromCollision
        ( primaryVertex ( object ) , m_collision , m_table ) ; }
  private:
    // the default constructor is disabled
    FromCollision();
  private:
    const LHCb::GenCollision*       m_collision ;
    const LHCb::MCVertex2Collision* m_table     ;
  } ;

} ; // end of namespace MC2Collision
   
   
// ============================================================================
// The END 
// ============================================================================
#endif // KERNEL_MC2COLLISION_H
// ============================================================================
