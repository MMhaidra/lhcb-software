// $Id$
// ============================================================================
#ifndef LOKI_PHYSMCPARTICLES_H 
#define LOKI_PHYSMCPARTICLES_H 1
// ============================================================================
// Include files
// ============================================================================
// LokiPhysMC 
// ============================================================================
#include "LoKi/MCTruth.h"
// ============================================================================
// LoKiGen
// ============================================================================
#include "LoKi/GenTypes.h"
// ============================================================================
// DaVinciMCKernel
// ============================================================================
#include "Kernel/HepMC2MC.h"
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
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-20 
 */
// ============================================================================
namespace LoKi 
{
  // =========================================================================
  /// forward declarations 
  class MCMatch ;
  // ==========================================================================
  namespace PhysMCParticles 
  { 
    // ========================================================================    
    /** @class GenMCTruth PhysMCParticles.h LoKi/PhysMCParticles.h
     *
     *  Helper and useful function to be used to check the matching of  
     *  LHCb::Particle and some HepMC::GenParticle
     *
     *  @code 
     * 
     *  // some sequence of HepMC-particles
     *  SEQUENCE hepmcps = ...  ;
     *  // get RC<-->MC-matcher 
     *  MCMatch   mc =  ... ;
     *  // get HepMC<-->MC matcher
     *  const LHCb::HepMC2MC* table = ... ;
     *  // create the function 
     *  Cut cut = GMCTRUTH( table , mc , hepmcps.begin() , hepmcps.end() ) ;
     *  
     *  const LHCb::Particle* B = ... ;
     *
     *  // use the predicate!
     * 
     *  const bool good = cut( B ) ;
     *
     *  @endcode 
     *
     *  @see LoKi::Cuts::GMCTRUTH 
     *  @see HepMC::GenParticle
     *  @see LHCb::Particle
     *  @see LoKi::MCMathch
     *  @see LoKi::MCMathchObj
     *  @see LHCb::HepMC2MC 
     *  @author Vanya BELYAEV ibelyaev@physics.sye.edu
     *  @date   2003-01-28
     */
    class GenMCTruth 
      : public LoKi::BasicFunctors<const LHCb::Particle*>::Predicate 
    {
    public:
      // ======================================================================
      /// constructor from HepMC::GenVertex
      GenMCTruth 
      ( const HepMC::GenVertex*             particle   , 
        const LHCb::HepMC2MC*               table      , 
        const LoKi::MCMatch&                matcher    ) ;
      /// constructor from HepMC::GenParticle
      GenMCTruth 
      ( const HepMC::GenParticle*           particle   , 
        const LHCb::HepMC2MC*               table      , 
        const LoKi::MCMatch&                matcher    ) ;
      /// constructor from container of HepMC::GenParticles
      GenMCTruth 
      ( const LoKi::GenTypes::GenContainer& particles  , 
        const LHCb::HepMC2MC*               table      , 
        const LoKi::MCMatch&                matcher    ) ;
      /// constructor from range of HepMC::GenParticles
      GenMCTruth 
      ( const LoKi::Types::GRange&          particles  , 
        const LHCb::HepMC2MC*               table      , 
        const LoKi::MCMatch&                matcher    ) ;
      /// templated constructor 
      template <class GENERATOR>
      GenMCTruth 
      ( GENERATOR                 first    ,
        GENERATOR                 last     ,
        const LHCb::HepMC2MC*     table    , 
        const LoKi::MCMatch&      match    ) 
        : LoKi::BasicFunctors<const LHCb::Particle*>::Predicate () 
        , m_table ( table ) 
        , m_match ( match ) 
        , m_hepmc () 
        , m_mc    () 
      { add ( first , last ) ; } 
      /// MANDATORY: copy constructor 
      GenMCTruth ( const GenMCTruth& right ) ;
      /// MANDATORY: virtual destructor
      virtual ~GenMCTruth() ;
      /// MANDATORY: clone method ("virtual constructor")
      virtual GenMCTruth* clone () const ; 
      /// MANDATORY: the only one essential method ("function")
      virtual result_type operator () ( argument ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /// rebuild the internal storages 
      StatusCode rebuild() ;
      /// add HepMC::GenParticle
      GenMCTruth& add ( const HepMC::GenParticle*           p ) ;
      /// add HepMC::GenVertex
      GenMCTruth& add ( const HepMC::GenVertex*             p ) ;
      /// add LoKi::GenTypes::GenContainer
      GenMCTruth& add ( const LoKi::GenTypes::GenContainer& p ) ;
      /// add LoKi::Types::GRange 
      GenMCTruth& add ( const LoKi::Types::GRange&          p ) ;
      /// add a sequence of something 
      template <class OBJECT>
      GenMCTruth& add ( OBJECT first , 
                        OBJECT last  ) 
      {
        _add ( first , last ) ;
        rebuild() ;
        return *this ;
      } 
      // ======================================================================
    public:
      // ======================================================================
      /// conversion operator to relation table:
      operator const LoKi::Interface<LHCb::HepMC2MC>&        () const 
      { return m_table ; }
      /// conversion operator to the HepMC-storage
      operator const LoKi::UniqueKeeper<HepMC::GenParticle>& () const 
      { return m_hepmc ; }
      // conversion operator to LoKi::MCMatch object 
      operator const LoKi::MCMatch&                          () const 
      { return m_match ; }
      // ======================================================================
    protected:
      // ======================================================================
      /// add HepMC::GenParticle
      void _add ( const HepMC::GenParticle*              p ) ;
      /// add HepMC::GenVertex
      void _add ( const HepMC::GenVertex*                p ) ;
      /// add LoKi::GenTypes::GenContainer
      void _add ( const LoKi::GenTypes::GenContainer&    p ) ;
      /// add LoKi::Types::GRange 
      void _add ( const LoKi::Types::GRange&             p ) ;
      /// add a sequence of something 
      template <class OBJECT>
      void _add ( OBJECT first , OBJECT last  ) 
      { for ( ; first != last ; ++first ) { _add ( *first ) ; } }
      // ======================================================================
    private:
      // the first relation table HepMC -> MC  
      LoKi::Interface<LHCb::HepMC2MC>        m_table ;
      // the helper object for MC->RC relations 
      LoKi::MCMatch                          m_match ;
      // local storage of generator particles 
      LoKi::UniqueKeeper<HepMC::GenParticle> m_hepmc ;
      // local storage of MC-particles 
      LoKi::UniqueKeeper<LHCb::MCParticle>   m_mc    ;
    } ;
    // ========================================================================
    /** @class RCTruth PhysMCParticles.h LoKi/PhysMCParticles.h
     *  
     *  Helper and useful function to be used to check the matching of  
     *  LHCb::Particle and some LHCb::MCParticle
     *
     *  @see LoKi::MCMatchObj
     *  @see LoKi::MCMatch
     *  @see LHCb::Particle
     *  @see LHCb::MCParticle
     *  @see LoKi::Cuts::RCTRUTH
     *
     *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
     *  @date   2003-01-28
     */
    class RCTruth 
      : public LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate
      , public LoKi::Keeper<LHCb::Particle>
    {
    public:      
      // ======================================================================
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param particle  pointer to particle object 
       */
      RCTruth 
      ( const LoKi::MCMatch&  match    , 
        const LHCb::Particle* particle ) ;
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param range "container" of particles 
       */
      RCTruth 
      ( const LoKi::MCMatch&      match , 
        const LoKi::Types::Range& range ) ;
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param range container of particles 
       */
      RCTruth
      ( const LoKi::MCMatch&          match , 
        const LHCb::Particle::Vector& range ) ;      
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param range container of particles 
       */
      RCTruth
      ( const LoKi::MCMatch&               match , 
        const LHCb::Particle::ConstVector& range ) ;
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param range container of particles 
       */
      RCTruth
      ( const LoKi::MCMatch&                match , 
        const LoKi::Keeper<LHCb::Particle>& range ) ;
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param range container of particles 
       */
      RCTruth
      ( const LoKi::MCMatch&                      match , 
        const LoKi::UniqueKeeper<LHCb::Particle>& range ) ;      
      /** constructor 
       *  @param match MCMatch object (working horse)
       *  @param begin begin iterator of any sequence of particles  
       *  @param end   end   iterator of any sequence of particles  
       */
      template <class PARTICLE>
      RCTruth 
      ( const LoKi::MCMatch&  match    , 
        PARTICLE              begin    , 
        PARTICLE              end      ) 
        : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate () 
        , LoKi::Keeper<LHCb::Particle> ( begin , end )  
        , m_match ( match )
      {} 
      /** constructor 
       *  @param particle  pointer to particle object 
       *  @param match MCMatch object (working horse)
       */
      RCTruth 
      ( const LHCb::Particle* particle ,
        const LoKi::MCMatch&  match    ) ;
      /** constructor 
       *  @param range "container" of particles 
       *  @param match MCMatch object (working horse)
       */
      RCTruth 
      ( const LoKi::Types::Range& range ,
        const LoKi::MCMatch&      match ) ;
      /** constructor 
       *  @param range container of particles 
       *  @param match MCMatch object (working horse)
       */
      RCTruth
      ( const LHCb::Particle::Vector& range ,
        const LoKi::MCMatch&          match ) ;
      /** constructor 
       *  @param range container of particles 
       *  @param match MCMatch object (working horse)
       */
      RCTruth
      ( const LHCb::Particle::ConstVector& range ,
        const LoKi::MCMatch&               match ) ;
      /** constructor 
       *  @param range container of particles 
       *  @param match MCMatch object (working horse)
       */
      RCTruth
      ( const LoKi::Keeper<LHCb::Particle>& range ,
        const LoKi::MCMatch&                match ) ;
      /** constructor 
       *  @param range container of particles 
       *  @param match MCMatch object (working horse)
       */
      RCTruth
      ( const LoKi::UniqueKeeper<LHCb::Particle>& range ,
        const LoKi::MCMatch&                      match ) ;
      /** constructor 
       *  @param begin begin iterator of any sequence of particles  
       *  @param end   end   iterator of any sequence of particles  
       *  @param match MCMatch object (working horse)
       */
      template <class PARTICLE>
      RCTruth 
      ( PARTICLE              begin    , 
        PARTICLE              end      ,
        const LoKi::MCMatch&  match    ) 
        : LoKi::BasicFunctors<const LHCb::MCParticle*>::Predicate () 
        , LoKi::Keeper<LHCb::Particle> ( begin , end )  
        , m_match ( match )
      {} 
      /// copy constructor
      RCTruth( const RCTruth& right ) ;
      /// MANDATORY : virtual destructor destructor 
      virtual ~RCTruth();
      /// MANDATORY: clone method ("virtual constructor")
      virtual RCTruth* clone    () const ;
      /// MANDATORY: the only one essential method ("function")
      virtual result_type operator () ( argument ) const ;
      /// OPTIONAL: the specific printout 
      virtual std::ostream& fillStream ( std::ostream& s ) const ;
      // ======================================================================
    public:
      // ======================================================================
      /** add additional particle to the 
       *  list of Particles to be matched 
       *  @param p particle to be added 
       */
      RCTruth& add ( const LHCb::Particle* p ) 
      { addObject ( p )  ; return *this ; }
      /** add additional particles to the 
       *  list of MCParticles to be matched 
       *  @param range new range of MC particles 
       */
      RCTruth& add ( const LoKi::Types::Range& range ) 
      { return add ( range.begin() , range.end() ) ; }
      /** add few additional particles to the list of 
       *  Particles  to be matched 
       *  @param first "begin" iterator of sequence 
       *  @param last   
       */
      template <class PARTICLE>
      RCTruth& add ( PARTICLE first  , 
                     PARTICLE last   ) 
      { 
        addObjects ( first , last ) ;
        return *this ;
      }      
      // ======================================================================
    private:
      // ======================================================================
      // default constructor is disabled 
      RCTruth();
      // assignement is disabled 
      RCTruth& operator=( const RCTruth& ) ;
      // ======================================================================
    private:
      // ======================================================================
      /// MC-match object
      LoKi::MCMatch  m_match ;                               // MC-match object
      // ======================================================================
    };
    // ========================================================================
    namespace Particles 
    {
      // ======================================================================
      /// import the type into the proper namespace 
      typedef LoKi::PhysMCParticles::MCTruth    MCTruth    ;      
      /// import the type into the proeper namespace 
      typedef LoKi::PhysMCParticles::GenMCTruth GenMCTruth ;
      // ======================================================================
    }
    // ========================================================================
    namespace MCParticles 
    {
      // ======================================================================
      /// import the type into proper namespace 
      typedef LoKi::PhysMCParticles::RCTruth    RCTruth    ;
      // ======================================================================
    }
    // ========================================================================
  } //                                   end of namespace LoKi::PhysMCParticles
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PHYSMCPARTICLES_H
// ============================================================================
