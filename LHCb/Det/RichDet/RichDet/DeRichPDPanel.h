// $Id: DeRichPDPanel.h,v 1.3 2003-10-22 10:48:28 papanest Exp $

#ifndef DERICHPDPANEL_H
#define DERICHPDPANEL_H 1


// Include files
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Plane3D.h"

#include "DetDesc/DetectorElement.h"
#include "DetDesc/ISolid.h"

#include "RichKernel/RichSmartID.h"

/** @class DeRichPDPanel DeRichPDPanel.h
 *
 * This is the definition of the Rich PDPanel detector class. It is used
 * as an interface for all detector panels
 *
 * @author Antonis Papanestis
 */
class DeRichPDPanel: public DetectorElement {

public:

  enum traceMode {
    loose = 0,
    tight
  };
  
  /**
   * This is where most of the geometry is read
   * @return StatusCode
   */
  virtual StatusCode initialize() = 0;


  /**
   * Returns a RichSmartID for a given point in global coordinates.
   * @return StatusCode
   */
  virtual StatusCode smartID( const HepPoint3D& globalPoint,
                              RichSmartID& id ) = 0;

  /**
   * Returns the detection point given a smartID. The detection is
   * on the inside of the PD window, on the photocathode.
   * @return StatusCode
   */
  virtual StatusCode detectionPoint( const RichSmartID& smartID,
                                     HepPoint3D& windowHitGlobal ) = 0;
  /**
   * Returns the intersection point with an PD window given a vector
   * and a point.
   * @return StatusCode
   */

  virtual StatusCode PDWindowPoint( const HepVector3D& vGlobal,
                                    const HepPoint3D& pGlobal,
                                    HepPoint3D& windowPointGlobal,
                                    RichSmartID& smartID ) = 0;
  /**
   * Returns the intersection point with the detector plane given a vector
   * and a point. If mode is tight, returns true only if point is within
   * the detector coverage.
   * @return bool
   */
  virtual bool detPlanePoint( const HepPoint3D& pGlobal,
                              const HepVector3D& vGlobal,
                              HepPoint3D& hitPosition,
                              traceMode mode = loose);

  /**
   * Returns the detection plane of the PD panel, defined at the top of the
   * PDs (a plane resting on the PDs touching the window).
   * @return HepPlane3D
   */
  virtual HepPlane3D detectionPlane() const = 0;

  /**
   * Returns a list with all the available readout channels, in form of
   * RichSmartIDs.
   * @return StatusCode
   */
  virtual StatusCode readoutChannelList( std::vector<RichSmartID>&
                                         readoutChannels ) = 0;


protected:

  /// detection plane
  HepPlane3D m_detectionPlane;
  HepPlane3D m_localPlane;
  HepVector3D m_localPlaneNormal;

  double m_detPlaneHorizEdge;
  double m_detPlaneVertEdge;

  HepTransform3D m_vectorTransf;

};

#endif    // DERICHPDPANEL_H
