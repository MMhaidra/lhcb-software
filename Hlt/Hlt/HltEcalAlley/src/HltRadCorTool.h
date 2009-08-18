// $Id: HltRadCorTool.h,v 1.1 2009-08-18 12:48:47 witekma Exp $
#ifndef TOOL_HLTRADCORTOOL_H 
#define TOOL_HLTRADCORTOOL_H 1

// Include files
#include "CaloInterfaces/IL0Calo2Calo.h"
#include "CaloDet/DeCalorimeter.h"
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "TrackInterfaces/ITracksFromTrack.h"            // Interface


/** @class HltRadCorTool HltRadCorTool.h tool/HltRadCorTool.h
 *   
 *
 *  @author Mariusz Witek
 *  @date   2009-05-25
 */
class HltRadCorTool : public GaudiTool, virtual public ITracksFromTrack {
public: 
  /// Standard constructor
  HltRadCorTool( const std::string& type, 
          const std::string& name,
          const IInterface* parent);

  virtual ~HltRadCorTool( ); ///< Destructor

  virtual StatusCode tracksFromTrack( const LHCb::Track& seed,
                                      std::vector<LHCb::Track*>& tracks );
  virtual StatusCode initialize();

protected:

private:

  void getRadiationFromClusters(const LHCb::Track& tra, double & enerad);

  const DeCalorimeter* m_detector;

  IL0Calo2Calo* m_tool;

};
#endif // TOOL_HLTRADCORTOOL_H
