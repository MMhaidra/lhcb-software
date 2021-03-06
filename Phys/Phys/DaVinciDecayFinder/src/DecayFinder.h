// $Id$
#ifndef TOOLS_DECAYFINDER_H
#define TOOLS_DECAYFINDER_H 1

//#define YYDEBUG 1
#ifdef YYDEBUG
#include <stdio.h>
#include <stdlib.h>
#endif

// Include files
// from STL
#include <string>

#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IDecayFinder.h"

// ============================================================================
namespace LHCb { class IParticlePropertySvc; }
// ============================================================================
class IDataProviderSvc;
struct yy_buffer_state;

/** @class DecayFinder DecayFinder.h
 *
 *  This tool allows you to search a vector of Particle for a specific decay.
 *  The decay is given through the property 'Decay' as a string which describe
 *  what you are looking for. The syntax of this expression has been designed
 *  to be as powerful as possible. It can express inclusive decays as well as
 *  (almost) any set of decay.
 *
 *  The description is made of 'particle' and 'decay'.
 *  A 'particle' is one of:
 *  - a particle name as known by the ParticlePropertySvc,
 *  - a set of 'particle's expressed like this: "{A, B, C}",
 *  - a set of 'particle's or their charge conjugate, expressed as "[A,B,C]cc",
 *  - a 'particle' with it's oscillation flag set ("[A]os") [NOT AVAILABLE ON
 *    RECONSTRUCTED DATA],
 *  - a 'wildcard', any particle with some quarks inside is expressed as "<Xq>"
 *    with up to tree quarks in place of "q". Quarks are u,d,c,s,t,b,u~,d~,...
 *    A simple '?' is also a wildcard matching any single particle,
 *  a 'decay' is one of:
 *  - an optional "(" and a 'particle' and the matching ")" if needed,
 *  - a "(", a 'particle', one of "->" or "=>", a blank separated list of decay,
 *    an optional "..." and a ")", e.g. "(B0 -> pi+ pi-)",
 *  - a "pp" one of "->" or "=>" and a blank separated list of decay.
 *  Note: the "()" are not needed around the first decay so the previous exemple
 *        could simply be written "B0 -> pi+ pi-" but they are mandatory for
 *        subdecay like "B0 -> (J/psi(1S) -> mu+ mu-) KS0".
 *  Note: If you want to find a stable particle you must explicily request that
 *        it decay to nothing but the arrow has to be dropped. So this looks
 *        like "B0 -> D_s- (K+)", here the D_s- can decay to whatever but
 *        the K+ must not decay. NB: Right now secondary interaction are seen
 *        as decay!
 *  Note: "->" means "decay to" and
 *        "=>" means "decay to .... with some resonnances in the middle".
 *        So if you want to catch B0 to D_s- K+ with any D*_s in beetween you
 *        simply write "B0 => D_s- K+ ...". NB: The "..." is here to catch any
 *        number of particles of any kind so you get the products of the D*_s-.
 *  Note: "pp" stands for "proton - proton collision". The decay products will
 *        be matched against the particles with no mother. An implicit "..." is
 *        appended. Only the boolean information for that kind of find can be
 *        used, the returned particle is set (Particle *)1 in case of match
 *        NULL otherwise.
 *  Note: alternate decays can be specified by puting them in a comma seperated
 *        list surrounded by braces (e.g. "B0 -> pi+ pi- , B0 -> K+ pi-").
 *  Note: the charge conjugate operator can also be applied to a set of decay
 *        (e.g. "[B0 -> K+ pi-]cc" == "B0 -> K+ pi- , B0~ -> K- pi+" (Please
 *        note the B0 turned into a B0~)).
 *  Note: alternate set of daughters can also be specified by putting the sets
 *        in a comma seperated list surrounded by braces (e.g.
 *        "B0 -> {pi+ pi-, K+ pi-, K- pi+, K+ K-}"). If one of the alternate
 *        set has only one daughter, DO NOT put it first in the list has it
 *        will not be able to parse the decay correctly (it will fail indeed).
 *  Note: you can also use the charge conjugate operator on the daughters set.
 *  Note: you can even mix the []cc and the {}.
 *
 * Extracting particles from the decay:
 * - It is possible to extract all particle in the decay tree which match a
 *   given id by preceeding the description with the list of particle you want
 *   to be extracted (cc, os, nos operator applies here too) terminated by a
 *   collon (e.g. "pi+ : B0 -> pi+ pi-").
 * - Precise particle inside the decay tree can also be flagged for extraction
 *   by preceeding them with a '^' (like "B0 -> (^J/psi(1S) -> mu+ mu-) KS0").
 * You can then retrieve these particles with the MCDecayFinder::members
 * method.
 *
 *  @author Olivier Dormond
 *  @date   23/04/2002
 */
class DecayFinder : public GaudiTool, virtual public IDecayFinder
{

public:

