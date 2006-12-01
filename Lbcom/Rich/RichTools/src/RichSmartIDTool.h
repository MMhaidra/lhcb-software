
//-----------------------------------------------------------------------------
/** @file RichSmartIDTool.h
 *
 *  Header file for tool : RichSmartIDTool
 *
 *  CVS Log :-
 *  $Id: RichSmartIDTool.h,v 1.17 2006-12-01 13:13:13 cattanem Exp $
 *
 *  @author Antonis Papanestis
 *  @date   2003-10-28
 */
//-----------------------------------------------------------------------------

#ifndef RICHTOOLS_RICHSMARTIDTOOL_H
#define RICHTOOLS_RICHSMARTIDTOOL_H 1

// Base class and interface
#include "RichKernel/RichToolBase.h"
#include "RichKernel/IRichSmartIDTool.h"

// from RichKernel
#include "RichKernel/BoostArray.h"
#include "RichKernel/RichSmartIDSorter.h"

// RichDet
#include "RichDet/DeRichHPDPanel.h"
#include "RichDet/DeRichSystem.h"

//-----------------------------------------------------------------------------
/** @class RichSmartIDTool RichSmartIDTool.h
 *
 *  A tool to preform the manipulation of RichSmartID channel identifiers
 *
 *  @author Antonis Papanestis
 *  @date   2003-10-28
 *
 *  @todo Move application of panel offset in RichSmartIDTool::globalToPDPanel
 *        into DeRichHPDPanel class
 *  @todo Implement return status code on all smartid <-> position and 
 *        position <-> position coversions, to allow failed conversions to
 *        be properly flagged
 */
//-----------------------------------------------------------------------------

class RichSmartIDTool : public RichToolBase,
                        virtual public IRichSmartIDTool
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichSmartIDTool( const std::string& type,
                   const std::string& name,
                   const IInterface* parent);

  virtual ~RichSmartIDTool( ); ///< Destructor

  // Initialization of the tool after creation
  virtual StatusCode initialize();

  // Finalization of the tool before deletion
  virtual StatusCode finalize  ();

public: // methods (and doxygen comments) inherited from interface

  // Converts a RichSmartID channel identification into a position in global LHCb coordinates.
  virtual Gaudi::XYZPoint globalPosition ( const LHCb::RichSmartID smartid ) const;

  // Converts an HPD RichSmartID identification into a position in global LHCb coordinates.
  virtual Gaudi::XYZPoint hpdPosition ( const LHCb::RichSmartID hpdid ) const;

  // Computes the global position coordinate for a given position in local
  virtual Gaudi::XYZPoint globalPosition ( const Gaudi::XYZPoint& localPoint,
                                           const Rich::DetectorType rich,
                                           const Rich::Side side ) const;

  // Converts a position in global coordinates to the corresponding RichSmartID
  virtual StatusCode smartID( const Gaudi::XYZPoint&  globalPoint,
                              LHCb::RichSmartID& smartid ) const;

  // Supplies a vector of all currently active and valid channels in the RICH detectors
  virtual const LHCb::RichSmartID::Vector& readoutChannelList( ) const;

  /** Converts a position in global coordinates to the local coordinate system.
   *  The panel Rich::Left has positive x and panel Rich::Right has negative
   *  x coordinates. Only the x and y coordinates are valid. z coordinate
   *  is set to 0.
   */
  virtual Gaudi::XYZPoint globalToPDPanel ( const Gaudi::XYZPoint& globalPoint ) const;

private:

  typedef boost::array<DeRichHPDPanel*, 2> HPDPanelsPerRich;
  boost::array<HPDPanelsPerRich, 2> m_photoDetPanels;

  /// List of active RichSmartIDs
  mutable LHCb::RichSmartID::Vector m_readoutChannels;

  /// RichSystem object
  DeRichSystem* m_richS;
  
};

#endif // RICHDETTOOLS_RICHSMARTIDTOOL_H
