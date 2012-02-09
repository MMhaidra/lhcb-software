// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DetDesc/detector/DetFactoryHelper.h"
#include "DetDesc/detector/CompactDetectors.h"

using namespace std;
using namespace DetDesc;
using namespace DetDesc::Geometry;

namespace DetDesc { namespace Geometry {
   
  template <> Ref_t DetElementFactory<DiskTracker>::create(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)  {
    xml_det_t  x_det     = e;
    Material   air       = lcdd.air();
    string     det_name  = x_det.nameStr();
    string     det_type  = x_det.typeStr();
    bool       reflect   = x_det.reflect();
    DetElement sdet      (lcdd,det_name,det_type,x_det.id());
    Volume     motherVol = lcdd.pickMotherVolume(sdet);
    int n = 0;

    for(xml_coll_t i(x_det,_X(layer)); i; ++i, ++n)  {
      xml_comp_t x_layer = i;
      string l_name = det_name+_toString(n,"_layer%d");
      double  zmin = x_layer.inner_z();
      double  rmin = x_layer.inner_r();
      double  rmax = x_layer.outer_r();
      double  z    = zmin, layerWidth = 0.;
      Tube    l_tub(lcdd,l_name,rmin,rmax,2.*z,2.*M_PI);
      Volume  l_vol(lcdd,l_name+"_volume",l_tub,air);
      int m = 0;

      l_vol.setVisAttributes(lcdd,x_layer.visStr());

      for(xml_coll_t j(x_layer,_X(slice)); j; ++j)  {
	double w = xml_comp_t(j).thickness();
	layerWidth += w;
      }
      for(xml_coll_t j(x_layer,_X(slice)); j; ++j, ++m)  {
	xml_comp_t x_slice = j;
	double     w = x_slice.thickness();
	Material mat = lcdd.material(x_slice.materialStr());
	string s_name = l_name+_toString(m,"_slice%d");
	Tube   s_tub(lcdd,s_name,rmin,rmax,2*M_PI);
	Volume s_vol(lcdd,s_name+"_volume", s_tub, mat);

	if ( x_slice.isSensitive() ) s_vol.setSensitiveDetector(sens);
	s_vol.setAttributes(lcdd,x_slice.regionStr(),x_slice.limitsStr(),x_slice.visStr());

	PlacedVolume spv = l_vol.placeVolume(s_vol,Position(0,0,z-zmin-layerWidth/2+w/2));
	spv.addPhysVolID(_X(layer),n);
      }

      PlacedVolume lpv = motherVol.placeVolume(l_vol,Position(0,0,zmin+layerWidth/2.));
      lpv.addPhysVolID(_X(system),sdet.id());
      lpv.addPhysVolID(_X(barrel),1);
      DetElement layer(lcdd,l_name,det_type+"/Layer",n);
      sdet.add(layer.setPlacement(lpv));
      if ( reflect )  {
	PlacedVolume lpvR = motherVol.placeVolume(l_vol,Position(0,0,-zmin-layerWidth/2),ReflectRot());
	lpvR.addPhysVolID(_X(system),sdet.id());
	lpvR.addPhysVolID(_X(barrel),2);
	DetElement layerR(lcdd,l_name+"_reflect",det_type+"/Layer",n);
	sdet.add(layerR.setPlacement(lpvR));
      }
    }
    sdet.setCombineHits(x_det.attr<bool>(_A(combineHits)),sens);
    return sdet;
  }
}}

DECLARE_NAMED_DETELEMENT_FACTORY(DetDesc,DiskTracker);