  /// Standard Constructor
  DecayFinder( const std::string& type,
               const std::string& name,
               const IInterface* parent );

  /// Destructor
  virtual ~DecayFinder( ); ///< Destructor

  StatusCode initialize( void );

  StatusCode finalize();

  /// Get/Set the decay string to find
  const std::string& decay( void ) { return m_source; }
  StatusCode setDecay( const std::string& decay );

  std::string revert( void );

  /// Does the described decay exists in the event?
  bool hasDecay( const LHCb::Particle::ConstVector& event );
  bool hasDecay( const LHCb::Particle::Container& event );
  bool hasDecay( void );

  /** Try to find the (next) match of the decay in the event.
   *
   *  @return true if found, false otherwise.
   *  @param event, the vector of particles in which to search for the decay.
   *  @param previous_result, the address of the previously matched particle
   *         or NULL if you want to start from the begining. This is an In/Out
   *         parameter, a reference to a const Particle *.
   *  The particle pointed to by previous_result must be contained in event.
   */
  bool findDecay( const LHCb::Particle::ConstVector &event,
                  const LHCb::Particle*& previous_result );
  bool findDecay( const LHCb::Particle::Container& event,
                  const LHCb::Particle*& previous_result );
  bool findDecay( const LHCb::Particle*& previous_result );

  /** Return the tree pointed at by head as a flat list.
   *
   *  @param head, the particle at the top of the decay tree.
   *  @param result, the list to fill with the particle in the 'head' tree.
   *         The list is not cleared before use.
   *  @param leaf, a bool indicating whether to include all particles or only
   *         the one at the ends of the branches. (Default: all)
   */
  void descendants( const LHCb::Particle *head,
                    LHCb::Particle::ConstVector& result,
                    bool leaf=false );

  /** Get a list of all the requested members that are present in a decay.
   *
   *  @param head, the head of a decay tree in which to find the members.
   *  @param members, the resulting list.
   *  The members are requested from the decay descriptor either as a list of
   *  particles seperated from the decay by a ':' and/or by putting a '^' before
   *  any particle in the decay.
   */
  void decayMembers( const LHCb::Particle *head,
                     LHCb::Particle::ConstVector& members );

  /// Get a vector of pairs <mother, products> for all sub-trees.
  void decaySubTrees( const LHCb::Particle *head,
                      std::vector<std::pair<const LHCb::Particle*,
                      LHCb::Particle::ConstVector > >& subtrees );

  /// Enumaration types used internally.
  enum Quarks { empty, up, down, charm, strange, top, bottom, antiup,
                antidown, anticharm, antistrange, antitop, antibottom };
  enum Quantums { Q_quantum=1, B_quantum, l_quantum, I_quantum, J_quantum,
                  S_quantum, L_quantum, G_quantum, C_quantum, P_quantum,
                  CP_quantum, c_quantum, s_quantum, t_quantum, b_quantum };
  enum Relations { eq_rel=1, lesseq_rel, greatereq_rel, less_rel, greater_rel,
                   noteq_rel };

private:

  /// The opaque representation of a particle matcher
  class ParticleMatcher
  {
  public:
    //
    ParticleMatcher( const LHCb::IParticlePropertySvc *ppSvc );
    ParticleMatcher( ParticleMatcher &copy );
    ParticleMatcher( std::string *name,
                     const LHCb::IParticlePropertySvc *ppSvc );
    ParticleMatcher( Quarks q1, Quarks q2, Quarks q3,
                     const LHCb::IParticlePropertySvc *ppSvc );
    ParticleMatcher( Quantums quantum, Relations relation, double value,
                     const LHCb::IParticlePropertySvc *ppSvc );
    bool test( const LHCb::Particle *part,
               LHCb::Particle::ConstVector *collect=NULL );
    void setLift( void ) { lift = true; }
    bool getLift( void ) { return lift; }
    void setEmpty( void ) { empty_f = true; }
    bool getEmpty( void ) { return empty_f; }
    void setQmark( void ) { qmark = true; }
    bool getQmark( void ) { return qmark; }
    void setConjugate( void ) { conjugate = true; }
    bool getConjugate( void ) { return conjugate; }
    void setOscillate( void ) { oscillate = true; }
    void setNotOscillate( void ) { noscillate = true; }
    bool getOscillate( void ) { return oscillate; }
    bool getNotOscillate( void ) { return noscillate; }
    void setInverse( void ) { inverse = true; }
    bool getInverse( void ) { return inverse; }
    void setStable( void ) { stable = true; }
    bool getStable( void ) { return stable; }
    bool getExact( void ) { return !qmark && !inverse
        && !conjugate && (type==id); }
    void conjugateID( void );
    std::string describe( void );
  private:
    int conjugatedID( int id );
    enum Type { notest, id, quark, quantum } type;
    union {
      struct { Quarks q1, q2, q3; } quarks;
      int stdHepID;
      struct { Quantums q; Relations r; double d; } relation;
    } parms;
    bool lift;
    bool empty_f;
    bool qmark;
    bool conjugate;
    bool oscillate;
    bool noscillate;
    bool inverse;
    bool stable;
    const LHCb::IParticlePropertySvc *m_ppSvc;
  };

