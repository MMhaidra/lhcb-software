// $Id: DeRich2HPDPanel.cpp,v 1.8 2003-10-22 10:48:28 papanest Exp $
#define DERICH2HPDPANEL_CPP

// Include files
#include "RichDet/DeRich2HPDPanel.h"


// Gaudi
#include "GaudiKernel/MsgStream.h"

#include "Kernel/CLHEPStreams.h"

// CLHEP files
#include "CLHEP/Units/SystemOfUnits.h"

/// Detector description classes
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"
#include "DetDesc/SolidTubs.h"
#include "DetDesc/SolidSphere.h"

#include "Kernel/CLHEPStreams.h"
//----------------------------------------------------------------------------
//
// Implementation of class :  DeRich2HPDPanel
//
//----------------------------------------------------------------------------

const CLID& CLID_DeRich2HPDPanel = 12012;  // User defined

// Standard Constructor
DeRich2HPDPanel::DeRich2HPDPanel()
{}

// Standard Destructor
DeRich2HPDPanel::~DeRich2HPDPanel() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRich2HPDPanel::classID() {
  return CLID_DeRich2HPDPanel;
}


StatusCode DeRich2HPDPanel::initialize() {

  StatusCode sc = StatusCode::FAILURE;

  MsgStream log(msgSvc(), "DeRich2HPDPanel" );
  log << MSG::VERBOSE <<"Starting initialisation of DeRich2HPDPanel" << endreq;
  this->printOut(log);

  m_pixelSize = 0.5*mm;

  HepPoint3D zero(0.0, 0.0, 0.0);

  m_HPDRows = userParameterAsInt("PDRows");
  m_HPDColumns = userParameterAsInt("PDColumns");
  m_PDMax = m_HPDColumns * m_HPDRows;

  // get the first HPD and follow down to the silicon block
  const IPVolume* pvHPDMaster0 = geometry()->lvolume()->pvolume(0);
  const IPVolume* pvHPDSMaster0 = pvHPDMaster0->lvolume()->pvolume(0);
  const IPVolume* pvSilicon0 = pvHPDSMaster0->lvolume()->pvolume(4);

  const ISolid* siliconSolid = pvSilicon0->lvolume()->solid();
  const SolidBox* siliconBox = dynamic_cast<const SolidBox*>(siliconSolid);

  // assume same size for all silicon detectors
  m_siliconHalfLengthX = siliconBox->xHalfLength();
  m_siliconHalfLengthY = siliconBox->yHalfLength();

  // HPD #0 coordinates
  HepPoint3D HPD0Centre = pvHPDMaster0->toMother(zero);
  log << MSG::DEBUG << "Centre of HPDPanel:" << geometry()->toGlobal(zero)
      << " Centre of HPD#0:" << geometry()->toGlobal(HPD0Centre) << std::endl;

  //get the next HPD. For Rich1 same row, for Rich2 same column
  const IPVolume* pvHPDMaster1 = geometry()->lvolume()->pvolume(1);
  HepPoint3D HPD1Centre = pvHPDMaster1->toMother(zero);
  m_rowPitch = HPD1Centre.y() - HPD0Centre.y();

  //get the HPD at next row for Rich1, next column for Rich2, NS == next set
  HepPoint3D HPDNSCentre;
  const IPVolume* pvHPDMasterNS = geometry()->lvolume()->pvolume(m_HPDRows);
  HPDNSCentre = pvHPDMasterNS->toMother(zero);
  m_columnPitch = HPDNSCentre.x() - HPD0Centre.x();

  double halfColumnPitch = 0.5 * m_columnPitch;
  double halfRowPitch = 0.5 * m_rowPitch;

  log << MSG::DEBUG <<"m_columnPitch:"<< m_columnPitch << " m_rowPitch:"
      << m_rowPitch << endreq;

  // get the Horizontal Edge of the HPD cover area. When the HPD centre
  // coordinate is positive the pitch is negative and vise versa
  m_panelHorizEdge = HPD0Centre.x() - halfColumnPitch;
  m_fabs_panelHorizEdge = fabs(m_panelHorizEdge);

  m_detPlaneHorizEdge = m_fabs_panelHorizEdge;
  
  //get the Vertical Edge for the two types of columns
  //numbers are 0 to 8
  m_panelVerticalEdgeEven = HPD0Centre.y() - halfRowPitch;
  m_panelVerticalEdgeOdd = HPDNSCentre.y() - halfRowPitch;

  m_panelVerticalEdge = fabs(m_panelVerticalEdgeOdd);
  m_detPlaneVertEdge = fabs(m_panelVerticalEdgeEven);
  
  if (m_detPlaneVertEdge < m_panelVerticalEdge) {
    m_detPlaneVertEdge = m_panelVerticalEdge;
    m_panelVerticalEdge = fabs(m_panelVerticalEdgeEven);
  }
  
  log << MSG::DEBUG <<"m_panelHorizEdge:"<< m_panelHorizEdge
      << " m_panelVerticalEdgeEven:" << m_panelVerticalEdgeEven
      << " m_panelVerticalEdgeOdd:" << m_panelVerticalEdgeOdd << endreq;

  // get the pv and the solid for the HPD quartz window
  const IPVolume* pvWindow0 = pvHPDSMaster0->lvolume()->pvolume(2);
  const ISolid* windowSolid0 = pvWindow0->lvolume()->solid();
  // get the inside radius of the window
  ISolid::Ticks windowTicks;
  unsigned int windowTicksSize = windowSolid0->
    intersectionTicks(HepPoint3D(0.0, 0.0, 0.0),HepVector3D(0.0, 0.0, 1.0),
                      windowTicks);
  if (windowTicksSize != 2) {
    log << MSG::ERROR << "Problem getting window radius" << endreq;
    return sc;
  }
  m_winR = windowTicks[0];
  m_winRsq = m_winR*m_winR;

  // get the coordinate of the centre of the HPD quarz window
  HepPoint3D HPDTop1(0.0, 0.0, m_winR);
  // convert this to HPDS master  coordinates
  HepPoint3D HPDTop2 = pvWindow0->toMother(HPDTop1);
  // and to silicon
  m_HPDTop = pvSilicon0->toLocal(HPDTop2);


  // find the top of 3 HPDs to create a detection plane.  We already have the
  // first in HPDSMaster coordinates.
  // now to HPD coordinates
  HepPoint3D pointAInHPD = pvHPDSMaster0->toMother(HPDTop2);
  HepPoint3D pointAInPanel = pvHPDMaster0->toMother(pointAInHPD);
  HepPoint3D pointA = geometry()->toGlobal(pointAInPanel);

  // for second point go to HPD at the end of the column.
  //The relative position inside the HPD is the same
  const IPVolume* pvHPDMasterB = geometry()->lvolume()->pvolume(m_HPDRows-1);
  HepPoint3D pointBInPanel = pvHPDMasterB->toMother(pointAInHPD);
  HepPoint3D pointB = geometry()->toGlobal(pointBInPanel);

  // now point 3 on the other end.
  int HPDForC = m_HPDRows*m_HPDColumns - m_HPDColumns/2;
  const IPVolume* pvHPDMasterC = geometry()->lvolume()->pvolume(HPDForC);
  HepPoint3D pointCInPanel = pvHPDMasterC->toMother(pointAInHPD);
  HepPoint3D pointC = geometry()->toGlobal(pointCInPanel);

  m_detectionPlane = HepPlane3D(pointA, pointB, pointC);
  m_detectionPlane.normalize();
  m_localPlane = HepPlane3D(pointAInPanel, pointBInPanel, pointCInPanel);
  m_localPlaneNormal = m_localPlane.normal();

  // Cache information for PDWindowPoint method
  m_vectorTransf = geometry()->matrix();
  m_HPDPanelSolid = geometry()->lvolume()->solid();
  for ( int HPD = 0; HPD < m_HPDColumns*m_HPDRows; ++HPD ) {
    m_pvHPDMasters.push_back( geometry()->lvolume()->pvolume(HPD) );
    m_pvHPDSMasters.push_back( m_pvHPDMasters[HPD]->lvolume()->pvolume(0) );
    m_pvWindows.push_back( m_pvHPDSMasters[HPD]->lvolume()->pvolume(2) );
    m_windowSolids.push_back( m_pvWindows[HPD]->lvolume()->solid() );
    m_vectorTransfHPD2s.push_back( m_pvHPDMasters[HPD]->matrix() );
  }

  log << MSG::DEBUG <<"Finished initialisation for DeRich2HPDPanel"<< endreq;

  return StatusCode::SUCCESS;
}

