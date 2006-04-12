
//-----------------------------------------------------------------------------
/** @file RichPixelPositionMonitor.h
 *
 *  Header file for algorithm class : RichPixelPositionMonitor
 *
 *  CVS Log :-
 *  $Id: RichPixelPositionMonitor.h,v 1.3 2006-04-12 14:30:25 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RichPixelPositionMonitor_H
#define RICHRECMONITOR_RichPixelPositionMonitor_H 1

// STD
#include <sstream>

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// RichKernel
#include "RichKernel/RichMap.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// RichDet
#include "RichDet/RichDAQDefinitions.h"

// Interfaces
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichKernel/IRichMCTruthTool.h"
#include "RichKernel/IRichSmartIDTool.h"

// boost
#include "boost/lexical_cast.hpp"

// RichDet
#include "RichDet/DeRichSystem.h"

// Event
#include "Event/MCRichHit.h"
#include "Event/MCRichOpticalPhoton.h"

//-----------------------------------------------------------------------------
/** @class RichPixelPositionMonitor RichPixelPositionMonitor.h
 *
 *  Monitor class for general position and geometry of RichRecPixels
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

class RichPixelPositionMonitor : public RichRecHistoAlgBase
{

public:

  /// Standard constructor
  RichPixelPositionMonitor( const std::string& name,
                            ISvcLocator* pSvcLocator );

  virtual ~RichPixelPositionMonitor( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // data

  /// Pointer to RichRecMCTruthTool interface
  const IRichRecMCTruthTool* m_richRecMCTruth;

  /// Pointer to MC truth tool
  const IRichMCTruthTool * m_mcTool;
  
  /// Pointer to RichSmartID tool
  const IRichSmartIDTool * m_idTool;

  /// Pointer to RICH system detector element
  const DeRichSystem * m_richSys;

  // variables for average PD positions
  Rich::Map<LHCb::RichSmartID::KeyType, double> m_xHits;
  Rich::Map<LHCb::RichSmartID::KeyType, double> m_yHits;
  Rich::Map<LHCb::RichSmartID::KeyType, double> m_zHits;
  Rich::Map<LHCb::RichSmartID::KeyType, int> m_hitCount;

};

#endif // RICHRECMONITOR_RichPixelPositionMonitor_H
