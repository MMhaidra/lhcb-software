// $Id: TableExtraTraits.h,v 1.1 2009-04-17 20:49:20 jpalac Exp $
#ifndef MICRODST_TABLETRAITS_H 
#define MICRODST_TABLETRAITS_H 1

// Include files
#include "Kernel/Particle2MCParticle.h"
#include "Kernel/Particle2Vertex.h"
/** @namespace _Relations Tables.h MicroDST/Tables.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2009-04-17
 */
namespace Relations 
{

  template <class TABLE>   
  struct Cloners
  {
    typedef boost::function< const typename TABLE::From (const typename TABLE::From )> From;
    typedef boost::function< const typename TABLE::To (const typename TABLE::To )> To;
  };


  template <class TABLE>
  struct EntryClonerBase
  {
    EntryClonerBase(const typename Relations::Cloners<TABLE>::From& fromCloner,
                    const typename Relations::Cloners<TABLE>::To& toCloner)
      :
      m_fromCloner(fromCloner),
      m_toCloner(toCloner)
    {
    }

  protected:
    typename Relations::Cloners<TABLE>::From m_fromCloner;
    typename Relations::Cloners<TABLE>::To m_toCloner;

  private:
    EntryClonerBase() {}
    
  };

  template <class TABLE>
  struct WEntryCloner : public EntryClonerBase<TABLE>
  {
    
    WEntryCloner(const typename Relations::Cloners<TABLE>::From& fromCloner,
                 const typename Relations::Cloners<TABLE>::To& toCloner)
      :
      EntryClonerBase<TABLE>(fromCloner, toCloner)
    {
    }
    template <typename Entry>
    void operator() (const Entry& entry, TABLE* table) const {
      const typename TABLE::From clonedFrom = m_fromCloner(entry.from());
      const typename TABLE::To clonedTo = m_toCloner( entry.to() );
      if (clonedFrom&&clonedTo) {
        table->relate(clonedFrom, clonedTo, entry.weight());
      }
    }
  };

  template <class TABLE>
  struct EntryCloner : public EntryClonerBase<TABLE>
  {
    EntryCloner(const typename Relations::Cloners<TABLE>::From& fromCloner,
                const typename Relations::Cloners<TABLE>::To& toCloner)
      :
      EntryClonerBase<TABLE>(fromCloner, toCloner)
    {
    }
    template <typename Entry>
    void operator() (const Entry& entry, TABLE* table) const {
      const typename TABLE::From clonedFrom = m_fromCloner(entry.from());
      const typename TABLE::To clonedTo = m_toCloner( entry.to() );
      if (clonedFrom&&clonedTo) {
        table->relate(clonedFrom, clonedTo);
      }
    }

  };

  template <class TABLE> struct Traits { };

  template<>
  struct Traits< LHCb::Relation1D<LHCb::Particle,LHCb::MCParticle> >
  {
    typedef Relations::Entry_<LHCb::Particle, LHCb::MCParticle> Entry;
    typedef EntryCloner< LHCb::Relation1D<LHCb::Particle,LHCb::MCParticle> > EntryCloner;
  };

  template<>
  struct Traits< LHCb::RelationWeighted1D<LHCb::Particle,LHCb::MCParticle, double> >
  {
    typedef Relations::WEntry_<LHCb::Particle, LHCb::MCParticle, double> Entry;
    typedef WEntryCloner< LHCb::RelationWeighted1D<LHCb::Particle,LHCb::MCParticle, double> > EntryCloner;
    
  };

  template<>
  struct Traits< LHCb::Relation1D<LHCb::Particle, LHCb::VertexBase> >
  {
    typedef Relations::Entry_<LHCb::Particle, LHCb::VertexBase> Entry;
    typedef EntryCloner< LHCb::Relation1D<LHCb::Particle,LHCb::VertexBase> > EntryCloner;
    
  };

  template<>
  struct Traits< LHCb::RelationWeighted1D<LHCb::Particle, LHCb::VertexBase, double> >
  {
    typedef Relations::WEntry_<LHCb::Particle, LHCb::VertexBase, double> Entry;
    typedef WEntryCloner< LHCb::RelationWeighted1D<LHCb::Particle,LHCb::VertexBase, double> > EntryCloner;    
  };
  
  
}

#endif // MICRODST_TABLETRAITS_H
