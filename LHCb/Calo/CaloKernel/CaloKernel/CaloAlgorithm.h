// $Id: CaloAlgorithm.h,v 1.19 2004-02-17 11:55:54 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOKERNEL_CALOALGORITHM_H 
#define CALOKERNEL_CALOALGORITHM_H 1
// ============================================================================
// Include files
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================

template<class TYPE>
class         AlgFactory     ;

/** @class  CaloAlgorithm CaloAlgorithm.h CaloKernel/CaloAlgorithm.h
 *   
 *  The useful base class for calo algorithm.
 *  small externsion of ordinary algorithm base class
 *  Extensions:
 *  useful protected print methods, exceptions and assertions
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   30/06/2001
 */

class CaloAlgorithm : public GaudiAlgorithm 
{
public:
  
  /// the actual type for container of 'addresses' 
  typedef std::vector<std::string> Inputs ;
  
public:
  
  /** standard initialization method
   *  @see  Algorithm
   *  @see IAlgorithm 
   *  @return status code 
   */
  virtual StatusCode initialize(); 
  
  /** standard execution method
   *  @see  Algorithm
   *  @see IAlgorithm 
   *  @return status code 
   */
  virtual StatusCode execute   (); 
  
  /** standard finalization method
   *  @see  Algorithm
   *  @see IAlgorithm 
   *  @return status code 
   */
  virtual StatusCode finalize  (); 
  
protected:
  
  /** Standard constructor (protected)
   *  @see  GaidiAlgorithm
   *  @see       Algorithm
   *  @param name           name of the algorithm
   *  @param pSvcLocator    poinetr to Service Locator 
   */
  CaloAlgorithm
  ( const std::string& name        , 
    ISvcLocator*       pSvcLocator );
  
  /// destructor, virtual and protected 
  virtual ~CaloAlgorithm();
  
protected: ///< "technical" methods
  
  /// address/location/name in Transient Store of input  data container 
  const std::string& inputData  () const { return m_inputData  ; }
  
  /// vector of input  data containers 
  const Inputs&       inputs    () const { return m_inputs     ; }
  
  /// address/location/name in Transient Store of output data container 
  const std::string& outputData () const { return m_outputData ; }
  
  /// address/location/name in Transient Store of detector data  
  const std::string& detData    () const { return m_detData    ; }
  
  /** set address/location/name in Transient Store of input  data container 
   *  @param  addr address/location/name in Transient Store of input data 
   */
  void setInputData ( const std::string& addr ) { m_inputData  = addr; }
  
  /** set input  data containers  
   *  @param  addr vector of  addresses in Transient Store of input data 
   */
  void setInputs    ( const Inputs&      addr ) { m_inputs     = addr  ; }
  
  /** add address/location/name in Transient Store of input  data container 
   *  @param  addr address/location/name in Transient Store of input data 
   */
  void addToInputs  ( const std::string& addr ) { m_inputs.push_back( addr ) ; }
  
  /** set address/location/name in Transient Store of output  data container 
   *  @param  addr address/location/name in Transient Store of output data 
   */
  void setOutputData( const std::string& addr ) { m_outputData = addr; }  
  
  /** set address/location/name in Transient Store of detector data  
   *  @param  addr address/location/name in Transient Store of detector 
   */
  void setDetData   ( const std::string& addr ) { m_detData  = addr; }
  
private:
  
  /// no default constructor 
  CaloAlgorithm ();
  /// no copy constructor 
  CaloAlgorithm            ( const CaloAlgorithm& );
  /// no assignment oprtator  
  CaloAlgorithm& operator= ( const CaloAlgorithm& );
  
private:
  
  // address/location/name in Transient Store of input  data container 
  std::string      m_inputData  ;
  // vector of addresses in the case of 'few' inputs 
  Inputs           m_inputs     ;
  // address/location/name in Transient Store of output data container 
  std::string      m_outputData ;
  // address/location/name in Transient Store of detector data  
  std::string      m_detData    ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // CALOALGORITHM_H 
// ============================================================================

