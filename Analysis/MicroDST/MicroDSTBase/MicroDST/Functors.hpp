// $Id: Functors.hpp,v 1.6 2009-04-17 20:47:43 jpalac Exp $
#ifndef MICRODST_FUNCTORS_HPP 
#define MICRODST_FUNCTORS_HPP 1

// Include files
#include "MicroDST/Functions.h"
/** @namespace MicroDST Functors.hpp MicroDST/Functors.hpp
 *  
 *
 *  Collection of useful functors satisfying the Cloner policy and
 *  dealing with the cloning and storing clones into TES locations.
 *
 *  Policy: Cloner must export a clone(const T*) method that returns
 *          a T*. If the input T* is null, Cloner must return a null
 *          pointer.
 *
 *  @author Juan PALACIOS
 *  @date   2007-10-24
 */
namespace MicroDST {

  /**
   *
   * Clone an item into a container of type T. 
   * The functor checks if an item with the same key already exists and only
   * clones and inserts the clone into the container if this is not the case.
   *
   * Requirements:
   *
   * Contained type T:
   * Must export container type as T::Container. 
   * T::Container must have a method 
   * insert(T::value_type, type of T::value_type::key() )
   * Must have a method T* object( type of T::key() )
   *
   * Type T must have method T::key()
   *
   *
   * @author Juan Palacios juancho@nikhef.nl
   * @date 16-10-2007
   */
  template <class T, class itemCloner>
  struct CloneKeyedContainerItem
  {
    CloneKeyedContainerItem(typename T::Container*& to) 
      : m_to(to) { }
    
    T* operator() ( const T* item )
    {
      return clone(item);
    }

    T* clone( const T* item ) 
    {
      if (0==item) return 0;
      if ( !m_to->object( item->key() ) ) {
        T* clonedItem = itemCloner::clone(item);
        if (0!=clonedItem) m_to->insert( clonedItem, item->key()) ;
        return clonedItem;
      }
      return m_to->object( item->key() );
    }

    typename T::Container* m_to;
    
  };


  /**
   *
   * BasicItemCloner satisfying the Cloner policy. 
   * Requirements on template parameter T:
   * T must export a method T* T::clone().
   *
   * @author Juan Palacios juancho@nikhef.nl
   * @date 16-10-2007
   */
  template <class T>
  struct BasicItemCloner 
  {
    T* operator () (const T* item)    { return clone(item);   }
    static T* copy(const T* item) { return clone(item); }
    static T* clone(const T* item) { return item ? item->clone() : 0; }
  };

  /**
   *
   * BasicCopy satisfying the Cloner policy. 
   * Requirements on template parameter T:
   * T must have a copy constructor.
   *
   * @author Juan Palacios juancho@nikhef.nl
   * @date 16-10-2007
   */
  template <class T>
  struct BasicCopy 
  {
    T* operator () (const T* item)    { return copy(item);   }
    static T* copy(const T* item) { return item ? new T(*item) : 0; }
    static T* clone(const T* item) { return copy(item); }
  };

//   template <class TABLE, class ENTRY_CLONER>
//   struct RelationTableCloner
//   {
 
//     RelationTableCloner(const FromCloner& fromCloner,
//                         const ToCloner& toCloner)
//       :
//       m_entryCloner(fromCloner, toCloner)
//     {
//     }
    
//     typename TABLE* operator(const typename TABLE* table) 
//     {
//       typedef typename BindType2Cloner<TABLE>::toType TO_TYPE;
//       typedef typename boost::remove_pointer<typename TABLE::To>::type _To;
//       typedef typename boost::remove_pointer<typename TABLE::From>::type _From;
 
//      TABLE* cloneTable = new TABLE();

//       typename TABLE::Range relations = table->relations();

//       const EntryCloner<TABLE, typename Relations::Traits<TABLE>::Entry> entryCloner(this);

//       for (typename TABLE::Range::const_iterator iRel = relations.begin();
//            iRel != relations.end();
//            ++iRel ) {
//         const typename TABLE::From from = iRel->from();
//         if (from) {
//           typename TABLE::Range range = table->relations(from);
//           for (typename TABLE::Range::const_iterator iRange = range.begin();
//                iRange != range.end();
//                ++iRange) {
//             entryCloner(*iRange, cloneTable);
//           } // loop on related Tos
//         } // if From* found
//       } // loop on all relations
  
//       return cloneTable;

//     }
//   private:
//     ToCloner m_toCloner;
//     FromCloner m_fromCloner;
//   };


  //===========================================================================


}
#endif // MICRODST_FUNCTORS_HPP
