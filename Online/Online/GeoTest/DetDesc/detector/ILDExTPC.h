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


  struct TPCData : public Geometry::DetElement::Object {
    typedef Geometry::Ref_t Ref_t;
    Ref_t outerWall;
    Ref_t innerWall;
    Ref_t gas;
    double pressure;
  };

  struct ILDExTPC : public Geometry::DetElement {
    typedef TPCData Object;
    typedef Geometry::Ref_t Ref_t;
    ILDExTPC(const Ref_t& e) : Geometry::DetElement(e) {}
    ILDExTPC(const Geometry::LCDD& lcdd, const std::string& name, const std::string& type, int id);
    void setInnerWall(Ref_t obj);
    void setOuterWall(Ref_t obj);
    void setGasVolume(Ref_t obj);
    double getVolume() const;
    double getWeight() const;
  };
}
