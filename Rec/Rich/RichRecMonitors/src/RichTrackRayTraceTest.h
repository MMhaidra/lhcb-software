
//---------------------------------------------------------------------------
/** @file RichTrackRayTraceTest.h
 *
 *  Header file for algorithm class : Rich::Rec::TrackRayTraceTest
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//---------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RICHTRACKGEOMMONI_H
#define RICHRECMONITOR_RICHTRACKGEOMMONI_H 1

// STL
#include <sstream>

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// Event
#include "Event/RichRecStatus.h"
#include "Event/RichRecTrack.h"

// Interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichSmartIDTool.h"

namespace Rich
{
  namespace Rec
  {

    //---------------------------------------------------------------------------
    /** @class TrackRayTraceTest RichTrackRayTraceTest.h
     *
     *  Monitors the general geometrical details of the RichRecTrack
     *  and RichRecSegment objects.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   05/04/2002
     */
    //---------------------------------------------------------------------------

    class TrackRayTraceTest : public Rich::Rec::HistoAlgBase
    {

    public:

      /// Standard constructor
      TrackRayTraceTest( const std::string& name,
                         ISvcLocator* pSvcLocator );

      virtual ~TrackRayTraceTest( ); ///< Destructor

      virtual StatusCode initialize();    // Algorithm initialization
      virtual StatusCode execute   ();    // Algorithm execution

    private: // methods

      // test ray tracing
      void testRayTrace( const LHCb::RichTraceMode traceMode,
                         LHCb::RichRecSegment * segment ) const;

    private: // data

      /// Pointer RichDetector ray tracing tool
      const IRayTracing * m_rayTrace = nullptr;

      /// Pointer to RichSmartID tool
      const ISmartIDTool * m_idTool = nullptr;

      /// Track selector
      const ITrackSelector * m_trSelector = nullptr;

    };

  }
}

#endif // RICHRECMONITOR_RICHTRACKGEOMMONI_H
