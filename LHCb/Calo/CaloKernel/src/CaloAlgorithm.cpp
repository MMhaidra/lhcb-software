// $Id: CaloAlgorithm.cpp,v 1.7 2002-04-04 15:25:24 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2002/04/04 13:16:26  ibelyaev
//  bug fix in CaloAlgorithm::put(...) method
//
// Revision 1.5  2002/04/04 12:54:38  ibelyaev
//  bug fix in CaloAlgorithm::put(...) method
//
// Revision 1.4  2002/04/01 12:50:24  ibelyaev
//  add templated accesssors to tools and improve exceptions
//
// Revision 1.3  2002/04/01 11:00:36  ibelyaev
// enrich CaloAlgorithm,CaloTool,CaloMap and CaloHashMap interafces
//
// Revision 1.2  2002/03/18 18:16:22  ibelyaev
//  small update for LHCbKernel package
//
// Revision 1.1.1.1  2001/11/25 14:07:38  ibelyaev
// New Package: substitution of the  previous CaloGen package
//
// Revision 1.5  2001/11/13 09:34:42  ibelyaev
//  add accessor to IToolSvc interface in CaloAlgorithm base class
//
// Revision 1.4  2001/11/12 19:01:02  ibelyaev
//  minor  reorganization of header files
//
// ============================================================================
#define  CALOKERNEL_CALOALGORITHM_CPP 1 
// ============================================================================

// Include files
#include <algorithm>
#include <cstdlib> 
// from GaudiKernel
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/ObjectVector.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/SmartRef.h" 
#include "GaudiKernel/IHistogramSvc.h" 
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/MsgStream.h"
// CaloKernel 
#include "CaloKernel/CaloException.h"
#include "CaloKernel/CaloAlgorithm.h"
// local 
 
// ============================================================================
/** @file CaloAlgorithm.cpp
 * 
 *  Implementation file for class : CaloAlgorithm
 * 
 *  @aauthor Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 30/06/2001 
 */
// ============================================================================

// ============================================================================
/** Standard constructor (protected)
 *  @param name           name of the algorithm
 *  @param pSvcLocator    poinetr to Service Locator 
 */
// ============================================================================
CaloAlgorithm::CaloAlgorithm( const std::string&  name       ,
                              ISvcLocator*        pSvcLocator)
  : Algorithm ( name , pSvcLocator )
  /// input/output/detector data
  , m_inputData  ( "" ) ///< no default value
  , m_outputData ( "" ) ///< no default value
  , m_detData    ( "" ) ///< no default value
{
  ///
  declareProperty  ("Input"        , m_inputData  );
  declareProperty  ("Output"       , m_outputData );
  declareProperty  ("Detector"     , m_detData    );
  ///
};

// ============================================================================
/** Destructor
 */
// ============================================================================
CaloAlgorithm::~CaloAlgorithm() {};

// ============================================================================
/** Print the error  message and return status code
 *  @param msg    error message 
 *  @param st     status code 
 *  @return       status code 
 */
// ============================================================================
StatusCode 
CaloAlgorithm::Error     ( const std::string& msg , 
                           const StatusCode & st  ) const 
{
  Stat stat( chronoSvc() , name()+":Error" ); 
  return Print( msg , st , MSG::ERROR ); 
};

// ============================================================================
/** Print the warning  message and return status code 
 *  @param msg    warning message 
 *  @param st     statsu code 
 *  @return       status code 
 */
// ============================================================================
StatusCode 
CaloAlgorithm::Warning   ( const std::string& msg , 
                           const StatusCode & st  ) const 
{
  Stat stat( chronoSvc() , name()+":Warning" ); 
  return Print( msg , st , MSG::WARNING ); 
};

// ============================================================================
/**  Print the message and return status code 
 *   @param msg    warning message 
 *   @param st     status code 
 *   @param lvl    print level 
 *   @return       status code 
 */
// ============================================================================
StatusCode 
CaloAlgorithm::Print     ( const std::string& msg , 
                           const StatusCode & st  ,
                           const MSG::Level & lvl ) const 
{
  MsgStream log( msgSvc() , name() ); 
  log << lvl 
      << System::typeinfoName( typeid( *this ) ) 
      << " "   << msg ;
  ///
  if( !st.isSuccess() ) { log << " \tStatusCode=" << st ;}
  ///
  log << endreq ; 
  return  st;
};