// ===========================================================================

StatusCode DeRich2HPDPanel::smartID (const HepPoint3D& globalPoint,
                                     RichSmartID& id) {

  if (!geometry()->isInside(globalPoint)) {
    MsgStream log(msgSvc(), "DeRich2HPDPanel" );
    log << MSG::ERROR << "Point outside HPD Panel" <<  endreq;
    id = RichSmartID(0);
    return StatusCode::FAILURE;
  }

  HepPoint3D inPanel = geometry()->toLocal(globalPoint);

  // HPD row and column, pixel row and column as well as HPD panel number
  // and rich id starts at 1 and not 0

  // find the correct HPD inside the panel
  unsigned int HPDColumn, HPDRow;
  int HPDNumber;

  HPDColumn = (unsigned int) floor((inPanel.x() - m_panelHorizEdge) /
                                   m_columnPitch);

  if (HPDColumn >= (unsigned int) m_HPDColumns) {
    MsgStream log(msgSvc(), "DeRich2HPDPanel" );
    log << MSG::ERROR << "Rich2 HPD column number too high" <<  endreq;
    id = RichSmartID(0);
    return StatusCode::FAILURE;
  }

  if (0 == HPDColumn%2) {
    HPDRow = (unsigned int) floor((inPanel.y() - m_panelVerticalEdgeEven)/
                                  m_rowPitch);
  } else {
    HPDRow = (unsigned int) floor((inPanel.y() - m_panelVerticalEdgeOdd)/
                                  m_rowPitch);
  }
  //std::cout << inPanel.y() <<"  " << m_panelVerticalEdgeEven << "  "
  //     << m_panelVerticalEdgeOdd << std::endl;

  if (HPDRow >= (unsigned int) m_HPDRows) {
    MsgStream log(msgSvc(), "DeRich2HPDPanel" );
    log << MSG::ERROR << "HPD row number too high" <<  endreq;
    id = RichSmartID(0);
    return StatusCode::FAILURE;
  }
  HPDNumber = HPDColumn * m_HPDRows + HPDRow;

  const IPVolume* pvHPDMaster = geometry()->lvolume()->pvolume(HPDNumber);
  const IPVolume* pvHPDSMaster = pvHPDMaster->lvolume()->pvolume(0);
  const IPVolume* pvSilicon = pvHPDSMaster->lvolume()->pvolume(4);
  //  const ISolid* siliconSolid = pvSilicon->lvolume()->solid();

  HepPoint3D inHPDMaster = pvHPDMaster->toLocal(inPanel);
  HepPoint3D inHPDSMaster = pvHPDSMaster->toLocal(inHPDMaster);
  HepPoint3D inSilicon = pvSilicon->toLocal(inHPDSMaster);

  //std::cout << inSilicon << std::endl;

  //  if (!siliconSolid->isInside(inSilicon)) {
  //    log << MSG::ERROR << "The point is outside the silicon box "
  //        << pvHPDMaster->name() <<  endreq;
  //    id = RichSmartID(0);
  //    return StatusCode::FAILURE;
  //  }

  if ( (fabs(inSilicon.x()) > m_siliconHalfLengthX) ||
       (fabs(inSilicon.y()) > m_siliconHalfLengthY)    ) {
    MsgStream log(msgSvc(), "DeRich2HPDPanel" );
    log << MSG::ERROR << "The point is outside the silicon box "
        << pvHPDMaster->name() <<  endreq;
    std::cout << inSilicon << std::endl;

    id = RichSmartID(0);
    return StatusCode::FAILURE;
  }

  unsigned int pixelColumn = (unsigned int) ((m_siliconHalfLengthX +
                                              inSilicon.x()) / m_pixelSize);
  unsigned int pixelRow    = (unsigned int) ((m_siliconHalfLengthY +
                                              inSilicon.y()) / m_pixelSize);

  //std::cout << pixelColumn << "  " <<pixelRow << std::endl;

  id = RichSmartID(0,0, HPDRow, HPDColumn, pixelRow, pixelColumn);
  return StatusCode::SUCCESS;
}

