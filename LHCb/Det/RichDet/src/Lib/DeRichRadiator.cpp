// $Id: DeRichRadiator.cpp,v 1.5 2003-12-03 17:46:48 papanest Exp $
#define DERICHRADIATOR_CPP

// Include files
#include "RichDet/DeRichRadiator.h"

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "Kernel/CLHEPStreams.h"

// CLHEP files
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Geometry/Vector3D.h"

/// Detector description classes
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBoolean.h"
#include "DetDesc/SolidTrd.h"


//----------------------------------------------------------------------------
//
// Implementation of class :  DeRichRadiator
//
//----------------------------------------------------------------------------

StatusCode DeRichRadiator::initialize() {

  MsgStream msg(msgSvc(), "DeRichRadiator" );
  msg << MSG::DEBUG <<"Starting initialisation for DeRichRadiator"<< endreq;

  std::string tempName = name();

  const std::string::size_type pos = tempName.find("Rich2");
  if( std::string::npos != pos ) {
    m_radiatorID = Rich::CF4;
    m_rich = Rich::Rich2;
  }
  else {
    const std::string::size_type pos = tempName.find("Aerogel");
    if( std::string::npos != pos ) {
      m_radiatorID = Rich::Aerogel;
      m_rich = Rich::Rich1;
    }
    else {
      const std::string::size_type pos = tempName.find("C4F10");
      if( std::string::npos != pos ) {
        m_radiatorID = Rich::C4F10;
        m_rich = Rich::Rich1;
      }
      else {
        msg << MSG::ERROR << "Cannot find radiator type" << endreq;
        return StatusCode::FAILURE;
      }
    }
  }

  msg << MSG::DEBUG <<"Finished initialisation for DeRichRadiator"<< endreq;

  return StatusCode::SUCCESS;
}
