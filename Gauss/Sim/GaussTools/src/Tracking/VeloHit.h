// $Id: VeloHit.h,v 1.1.1.1 2002-08-26 10:36:12 witoldp Exp $
#ifndef TRACKING_VELOHIT_H 
#define TRACKING_VELOHIT_H 1

// Include files
#include "TrackerHit.h"

/** @class VeloHit VeloHit.h Tracking/VeloHit.h
 *  
 *
 *  @author Witold Pokorski
 *  @date   2002-08-16
 */
class VeloHit : public TrackerHit {
public:
  /// Standard constructor
  VeloHit();
  
  ~VeloHit( ); ///< Destructor

  /// Retrieve Sensor Number
  G4long Sensor() const; 

  /// Update Sensor Number
  void SetSensor(G4long value);


protected:

private:

  G4long m_sensor; ///< Sensor Number

};

inline G4long VeloHit::Sensor() const 
{
  return m_sensor;
}

inline void VeloHit::SetSensor(G4long value)
{
  m_sensor = value; 
}

typedef G4THitsCollection<VeloHit> VeloHitsCollection;


#endif // TRACKING_VELOHIT_H