//============================================================================

StatusCode DeRich2HPDPanel::detectionPoint (const RichSmartID& smartID,
                                            HepPoint3D& windowHitGlobal)
{

  // HPD row and column, pixel row and column as well as HPD panel number
  // and rich id starts at 1 and not 0

  int HPDNumber = smartID.PDCol()*m_HPDRows + smartID.PDRow();

  // find the correct HPD and silicon block inside it
  const IPVolume* pvHPDMaster = geometry()->lvolume()->pvolume(HPDNumber);
  const IPVolume* pvHPDSMaster = pvHPDMaster->lvolume()->pvolume(0);
  //  const IPVolume* pvSilicon = pvHPDSMaster->lvolume()->pvolume(4);

  const IPVolume* pvWindow = pvHPDSMaster->lvolume()->pvolume(2);
  //  const ISolid* windowSolid = pvWindow->lvolume()->solid();

  // convert pixel number to silicon coordinates
  double inSiliconX = smartID.pixelCol() * m_pixelSize + m_pixelSize/2.0 -
    m_siliconHalfLengthX;
  double inSiliconY = smartID.pixelRow() * m_pixelSize + m_pixelSize/2.0 -
    m_siliconHalfLengthY;

  double inWindowX = -5.0 * inSiliconX;
  double inWindowY = -5.0 * inSiliconY;
  double inWindowZ = sqrt(m_winRsq-inWindowX*inWindowX-inWindowY*inWindowY);
  HepPoint3D windowHit(inWindowX, inWindowY, inWindowZ);
  //  std::cout << windowHit << std::endl;

  HepPoint3D windowHitInHPDS = pvWindow->toMother(windowHit);
  HepPoint3D windowHitInHPD = pvHPDSMaster->toMother(windowHitInHPDS);
  HepPoint3D windowHitInPanel = pvHPDMaster->toMother(windowHitInHPD);
  windowHitGlobal = geometry()->toGlobal(windowHitInPanel);

  return StatusCode::SUCCESS;

}