// ============================================================================
/** Create and (re)-throw the exception  
 *  @param msg    exception message 
 *  @param exc    (previous) exception of type GaudiException
 *  @param lvl    print level 
 *  @param sc     status code  
 *  @return       status code (fictive) 
 */
// ============================================================================
StatusCode CaloAlgorithm::Exception ( const std::string    & msg ,
                                      const GaudiException & exc ,
                                      const MSG::Level     & lvl ,
                                      const StatusCode     & sc  ) const   
{ 
  Error( msg , lvl );
  throw CaloException( name() + ":: " + msg , exc, sc );
  return  sc ;
};

// ============================================================================
/** Create and (re)-throw the exception  
 *  @param msg    exception message 
 *  @param exc    (previous) exception of type GaudiException
 *  @param lvl    print level 
 *  @param sc     status code  
 *  @return       status code (fictive) 
 */
// ============================================================================
StatusCode CaloAlgorithm::Exception ( const std::string    & msg ,  
                                      const std::exception & exc , 
                                      const MSG::Level     & lvl ,
                                      const StatusCode     & sc  ) const   
{ 
  Error( msg , lvl );
  throw CaloException( name() + ":: " + msg+"("+exc.what()+")", sc );
  return  sc ;
};

// ============================================================================
/** Create and throw the exception  
 *  @param msg    exception message 
 *  @param lvl    print level 
 *  @param sc     status code  
 *  @return       status code (fictive) 
 */
// ============================================================================
StatusCode CaloAlgorithm::Exception ( const std::string    & msg ,  
                                      const MSG::Level     & lvl ,
                                      const StatusCode     & sc  ) const 
{ 
  Error( msg , lvl );
  throw CaloException( name() + ":: " + msg , sc );
  return  sc ;
};

// ============================================================================
/** @brief put results into Gaudi Event Transient Store 
 * 
 * 'Output' results: 
 *  register the output object in Gaudi Event Transient Store 
 *  @param object object to be registered 
 *  @param address address in Gaudi Event Transient Store 
 *           ("/Event" could be omitted )
 *  @exception CaloException for invalid event data service 
 *  @exception CaloException for invalid object 
 *  @exception CaloException for error result from event data service 
 *  @return status code 
 */
// ============================================================================
StatusCode CaloAlgorithm::put
( DataObject*        object  , 
  const std::string& address ) const 
{
  // check arguments 
  Assert( 0 != eventSvc()  , " put(): Invalid 'eventSvc()'!"   );
  Assert( 0 != object      , " put(): Invalid 'Object'!"       );
  Assert( !address.empty() , " put(): Invalid 'address' = '' " );
  // register the object!
  const std::string Address = 
    '/' == address[0] ? address : "/Event/" + address ;
  const std::string Type    = 
    System::typeinfoName( typeid( *object ) );
  StatusCode sc = 
    eventSvc()-> registerObject( Address , object );
  // check the result!
  Assert( sc.isSuccess() , 
          " put(): could not register '"
          + Type    + "' at address '"
          + Address + "'"  , sc );
  //
  Print( ":: The object of type '" + Type +
         "' is registered in TS at address '" 
         + Address + "'" , sc , MSG::DEBUG );
  //
  return sc ;
};



// ============================================================================
/** standard initialization method
 *  @return status code 
 */
// ============================================================================
StatusCode CaloAlgorithm::initialize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG 
      << " ==> Initialize the base class CaloAlgorithm" << endreq;
  ///
  log << MSG::DEBUG
      << " \tInput data    ='" << inputData  () << "'" << endreq 
      << " \tOutput  data  ='" << outputData () << "'" << endreq 
      << " \tDetector data ='" << detData    () << "'" << endreq ;
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** standard finalization method
 *  @return status code 
 */
// ============================================================================
StatusCode CaloAlgorithm::finalize() 
{
  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG 
      << " ==> Finalize the base class CaloAlgorithm " << endreq;
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** standard execution method
 *  @return status code 
 */
// ============================================================================
StatusCode CaloAlgorithm::execute() 
{
  MsgStream  log( msgSvc(), name() );
  log << MSG::ERROR 
      << "==> One should NEVER see this message !!" << endreq;
  ///
  return StatusCode::FAILURE ;
  ///
};

// ============================================================================
// The End 
// ============================================================================
