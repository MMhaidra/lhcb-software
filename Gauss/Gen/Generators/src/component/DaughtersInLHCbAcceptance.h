// $Id: DaughtersInLHCbAcceptance.h,v 1.4 2008-05-29 14:21:59 gcorti Exp $
#ifndef GENERATORS_DAUGHTERSINLHCBACCEPTANCE_H 
#define GENERATORS_DAUGHTERSINLHCBACCEPTANCE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Generators/IGenCutTool.h"
#include "GaudiKernel/Transform4DTypes.h"

/** @class DaughtersInLHCbAcceptance DaughtersInLHCbAcceptance.h 
 *  
 *  Tool to keep events with daughters from signal particles 
 *  in LHCb acceptance.
 *  Concrete implementation of IGenCutTool.
 * 
 *  @author Alex Shires
 *  @date   2011-03-02
 */
class DaughtersInLHCbAcceptance : public GaudiTool, virtual public IGenCutTool {
 public:
  /// Standard constructor
  DaughtersInLHCbAcceptance( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);
  
  virtual ~DaughtersInLHCbAcceptance( ); ///< Destructor

  /** Accept events with daughters in LHCb acceptance (defined by min and
   *  max angles, different values for charged and neutrals)
   *  Implements IGenCutTool::applyCut.
   */
  virtual bool applyCut( ParticleVector & theParticleVector , 
                         const HepMC::GenEvent * theEvent , 
                         const LHCb::GenCollision * theCollision ) const ;

 private:
  /** Study a particle a returns true when all stable daughters
   *  are in LHCb acceptance
   */
  bool passCuts( const HepMC::GenParticle * theSignal ) const ;  

  /** Momentum Cut function
   *
   */
  bool momentumCut( const HepMC::GenParticle *, double ) const ;  

  // Minimum value of angle around z-axis for charged daughters
  double m_chargedThetaMin ;

  // Maximum value of angle around z-axis for charged daughters
  double m_chargedThetaMax ;

  // Minimum value of angle around z-axis for neutral daughters
  double m_neutralThetaMin ;

  // Maximum value of angle around z-axis for neutral daughters
  double m_neutralThetaMax ;

    //min value of momentum
    double m_minMuonP;
    double m_minTrackP;
};
#endif // GENERATORS_DAUGHTERSINLHCBACCEPTANCE_H
