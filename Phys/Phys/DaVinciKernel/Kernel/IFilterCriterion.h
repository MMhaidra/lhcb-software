// $Id: IFilterCriterion.h,v 1.1 2005-01-06 10:30:43 pkoppenb Exp $
#ifndef DAVINCIKERNEL_IFILTERCRITERION_H 
#define DAVINCIKERNEL_IFILTERCRITERION_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
// from Event
#include "Event/Particle.h"

static const InterfaceID IID_IFilterCriterion("IFilterCriterion", 1 , 0); 

/** @class IFilterCriterion IFilterCriterion.h Kernel/IFilterCriterion.h 
 *  Interface Class for FilterCriterion.
 *  Given a Particle, returns a yes/no decision.
 *  @author Paul Colrain
 *  @date   14/03/2002
 */

class IFilterCriterion : virtual public IAlgTool {

public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() {
    return IID_IFilterCriterion;
  }

  /// Test if filter is satisfied
  virtual bool isSatisfied( const Particle* const & ) = 0;

  /// Test if filter is satisfied
  virtual bool operator()( const Particle* const & ) = 0;

};

#endif // DAVINCIKERNEL_IFILTERCRITERION_H


