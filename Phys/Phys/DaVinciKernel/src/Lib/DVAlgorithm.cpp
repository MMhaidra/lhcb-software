// $Id: DVAlgorithm.cpp,v 1.62 2009-10-30 16:15:04 jpalac Exp $
// ============================================================================
// Include 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgContextSvc.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/DVAlgorithm.h"
#include "Kernel/IOnOffline.h"
#include "Kernel/IRelatedPVFinder.h"
#include "Kernel/DaVinciFun.h"
#include "Kernel/DaVinciGuards.h"
#include "Kernel/DaVinciStringUtils.h"
// ============================================================================
/** @file
 *  The implementation for class DVAlgorithm
 */                                                                 
// ============================================================================
// Standard constructor
// ============================================================================
DVAlgorithm::DVAlgorithm 
( const std::string& name, 
  ISvcLocator* pSvcLocator ) 
  : GaudiTupleAlg ( name , pSvcLocator )
  //
    , m_desktop               ( 0 )
    , m_desktopName           ( "PhysDesktop" )
  // 
    , m_vertexFitNames        () 
    , m_vertexFits            () 
  //
    , m_geomToolNames         () 
    , m_geomTools             () 
  //
    , m_filterNames           () 
    , m_filters               () 
  //
    , m_particleCombinerNames ()
    , m_particleCombiners     ()
  // 
    , m_particleReFitterNames ()
    , m_particleReFitters     ()
  //
    , m_massFitterNames       ()
    , m_massFitters           ()
  //
    , m_massVertexFitterNames ()
    , m_massVertexFitters     ()
  //
    , m_lifetimeFitterNames   ()
    , m_lifetimeFitters       ()
  //
    , m_directionFitterNames  ()
    , m_directionFitters      ()
    //
    , m_distanceCalculatorNames  ()
    , m_distanceCalculators      ()
  //
    , m_checkOverlapName      ( "CheckOverlap" ) 
    , m_checkOverlap          ( 0 )
    , m_taggingToolName       ( "BTaggingTool" )
    , m_taggingTool           ( 0 )
    , m_descendants           ( 0 )
    , m_descendantsName       ("ParticleDescendants")
    , m_writeSelResultName    ( "WriteSelResult" )
    , m_writeSelResult        ( 0 )
    , m_onOffline             ( 0 )
    , m_pvRelator             ( 0 )
    , m_ppSvc                 ( 0 )
    , m_setFilterCalled       ( false )
    , m_countFilterWrite      ( 0 )
    , m_countFilterPassed     ( 0 )
    , m_refitPVs              ( false )
    , m_multiPV               ( false )
    , m_useP2PV               ( true  )
    , m_writeP2PV             ( true  )
{
  m_inputLocations.clear() ;
  declareProperty( "InputLocations", 
                   m_inputLocations, 
                   "Input Locations forwarded to PhysDesktop" );

  m_p2PVInputLocations.clear() ;
  declareProperty( "P2PVInputLocations", 
                   m_p2PVInputLocations, 
                   "Particle -> PV Relations Input Locations forwarded to PhysDesktop" );

  declareProperty("UseP2PVRelations", m_useP2PV,
                  "Use P->PV relations internally. Forced to true if re-fitting PVs. Otherwise disabled for single PV events. Default: true.");

  declareProperty("WriteP2PVRelations", m_writeP2PV, 
                  "Write out P->PV relations table to TES. Default: true");

  // 
  m_vertexFitNames [ "Offline"       ] = "OfflineVertexFitter" ;
  m_vertexFitNames [ "Trigger"       ] = "TrgVertexFitter"     ;
  m_vertexFitNames [ "Blind"         ] = "BlindVertexFitter"   ;
  m_vertexFitNames [ "LoKi"          ] = "LoKi::VertexFitter"  ;
  m_vertexFitNames [ "Kalman"        ] = "LoKi::VertexFitter"  ;
  m_vertexFitNames [ "ParticleAdder" ] = "ParticleAdder"       ;
  declareProperty ( "VertexFitters"     , m_vertexFitNames, "Names of vertex fitters" ) ;
  //
  m_geomToolNames  [ "" ] = "GeomDispCalculator" ;
  m_geomToolNames  [ "Offline" ] = "GeomDispCalculator" ;
  declareProperty ( "GeomTools"         , m_geomToolNames, "Names of geomery tools" ) ;
  //
  declareProperty ( "CheckOverlapTool"  , m_checkOverlapName, "Name of Overlap Tool"  ) ;
  //
  declareProperty ( "WriteSelResultTool"  , m_writeSelResultName, "Name of SelResult Writer Tool" ) ;
  //
  m_filterNames    [ "" ]        = "ParticleFilter" ;
  declareProperty ( "ParticleFilters"   , m_filterNames, "Names of ParticleFilters"   ) ;
  // 
  declareProperty ( "ReFitPVs"    , m_refitPVs, "Refit PV"     ) ; 

  m_particleCombinerNames [ "Offline"       ] = "OfflineVertexFitter" ;
  m_particleCombinerNames [ "Trigger"       ] = "TrgVertexFitter"     ;
  m_particleCombinerNames [ "Kalman"        ] = "LoKi::VertexFitter"  ;
  m_particleCombinerNames [ "Blind"         ] = "BlindVertexFitter"   ;
  m_particleCombinerNames [ "LoKi"          ] = "LoKi::VertexFitter"  ;
  m_particleCombinerNames [ "ParticleAdder" ] = "ParticleAdder"       ;
  m_particleCombinerNames [ "Adder"         ] = "ParticleAdder"       ;
  m_particleCombinerNames [ "Combiner"      ] = "MomentumCombiner"    ;
  m_particleCombinerNames [ "Momenta"       ] = "MomentumCombiner"    ;
  m_particleCombinerNames [ "Jet"           ] = "MomentumCombiner"    ;
  declareProperty
    ( "ParticleCombiners"     , 
      m_particleCombinerNames , 
      "Names of particle combiners, the basic tools for creation of composed particles" ) ;
  //
  m_particleReFitterNames [ ""              ] = "OfflineVertexFitter"   ;
  m_particleReFitterNames [ "Offline"       ] = "OfflineVertexFitter"   ;
  m_particleReFitterNames [ "Vertex"        ] = "OfflineVertexFitter"   ;
  m_particleReFitterNames [ "Blind"         ] = "BlindVertexFitter"     ;
  m_particleReFitterNames [ "Kalman"        ] = "LoKi::VertexFitter"    ;
  m_particleReFitterNames [ "LoKi"          ] = "LoKi::VertexFitter"    ;
  m_particleReFitterNames [ "Mass"          ] = "LoKi::MassFitter"      ;
  m_particleReFitterNames [ "Direction"     ] = "LoKi::DirectionFitter" ;
  m_particleReFitterNames [ "ParticleAdder" ] = "ParticleAdder"         ;
  m_particleReFitterNames [ "Adder"         ] = "ParticleAdder"         ;
  declareProperty  ( "ParticleReFitters" , m_particleReFitterNames, "Names of particle refitters" ) ;
  //
  m_pvReFitterNames [ ""           ] = "AdaptivePVReFitter" ;
  m_pvReFitterNames [ "PVReFitter" ] = "PVReFitter"         ;
  m_pvReFitterNames [ "Adaptive"   ] = "AdaptivePVReFitter" ;
  m_pvReFitterNames [ "Cheated"    ] = "CheatedPVReFitter"  ;
  declareProperty  ( "PVReFitters" , m_pvReFitterNames, "Names of PV refitters" ) ;
  //
  m_massFitterNames [ ""     ] = "LoKi::MassFitter" ;
  m_massFitterNames [ "LoKi" ] = "LoKi::MassFitter" ;
  declareProperty  
    ( "MassFitters"       , m_massFitterNames       , 
      "The mapping of nick/name/type for IMassFit tools"        ) ;
  //
  declareProperty  
    ( "MassVertexFitters" , m_massVertexFitterNames , 
      "The mapping of nick/name/type for IMassVertexFit tools"  ) ;
  //
  m_lifetimeFitterNames  [ ""     ] = "PropertimeFitter"     ;
  m_lifetimeFitterNames  [ "LoKi" ] = "LoKi::LifetimeFitter" ;
  declareProperty  
    ( "LifetimeFitters"    , m_lifetimeFitterNames , 
      "The mapping of nick/name/type for ILifetimeFitter tools" ) ;
  //
  m_directionFitterNames [ ""     ] = "DirectionFitter" ;
  m_directionFitterNames [ "LoKi" ] = "LoKi::DirectionFitter" ;
  declareProperty  
    ( "DirectionFitters"    , m_directionFitterNames , 
      "The mapping of nick/name/type for IDirectionFit tools"   ) ;
  //
  m_distanceCalculatorNames [ ""        ] = "LoKi::DistanceCalculator"    ;
  m_distanceCalculatorNames [ "LoKi"    ] = "LoKi::DistanceCalculator"    ;
  m_distanceCalculatorNames [ "Trg"     ] = "LoKi::TrgDistanceCalculator" ;
  m_distanceCalculatorNames [ "Hlt"     ] = "LoKi::TrgDistanceCalculator" ;
  m_distanceCalculatorNames [ "Trigger" ] = "LoKi::TrgDistanceCalculator" ;
  declareProperty 
    ( "DistanceCalculators" , m_distanceCalculatorNames , 
      "The mapping of nick/name/type for IDistanceCalculator tools"   ) ;
  //
  declareProperty ( "DecayDescriptor"   , m_decayDescriptor   = "", "Describes the decay" ) ;
  declareProperty ( "AvoidSelResult"    , m_avoidSelResult    = false, "If true so Selresult is written" ) ;
  declareProperty ( "PrintSelResult"    , m_printSelResult    = false , "If true SelResult is printed" ) ;
  declareProperty ( "AvoidForcedOutput" , m_avoidEmptyOutput  = false , "If true TES location is written" ) ;
  declareProperty ( "PreloadTools"      , m_preloadTools      = true, "If true all tools are pre-loaded in initialize" ) ;
  //
  // enforce the registration for algorithm context service
  setProperty ( "RegisterForContextService" , true ).ignore() ;
}
// ============================================================================
// Initialize the thing
// ============================================================================
StatusCode DVAlgorithm::initialize () 
{
  
  // register for the algorithm context service 
  IAlgContextSvc* ctx = 0 ;
  if ( registerContext() ) { ctx = contextSvc() ; }  
  // setup sentry/guard
  Gaudi::Utils::AlgContext sentry ( ctx , this ) ;
  
  // initialize the base  
  StatusCode sc = GaudiTupleAlg::initialize();
  if ( sc.isFailure() ) { return sc; }                          // RETURN
  if (msgLevel(MSG::DEBUG)) debug() << "GaudiTupleAlg is initialized" <<endmsg ;

  if ( !registerContext() || 0 == contextSvc() ) 
  {
    std::string mgs = "Registration for Algorithm Context Service is disabled." ;
    Warning( mgs +  "Some tools/utilities could have the problems." );
  }
  
  if ( m_avoidSelResult ) { if (msgLevel(MSG::DEBUG)) debug() << "Avoiding SelResult" << endmsg; }
  
  
  // Load tools very
  sc = loadTools() ;
  if ( sc.isFailure() ) { return Error("Unable to load tools", sc ) ; }
  
  if (msgLevel(MSG::DEBUG)){
    if ( m_decayDescriptor == "" )
    { debug() << "Decay Descriptor string not specified"   << endmsg; } 
    else
    { debug() << "Decay Descriptor: " << m_decayDescriptor << endmsg; }
  }

  if (!m_p2PVInputLocations.empty() ) 
  {
    DaVinci::StringUtils::expandLocations( m_p2PVInputLocations.begin(),
                                           m_p2PVInputLocations.end(),
                                           onOffline()->trunkOnTES()     );
    if (msgLevel(MSG::DEBUG)) debug() << ">>> Preloading PhysDesktop with P->PV locations " << endmsg;
    desktop()->setP2PVInputLocations(m_p2PVInputLocations);
  }
   
  desktop()->setWriteP2PV( m_writeP2PV );

  if (msgLevel(MSG::DEBUG)) debug() << "End of DVAlgorithm::initialize with " << sc << endmsg;
  
  return sc;
}
// ============================================================================
// Load standard tools
// ============================================================================
StatusCode DVAlgorithm::loadTools() 
{
  
  if ( !m_preloadTools ) 
  { return Warning( "Not preloading tools", StatusCode::SUCCESS ) ; }
  
  if (msgLevel(MSG::DEBUG)) debug() << ">>> Preloading tools" << endmsg;
 
  DaVinci::StringUtils::expandLocations( m_inputLocations.begin(),
                                         m_inputLocations.end(),
                                         onOffline()->trunkOnTES() );
 
  if (msgLevel(MSG::DEBUG)) debug() << ">>> Preloading PhysDesktop with locations " << m_inputLocations << endmsg;
  desktop()->setInputLocations(m_inputLocations);
  
  // vertex fitter
  
  if ( m_particleCombinerNames.end() == m_particleCombinerNames.find("") )
  {
    m_particleCombinerNames[""] = onOffline()->particleCombinerType() ;
  }
  
  if ( m_vertexFitNames.end() == m_vertexFitNames.find("") )
  {
    m_vertexFitNames[""] = onOffline()->vertexFitterType() ;
  }
  
  if (msgLevel(MSG::DEBUG)) debug() << ">>> Preloading "
          << m_vertexFitNames[""] << " as IVertexFit " << endmsg;
  vertexFitter() ;
  
  // geometry THIS IS OBSOLETE
  if ( m_distanceCalculatorNames.end() == m_distanceCalculatorNames.find("") )
  {
    m_distanceCalculatorNames[""] = onOffline()->distanceCalculatorType() ;
  }
  
  // distance geometry
  if (msgLevel(MSG::DEBUG)) debug() << ">>> Preloading " 
          << m_distanceCalculatorNames[""] 
          << " as IDistanceCalculator" << endmsg;
  distanceCalculator();
  
  if (msgLevel(MSG::DEBUG)) 
  { debug() << ">>> Preloading CheckOverlap Tool" << endmsg; }
  checkOverlap();
  
  if (msgLevel(MSG::DEBUG))
  { debug() << ">>> Preloading WriteSelResults Tool" << endmsg; }
  
  writeSelResult();
  
  if (msgLevel(MSG::DEBUG)) 
  { debug() << ">>> Preloading LHCb::ParticlePropertySvc" << endmsg; }
  ppSvc() ;
  
  return StatusCode::SUCCESS;
}
// ============================================================================
// Execute
// ============================================================================
StatusCode DVAlgorithm::sysExecute () 
{
  
  // register for the algorithm context service
  IAlgContextSvc* ctx = 0 ;
  if ( registerContext() ) { ctx = contextSvc() ; }
  // setup sentry/guard
  Gaudi::Utils::AlgContext sentry ( ctx , this ) ;

  DaVinci::Guards::CleanDesktopGuard desktopGuard(desktop());

  desktop()->setUsingP2PV(this->useP2PV());

  StatusCode sc = desktop()->getEventInput();
  if ( sc.isFailure()) 
  { return Error (  "Not able to fill PhysDesktop" , sc ) ; }
  
  const LHCb::RecVertices* pvs = desktop()->primaryVertices();
  
  m_multiPV = 0!=pvs ? pvs->size() > 1 : false;

  // execute the algorithm 
  sc = this->Algorithm::sysExecute();
  if ( sc.isFailure() ) return sc;
  
  if ( !m_setFilterCalled ) 
  { Warning ( "SetFilterPassed not called for this event!" ).ignore() ; }
  
  /// count number of "effective filters"  
  counter("#accept") += filterPassed() ;

  if (!m_avoidSelResult) 
  { sc = fillSelResult () ; }
  else 
  { verbose() << "Avoiding selresult" << endmsg ; }
  
  // Reset for next event
  m_setFilterCalled = false;
  
  // Make sure each DVAlgorithm has written out something
  if ( !m_avoidEmptyOutput ) 
  { sc = desktop()->writeEmptyContainerIfNeeded(); }
  else 
  { verbose() << "Avoiding mandatory output" << endmsg ; }

  return sc ;
}
// ============================================================================
void DVAlgorithm::setFilterPassed  (  bool    state  ) 
{
  ///
  this->Algorithm::setFilterPassed(state); 
  m_setFilterCalled = true;
  return;
}
// ============================================================================
const LHCb::VertexBase* DVAlgorithm::calculateRelatedPV(const LHCb::Particle* p) const
{
  //  always() << "ATTENTION! called calculateRelatedPV!" << endmsg;
  if (msgLevel(MSG::VERBOSE)) verbose() << "DVAlgorithm::calculateRelatedPV" << endmsg;
  const IRelatedPVFinder* finder = this->relatedPVFinder();
  const LHCb::RecVertex::Container* PVs = this->primaryVertices();
  if (0==finder || 0==PVs) {
    Error("NULL IRelatedPVFinder or primary vertex container", StatusCode::FAILURE, 1 ).ignore() ;
    return 0;
  }
  // re-fit vertices, then look for the best one.
  if ( refitPVs() ) {
    if (msgLevel(MSG::VERBOSE)) verbose() << "Re-fitting " 
                                          << PVs->size() << " PVs"<< endmsg;
    const IPVReFitter* fitter = primaryVertexReFitter();
    if (0==fitter) {
      Error("NULL IPVReFitter", StatusCode::FAILURE, 1).ignore();
      return 0;
    }
    LHCb::RecVertex::ConstVector reFittedPVs;
    DaVinci::Guards::PointerContainerGuard<LHCb::RecVertex::ConstVector> guard(reFittedPVs);
    for (LHCb::RecVertex::Container::const_iterator iPV = PVs->begin();
         iPV != PVs->end(); ++iPV) {
      LHCb::RecVertex* reFittedPV = new LHCb::RecVertex(**iPV);
      if ( (fitter->remove(p, reFittedPV)).isSuccess() ) {
        reFittedPVs.push_back(reFittedPV); 
      } else {
        delete  reFittedPV;
        Error("PV re-fit failed", StatusCode::FAILURE, 1 ).ignore() ;
      } 
    }
    if (msgLevel(MSG::VERBOSE)) verbose() << "have " << reFittedPVs.size()
                                          << " re-fitted PVs" 
                                          << endmsg;

    const LHCb::VertexBase* vb = finder->relatedPV(p, reFittedPVs);

    if (0==vb) {
      Warning("IRelatedPVFinder found no best vertex",
              StatusCode::FAILURE, 10).ignore();
      return 0;
    }
    
    const LHCb::RecVertex*  pv = dynamic_cast<const LHCb::RecVertex*>(vb) ;

    if (0==pv) {
      Warning("VertexBase -> RecVertex dynamic cast failed",
              StatusCode::FAILURE, 10).ignore();
      return 0;
    } else {
      const LHCb::RecVertex* returnPV = desktop()->keep(pv);
      return returnPV;
    }
  } else { // no PV re-fit
    if (msgLevel(MSG::VERBOSE)) verbose() << "Getting related PV from finder" << endmsg;
    const LHCb::VertexBase* pv  = finder->relatedPV(p, *PVs);

    if (0!=pv) {
      if (msgLevel(MSG::VERBOSE)) verbose() << "Returning related vertex\n" 
                                            << pv << endmsg;
    } else {
      if (msgLevel(MSG::VERBOSE)) verbose() << "no related PV found" << endmsg;
    }
     return pv;
  }
  
}
// ============================================================================
const LHCb::VertexBase* DVAlgorithm::_getRelatedPV(const LHCb::Particle* part) const
{
  //  always() << "ATTENTION! called getRelatedPV!" << endmsg;
  if (msgLevel(MSG::VERBOSE)) verbose() << "_getRelatedPV! Getting range" << endmsg;
  if (0==part) {
    error() << "input particle is NULL" << endmsg;
    return 0;
  }

  if (!hasStoredRelatedPV(part) ) {
    if (msgLevel(MSG::VERBOSE)) {
      verbose() << "particle2Vertices empty. Calling calculateRelatedPV" 
                << endmsg;
    }
    const LHCb::VertexBase* pv = calculateRelatedPV(part);
    if (0!=pv) {
      if (msgLevel(MSG::VERBOSE)) {
        verbose() << "Found related vertex. Relating it" << endmsg;
      }
      relateWithOverwrite(part, pv);
      return pv;
    } else {
      Warning("Found no related vertex", StatusCode::FAILURE, 10).ignore();
      return 0;
    }
  } else {
    const Particle2Vertex::Table::Range range = desktop()->particle2Vertices(part);
    return DaVinci::bestVertexBase(range);
  }
  
}
// ============================================================================
bool DVAlgorithm::hasStoredRelatedPV(const LHCb::Particle* particle) const
{
  const Particle2Vertex::Table::Range p2pvRange = desktop()->particle2Vertices(particle);
  return !p2pvRange.empty();
}
// ============================================================================
StatusCode DVAlgorithm::fillSelResult () {

  // Create and fill selection result object
  LHCb::SelResult myResult;
  myResult.setFound(filterPassed());
  myResult.setLocation( ("/Event/Phys/"+name()));
  if (msgLevel(MSG::VERBOSE)) verbose() << "SelResult location set to " << "/Event/Phys/"+name() 
                                        << endmsg;
  myResult.setDecay(getDecayDescriptor());

  StatusCode sc = writeSelResult()->write(myResult);
    
  if (filterPassed()) m_countFilterPassed++;
  m_countFilterWrite++;
  if (msgLevel(MSG::VERBOSE)) verbose() << "wrote " << filterPassed() 
                                        << " -> " << m_countFilterWrite 
                                        << " & " 
                                        << m_countFilterPassed << endmsg ;

  return sc ;

}
// ============================================================================
// Finalize the algorithm + post-actions
// ============================================================================
StatusCode DVAlgorithm::finalize () 
{

  // register for the algorithm context service
  IAlgContextSvc* ctx = 0 ;
  if ( registerContext() ) { ctx = contextSvc() ; }
  // setup sentry/guard
  Gaudi::Utils::AlgContext sentry ( ctx , this ) ;


  if ((m_printSelResult) && (!m_avoidSelResult)){
    
    if (m_countFilterWrite < m_countFilterPassed ){
      warning() << "Executed " << m_countFilterWrite << 
        " times and flagged as passed " << m_countFilterPassed <<
        " times" << endmsg;      
    } else if (m_countFilterWrite <= 0 ){      
      warning() << "Executed " << m_countFilterWrite << " times" 
                << endmsg;
    } else if (m_countFilterPassed <= 0 ){
      always() << "No events selected in " << 
        m_countFilterWrite << " calls." << endmsg;
    } else if (m_countFilterPassed == m_countFilterWrite ){
      always() << "All events selected in " << 
        m_countFilterWrite << " calls." << endmsg;
    } else {
      double eta = (double)m_countFilterPassed/(double)m_countFilterWrite ;
      double delta = sqrt( eta*((double)1.-eta)/(double)m_countFilterWrite );
      double r = (double)1./eta ;
      double re = r*(delta/eta) ;
      
      always() << "Passed " << m_countFilterPassed << 
        " times in " << m_countFilterWrite << " calls -> (" <<
        100.0*eta << "+/-" << 100.0*delta <<  ")%, rejection= " << 
        r << "+/-" << re << endmsg;
    }
  }
  
  // finalize GaudiTupleAlg base class
  return GaudiTupleAlg::finalize();

} 
// ============================================================================
void DVAlgorithm::imposeOutputLocation(const std::string& outputLocationString)
{
  if ( 0==desktop() ) {
    fatal() << "Desktop has not been created yet" << endmsg;
  }
  desktop()->imposeOutputLocation(outputLocationString);  
  return;  
}
// ============================================================================
// The END 
// ============================================================================
