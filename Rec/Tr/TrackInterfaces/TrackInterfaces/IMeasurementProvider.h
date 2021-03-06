// $Id: IMeasurementProvider.h,v 1.10 2007-11-30 14:19:45 wouter Exp $
#ifndef TRACKINTERFACES_IMEASUREMENTPROVIDER_H 
#define TRACKINTERFACES_IMEASUREMENTPROVIDER_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include <vector>

// Forward declarations
namespace LHCb {
 class LHCbID;
 class Track;
 class Measurement;
 class StateVector;
 class ZTrajectory ; 
}

static const InterfaceID IID_IMeasurementProvider ( "IMeasurementProvider", 1, 0 );

/** @class IMeasurementProvider IMeasurementProvider.h TrackInterfaces/IMeasurementProvider.h
 *  
 *  Interface for the measurement provider tool
 *
 *  @author Jose Hernando
 *  @author Eduardo Rodrigues
 *  @date   2005-06-28
 */
class IMeasurementProvider : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IMeasurementProvider; }

  virtual StatusCode initialize() = 0;

  /** Load (=create) all the Measurements from the list of LHCbIDs
   *  on the input Track
   */
  virtual StatusCode load( LHCb::Track& track ) const = 0;

  /** Construct a Measurement of the type of the input LHCbID
   *  Note: this method is not for general use. A user should preferably call
   *  the "load( Track& track )" method to load=create "in one go" all the
   *  Measurements from the list of LHCbIDs on the Track.
   *  This method is in fact called internally by "load( Track& track )".
   *  @return Pointer the the Measurement created
   *  @param  id:  input LHCbID
   *  @param  localY: creates y trajectory for muon, if true
   */
  virtual LHCb::Measurement* measurement( const LHCb::LHCbID& id,
					  bool localY = false ) const = 0;

  /** Construct a measurement with a statevector. This takes care that
      things like errors depending on track angle are correctly
      set. */

  /** Construct a measurement with a reference trajectory. This takes care that
      things like errors depending on track angle are correctly set. */
  virtual LHCb::Measurement* measurement( const LHCb::LHCbID& id, 
                                          const LHCb::ZTrajectory& reftraj,
                                          bool localY = false ) const = 0;
  
  /** return the 'nominal' z-coordinate for the hit **/
  virtual double nominalZ(const LHCb::LHCbID& id) const = 0 ;
  
  /** create measurements for a set of LHCbIDs **/
  virtual void addToMeasurements( const std::vector<LHCb::LHCbID>&,
                                  std::vector<LHCb::Measurement*>&,
                                  const LHCb::ZTrajectory& ) const = 0 ;
};
#endif // TRACKINTERFACES_IMEASUREMENTPROVIDER_H
