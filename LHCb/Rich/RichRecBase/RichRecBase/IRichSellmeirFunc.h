
//-----------------------------------------------------------------------------
/** @file IRichSellmeirFunc.h
 *
 *  Header file for RICH reconstruction tool interface : IRichSellmeirFunc
 *
 *  CVS Log :-
 *  $Id: IRichSellmeirFunc.h,v 1.6 2006-01-23 14:08:55 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_IRICHSELLMEIRFUNC_H
#define RICHRECTOOLS_IRICHSELLMEIRFUNC_H 1

// Event model
namespace LHCb
{
  class RichRecSegment;
}

/// Static Interface Identification
static const InterfaceID IID_IRichSellmeirFunc( "IRichSellmeirFunc", 1, 0 );

//-----------------------------------------------------------------------------
/** @class IRichSellmeirFunc IRichSellmeirFunc.h
 *
 *  Interface for tools which calculate quantities using the Sellmeir
 *  function and related parameters.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

class IRichSellmeirFunc : public virtual IAlgTool
{

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichSellmeirFunc; }

  /** Computes the number of photons emitted in the given energy range for a
   *  given RichRecSegment under a certain mass hypothesis
   *
   *  @param segment  The RichRecSegment to use
   *  @param id       The assumed mass hypothesis
   *  @param botEn    The lower bound to the energy range
   *  @param topEn    The upper bound to the energy range
   *
   *  @return The number of emitted photons in the given energy range
   */
  virtual double photonsInEnergyRange( LHCb::RichRecSegment * segment,
                                       const Rich::ParticleIDType id,
                                       const double botEn,
                                       const double topEn ) const = 0;

};

#endif // RICHRECTOOLS_IRICHSELLMEIRFUNC_H
