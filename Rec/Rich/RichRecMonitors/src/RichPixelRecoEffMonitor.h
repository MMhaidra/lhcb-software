
//-----------------------------------------------------------------------------
/** @file RichPixelRecoEffMonitor.h
 *
 *  Header file for algorithm class : Rich::Rec::MC::PixelRecoEffMonitor
 *
 *  CVS Log :-
 *  $Id: RichPixelRecoEffMonitor.h,v 1.3 2007-02-02 10:07:12 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RichPixelRecoEffMonitor_H
#define RICHRECMONITOR_RichPixelRecoEffMonitor_H 1

// STD
#include <sstream>

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// RichKernel
//#include "RichKernel/RichMap.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// Event
#include "Event/MCRichDigitSummary.h"

// RichDet
#include "RichDet/DeRichSystem.h"

// Interfaces
#include "RichKernel/IRichMCTruthTool.h"
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  /** @namespace Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @namespace MC
     *
     *  General namespace for RICH MC related software
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   05/12/2006
     */
    //-----------------------------------------------------------------------------
    namespace MC
    {

      //-----------------------------------------------------------------------------
      /** @class PixelRecoEffMonitor RichPixelRecoEffMonitor.h
       *
       *  Monitor class for the reconstruction efficiency for RichRecPixels
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   05/04/2002
       */
      //-----------------------------------------------------------------------------

      class PixelRecoEffMonitor : public Rich::Rec::HistoAlgBase
      {

      public:

        /// Standard constructor
        PixelRecoEffMonitor( const std::string& name,
                             ISvcLocator* pSvcLocator );

        virtual ~PixelRecoEffMonitor( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution
        virtual StatusCode finalize  ();    // Algorithm finalization

      private: // data

        /// Pointer to RichRecMCTruthTool interface
        const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth;

        /// Pointer to RichMCTruthTool
        const Rich::MC::IMCTruthTool * m_truth;

        /// Raw Buffer Decoding tool
        const Rich::DAQ::IRawBufferToSmartIDsTool * m_decoder;

        /// Pointer to RICH system detector element
        const DeRichSystem * m_richSys;

      };

    }
  }
}

#endif // RICHRECMONITOR_RichPixelRecoEffMonitor_H
