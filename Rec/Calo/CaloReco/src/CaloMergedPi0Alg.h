// $Id: CaloMergedPi0Alg.h,v 1.2 2004-03-17 16:32:22 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2 2004/09/02 20:46:40  odescham 
// - Transv. Shape parameters in option file 
// - new parameters tuned for DC04
//
// Revision 1.1.1.1  2002/11/13 20:46:40  ibelyaev
// new package 
//
// Revision 1.4  2002/07/05 10:03:13  ibelyaev
//  new version of MergedPi0 algorithm
//
// ============================================================================
#ifndef CALOEX_CaloMergedPi0Alg_H 
#define CALOEX_CaloMergedPi0Alg_H 1
// Include files
// from STL
#include <string>
#include <vector>
// From GaudiKernel
#include "GaudiKernel/NTuple.h"
// from   CaloKernel
#include "CaloKernel/CaloAlgorithm.h"
// forward declarations
class    CaloCluster   ;
class   ICaloHypoTool  ;

/** @class CaloMergedPi0Alg CaloMergedPi0Alg.h
 *  
 *  Merged pi0 reconstruction with Iterativ Method
 *
 *  @author Olivier Deschamps 
 *  @date   05/10/2002
 */

class CaloMergedPi0Alg : public CaloAlgorithm 
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloMergedPi0Alg>;
  
public:
  
  /** standard algorithm initialization 
   *  @return status code 
   */
  virtual StatusCode initialize();   
  
  /** standard algorithm execution 
   *  @return status code 
   */
  virtual StatusCode execute   ();   
  
  /** standard algorithm finalization 
   *  @return status code 
   */
  virtual StatusCode finalize  ();   
  
protected:

  /** Standard constructor
   *  @param   name   algorithm name 
   *  @param   svcloc pointer to service locator 
   */
  CaloMergedPi0Alg( const std::string& name   , 
              ISvcLocator*       svcloc );
  
  /** destructor
   */
  virtual ~CaloMergedPi0Alg();
  
protected:
  
  /** helper function to calculate number of digits 
   *  in the cluster with given status 
   *  @param cluster pointet to the cluster object
   *  @param status  digit statsu to be checked
   *  @return number of digits with given status.
   *       In the case of errors it returns -1
   */
  long numberOfDigits ( const CaloCluster*             cluster ,
                        const CaloDigitStatus::Status& status  ) const ;

  // L-correction
  double BarZ ( const double e,
                const double eprs,
                const unsigned int area,
                const double x,
                const double y,
                const double z );

  // S-correction
  double BarXY (const int axis,
                const unsigned int area,
                const double e[3][3]);


  // Shower Transverse fit
  double TrShape( const int Neig,
                  const unsigned int area,
                  const double SpdHit,
                  const double D3D);
  
    
private:
  /** NTuple 
   */
  INTuple*            m_ntuple    ; ///< pointer to NTuple 
  std::string         m_ntupleLUN ; ///< logical file unit for NTuple
  NTuple::Item<float>  m_mas ; 
  NTuple::Item<long>   m_cls ; 
  NTuple::Item<long>   m_iter ; 
  std::string          m_nameOfSplitPhotons  ;
  std::string          m_nameOfSplitClusters ;


  // tools to be applied for all hypotheses 
  typedef std::vector<std::string>     Names ;
  typedef std::vector<ICaloHypoTool*>  Tools ;
  Names        m_toolTypeNames               ;
  Tools        m_tools                       ;
  
  // Input Parameters
  typedef std::vector<double>    Parameters  ;
  Parameters TrShOut_nospd;
  Parameters TrShMid_nospd;
  Parameters TrShInn_nospd;
  Parameters TrShOut_spd;
  Parameters TrShMid_spd;
  Parameters TrShInn_spd;
  Parameters SPar;
  Parameters LPar_Al1;
  Parameters LPar_Al2;
  Parameters LPar_Al3;
  Parameters LPar_Be0;
  Parameters LPar_Be1;
  Parameters LPar_Be2;
  Parameters LPar_Be3;
  Parameters LPar_z0;
};

// ============================================================================
// The End 
// ============================================================================
#endif // CaloMergedPi0Alg_H
// ============================================================================
