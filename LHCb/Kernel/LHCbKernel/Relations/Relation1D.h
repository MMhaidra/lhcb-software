// $Id: Relation1D.h,v 1.16 2004-05-03 15:15:38 cattanem Exp $
// =============================================================================
#ifndef RELATIONS_Relation1D_H
#define RELATIONS_Relation1D_H 1
// Include files
#include "Relations/PragmaWarnings.h"
// STD & STL
#include <algorithm>
// from Gaudi
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StreamBuffer.h"
#include "GaudiKernel/MsgStream.h"
// From Relations
#include "Relations/RelationUtils.h"
#include "Relations/IRelation.h"
#include "Relations/RelationBase.h"
#include "Relations/Relation.h"

/** @class Relation1D Relation1D.h Relations/Relation1D.h
 *
 *  @brief Implementation of ordinary unidirectional relations 
 * 
 *  Implementation of ordinary unidirectional relations 
 *  from objects of type "FROM" to objects of type "TO".
 *  
 *  Data types for "FROM" and "TO":
 *
 *   - either inherits form ContainedObject/DataObject 
 *   - or have the templated specializations through 
 *     ObjectTypeTraits structure with defined streamer operator 
 *     to/from StreamBuffer class 
 *  
 *  @see DataObject 
 *  @see ContainedObject 
 *  @see ObjectTypeTraits 
 *  @see StreamBuffer 
 *  @warning for the current implementation the actual type of 
 *           FROM should differ from the actual type of TO
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/01/2002
 */

