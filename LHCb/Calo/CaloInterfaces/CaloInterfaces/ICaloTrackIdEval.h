// $Id: ICaloTrackIdEval.h,v 1.4 2004-09-02 18:52:51 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOINTERFACES_ICALOTRACKIDEVAL_H 
#define CALOINTERFACES_ICALOTRACKIDEVAL_H 1
// Include files
// STD & STL 
#include <functional>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// forward declaration
class TrStoredTrack ;
class TrgTrack      ;

/** @class ICaloTrackIdEval ICaloTrackIdEval.h CaloInterfaces/ICaloTrackIdEval.h
 *  
 *  The abstract interface for generic track ID estimator 
 *  with calorimeters  
 *
 *  The obvious candidates for implementation
 *   - e+/e-   identification with Prs/Spd 
 *   - e+/e-   identification with Ecal  
 *   - e+/e-   identification with Hcal  
 *   - mu+/mu- identification with Hcal  
 *  
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-11-10
 */

class ICaloTrackIdEval : 
  public virtual                                 IAlgTool ,
  public std::unary_function<const TrStoredTrack*,double>
{
  
public:
  
  /** static interface identification
   *  @see IInterface
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() ;
  
  /** The main processing method 
   *  It evaluated the Track ID estimators using the calorimeter information  
   *  @param  track  pointer to the object to be processed
   *  @param  value  (return) the value of the estimator
   *  @return status code 
   */  
  virtual StatusCode process   
  ( const TrStoredTrack* track , 
    double&              value ) const = 0 ;
  
  /** The main processing method (functor interface)
   *  It evaluated the Track ID estimators using the calorimeter information  
   *  @param  track  pointer to the object to be processed
   *  @param  the value of the estimator
   */  
  virtual double     operator() 
    ( const TrStoredTrack* track ) const = 0 ;
  
  /** The main processing method 
   *  It evaluated the Track ID estimators using the calorimeter information  
   *  @param  track  pointer to the object to be processed
   *  @param  value  (return) the value of the estimator
   *  @return status code 
   */  
  virtual StatusCode process  
  ( const TrgTrack*      track , 
    double&              value ) const = 0 ;
  
  /** The main processing method (functor interface)
   *  It evaluated the Track ID estimators using the calorimeter information  
   *  @param  track  pointer to the object to be processed
   *  @param  the value of the estimator
   */  
  virtual double     operator() 
    ( const TrgTrack*      track ) const = 0 ;
  
protected:
  
  /// protected destructor 
  virtual ~ICaloTrackIdEval();
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // CALOINTERFACES_ICALOTRACKIDEVAL_H
// ============================================================================
