// $Id: ICaloCorrection.h,v 1.3 2004-02-17 11:51:54 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2004/02/17 11:42:27  ibelyaev
//  fix in Likel(i,y)hoods
//
// Revision 1.1  2002/04/02 11:08:12  ibelyaev
//  new interfaces
// 
// ============================================================================
#ifndef CALOINTERFACES_ICALOCORRECTION_H 
#define CALOINTERFACES_ICALOCORRECTION_H 1
// Include files
// STD & STL 
#include <functional>
// GaudiKernel
#include "GaudiKernel/IAlgTool.h"
// forward declaration 
class HepPoint3D   ;   // CLHEP 
class CaloPosition ;   // CaloEvent/Event 
 
/** @class ICaloCorrection ICaloCorrection.h CaloInterfaces/ICaloCorrection.h
 *  
 *  A minimal abstract interface for S-,L-shape correction functions 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   22/03/2002
 */
class ICaloCorrection :
  public virtual IAlgTool , 
  public std::binary_function<const CaloPosition,const HepPoint3D,CaloPosition*>
{
public:
  
  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() ;
  
  /** The main processing method 
   *  @param  position  uncorrected position
   *  @param  seed      position of the 'seed' 
   *  @param  corrected new created corrected CaloPosition object 
   *  @return status code 
   */  
  virtual StatusCode     process    
  ( const CaloPosition & position  , 
    const HepPoint3D   & seed      , 
    CaloPosition*&       corrected ) const = 0 ;
  
  /** The main processing method 
   *  @param  position  uncorrected position
   *  @param  seed      position of the 'seed' 
   *  @return new created corrected CaloPosition object 
   */  
  virtual CaloPosition* process    
  ( const CaloPosition & position , 
    const HepPoint3D   & seed     ) const = 0 ;
  
  /** The main processing method (functor interface)
   *  @param  position  uncorrected position
   *  @param  seed      position of the 'seed' 
   *  @return new created corrected CaloPosition object 
   */  
  virtual CaloPosition* operator() 
    ( const CaloPosition & position , 
      const HepPoint3D   & seed     ) const = 0 ;

protected:
  
  /// destructor, virtual
  virtual ~ICaloCorrection();
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_ICALOCORRECTION_H
// ============================================================================
