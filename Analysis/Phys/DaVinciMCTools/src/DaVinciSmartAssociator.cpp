// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "DaVinciSmartAssociator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DaVinciSmartAssociator
//
// 2009-01-13 : Vladimir Gligorov 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( DaVinciSmartAssociator );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DaVinciSmartAssociator::DaVinciSmartAssociator( const std::string& type,
                                                              const std::string& name,
                                                              const IInterface* parent )
  : 
  GaudiTool ( type, name , parent ),
  m_linkerTool_cPP(0),
  m_linkerTool_nPP(0),
  m_bkg(0)
{
  declareInterface<IDaVinciSmartAssociator>(this);
}
//=============================================================================
// Destructor
//=============================================================================
DaVinciSmartAssociator::~DaVinciSmartAssociator() {} 
//=============================================================================
// Make & return the linker
//============================================================================= 
Particle2MCLinker::ToRange DaVinciSmartAssociator::associate(const LHCb::Particle* particleToBeAssociated ) {
//We associate according to the particle type: protoparticle associators
//are used for neutral and charged stable tracks, otherwise we use BackCat
//for composites. The associator wrapper makes sure the linkers thus created are
//deleted in the correct manner.

  ProtoParticle2MCLinker::ToRange associatedParts;
  //Now we get the association result based on the particle type
  if (particleToBeAssociated->isBasicParticle()){ //if this is a stable
    ProtoParticle2MCLinker* linker(0);
    verbose() << "About to get the array of matching particles" << endmsg;
    if ( particleToBeAssociated->particleID().pid() == 22 || 
         particleToBeAssociated->particleID().pid() == 111) {
      linker = dynamic_cast<ProtoParticle2MCLinker*>(m_linkerTool_nPP->linker(Particle2MCMethod::NeutralPP) );
    } else {
      linker = dynamic_cast<ProtoParticle2MCLinker*>(m_linkerTool_cPP->linker(Particle2MCMethod::ChargedPP) ); 
    }
    if (0!=linker) {
      associatedParts = linker->rangeFrom(particleToBeAssociated->proto());
    }
    
  } else{ //If composite use BackCat
    if (0!=m_bkg) {
      associatedParts.push_back(MCAssociation(m_bkg->origin(particleToBeAssociated), 1.));  
    }
  }

  return associatedParts; 
}
//=============================================================================
// initialize
//=============================================================================
StatusCode DaVinciSmartAssociator::initialize() {
  StatusCode sc = GaudiTool::initialize();
  //Init the BackCat instance
  m_bkg = tool<IBackgroundCategory>( "BackgroundCategory", this );
  //And the associator wrappers
  m_linkerTool_cPP = tool<IDaVinciAssociatorsWrapper>("DaVinciAssociatorsWrapper",
                                                      "Wrapper_CAT_cPP",
                                                      this);
  m_linkerTool_nPP = tool<IDaVinciAssociatorsWrapper>("DaVinciAssociatorsWrapper",
                                                      "Wrapper_CAT_nPP",
                                                      this);
  return sc;
}
//=============================================================================
// finalize
//=============================================================================
StatusCode DaVinciSmartAssociator::finalize() {
  return GaudiTool::finalize() ; 
}