//============================================================================

StatusCode DeRich2HPDPanel::PDWindowPoint( const HepVector3D& vGlobal,
                                           const HepPoint3D& pGlobal,
                                           HepPoint3D& windowPointGlobal,
                                           RichSmartID& smartID ) {

  // transform point and vector to the HPDPanel coordsystem.
  HepPoint3D pLocal = geometry()->toLocal(pGlobal);
  HepVector3D vLocal = vGlobal;
  vLocal.transform(m_vectorTransf);

  unsigned int noTicks;
  ISolid::Ticks HPDPanelTicks;
  //  noTicks =m_HPDPanelSolid->intersectionTicks(pLocal,vLocal,HPDPanelTicks);
  //  if ( 0 == noTicks ) return StatusCode::FAILURE;
  //  HepPoint3D panelIntersection = pLocal + HPDPanelTicks[0]*vLocal;

  double scalar1 = vLocal*m_localPlaneNormal;
  double distance = 0.0;

  if ( scalar1 == 0.0 ) return StatusCode::FAILURE;

  distance = -(m_localPlane.d() + pLocal*m_localPlaneNormal) / scalar1;
  HepPoint3D panelIntersection( pLocal + distance*vLocal );

  const IPVolume* pvHPDMaster = 0;
  const IPVolume* pvHPDSMaster = 0;
  const IPVolume* pvWindow = 0;
  const ISolid* windowSolid;

  HepPoint3D pInWindow;
  HepVector3D vInHPDMaster;

  ISolid::Ticks HPDWindowTicks;

  int HPDColumn(-1), HPDRow(-1), HPDNumber(-1);
  bool HPDFound(false);

  if (fabs(panelIntersection.x()) <= m_fabs_panelHorizEdge) {
       //&& (fabs(panelIntersection.y()) <= m_panelVerticalEdge))

    HPDColumn = (int) floor((panelIntersection.x()- m_panelHorizEdge)
                            /m_columnPitch);

    if (0 == HPDColumn%2) {
      HPDRow = (int) floor((panelIntersection.y()-m_panelVerticalEdgeEven)
                           /m_rowPitch);
    } else {
      HPDRow = (int) floor((panelIntersection.y()-m_panelVerticalEdgeOdd)
                           /m_rowPitch);
    }
    if (HPDRow >= 0 && HPDRow < m_HPDRows) {

      HPDNumber = HPDColumn*m_HPDRows + HPDRow;

      // find the correct HPD and quartz window inside it
      pvHPDMaster = geometry()->lvolume()->pvolume(HPDNumber);
      // just in case
      if ( !pvHPDMaster ) {
        MsgStream log(msgSvc(), "DeRich2HPDPanel" );
        log << MSG::ERROR << "Inappropriate HPDNumber:" << HPDNumber
            << " from HPDRow:" << HPDRow << " and HPDColumn:" << HPDColumn
            << " please notify Antonis" << endreq
            << " x:" << panelIntersection.x()
            << " y:" << panelIntersection.y() << " edge1:"
            << m_panelHorizEdge <<" edge2:"<<m_panelVerticalEdgeEven << endreq;
        return StatusCode::FAILURE;
      }
      
      pvHPDSMaster = pvHPDMaster->lvolume()->pvolume(0);
      pvWindow = pvHPDSMaster->lvolume()->pvolume(2);
      windowSolid = pvWindow->lvolume()->solid();

      // convert point to local coordinate systems
      pInWindow = pvWindow->toLocal(pvHPDSMaster->toLocal(pvHPDMaster->
                                                          toLocal(pLocal)));
      // convert local vector assuming that only the HPD can be rotated
      const HepTransform3D vectorTransfHPD = pvHPDMaster->matrix();
      vInHPDMaster = vLocal;
      vInHPDMaster.transform(vectorTransfHPD);

      noTicks = windowSolid->intersectionTicks(pInWindow, vInHPDMaster,
                                               HPDWindowTicks );
      if ( 0 != noTicks ) HPDFound = true;
    }
    
  }

  if ( !HPDFound ) {
    // Not in central HPD : Try all others

    for ( int HPD = 0; HPD < m_PDMax; ++HPD ) {

      // convert point to local coordinate systems
      pInWindow = m_pvWindows[HPD]->toLocal(m_pvHPDSMasters[HPD]->
                                            toLocal(m_pvHPDMasters[HPD]->
                                                    toLocal(pLocal)));

      // convert local vector assuming that only the HPD can be rotated
      vInHPDMaster = vLocal;
      vInHPDMaster.transform( m_vectorTransfHPD2s[HPD] );

      noTicks = m_windowSolids[HPD]->intersectionTicks( pInWindow,
                                                        vInHPDMaster,
                                                        HPDWindowTicks );
      if ( 2 == noTicks ) {
        //        std::cout << "Found 2nd " << HPDRow << "  " << HPDColumn 
        //          << panelIntersection;
        
        HPDFound = true;
        HPDNumber = HPD;
        pvHPDMaster = m_pvHPDMasters[HPD];
        pvHPDSMaster = m_pvHPDSMasters[HPD];
        pvWindow = m_pvWindows[HPD];
        HPDRow = HPDNumber%m_HPDRows;
        HPDColumn = HPDNumber/m_HPDRows;
        //std::cout << HPDRow << "  " << HPDColumn << std::endl;
        //HepPoint3D windowPoint = pInWindow + HPDWindowTicks[1]*vInHPDMaster;
        //std::cout << windowPoint << std::endl;
        break;
      }

    }

  }

  if ( !HPDFound ) return StatusCode::FAILURE;

  HepPoint3D windowPoint = pInWindow + HPDWindowTicks[1]*vInHPDMaster;
  HepPoint3D windowPointInHPDS = pvWindow->toMother(windowPoint);
  HepPoint3D windowPointInHPD = pvHPDSMaster->toMother(windowPointInHPDS);
  HepPoint3D windowPointInPanel = pvHPDMaster->toMother(windowPointInHPD);
  windowPointGlobal = geometry()->toGlobal(windowPointInPanel);

  smartID.setPDRow( HPDRow );
  smartID.setPDCol( HPDColumn );
  // For the moment do not bother with pixel info
  smartID.setPixelRow( 0 );
  smartID.setPixelCol( 0 );

  return StatusCode::SUCCESS;

}

