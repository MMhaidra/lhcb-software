// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DetDesc/lcdd/Detector.h"

namespace DetDesc {

  struct TPCData;

  struct GearTPC : public Geometry::DetElement {
    typedef TPCData Object;
    GearTPC(const Geometry::Ref_t& e);
    double innerRadius() const;
    double outerRadius() const;
    double pressure() const;
  };
}
