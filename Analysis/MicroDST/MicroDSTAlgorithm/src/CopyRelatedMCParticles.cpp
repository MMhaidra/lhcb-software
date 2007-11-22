// $Id: CopyRelatedMCParticles.cpp,v 1.4 2007-11-22 16:19:48 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "Event/MCVertex.h"

// local
#include "CopyRelatedMCParticles.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyRelatedMCParticles
//
// 2007-11-02 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyRelatedMCParticles );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================

CopyRelatedMCParticles::CopyRelatedMCParticles( const std::string& name,
                                                ISvcLocator* pSvcLocator )
  : CopyAndStoreData ( name , pSvcLocator )
{

}

//=============================================================================
// Destructor
//=============================================================================

CopyRelatedMCParticles::~CopyRelatedMCParticles() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyRelatedMCParticles::initialize() {

  StatusCode sc = CopyAndStoreData::initialize(); // must be executed first

  debug() << "==> Initialize" << endmsg;

  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if (inputTESLocation()=="")  {
    error() << "No TES location " << endmsg;
    return StatusCode::FAILURE;
  }
  verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

  m_compositeLinker = new Particle2MCLinker(this,
                                            Particle2MCMethod::Composite,"")  ;

  m_linksLinker     = new Particle2MCLinker(this,
                                            Particle2MCMethod::Links,"")  ;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================

StatusCode CopyRelatedMCParticles::execute() {

  debug() << "==> Execute" << endmsg;
  verbose() << "Going to store MCParticles related to Particles from " 
            << inputTESLocation() << endmsg;

  const LHCb::Particle::Container* particles = 
    get<LHCb::Particle::Container>( inputTESLocation() );

  verbose() << "Found " << particles->size() << " particles" << endmsg;

  // Actually this needs to be iterative, so we store the MCParticles for
  // everything in the tree.
  
  for (LHCb::Particle::Container::const_iterator iPart = particles->begin();
       iPart != particles->end();
       ++iPart) {
    LHCb::MCParticle* mcPart = storeMCParticle( *iPart);
  }

  setFilterPassed(true);

  verbose() << "Going to store to TES" << endmsg;

  return copyLocalStoreToTES();


}
//=============================================================================
LHCb::MCParticle* CopyRelatedMCParticles::storeMCParticle(const LHCb::Particle* particle)
{

  // Get a vector of associated particles
  // Store them on the local store.
  LHCb::MCParticle::ConstVector AssociatedMCParticles;

  StatusCode sc = associatedMCParticles(particle, AssociatedMCParticles);
  
  return 0;

}
//=============================================================================
StatusCode CopyRelatedMCParticles::associatedMCParticles(const LHCb::Particle* particle,
                                                         LHCb::MCParticle::ConstVector&) 
{

  Particle2MCLinker* linker = (particle->isBasicParticle() ) ? m_linksLinker : m_compositeLinker;

  return StatusCode::SUCCESS;
  
}
//=============================================================================
const LHCb::MCVertex* CopyRelatedMCParticles::storeMCVertex(const LHCb::MCVertex* vertex) 
{
  return 0;
}
//=============================================================================
//  Finalize
//=============================================================================

StatusCode CopyRelatedMCParticles::finalize() {

  debug() << "==> Finalize" << endmsg;

  return CopyAndStoreData::finalize();  // must be called after all other actions
}
//=============================================================================
