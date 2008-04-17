// $Id: CombineParticles.cpp,v 1.12 2008-04-17 16:50:58 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "Kernel/IDecodeSimpleDecayString.h"
#include "Kernel/IFilterCriterion.h"
#include "Kernel/IPlotTool.h"
#include "Kernel/ICheckOverlap.h"
#include "Kernel/IParticleDescendants.h"
// local
#include "CombineParticles.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CombineParticles
//
// 2007-06-26 : Juan Palacios
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CombineParticles );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CombineParticles::CombineParticles( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator ),
    m_filter0(0),
    m_filter1(0),
    m_filter2(0),
    m_checkOverlap(),
    m_particleDescendants(),
    m_plots0(),
    m_plots1(),
    m_plots2()
{
  declareProperty("Phase0FilterName", m_phase0FilterName = "Phase0Filter");
  declareProperty("Phase1FilterName", m_phase1FilterName = "Phase1Filter");
  declareProperty("Phase2FilterName", m_phase2FilterName = "Phase2Filter");
  declareProperty( "Plot0Tool", m_plot0Tool = "RecursivePlotTool/Plots0");  
  declareProperty( "Plot1Tool", m_plot1Tool = "RecursivePlotTool/Plots1");  
  declareProperty( "Plot2Tool", m_plot2Tool = "RecursivePlotTool/Plots2");  
  declareProperty( "Plots0Path", m_plots0Path = "");  
  declareProperty( "Plots1Path", m_plots1Path = "");  
  declareProperty( "Plots2Path", m_plots2Path = "");  
  declareProperty( "DecayDescriptors", m_decayDescriptors );  
  declareProperty( "KillOverlap", m_killOverlap = true);
  declareProperty( "OutputLocation" , m_outputLocation = "" ) ;
  declareProperty( "PrintStats", m_printStats = false ) ;
  declareProperty( "PrintMoreStats", m_printMoreStats = false ) ;
  declareProperty( "MaxCandidates", m_maxCandidates = 100000 ) ;
  return ;
  
}
//=============================================================================
// Destructor
//=============================================================================
CombineParticles::~CombineParticles() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CombineParticles::initialize() {

  StatusCode sc = DVAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  if ( msgLevel( MSG::DEBUG )) debug() << "==> Initialize" << endmsg;

  // add default filtercriteria
  m_criteriaNames[ "Phase0Filter" ] =  "LoKi::Hybrid::FilterCriterion/FILTER0";
  m_criteriaNames[ "Phase1Filter" ] =  "LoKi::Hybrid::FilterCriterion/FILTER1";
  m_criteriaNames[ "Phase2Filter" ] =  "LoKi::Hybrid::FilterCriterion/FILTER2";
  
  m_filter0 = filterCriterion( m_phase0FilterName ) ;
  m_filter1 = filterCriterion( m_phase1FilterName ) ;
  m_filter2 = filterCriterion( m_phase2FilterName ) ;

  if ( m_killOverlap ){
    m_checkOverlap = checkOverlap() ;
    m_particleDescendants = descendants();
    if ( !m_checkOverlap || ! m_particleDescendants ){
      err() << "Cannot retrieve CheckOverlap or ParticleDescendants Tools" 
            << endmsg ;
      return StatusCode::FAILURE ;    
    }
    if ( msgLevel( MSG::VERBOSE )) verbose() << "Got overlap and particle descendants tools" << endmsg ;
  }
  if ( msgLevel( MSG::DEBUG )) debug() << "Going to createDecays()" << endmsg;

  return initializePlotters() && createDecays();
}

