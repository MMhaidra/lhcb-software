// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IInterface.h"
// ============================================================================
// Kernel ?
// ============================================================================
#include "Kernel/IMC2Collision.h"
// ============================================================================
/** @file
 *
 *  Implementation file for class IMC2Collision
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the smear campaign of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-18 
 *
 *                    $Revision$
 *  Last modification $Date$
 *                 by $Author$
 */
// ============================================================================
namespace 
{
  // ==========================================================================
  /** @var IID_IMC2Collision
   *  unique static identifier of IMC2Collision interface 
   *  @see IMC2Collision
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-03-18
   */
  // ==========================================================================
  const InterfaceID IID_IMC2Collision ( "IMC2Collision" , 1 , 0 ) ;
  // ==========================================================================
}
// ============================================================================
// Return the unique interface identifier
// ============================================================================
const InterfaceID& IMC2Collision::interfaceID() { return IID_IMC2Collision ; } 
// ============================================================================
// destructor 
// ============================================================================
IMC2Collision::~IMC2Collision() {}
// ============================================================================
// The END 
// ============================================================================