template<class FROM,class TO>
class Relation1D :
  public  DataObject,
  public  IRelation<FROM,TO>
{
  
public:
  
  /// short cut for own     type
  typedef Relation1D<FROM,TO>              OwnType        ;
  /// short cut for inverse type
  typedef Relation1D<TO,FROM>              InvType        ;
  /// short cut for interface 
  typedef IRelation<FROM,TO>               IBase          ;
  /// import "Range" type from the base 
  typedef typename IBase::Range            Range          ;
  /// import "From"  type from the base 
  typedef typename IBase::From             From           ;
  /// import "To"    type from the base 
  typedef typename IBase::To               To             ;
  /// short cut for the actual implementation type 
  typedef typename Relations::Relation<FROM,TO> Base      ;

  
public:
  
  /// the default constructor
  Relation1D ( const size_t reserve = 0 ) 
    : DataObject() , m_base ( reserve )  
  {
#ifdef COUNT_INSTANCES 
    Relations::InstanceCounter::instance().increment( type() ) ;
#endif // COUNT_INSTANCES
  };
  
  /** constructor from "inverted object"
   *  @param inv object to be inverted
   *  @param flag artificial argument to distinguisch from 
   *  copy constructor
   */
  Relation1D ( const InvType& inv , int flag ) 
    : DataObject( inv ) , m_base( inv  , flag ) 
  {
#ifdef COUNT_INSTANCES 
    Relations::InstanceCounter::instance().increment( type() ) ;
#endif // COUNT_INSTANCES
  };
  
  /** constructor from "inverted interface"
   *  @param inv object to be inverted
   *  @param flag artificial argument to distinguisch from 
   *  copy constructor
   */
  Relation1D ( const typename IBase::InverseType& inv , int flag ) 
    : DataObject() , m_base( inv  , flag )
  {
#ifdef COUNT_INSTANCES 
    Relations::InstanceCounter::instance().increment( type() ) ;
#endif // COUNT_INSTANCES
  };

  /// destructor (virtual)
  virtual ~Relation1D() 
  {
#ifdef COUNT_INSTANCES 
    Relations::InstanceCounter::instance().decrement( type() ) ;
#endif // COUNT_INSTANCES
  };
  
  /// the type name 
  const std::string& type() const 
  {
    static const std::string s_type( System::typeinfoName( typeid(OwnType) ) ) ;
    return s_type ;
  };
  
  /** object identification (static method)
   *  @see DataObject
   *  @see ObjectTypeTraits
   *  @see RelationWeightedTypeTraits
   *  @return the unique class identifier
   */
  static  const CLID& classID()
  {
    static const CLID s_clid =
      Relations::clid( "Relation1D"             ,
                       FromTypeTraits:: id()    ,
                       ToTypeTraits::   id()    ,
                       0                        ,
                       TypeTraits::     version , 0 );
    return s_clid ;
  };
  
  /** object identification (virtual method)
   *  @see DataObject
   *  @return the unique class identifier
   */
  virtual const CLID& clID()     const { return classID() ; }
  
  /** object serialization for writing
   *  @see DataObject
   *  @param  s reference to the output stream
   *  @return   reference to the output stream
   */
  virtual StreamBuffer& serialize ( StreamBuffer& s ) const
  {
    typedef typename FromTypeTraits::Apply      ApplyF     ;
    typedef typename ToTypeTraits::Apply        ApplyT     ;
    typedef typename FromTypeTraits::Serializer SerializeF ;
    typedef typename ToTypeTraits::Serializer   SerializeT ;
    // serialize the base class
    DataObject::serialize( s );
    // get all relations 
    typename IRelation<FROM, TO>::Range range = 
      i_relations() ;
    // serialize the number of relations 
    unsigned long _size = range.size();
    s << _size ;
    // serialise all relations
    for( typename Relation1D<FROM, TO>::iterator entry = range.begin() ;
         range.end() != entry ; ++entry ) 
      {
        SerializeF::serialize 
          ( s , ApplyF::apply ( (*entry).first   , this ) );
        SerializeT::serialize 
          ( s , ApplyT::apply ( (*entry).second  , this ) );
      };
    ///
    return s ;
  };
  
  /** object serialization for reading
   *  @see DataObject
   *  @param  s reference to the input  stream
   *  @return   reference to the input  stream
   */
  virtual StreamBuffer& serialize ( StreamBuffer& s )
  {
    typedef typename FromTypeTraits::Apply      ApplyF     ;
    typedef typename ToTypeTraits::Apply        ApplyT     ;
    typedef typename FromTypeTraits::Serializer SerializeF ;
    typedef typename ToTypeTraits::Serializer   SerializeT ;
    // clear all existing relations 
    i_clear();
    // serialize the base class
    DataObject::serialize( s );
    unsigned long _size ;
    s >> _size ;
    m_base.reserve( _size ) ;
    typename IBase::From from ;
    typename IBase::To   to ;
    while( _size-- > 0 )
      {
        //
        SerializeF::serialize ( s , ApplyF::apply ( from   , this ) ) ;
        SerializeT::serialize ( s , ApplyT::apply ( to     , this ) ) ;
        //
        i_relate( from , to ) ;
      }
    return s ;
  };
  
public:  // major functional methods (fast, 100% inline)
  
  /// retrive all relations from the object (fast,100% inline)
  inline   Range       i_relations
  ( const  From&       object    ) const
  { return m_base.i_relations ( object ) ; }
  
  /// retrive all relations from ALL objects (fast,100% inline)
  inline   Range        i_relations () const
  { return m_base.i_relations () ; }
  
  /// make the relation between 2 objects (fast,100% inline method) 
  inline   StatusCode i_relate
  ( const  From&      object1 ,
    const  To&        object2 )
  { return m_base.i_relate   ( object1 , object2 ) ; }
  
  /// remove the concrete relation between objects (fast,100% inline method)
  inline   StatusCode i_remove 
  ( const  From&      object1 ,
    const  To&        object2 )
  { return m_base.i_remove ( object1 , object2 ) ; }
  
  /// remove all relations FROM the defined object (fast,100% inline method)
  inline   StatusCode i_removeFrom 
  ( const  From&      object )
  { return m_base.i_removeFrom ( object ) ; }    
  
  /// remove all relations TO the defined object (fast,100% inline method)
  inline   StatusCode i_removeTo
  ( const  To&        object )
  { return m_base.i_removeTo( object ) ; }
  
  /// remove ALL relations form ALL  object to ALL objects  (fast,100% inline)
  inline  StatusCode i_clear() 
  { return m_base.i_clear() ; };
  
  /// rebuild ALL relations form ALL  object to ALL objects(fast,100% inline)
  inline  StatusCode i_rebuild() 
  { return m_base.i_rebuild() ; };
  
public: // abstract methods from interface 
  
  /** retrive all relations from the given object object
   *  @param  object  the object
   *  @return pair of iterators for output relations
   */
  virtual Range      relations
  ( const From&      object    ) const { return i_relations( object ) ; }
  
  /** retrive ALL relations from ALL objects  
   *  @return pair of iterators for output relations
   */
  virtual Range      relations () const { return i_relations() ; }
  
  /** make the relation between 2 objects
   *  @param  object1 the first object
   *  @param  object2 the second object
   *  @return status  code
   */
  virtual StatusCode relate
  ( const From&      object1 ,
    const To&        object2 ) { return i_relate( object1 , object2 ) ; }

  /** remove the concrete relation between objects
   *
   *   - if there are no relations between the given object
   *     the error code will be returned
   *
   *  Example:
   *  @code
   *    From object1 = ... ;
   *    To   object2 = ... ;
   *    irel->remove( object1 , object2 );
   *  @endcode
   *
   *  @param  object1 the first object
   *  @param  object2 the second object
   *  @return status  code
   */
  virtual StatusCode   remove
  ( const From&        object1 ,
    const To&          object2 ) { return i_remove( object1 , object2 ) ; }
  
  /** remove all relations FROM the defined object
   *
   *   - if there are no relations from the given onject
   *     the error code will be returned
   *
   *  Example:
   *  @code
   *    From object1 = ... ;
   *    irel->removeFrom( object1 );
   *  @endcode
   *
   *
   *  @param  object the object
   *  @return status code
   */
  virtual StatusCode   removeFrom
  ( const From&        object ) { return i_removeFrom( object ) ; }
  
  /** remove all relations TO the defined object
   *
   *   - if there are no relations to the given object
   *     the error code will be returned
   *
   *  Example:
   *  @code
   *    To object1 = ... ;
   *    irel->removeTo( object1 );
   *  @endcode
   *
   *  @param  object the object
   *  @return status code
   */
  virtual StatusCode   removeTo
  ( const To&          object ) { return i_removeTo( object ) ; }
  
  /** remove ALL relations from ALL to ALL objects
   *
   *  @return status code
   */
  virtual StatusCode   clear () { return i_clear() ; }
  
  /** rebuild ALL relations from ALL  object to ALL objects 
   *
   *  @see IRelationBase 
   *  @return status code
   */
  virtual  StatusCode rebuild() { return i_rebuild () ; };
  
  /** update the object after POOL/ROOT reading 
   *  @see IUpdateable 
   *  @param flag    0 - update after read, 1 - update before write 
   *  @return status code
   */
  virtual StatusCode update( int flag ) 
  {
    if( 0 == flag ) { return i_rebuild() ; }
    return StatusCode::SUCCESS ;
  };
  
public:
  
  /** query the interface
   *  @see    IRelation
   *  @see    IInterface
   *  @param  id  interface identifier
   *  @param  ret placeholder for returned interface 
   *  @return status code
   */
  virtual StatusCode queryInterface
  ( const InterfaceID& id , void** ret )
  {
    if( 0 == ret  )          { return StatusCode::FAILURE ; } // RETURN !!!
    if( IInterface::interfaceID() == id )
      { *ret = static_cast<IInterface*> ( this ); }
    else if( IBase::interfaceID() == id )
      { *ret = static_cast<IBase*>      ( this ); }
    else                     { return StatusCode::FAILURE ; } //  RETURN !!!
    ///
    addRef() ;
    return StatusCode::SUCCESS ;
  };

  /** increase the reference counter
   *  @see    IInterface
   *  @see    DataObject
   *  @return current number of references
   */
  virtual unsigned long addRef  () { return  DataObject::addRef  () ; }
  
  /** release the reference counter
   *  @see    IInterface
   *  @see    DataObject
   *  @return current number of references
   */
  virtual unsigned long release () { return  DataObject::release () ; }
  
private:
  
  Base m_base ;
  
};

// =============================================================================
// The End
// =============================================================================
#endif // RELATIONS_Relation1D_H
// =============================================================================
