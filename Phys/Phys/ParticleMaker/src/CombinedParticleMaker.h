
//-----------------------------------------------------------------------------
/** @file CombinedParticleMaker.h
 *
 * Header file for Particle maker CombinedParticleMaker
 *
 * CVS Log :-
 * $Id: CombinedParticleMaker.h,v 1.16 2008-11-13 08:56:16 pkoppenb Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

#ifndef PARTICLEMAKER_COMBINEDPARTICLEMAKER_H
#define PARTICLEMAKER_COMBINEDPARTICLEMAKER_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/HashMap.h"

// from EventSys
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"

// Interfaces
#include "Kernel/IBremAdder.h"
#include "Kernel/IParticleMaker.h"
#include "Kernel/IProtoParticleFilter.h"
#include "Kernel/IParticle2State.h"
#include "TrackInterfaces/ITrackSelector.h"

/** @class CombinedParticleMaker CombinedParticleMaker.h
 *
 *  Particle maker to be used as DEFAULT unique one for charged
 *  Particles.
 *
 *  Uses all detector information available for a given hypothesis.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 2006-05-03
 *
 *  @todo Add error on measured momentum (from PDG ?)
 *  @todo Re-assess how the confidence level is calculated
 */

class CombinedParticleMaker : public GaudiTool,
                              virtual public IParticleMaker
{

public:

  /// Standard constructor
  CombinedParticleMaker( const std::string& type,
                         const std::string& name,
                         const IInterface* parent);

  virtual ~CombinedParticleMaker( ); ///< Destructor

  /// Make the particles
  virtual StatusCode makeParticles( LHCb::Particle::ConstVector & parts );

  /// Standard initialization of tool
  virtual StatusCode initialize();

  /// Standard finalization of tool
  virtual StatusCode finalize();

private:

  /** Fill Particle Parameters from ProtoParticle given a ParticleProperty
   *  @param proto     Pointer to ProtoParticle
   *  @param pprop     Pointer to ParticleProperty
   *  @param particle  Pointer to Particle
   *  @return StatusCode
   */
  StatusCode fillParticle( const LHCb::ProtoParticle * proto,
                           const ParticleProperty    * pprop,
                           LHCb::Particle            * particle ) const;

  /// Converts a string to a constant format
  std::string convertName( const std::string & in ) const;

  /// Set the Particle confidence level
  void setConfLevel( const LHCb::ProtoParticle * proto,
                     const ParticleProperty    * pprop,
                     LHCb::Particle            * particle ) const;

  /// Test the PID information consistency
  void checkPIDInfo( const LHCb::ProtoParticle * proto ) const;

private:

  /// Job option wtih TES input for ProtoParticles
  std::string m_input;

  /** @brief Job Option List of particles type to make.
   * Possible values are muons, electrons, kaons, protons, pions
   */
  std::vector<std::string> m_particleList;

  std::string m_elProtoFilter; ///< The tool type to use for electron selection
  std::string m_muProtoFilter; ///< The tool type to use for muon selection
  std::string m_piProtoFilter; ///< The tool type to use for pion selection
  std::string m_kaProtoFilter; ///< The tool type to use for kaon selection
  std::string m_prProtoFilter; ///< The tool type to use for proton selection
  // Activate the BremStrahlung correction for electrons
  bool m_addBremPhoton;  
  /// Job option for exclusive selection of particle types
 /// PIDs are checked in some order and all tracks that
 /// are compatible with being a muon are made a muon, then an electron,
 /// proton, kaon and pion. What you get will depends on
 /// this order, which is already a good enough reason not to want that.
 /// If you do B->pipi and one pi decays to a muon, it will never be a pi
 /// candidate. Even worse, if you or someone relaxes the mu ID cuts  
 /// between two jobs or two DaVinci versions, then suddenly what used to be a nice
 /// pion in one version of DV might now becomes a muon candidate. Your
 /// efficiency decreases although nothing affecting pion or kaon ID  
 /// changed.
 /// That makes it very hard to estimate signal efficiencies as all
 /// efficiencies depend on everything.
 /// If you want to cut hard on PID, use PID cuts. If you don't want to use
 /// the same track several times (which is a valid point for tagging),  
 /// then use the Overlap Tool and re-weight the track accordingly if needed.
  bool m_exclusive;

  /// Map type that takes a particle type to a ProtoParticle filter
  typedef std::pair< const ParticleProperty *, const IProtoParticleFilter* > ProtoPair;
  typedef std::vector < ProtoPair > ProtoMap;

  /// Map that takes a particle type to a ProtoParticle filter
  ProtoMap m_protoMap;

  /// Tool to fill State information into a Particle
  IParticle2State* m_p2s ;

  /// Track selector tool
  ITrackSelector * m_trSel;

  /// Track selector tool
  IBremAdder* m_brem;

  // tallies

  /// Simple utility tally class
  class TrackTally
  {
  public:
    /// Default constructor
    TrackTally() : totProtos(0), selProtos(0), el(0), mu(0), pi(0), ka(0), pr(0) { }
    unsigned long totProtos; ///< Total number of ProtoParticles considered
    unsigned long selProtos; ///< Total number of ProtoParticles selected
    unsigned long el;        ///< Total number of electrons created
    unsigned long mu;        ///< Total number of muons created
    unsigned long pi;        ///< Total number of pions created
    unsigned long ka;        ///< Total number of kaons created
    unsigned long pr;        ///< Total number of protons created
    /// Increment the count for the given particle type (as string)
    inline void addToType( const std::string & type )
    {
      if      ( "e+"  == type ) { ++el; }
      else if ( "mu+" == type ) { ++mu; }
      else if ( "pi+" == type ) { ++pi; }
      else if ( "K+"  == type ) { ++ka; }
      else if ( "p+"  == type ) { ++pr; }
    }
  };

  /// Map type containing tally for various track types
  typedef GaudiUtils::HashMap < const LHCb::Track::Types, TrackTally > TrackMap;

  /// Total number of tracks considered and selected
  TrackMap m_nTracks;

  /// Minimum percentage to include in final summary printout
  double m_minPercForPrint;

  /// Flag to turn on consistency checks on PID information
  bool m_testPIDinfo;

};

#endif // PARTICLEMAKER_COMBINEDPARTICLEMAKER_H