  /// The opaque representation of the decay chain.
  class Descriptor
  {
  public:
    Descriptor( const LHCb::IParticlePropertySvc *ppSvc, double resonnanceThreshold );
    Descriptor( Descriptor &copy );
    Descriptor( ParticleMatcher *mother,
                const LHCb::IParticlePropertySvc *ppSvc,
                double resonnanceThreshold );

    ~Descriptor();

    template<class Iter> bool test( const Iter first, const Iter last,
                                    const LHCb::Particle*& previous_result ) {
      Iter start;
      if( previous_result &&
          ((start=std::find(first,last,previous_result)) == last) ) {
        previous_result = NULL;
        return false; // Bad previous_result
      }
      if( previous_result )
        start++;

      if( mother == NULL ) { // No mother == pp collision
        std::list<const LHCb::Particle*> prims;
        Iter i;
        for( i=(previous_result ? start : first); i != last; i++ ) {
          // Particle have no origin, let's say it comes from the pp collision.
          prims.push_back(*i);
        }
        if( skipResonnance )
          filterResonnances( prims );
        if( testDaughters(prims) ) {
          previous_result = (const LHCb::Particle *)1;
          return true;
        }
        if( getAlternate() )
          getAlternate()->test(first,last,previous_result);
        return false;
      }

      Iter part_i;
      part_i = (previous_result ? start : first);
      while( (part_i != last) && (test(*part_i) == false) )
        part_i++;

      if( part_i != last ) {
        previous_result = *part_i;
        return true;
      }
      return false;
    }
    bool test(const LHCb::Particle *mother, LHCb::Particle::ConstVector *collect=NULL,
              std::vector<std::pair<const LHCb::Particle*,LHCb::Particle::ConstVector >
              > *subTree=NULL);

    void setAlternate( Descriptor *a ) { alternate = a; }
    Descriptor *getAlternate( void ) { return alternate; }

    void setMother( ParticleMatcher *m ) { mother = m; }

    void addDaughter( Descriptor *daughter );
    std::vector<Descriptor *> &getDaughters( void ) { return daughters; }

    void setElipsis( void ) { elipsis = true; }
    bool getElipsis( void ) { return elipsis; }
    void setResonnance( void ) { skipResonnance = true; }
    void conjugate( void );
    std::string describe( void );
  private:
    bool testDaughters( std::list<const LHCb::Particle*> &parts,
                        LHCb::Particle::ConstVector *collect=NULL,
                        std::vector<std::pair<const LHCb::Particle*,LHCb::Particle::ConstVector >
                        > *subTree=NULL );
    void addNonResonnantDaughters( std::list<const LHCb::Particle*> &parts,
                                   const LHCb::Particle *part );
    void filterResonnances( std::list<const LHCb::Particle*> &parts );

    ParticleMatcher *mother;
    std::vector<Descriptor *> daughters; // wildcard mother at the end!
    bool skipResonnance;
    bool elipsis;
    double m_resThreshold;

    const LHCb::IParticlePropertySvc *m_ppSvc;

    Descriptor *alternate;
  };

  class DescriptorError
  {
  public:
    DescriptorError( const std::string& s ) : msg(s) {}
    std::string &cause( void ) { return msg; }
  private:
    std::string msg;
  };

  const LHCb::IParticlePropertySvc *m_ppSvc;
  IDataProviderSvc *m_EDS;
  std::string m_source;
  Descriptor *m_decay;
  std::vector<ParticleMatcher *> *m_members;
  double m_resThreshold;

  bool compile( const std::string &decay );

#include "recparser.h"

  int yparse( void );
  void yerror( const std::string& msg ) { throw DescriptorError(msg); }

  int ylex( void );

  int yygrowstack( void );

  static const short int ylhs[];
  static const short int ylen[];
  static const short int ydefred[];
  static const short int ydgoto[];
  static const short int ysindex[];
  static const short int yrindex[];
  static const short int ygindex[];
  static const short int ytable[];
  static const short int ycheck[];
  static int ydebug;
  static int ynerrs;
  static int yerrflag;
  static int ychar;
  static int ystacksize;
  static short int *yssp;
  static YYSTYPE *yvsp;
  static YYSTYPE yval;
  static short int *yss;
  static short int *ysslim;
  static YYSTYPE *yvs;
#ifdef YYDEBUG
  static const char * const yname[];
  static const char * const yrule[];
#endif
public:
  static YYSTYPE ylval;
};
#endif // TOOLS_DECAYFINDER_H
