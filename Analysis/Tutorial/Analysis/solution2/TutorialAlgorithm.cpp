// $Id: TutorialAlgorithm.cpp,v 1.4 2005-11-07 11:54:42 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "TutorialAlgorithm.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TutorialAlgorithm
//
// 2004-11-04 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<TutorialAlgorithm>          s_factory ;
const        IAlgFactory& TutorialAlgorithmFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TutorialAlgorithm::TutorialAlgorithm( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
    , m_ID(0)
    , m_Mass(0.)
    , m_nFound(0)
    , m_nEvents(0)
{
  declareProperty("MassWindow", m_MassWin = 10.*GeV); 
  declareProperty("MaxChi2",    m_Chi2 = 1000.);
  declareProperty("Particle",   m_Name = "J/psi(1S)");
}
//=============================================================================
// Destructor
//=============================================================================
TutorialAlgorithm::~TutorialAlgorithm() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TutorialAlgorithm::initialize() {

  debug() << "==> Initialize" << endmsg;
  // particle property service
  ParticleProperty* m_pp = ppSvc()->find( m_Name );
  if ( !m_pp ) { // Check the pointer or you'll core dump on a typo in the name
    err() << "Cannot find particle property for " << m_Name << endreq ; 
    return StatusCode::FAILURE;
  }
  m_ID = m_pp->pdgID();
  m_Mass = m_pp->mass();
  info() << "Will reconstruct " << m_pp->particle() << " (ID=" << m_ID
         << ") with mass " << m_Mass << endreq ;
  info() << "Mass window is " << m_MassWin << " MeV" << endreq ;
  info() << "Max chi^2 is " << m_Chi2 << endreq ;

  return StatusCode::SUCCESS;
};
//=============================================================================
// Main execution
//=============================================================================
StatusCode TutorialAlgorithm::execute() {

  debug() << "==> Execute" << endmsg;
  setFilterPassed(false);   // Mandatory. Set to true if event is accepted.
  ++m_nEvents;
  
  // get particles from the desktop. This will get all particles that are found
  // in the InputLocations of the PhysDesktop.
  const ParticleVector& parts = desktop()->particles();

  // Now filter out the ones that are real muons. 
  // This requires the ParticleFilter to be properly configured
  ParticleVector PartPlus, PartMinus;
  StatusCode sc = particleFilter()->filterNegative(parts,PartMinus);
  if (sc) sc = particleFilter()->filterPositive(parts,PartPlus);
  if (!sc) {
    err() << "Error while filtering" << endreq ;
    return sc ;
  } 
  debug() << "Filtered " << PartMinus.size() << " positive and " 
          << PartPlus.size() << " negative particles" << endreq ;

  // combine mu+ and mu-
  ParticleVector::const_iterator ipp, ipm;
  for ( ipm = PartMinus.begin() ; ipm !=  PartMinus.end() ; ++ipm ){
    plot((*ipm)->momentum().perp(),"Pt",0.*GeV,10.*GeV);// Plot Pt
    for ( ipp = PartPlus.begin() ; ipp !=  PartPlus.end() ; ++ipp ){
      if (ipm == PartMinus.begin()) plot((*ipp)->momentum().perp(),"Pt",0.*GeV,10.*GeV);// Plot Pt but only once
      HepLorentzVector twoP = (*ipp)->momentum() + (*ipm)->momentum() ;
      verbose() << "Two prong mass is " << twoP.m()/MeV << endreq ;
      // mass cut
      plot(twoP.m(),"Mass",0.5*m_Mass,1.5*m_Mass); // +.- 50% mass window
      if ( fabs ( twoP.m() - m_Mass ) > m_MassWin ) continue ;
      // vertex fit
      Vertex PPVertex;
      sc = vertexFitter()->fitVertex(*(*ipp),*(*ipm),PPVertex);
      if (!sc){
        info() << "Failed to fit vertex" << endreq ; // no bid deal
        continue ;
      }  
      debug() << "Vertex fit at " << PPVertex.position()/cm 
              << " with chi2 " << PPVertex.chi2() << endreq;
      // chi2 cut
      plot(PPVertex.chi2(),"Chi^2",0.,200.);
      if ( PPVertex.chi2() > m_Chi2 ) continue ;
      // make particle
      Particle Mother ;
      sc = particleStuffer()->fillParticle(PPVertex,Mother,ParticleID(m_ID));
      Particle* pMother = desktop()->createParticle(&Mother);
      info() << "Created " << m_Name << " candidate with m=" 
             << Mother.mass() << " and " 
             << "chi^2=" << PPVertex.chi2() << endreq ;      
      plot((*ipm)->momentum().perp(),"Selected Pt",0.*GeV,10.*GeV);
      plot((*ipp)->momentum().perp(),"Selected Pt",0.*GeV,10.*GeV);
      plot(twoP.m(),"Selected mass",m_Mass-m_MassWin,m_Mass+m_MassWin);
      if (!pMother){
        err() << "Cannot save particle to desktop" << endreq ;
        return StatusCode::FAILURE;
      } 
      setFilterPassed(true);
      ++m_nFound ;
    } // ipp
  } // ipm
  // save desktop. This will save all particles to /Event/Phys/TutorialAlgorithm/Particles.
  sc = desktop()->saveDesktop();
  return sc;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode TutorialAlgorithm::finalize() {

  debug() << "==> Finalize" << endmsg;
  info() << "Found " << m_nFound << " " << m_Name << " in " 
         << m_nEvents << " events" << endreq;

  return  StatusCode::SUCCESS;
}

//=============================================================================