//=============================================================================
StatusCode CombineParticles::initializePlotters() 
{
  if ( produceHistos() ){
    return initializePlotter(m_plots0,m_plot0Tool, m_plots0Path) &&
      initializePlotter(m_plots1,m_plot1Tool, m_plots1Path) &&
      initializePlotter(m_plots2,m_plot2Tool, m_plots2Path);
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode CombineParticles::initializePlotter(IPlotTool*& plotter, 
                                               const std::string& plotterName,
                                               std::string& plotsPath) 
{
  if ( plotterName != "NONE" ){
    plotter = tool<IPlotTool>(plotterName, this);
    if( !plotter ) {
      err() << "Unable to get " << plotterName << endmsg;
      return StatusCode::FAILURE;
    }
    if (plotsPath == "") plotsPath = name()+plotterName;
    else plotter->setPath(plotsPath);
    if (msgLevel(MSG::DEBUG)) debug() << plotterName <<" plots will be in " 
                                      << plotsPath << endmsg ;
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CombineParticles::execute() {

  if ( msgLevel( MSG::DEBUG )) debug() << "==> Execute" << endmsg;

  ++m_nEvents ;

  setFilterPassed(false);   // Mandatory. Set to true if event is accepted.
  
  LHCb::Particle::ConstVector Daughters, Resonances ;
  StatusCode sc = applyFilter(desktop()->particles(),
                              Daughters,
                              m_filter0);
  
  if (!sc) {
    err() << "Unable to filter daughters" << endmsg;
    return StatusCode::FAILURE ;  
  }

  if ( msgLevel( MSG::DEBUG ) || m_printMoreStats ) 
    always() << "Applyed FILTER0 to " << desktop()->particles().size() 
             << " particles and found " << Daughters.size() << endmsg;
  if (Daughters.empty()) {
    if ( msgLevel( MSG::DEBUG )) debug() << "No daughters found at all" << endmsg ;
    return StatusCode::SUCCESS;
  }
  sc = makePlots(Daughters,m_plots0);
  if (!sc) return sc;

  if ( Daughters.size() > m_maxCandidates ){
    Warning("Reached maximal number of daughters. Exiting");
    return StatusCode::SUCCESS ;
  }

  // The LOOP ///
  for ( Decays::iterator decay = m_decays.begin() ; 
        decay != m_decays.end() ; ++decay ){
    if ( msgLevel( MSG::VERBOSE )) verbose() << "New Decay loop" << endmsg ;
    if (!decay->fillPidParticles(Daughters)){
      if ( msgLevel( MSG::DEBUG )) debug() << "Not all necessary particles found for decay" << endmsg ;
      continue ;
    }
    sc = applyDecay(*decay, Resonances); // make the resonances
    if (!sc) return StatusCode::SUCCESS; // exit silently
  }

  sc = makePlots(Resonances, m_plots1);
  if (!sc) return sc;

  LHCb::Particle::ConstVector Final ;
  sc = applyFilter(Resonances, Final, m_filter2);
  if (!sc) return Error("Unable to filter mothers");
  if ( msgLevel( MSG::DEBUG ) || m_printMoreStats ) 
    always() << "Applyed FILTER2 to " << Resonances.size() 
            << " particles and found " << Final.size() << endmsg;

  sc = makePlots(Final,m_plots2);
  if (!sc) return sc;

  if ( msgLevel(MSG::VERBOSE))  verbose() << "Saving " << Final.size() 
                                          << " candidates " << endmsg ;
  sc = desktop()->saveTrees(Final);
  if (!sc) {
    err() << "Unable to save mothers" << endmsg;
    return StatusCode::FAILURE ;  
  }  
  if ( msgLevel(MSG::DEBUG)) debug() << "Saved " << Final.size() 
                                     << " candidates " << endmsg ;
  if ( !Final.empty()){
    ++m_nAccepted;
    m_nCandidates+=Final.size();
    setFilterPassed(true);   // Mandatory. Set to true if event is accepted.
    sc = StatusCode::SUCCESS;
  }
  if ( msgLevel(MSG::DEBUG) || m_printStats )
    always() << "Found " << Final.size() << " candidates " << endmsg ;
  
  return sc;
}
//=============================================================================
// Filter particles
//=============================================================================
StatusCode CombineParticles::applyFilter(const LHCb::Particle::ConstVector& vIn, 
                                         LHCb::Particle::ConstVector& vOut, 
                                         IFilterCriterion* fc) const{
  if (fc==NULL) { // not possible yet
    if ( msgLevel( MSG::DEBUG )) debug() << "Null filter criterion" << endmsg ;
    vOut = vIn ;
    return StatusCode::SUCCESS;
  }

  for ( LHCb::Particle::ConstVector::const_iterator p = vIn.begin() ; 
        p!=vIn.end(); ++p){
    if ( msgLevel( MSG::VERBOSE )) verbose() << "applyFilter considering particle " << **p << endmsg;
    if ( consideredPID((*p)->particleID().pid()) && fc->isSatisfied(*p) ) {
      vOut.push_back(*p);
    }
    
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
// Apply one decay
//=============================================================================
StatusCode CombineParticles::applyDecay(Decay& decay, 
                                        LHCb::Particle::ConstVector& Resonances){
  StatusCode sc = StatusCode::SUCCESS ;
  unsigned int nc = 0 ;
  unsigned int nr = 0 ;
  if ( msgLevel( MSG::VERBOSE )) verbose() << "In applyDecay" << endmsg ;
  LHCb::Particle::ConstVector DaughterVector ;

  bool inloop = decay.getFirstCandidates(DaughterVector); // get first daughter vector
  while (inloop){ 
    if ( msgLevel( MSG::VERBOSE )) verbose() << "In while loop " << DaughterVector.size() << endmsg ;

    // Find invariant mass
    LHCb::Particle candidateP;
    LHCb::Vertex candidateV;
    sc = vertexFitter("ParticleAdder")->fit(DaughterVector, 
                                            candidateP, 
                                            candidateV);
    if (sc) {
      if ( msgLevel( MSG::VERBOSE )) verbose() << "Applying filter1 to particle " << candidateP 
                                               << " with vertex " << candidateV << endmsg;
      nc++ ;
    } else {
      if ( msgLevel( MSG::DEBUG )) debug() << "ParticleAdder failed" << endmsg;
    }
    
    if (sc && m_filter1->isSatisfied(&candidateP) ) {

      const LHCb::Particle* Mother = makeMother(DaughterVector,
                                                decay.getMotherPid());
      if (0==Mother){
        sc = Warning("Something failed in vertex fitting",
                     StatusCode::SUCCESS,1);
      } else {
        if ( msgLevel( MSG::VERBOSE )) verbose() << "Getting mother " << Mother->particleID().pid()
                                                 << " " << Mother->momentum() << endmsg ;
        Resonances.push_back(Mother);
        nr++ ;
        if ( nr > m_maxCandidates ){
          Warning("Reached maximal number of candidates. Exiting");
          return StatusCode::FAILURE ;
        }
      }
    }
    
    inloop = decay.getNextCandidates(DaughterVector);
  }
  
  
  if ( msgLevel( MSG::DEBUG ) || m_printMoreStats )
    always() << "Applyed FILTER1 to " << nc << " candidates and found " << nr
             << " candidates with PID " 
             << decay.getMotherPid().pid() << endmsg ;
  return sc ;  
}
//=============================================================================
//  Make Mother Vertex fit
//=============================================================================
const LHCb::Particle* 
CombineParticles::makeMother(const LHCb::Particle::ConstVector& Daughters,
                             const LHCb::ParticleID& motherPid){

  if ( msgLevel( MSG::VERBOSE )) verbose() << "Will make particle with PID " << motherPid.pid() << endmsg ;

  StatusCode sc = StatusCode::SUCCESS;
  LHCb::Vertex CandidateVertex(Gaudi::XYZPoint(0.,0.,0.)) ;
  LHCb::Particle Candidate(motherPid);

  sc = vertexFitter()->fit(Daughters,Candidate,CandidateVertex);
  if (!sc){
    Warning("Failed to fit vertex",StatusCode::SUCCESS,1).ignore();
    return NULL;
  }
  if ( msgLevel( MSG::DEBUG )) debug() << "Fit vertex at " << CandidateVertex.position()
                                       << " with chi^2 " << CandidateVertex.chi2() << endmsg;
  // may add a chi^2 cut here
  
  if (!sc) return NULL;
  
  if ( msgLevel( MSG::VERBOSE )) verbose() << "Calling desktop()->keep(const LHCb::Particle*)" << endmsg ;
  return desktop()->keep(&Candidate);

};
//=============================================================================
StatusCode CombineParticles::makePlots(const LHCb::Particle::ConstVector& PV,
                                       IPlotTool* PT) {

  if ( msgLevel( MSG::DEBUG )) debug() << "Welcome to makePlots " << endmsg;
  if (!produceHistos()) {
    if ( msgLevel( MSG::DEBUG )) debug() << "!produceHistos()" << endmsg;
    return StatusCode::SUCCESS;
  }
  
  if (!PT) {
    if ( msgLevel( MSG::DEBUG )) debug() << "No plot tool!" << endmsg;
    return StatusCode::SUCCESS;
  }
  
  if ( msgLevel( MSG::DEBUG )) debug() << "Plotting " << endmsg;

  return PT->fillPlots(PV) ;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CombineParticles::finalize() {

  if ( msgLevel( MSG::DEBUG )) debug() << "==> Finalize" << endmsg;
  info() << "Found " << m_nCandidates << " candidates in " 
         << m_nAccepted << " accepted events among " 
         << m_nEvents << " events" << endmsg ;
  return DVAlgorithm::finalize();
}
//=============================================================================
// Create decays, Stolen from MakeResonances
//=============================================================================
StatusCode CombineParticles::createDecays(){
  if ( msgLevel( MSG::DEBUG )) debug() << "In createDecays()" << endmsg;
  // get string decoder
  IDecodeSimpleDecayString* dsds = 
    tool<IDecodeSimpleDecayString>("DecodeSimpleDecayString",this);

  if ( !dsds ) return StatusCode::FAILURE ;
  
  if ( m_decayDescriptors.empty() ){
    if ( msgLevel( MSG::DEBUG )) debug() << "No decay descriptors array defined. Pushing back " 
                                         << getDecayDescriptor() << endmsg ;
    m_decayDescriptors.push_back(getDecayDescriptor());
  }
  if ( msgLevel( MSG::DEBUG )) debug() << "Decay descriptors are " << m_decayDescriptors << endmsg ;

  for ( std::vector<std::string>::const_iterator dd = m_decayDescriptors.begin() ;
        dd != m_decayDescriptors.end() ; ++dd ){
    
    if ( msgLevel( MSG::DEBUG )) debug() << " setting up " << *dd << endmsg;

    // initialize string decode
    StatusCode sc = dsds->setDescriptor(*dd);
    if ( sc.isFailure()) return sc ;  

    std::string mother;
    Strings daughters;
    if ( msgLevel( MSG::DEBUG )) debug() << "Getting strings for " << dsds->getDescriptor() << endmsg;
    sc = dsds->getStrings(mother, daughters);
    if (sc.isFailure()) return sc;  
    std::sort(daughters.begin(),daughters.end()); // helps a lot, and used to avoid duplication of mothers in cc
    if ( msgLevel( MSG::DEBUG )) debug() << "Sorted daughters to " << daughters << endmsg ;
    sc = createDecay(mother, daughters);
    if (sc.isFailure()) return sc;
    
    // once again for cc if needed
    if ( dsds->is_cc() ){
      // LF : avoid duplication of mothers when using []cc
      strings daughtersBeforecc = daughters; // daughters have been sorted
      std::string motherBeforecc = mother ;
      if ( msgLevel( MSG::DEBUG )) debug() << "Sorted before cc daughters to " 
                                           << daughtersBeforecc << endmsg;
      if ( msgLevel( MSG::DEBUG )) debug() << "Setting up cc for " 
                                           << dsds->getDescriptor() << endmsg;
      sc = dsds->getStrings_cc(mother, daughters);
      if (sc.isFailure()) return sc;
      std::sort(daughters.begin(),daughters.end()); // needed to compare daughters for original and cc
      if ( msgLevel( MSG::DEBUG )) debug() << "Sorted cc daughters to " << daughters << endmsg ;

      if(daughtersBeforecc == daughters){
        warning() << "You have chosen two charged-conjugated modes with identical final states: " 
                  << m_decayDescriptors << endmsg ;
        warning() << "All final state particle will be duplicated as " 
                  << motherBeforecc << " and " << mother << endmsg ;
      }
      sc = createDecay(mother, daughters);
      if (sc.isFailure()) return sc;
      
    } else if ( msgLevel( MSG::VERBOSE )) verbose() << dsds->getDescriptor() << " is not a cc mode" << endmsg ;
  }
  if ( msgLevel( MSG::DEBUG )) debug() << "createDecays() will return SUCCESS" << endmsg;
  
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode CombineParticles::createDecay(const std::string& mother, 
                                         const Strings& daughters){

  if (msgLevel(MSG::DEBUG)) debug() << "Creating " << mother << " -> " << daughters << endmsg;
  
  // mother

  ParticleProperty* pmother = ppSvc()->find(mother);

  if (NULL==pmother){
    err() << "Cannot find particle property for mother " << mother << endmsg ;
    return StatusCode::FAILURE;
  } else {
    if ( msgLevel( MSG::VERBOSE )) verbose() << "Found ParticleProperty " << pmother->pdgID() << endmsg ;
  }
  
  const int pid = pmother->pdgID() ;
  if ( msgLevel( MSG::VERBOSE )) verbose() << "Found pid of " << mother << " = " << pid << endmsg ;
  if (!consideredPID(pid)) m_allPids.push_back(pid) ;
  
  //daughters
  std::vector<int> daughterPIDs ;
  for (strings::const_iterator daughter=daughters.begin(); 
       daughter!=daughters.end(); ++daughter){
    // mother
    ParticleProperty* pd = ppSvc()->find(*daughter);
    if (!pd){
      err() << "Cannot find particle property for daughter " 
            << *daughter << endmsg ;
      return StatusCode::FAILURE;
    }
    if ( msgLevel( MSG::VERBOSE )) verbose() << "Found pid of " << *daughter 
                                             << " = " << (pd->pdgID()) << endmsg ;
    daughterPIDs.push_back(pd->pdgID()) ;
    
    // add to list of all PIDs
    if (!consideredPID(pd->pdgID())) m_allPids.push_back(pd->pdgID()) ;
  }
  if ( msgLevel( MSG::VERBOSE )) verbose() << "Pushed back " << daughterPIDs.size() 
                                           << " daughters" << endmsg ;

  // for slc3, need to build a const ParticleID explicitely.
  const LHCb::ParticleID _pid(pid);
  // fill decay container
  Decay decay(_pid,daughterPIDs,
              m_checkOverlap, m_particleDescendants);
  
  m_decays.push_back(decay);

  if (msgLevel(MSG::DEBUG)) debug() << "Initialized decay # " << m_decays.size() << endmsg ;
  
  return StatusCode::SUCCESS;
  
}

//=============================================================================
//#############################################################################
// Decay class
//#############################################################################
StatusCode CombineParticles::Decay::initialize(const std::vector<int>& pids){ 

  for ( std::vector<int>::const_iterator p = pids.begin() ; 
        p != pids.end() ; 
        ++p)
  {
    // check for identical PIDs
    PidParticles PP(*p);
    for ( std::vector<PidParticles>::const_iterator p2 = m_pidParticles.begin();
          p2 != m_pidParticles.end() ; 
          ++p2)
    {
      if ( (*p)==(*p2).getPid() ){
        m_checkOrder = true ;       
        PP.setSamePid(true);
      }
    }
    m_pidParticles.push_back(PP);
  }  
  
  return StatusCode::SUCCESS ;
}
//=============================================================================
// Fill PidParticles at each event
//=============================================================================
bool CombineParticles::Decay::fillPidParticles(const LHCb::Particle::ConstVector& PV){

  for ( std::vector<PidParticles>::iterator pp=m_pidParticles.begin() ;
        pp != m_pidParticles.end() ; ++pp ){    
    if (!pp->fillParticles(PV))  return false ;
  }
  return true ;
}

//=============================================================================
// Get first set of daughters candidates 
//=============================================================================
bool CombineParticles::Decay::getFirstCandidates(LHCb::Particle::ConstVector& DaughterVector){
  return getCandidates(DaughterVector);
}
//=============================================================================
// Get next set of daughters candidates 
//=============================================================================
bool CombineParticles::Decay::getNextCandidates(LHCb::Particle::ConstVector& DaughterVector){
  if (!iterate()) return false; // first iterate to next iterators
  return getCandidates(DaughterVector);
}
//=============================================================================
// Get set of daughters candidates 
//=============================================================================
bool CombineParticles::Decay::getCandidates(LHCb::Particle::ConstVector& DaughterVector){
  bool refusedcombination = true ;
  while ( refusedcombination ){    
    DaughterVector.clear();
    refusedcombination = false ;
    for ( std::vector<PidParticles>::const_iterator pp = m_pidParticles.begin() ;
          pp != m_pidParticles.end() ; ++pp ){
      const LHCb::Particle* P = pp->getCandidate() ;
      if (!P) { 
        std::cout << "CombineParticles::Decay::getNextCandidates : This should never happen (1)" << std::endl;
        return false ;
      } else {
        if ( m_checkOrder && pp->samePid() ) {  
          if (*(DaughterVector.rbegin())) {
            if ( (*(DaughterVector.rbegin()))->key() >= P->key()){ // in case of same PID I want one key to be greater

              refusedcombination = true ;
              break ;
            }
          } else {
            std::cout << "CombineParticles::Decay::getNextCandidates : This should never happen (2)" << std::endl;
            return false ;
          } 
        }

        DaughterVector.push_back(P);
      } // there is a particle
    } // pp loop
    
    if ( (!refusedcombination) && ( m_checkOverlap )) {

      if ( foundOverlap(DaughterVector) ){

        refusedcombination = true ;
      }
    }
    if (refusedcombination){
      if (!iterate()) return false; // iterate if possible
    }
    
  } // while

  return true; // it's OK
}
//=============================================================================
// Found Overlap in vector?
//=============================================================================
bool CombineParticles::Decay::foundOverlap(const LHCb::Particle::ConstVector& DaughterVector){

  LHCb::Particle::ConstVector Children = DaughterVector ;

  replaceResonanceByDaughters( Children );

  bool found = m_checkOverlap->foundOverlap( Children );


  return found ;
}
//=============================================================================
// Replace resonance by daughters in vector
//=============================================================================
void CombineParticles::Decay::replaceResonanceByDaughters(LHCb::Particle::ConstVector& Children){

  LHCb::Particle::ConstVector stableParticles;
  
  for ( LHCb::Particle::ConstVector::iterator c = Children.begin() ; 
        c !=  Children.end() ; ++c ){

    if ( (*c)->isBasicParticle() ) {

      stableParticles.push_back(*c);
    } else {
      const LHCb::Particle::ConstVector finalStates = 
        m_particleDescendants->finalStates(*c);
      if ( finalStates.empty() ) {
        stableParticles.push_back(*c);
      } else {
        for (LHCb::Particle::ConstVector::const_iterator f = finalStates.begin();
             f != finalStates.end(); ++f) {

          stableParticles.push_back(*f);
        }
      }
    }
  }

  Children.clear();
  Children = stableParticles;
  
}
//=============================================================================
// iterate to next combination
//=============================================================================
bool CombineParticles::Decay::iterate(){
  // now iterate to the next combination
  bool didbreak = false ;
  for ( std::vector<PidParticles>::reverse_iterator pp = m_pidParticles.rbegin() ;
        pp != m_pidParticles.rend() ; ++pp ){ // reverse iterator to first increment vector n 
    if (pp->iterate()) {
      didbreak = true ;
      break ; // if I can go to the next in this vector I can exit the loop
    }
#ifdef PKDEBUG
    std::cout << "   iterate could not iterate for pid " << pp->getPid() << std::endl ;     
#endif
  }
#ifdef PKDEBUG
  std::cout << "   iterate returns " << didbreak << std::endl ;
#endif
  return didbreak ; // If I did not break, then I'm at the end of the loop
}
