// $Id: IParticleStuffer.h,v 1.2 2002-05-15 23:07:13 gcorti Exp $
#ifndef DAVINCITOOLS_IPARTICLESTUFFER_H 
#define DAVINCITOOLS_IPARTICLESTUFFER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
// from Event
#include "Event/Particle.h"

static const InterfaceID IID_IParticleStuffer("IParticleStuffer", 1 , 0); 

/** @class IParticleStuffer IParticleStuffer.h 
 *  Interface Class for Filling a Particle.
 *  Fill a particle given a vertex and a particle ID. 
 *  @author Paul colrain
 *  @date   14/03/2002
 */

class IParticleStuffer : virtual public IAlgTool {

public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() {
    return IID_IParticleStuffer;
  }

  /// Fill Composite Particle from Vertex
  virtual StatusCode fillParticle( const Vertex&, Particle&, 
                                   const ParticleID& ) = 0;

};
#endif // DAVINCITOOLS_IPARTICLESTUFFER_H


