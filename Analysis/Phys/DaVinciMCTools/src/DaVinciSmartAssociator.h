#ifndef DAVINCISMARTASSOCIATOR_H 
#define DAVINCISMARTASSOCIATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IDaVinciSmartAssociator.h"            // Interface
#include "Kernel/IBackgroundCategory.h"
#include "Kernel/IDaVinciAssociatorsWrapper.h"
#include "Kernel/Particle2MCLinker.h"

/** @class DaVinciSmartAssociator DaVinciSmartAssociator.h
 *  
 *  A ``smart'' associator for any kind of particle. Returns a vector of
 *  MCAssociation classes, each of which represents a single associated MCParticle
 *  and its association weight. The association methods used are
 *
 *  Charged stables: Protoparticle associator
 *  Neutral stables: Neutral Protoparticle associator
 *  Composite: BackgroundCategory
 *
 *  For composites, there is always at most one associated MCParticle, whose
 *  association weight is set to 1 by definition
 *  
 *  @author V. Gligorov
 *  @date   2009-01-13
 */
class DaVinciSmartAssociator : public GaudiTool, 
                               virtual public IDaVinciSmartAssociator {
public: 
  /// Standard constructor
  DaVinciSmartAssociator( const std::string& type, 
                          const std::string& name,
                          const IInterface* parent);

  virtual ~DaVinciSmartAssociator( ); ///< Destructor

  StatusCode initialize() ;
  StatusCode finalize() ;

  Particle2MCLinker::ToRange associate(const LHCb::Particle* particleToBeAssociated);

private:

  IDaVinciAssociatorsWrapper* m_linkerTool_cPP;
  IDaVinciAssociatorsWrapper* m_linkerTool_nPP;

  IBackgroundCategory* m_bkg; //for composites

};
#endif // DAVINCISMARTASSOCIATOR_H
