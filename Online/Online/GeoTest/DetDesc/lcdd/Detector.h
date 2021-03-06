// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#ifndef DETDESC_LCDD_DETECTOR_H
#define DETDESC_LCDD_DETECTOR_H

// Framework include files
#include "DetDesc/Handle.h"
#include "DetDesc/lcdd/Objects.h"
#include "DetDesc/lcdd/Volumes.h"
#include "DetDesc/lcdd/Readout.h"
#include "DetDesc/lcdd/Segmentations.h"

// C/C++ include files
#include <map>


/*
 *   DetDesc namespace declaration
 */
namespace DetDesc {

  /*
   *   Geometry namespace declaration
   */
  namespace Geometry  {

    struct SensitiveDetector;
    struct Detector;
    struct LCDD;

    /** @class SensitiveDetector Detector.h DetDesc/lcdd/Detector.h
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    struct SensitiveDetector : public Ref_t  {
      struct Object  {
	unsigned int magic;
        int          verbose;
        int          combine_hits;
        double       ecut;
        std::string  eunit;
        std::string  hits_collection;
        Segmentation segmentation;
        Ref_t  id;
        Object() : magic(magic_word()), verbose(0), segmentation() {}
      };

      /// Default constructor
      SensitiveDetector() : Ref_t() {}
      /// Templated constructor for handle conversions
      template <typename Q>
      SensitiveDetector(const Handle<Q>& e) : Ref_t(e) {}
      /// Constructor for a new sensitive detector element
      SensitiveDetector(const LCDD& lcdd, const std::string& type, const std::string& name);

      /// Additional data accessor
      Object& _data()   const {  return *data<Object>();  }
      /// Access the type of the sensitive detector
      std::string type() const;
      /// Set flag to handle hits collection
      SensitiveDetector& setCombineHits(bool value);
      /// Assign the name of the hits collection
      SensitiveDetector& setHitsCollection(const std::string& spec);
      /// Assign the IDDescriptor reference
      SensitiveDetector& setIDSpec(const Ref_t& spec);
      /// Assign the readout segmentation reference
      SensitiveDetector& setSegmentation(const Segmentation& seg);
      
    };

    /** @class SubDetector Detector.h DetDesc/lcdd/Detector.h
     *
     *  Please note: 
     *  Though nowhere enforced, it is obvious that only placements
     *  of the **SAME** logical volume should be added.
     *
     *  We explicitly rely here on the common sense of the user
     *  of this class. People without a brain can always screw 
     *  things completely - nothing one can do about!
     *
     *  @author  M.Frank
     *  @version 1.0
     */
    struct DetElement : public Ref_t   {
      typedef std::map<std::string,DetElement> Children;
      typedef std::vector<PlacedVolume>        Placements;
      struct Object  {
	unsigned int      magic;
        int               id;
        int               combine_hits;
        Readout           readout;
	Alignment         alignment;
	Volume            volume;
	Conditions        conditions;
        Placements        placements;
        Children          children;
        Object();
      };

      /// Additional data accessor
      Object& _data()   const {  return *data<Object>();  }

      void check(bool condition, const std::string& msg) const;

      /// Default constructor
      DetElement() : Ref_t()  {}
      /// Templated constructor for handle conversions
      template<typename Q> DetElement(const Handle<Q>& e) : Ref_t(e)  {}
      /// Constructor for a new subdetector element
      DetElement(const LCDD& lcdd, const std::string& name, const std::string& type, int id);

      DetElement&     setCombineHits(bool value, SensitiveDetector& sens);
      DetElement&     add(const DetElement& sub_element);
      int             id() const;
      std::string     type() const;
      bool            isTracker() const;
      bool            isCalorimeter() const;
      bool            isInsideTrackingVolume() const;
      bool            combineHits() const;

      /// Set all attributes in one go
      DetElement& setAttributes(const LCDD& lcdd, const Volume& volume,
				const std::string& region, 
				const std::string& limits, 
				const std::string& vis);

      /// Set Visualization attributes to the detector element
      DetElement& setVisAttributes(const LCDD& lcdd, const std::string& name, const Volume& volume);
      /// Set the regional attributes to the detector element
      DetElement& setRegion(const LCDD& lcdd, const std::string& name, const Volume& volume);
      /// Set the limits to the detector element
      DetElement& setLimitSet(const LCDD& lcdd, const std::string& name, const Volume& volume);

      /// Access the readout structure 
      Readout         readout() const;
      /// Assign readout definition
      DetElement&     setReadout(const Readout& readout);

      /// Access the physical volumes of the detector element
      Placements      placements() const;
      /// Set the physical volumes of the detector element
      DetElement&     addPlacement(const PlacedVolume& volume);
      /// Access to the logical volume of the placements (all daughters have the same!)
      Volume          volume() const;

      /// Access to the list of children
      const Children& children() const;
      /// Access to individual children by name
      DetElement      child(const std::string& name) const;
    };

  }       /* End namespace Geometry      */
}         /* End namespace DetDesc       */
#endif    /* DETDESC_LCDD_DETECTOR_H     */
