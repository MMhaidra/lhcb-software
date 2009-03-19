// $Id: IHltInspector.cpp,v 1.1 2009-03-19 13:16:12 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <string>
#include <vector>
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/IHltInspector.h"
// ============================================================================
/** @file 
 *  Implementation file for class Hlt::IInspector
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-03-17 
 */
// ============================================================================
// virtual & protected destructor 
// ============================================================================
Hlt::IInspector::~IInspector(){}
// ============================================================================
// Return the unique interface ID 
// ============================================================================
const InterfaceID& Hlt::IInspector::interfaceID() 
{
  static const InterfaceID s_IID ( "Hlt::IInspector" , 1 , 0 ) ;
  return s_IID ; 
}
// ============================================================================
// The END 
// ============================================================================
