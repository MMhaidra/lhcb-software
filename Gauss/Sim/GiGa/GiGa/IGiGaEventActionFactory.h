/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#ifndef     GIGA_IGiGaEventActionFACTORY_H
#define     GIGA_IGiGaEventActionFACTORY_H 1
/// ===========================================================================


// 
#include "GaudiKernel/IFactory.h"

#include "GiGa/IIDIGiGaEventActionFactory.h"


class ISvcLocator;
class IGiGaEventAction;


/** @class IGiGaEventActionFactory IGiGaEventActionFactory.h 
 *    
 *  The IGiGaEventActionFactory is the interface used to create 
 *  the contrete instances of GiGa event Action 
 *   
 *  @author Vanya Belyaev
 *  @author 17/03/2001
 */

class IGiGaEventActionFactory : virtual public IFactory 
{
  ///
 public:
  
  /** retrieve interface ID
   *  @return unique interafce identifier 
   */
  static const InterfaceID& interfaceID () 
  { return IID_IGiGaEventActionFactory; }
  
  /** create an instance of a concrete GiGaeventAction 
   *  @param name name of the instance 
   *  @param svc  pointer to Service Locator 
   *  @return pointer to created Event Action object 
   */
  virtual IGiGaEventAction* instantiate ( const std::string& name , 
                                          ISvcLocator*       svc  ) const = 0;
  
  /** access to the GiGaEventAction type
   *  @return type of GiGaEventAction 
   */
  virtual const std::string&  eventActionType ()    const  = 0;
  ///
};

/// ===========================================================================
#endif   ///< GIGA_IGiGaEventActionFACTORY_H
/// ===========================================================================

