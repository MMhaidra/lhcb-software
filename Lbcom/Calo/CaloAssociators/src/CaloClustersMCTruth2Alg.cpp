// $Id: CaloClustersMCTruth2Alg.cpp,v 1.1 2002-06-13 12:28:47 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// LHCbKernel 
#include "Relations/RelationWeighted2D.h"
#include "Relations/RelationWeighted1D.h"
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/StreamBuffer.h"
// Event 
#include "Event/MCParticle.h"
#include "Event/KeyedObject.h"
// CaloDet
#include "CaloDet/DeCalorimeter.h"
// CasloEvent/Event
#include "Event/CaloCluster.h"
#include "Event/CaloMCTools.h"
// local
#include "CaloClustersMCTruth2Alg.h"

// ============================================================================
/** @file CaloClusterMCTruth.cpp
 * 
 *  Implementation file for class : CaloClustersMCTruth2Alg
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 08/04/2002 
 */
// ============================================================================

// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
static const  AlgFactory<CaloClustersMCTruth2Alg>         s_factory ;
const        IAlgFactory&CaloClustersMCTruth2AlgFactory = s_factory ; 
// ============================================================================


// ============================================================================
/** Standard constructor
 *  @param name name of the algorithm
 *  @param svc  service locator
 */
// ============================================================================
CaloClustersMCTruth2Alg::CaloClustersMCTruth2Alg
( const std::string& name ,
  ISvcLocator*       svc  )
  : CaloAlgorithm ( name , svc ) 
{};
// ============================================================================

// ============================================================================
/// Destructor
// ============================================================================
CaloClustersMCTruth2Alg::~CaloClustersMCTruth2Alg() {}; 
// ============================================================================

// ============================================================================
/** standard initialization of the algorithm
 *  @see CaloAlgorithm 
 *  @see     Algorithm 
 *  @see    IAlgorithm 
 *  @return StatusCode
 */
// ============================================================================
StatusCode CaloClustersMCTruth2Alg::initialize()
{  
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;
  
//  std::cout << Conversion<char,int>::exists << std::endl ;

  // initialize the base class 
  StatusCode sc = CaloAlgorithm::initialize() ;
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class CaloAlgorithm",sc);}
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** standard finalization of the algorithm
 *  @see CaloAlgorithm 
 *  @see     Algorithm 
 *  @see    IAlgorithm 
 *  @return StatusCode
 */
// ============================================================================
StatusCode CaloClustersMCTruth2Alg::finalize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;
  /// finalize thebase class
  return CaloAlgorithm::finalize () ;
};

// ============================================================================
/** standard execution of the algorithm
 *  @see CaloAlgorithm 
 *  @see     Algorithm 
 *  @see    IAlgorithm 
 *  @return StatusCode
 */
// ============================================================================
StatusCode CaloClustersMCTruth2Alg::execute() 
{
  /// avoid the long name and always use "const" qualifier  
  using namespace CaloMCTools;
  typedef const CaloClusters                               Clusters  ;
  typedef const DeCalorimeter                              Detector  ;
  typedef RelationWeighted1D<CaloCluster,MCParticle,float> Table     ;
  
  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;
  
  // get input clusters 
  Clusters*   clusters  = get ( eventSvc () , inputData () , clusters ) ;
  if( 0 == clusters  )               { return StatusCode::FAILURE ; }
  
  // get the detector 
  Detector*   detector  = get ( detSvc   () , detData   () , detector );
  if( 0 == detector  )               { return StatusCode::FAILURE ; }
  
  const double activeToTotal = detector->activeToTotal() ;
  
  // create relation table and register it in the event transient store 
  Table* table = new Table();
  StatusCode sc = put( table , outputData () );
  if( sc.isFailure() ) { return sc ; }

  if( 0 == clusters -> size () ) 
    { Warning("Empty container of clusters '" + inputData() + "'"); }
  if( 0 == clusters -> size () )     { return StatusCode::SUCCESS ; }
  
  
  // create the MCtruth evaluator 
  LargestDeposition<CaloCluster> evaluator;
  
  // fill the relation data 
  for( Clusters::const_iterator icluster = clusters->begin() ;
       clusters->end() != icluster ; ++icluster ) 
    {
      const CaloCluster* cluster = *icluster ;
      if( 0 == cluster  ) { continue ; }                   // Skip NULLs 
      
      // use the evaluator to extract MCtruth information (follow references)
      const ParticlePair p( evaluator( cluster ) );
      const MCParticle* particle = p.second ;
      const double      energy   = p.first  ;
      if( 0 == particle || 0 == energy ) { continue ; }    // Skip NULLs 
      
      // put relation to relation table 
      table->relate( cluster , particle , (float) energy * activeToTotal );
    };
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================
