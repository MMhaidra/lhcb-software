// $Id: LongTrackReferenceCreator.h,v 1.2 2006-06-14 19:53:58 jvantilb Exp $
#ifndef _LongTrackReferenceCreator_H
#define _LongTrackReferenceCreator_H

/** @class LongTrackReferenceCreator TrackTools/LongTrackReferenceCreator.h
 *
 * Implementation of TrackCaloMatch tool
 * see interface header for description
 *
 *  @author M.Needham
 *  @date   30/12/2005
 */

#include "GaudiAlg/GaudiTool.h"

#include "TrackInterfaces/ITrackManipulator.h"

#include <string>

namespace LHCb{
  class Track;
  class State;
  class Measurement;
}

class ITrackExtrapolator;
class IMagneticFieldSvc;
class ITrajPoca;

class LongTrackReferenceCreator: public GaudiTool,
                                 virtual public ITrackManipulator  {

public:

  /** constructer */
  LongTrackReferenceCreator(const std::string& type,
                            const std::string& name,
                            const IInterface* parent);  

  /** destructer */
  virtual ~LongTrackReferenceCreator();

  /** intialize */
  StatusCode initialize();

  /** add reference info to the track */  
  StatusCode execute(const LHCb::Track& aTrack) const;

private:

  void addReference(LHCb::Measurement* meas, LHCb::State& aState) const;

  ITrackExtrapolator* m_extrapolator;
  IMagneticFieldSvc* m_pIMF;          ///< Pointer to the magn. field service
  ITrajPoca*         m_poca;          ///< Pointer to the ITrajPoca interface

};

#endif
