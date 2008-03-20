// $Id: CopyParticle2PVLink.h,v 1.4 2008-03-20 13:39:02 jpalac Exp $
#ifndef COPYPARTICLE2PVLINK_H 
#define COPYPARTICLE2PVLINK_H 1

// Include files
// from Gaudi
#include "MicroDST/MicroDSTAlgorithm.h"

// from DaVinci
#include "Kernel/Particle2Vertex.h"

/** @class CopyParticle2PVLink CopyParticle2PVLink.h
 *  
 *  MicroDSTAlgorithm to clone the associations between an LHCb::Particle
 *  and and LHCb::RecVertex. Typically this association should be between
 *  a candidate LHCb::Particle and any primary vertices used to select
 *  it. 
 *  It inherits the std::string properties InputLocation and OutputPrefix from
 *  MicroDSTCommon. The LHCb::Particles are taken from the TES location 
 *  defined by InputLocation, and are cloned and put in TES location 
 *  "/Event" + OutputPrefix + InputLocation. If InputLocation already contains
 *  a leading "/Event" it is removed.
 *  The associations are typically in a Particle2Vertex::Table in a TES 
 *  location like "/Event/Phys/SomeParticleMakingAlg/Particle2VertexRelations".
 *  The algorithm picks up this relations table, which relates LHCb::Particles
 *  from "/Event/Phys/SomaParticleMakingAlg/Particles" to LHCb::RecVertex.
 *  For each LHCb::Particle to LHCb::RecVertex associaiton, it picks up the 
 *  corresponding <b>previously cloned</b> LHCb::Particle from 
 *  "/Event/MyLocation/Phys/SomeParticleMakerAlg/Particles" and LHCb::RecVertex
 *  from "/Event/MyLocation/Phys/<location of associated vertices>".
 *
 *  <b>Important conditions</b>: This algorithm relies on the LHCb::Particle
 *  and LHCb::RecVertex sets in the associations to have been previously
 *  cloned by CloneParticles and ClonePrimaryVertices. Furthermore, the
 *  <b>OutputPrefix</b> property of this algorithm and the CopyParticles and
 *  CopyPrimaryVertices used must be the same.
 *  of this algorithm. At the moment, the filling of the associations is only
 *  performed if the LHCb:: Particle creation and selection involves the
 *  primary vertex.
 *
 *  <b>Example</b>: Clone LHCb::Particle -> LHCb::recVertex associations
 *  from "/Event/Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particle2VertexRelations"
 *  to "/Event/MyLocation/Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particle2VertexRelations"
 *  <code>
 *  SeqDC06selBd2Jpsi2MuMu_Kst2KPi.Members += {"CopyParticle2PVLink"};
 *  CopyParticle2PVLink.InputLocation = "Phys/DC06selBd2Jpsi2MuMu_Kst2KPi/Particle2VertexRelations";
 *  CopyParticle2PVLink.OutputPrefix = "MyLocation";
 *  </endcode>
 *
 *  @todo The dependencies locations of two other algorithms are a bit annoying. Try to fix this.
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2007-10-15
 */
class CopyParticle2PVLink : public MicroDSTAlgorithm {
public: 
  /// Standard constructor
  CopyParticle2PVLink( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyParticle2PVLink( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  Particle2Vertex::Table* tableOfClones(const Particle2Vertex::Table* table);

  const std::string particleLocation(const std::string& tableLocation);
  
};
#endif // COPYPARTICLE2PVLINK_H
