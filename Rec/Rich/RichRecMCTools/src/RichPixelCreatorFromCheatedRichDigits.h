// $Id: RichPixelCreatorFromCheatedRichDigits.h,v 1.2 2004-02-02 14:24:40 jonesc Exp $
#ifndef RICHRECTOOLS_RICHPIXELCREATORFROMCHEATEDRICHDIGITS_H
#define RICHRECTOOLS_RICHPIXELCREATORFROMCHEATEDRICHDIGITS_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichPixelCreator.h"
#include "RichDetTools/IRichSmartIDTool.h"
#include "RichRecBase/IRichRecMCTruthTool.h"

// Event
#include "Event/RichDigit.h"
#include "Event/MCRichDigit.h"
#include "Event/MCRichHit.h"

/** @class RichPixelCreatorFromCheatedRichDigits RichPixelCreatorFromCheatedRichDigits.h
 *
 *  Tool for the creation and book-keeping of RichRecPixel objects.
 *  Uses RichDigits from the OO digitisation but then refers to the
 *  MCRichHits objects to use the True MC hit positions
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/09/2003
 */

class RichPixelCreatorFromCheatedRichDigits : public RichRecToolBase,
                                              virtual public IRichPixelCreator,
                                              virtual public IIncidentListener {

public:

  /// Standard constructor
  RichPixelCreatorFromCheatedRichDigits( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent );

  /// Destructor
  virtual ~RichPixelCreatorFromCheatedRichDigits(){}

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Implement the handle method for the Incident service.
  /// This is used to inform the tool of software incidents.
  void handle( const Incident& incident );

  /// Returns a RichRecPixel object pointer for given ContainedObject.
  /// If if it not possible NULL is return.
  RichRecPixel * newPixel( const ContainedObject * obj ) const;

  /// Form all possible RichRecPixels from input RichDigits.
  /// The most efficient way to make all RichRecPixel objects in the event.
  StatusCode newPixels() const;

  /// Returns a pointer to the RichRecPixels
  RichRecPixels * richPixels() const;

private: // methods

  RichRecPixel * newPixelFromHit( const RichDigit * digit,
                                  const MCRichHit * hit ) const;

private: // data

  /// Pointer to RichRecPixels
  RichRecPixels * m_pixels;

  /// Pointer to RichSmartID tool
  IRichSmartIDTool * m_smartIDTool;

  /// MC Truth tool
  IRichRecMCTruthTool * m_mcTool;

  /// String containing input RichDigits location in TES
  std::string m_recoDigitsLocation;

  /// Location of RichRecPixels in TES
  std::string m_richRecPixelLocation;

  /// Flag to signify all pixels have been formed
  mutable bool m_allDone;

  /// Pointer to pixel map
  mutable std::map< RichSmartID::KeyType, RichRecPixel* > m_pixelExists;
  mutable std::map< RichSmartID::KeyType, bool > m_pixelDone;

};

#endif // RICHRECTOOLS_RICHPIXELCREATORFROMCHEATEDRICHDIGITS_H
