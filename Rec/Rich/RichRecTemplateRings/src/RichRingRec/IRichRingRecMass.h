// $Id: IRichRingRecMass.h,v 1.2 2009-06-05 17:21:32 jonrob Exp $
#ifndef RICHRINGREC_IRICHRINGRECMASS_H
#define RICHRINGREC_IRICHRINGRECMASS_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

namespace Rich
{
  namespace Rec
  {
    namespace TemplateRings
    {

      static const InterfaceID IID_IRichRingRecMass ( "IRichRingRecMass", 1, 0 );

      /** @class IRichRingRecMass IRichRingRecMass.h RichRingRec/IRichRingRecMass.h
       *
       *
       *  @author Sajan EASO
       *  @date   2007-07-06
       */
      class IRichRingRecMass : virtual public IAlgTool {
      public:

        // Return the interface ID
        static const InterfaceID& interfaceID() { return IID_IRichRingRecMass; }

        virtual void RichReconMassForTrackSegment(int itk, int irad )=0;
        virtual StatusCode bestReconstructedMassForRichTrack (VI SegmentIndex,
                                                              int currentTrackIndex)=0;


      protected:

      private:

      };

    }
  }
}

#endif // RICHRINGREC_IRICHRINGRECMASS_H
