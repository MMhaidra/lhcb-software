// $Id: ICaloHypoLikelihood.h,v 1.2 2004-02-17 11:51:54 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2004/02/17 11:42:28  ibelyaev
//  fix in Likel(i,y)hoods
//
// ============================================================================
#ifndef CALOINTERFACES_ICALOHYPOLIKELIHOOD_H 
#define CALOINTERFACES_ICALOHYPOLIKELIHOOD_H 1
// Include files
// STD & STL 
#include <functional>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// LHCbKernel
#include "Kernel/CaloHypotheses.h"
// forward declaration
class     CaloHypo     ;     // from CaloEvent/Event

class ICaloHypoLikelihood:
  public  virtual IAlgTool                               ,
  public  std::unary_function<const CaloHypo*,double>
{
public:
  
  /** static interface identification
   *  @see IInterface
   *  @see IID_ICaloHypoLikelihood
   *  @return the unique interface identifier
   */
  static const InterfaceID& interfaceID() ;
  
  /** calorimeter hypothesis to be evaluated
   *  @see CaloHypotheses 
   *  @return calorimeter hypothesis to be evaluated 
   */
  virtual const CaloHypotheses::Hypothesis& hypothesis() const = 0 ;
  
  /** evaluate the likelihood of the cluster for the given hypotheses
   *  @param  cluster pointer to cluster, to be evaluated (const!)
   *  @return likelyhood of the hypothesis 
   */
  virtual double likelihood ( const CaloHypo* hypo ) const = 0 ;
  
  /** evaluate the likelihood of the cluster for the given hypotheses
   *  (functor interface)
   *  @param  cluster pointer to cluster, to be evaluated (const!)
   *  @return likelyhood of the hypothesis 
   */
  virtual double operator() ( const CaloHypo* cluster ) const = 0 ;
  
protected:
  
  /// dectructor (virtual and protected)
  virtual ~ICaloHypoLikelihood();
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_ICALOHYPOLIKELIHOOD_H
// ============================================================================



