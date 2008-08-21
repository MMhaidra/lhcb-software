
//-----------------------------------------------------------------------------
/** @file STHitCreator.h
 *
 *  Header file for class : Tf::STHitCreator
 *
 *  $Id: STHitCreator.h,v 1.3 2008-08-21 18:03:18 smenzeme Exp $
 *
 *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
 *  @date   2007-06-01
 */
//-----------------------------------------------------------------------------

#ifndef TFKERNEL_STHitCreator_H
#define TFKERNEL_STHitCreator_H 1

#include <string>

#include "GaudiAlg/GaudiTool.h"

#include "GaudiKernel/IIncidentListener.h"
#include "TfKernel/STHit.h"

namespace Tf

{

  // forward declaration of the class that is holding the hit data
  namespace HitCreatorGeom {
    class STDetector ;
  }

  /** @class STHitCreator STHitCreator.h
   *
   *  Implementation of Tf::IITHitCreator and Tf::ITTHitCreator
   *
   *  Creates the Tf::STHit objects for the Tf tracking framework
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-06-01
   */

  template<class Trait>
  class STHitCreator: public GaudiTool,
                      virtual public Trait::ISTHitCreator,
                      virtual public IIncidentListener

  {

  public:
    /// The region type for TT hit (could get this from the Trait::ISTHitCreator)
    typedef Tf::Region<STHit> STRegion ;

    /// constructor
    STHitCreator(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

    /// destructor
    virtual ~STHitCreator();

    /// initialize
    virtual StatusCode initialize();

    /// finalize
    virtual StatusCode finalize();

    /// incident service handle
    virtual void handle( const Incident& incident ) ;

    // RestUsed flag for all OT hits
    virtual  void resetUsedFlagOfHits() const;

    // Load all the IT hits
    virtual STHitRange hits() const ;

    // Load the ST hits for a given region of interest
    virtual STHitRange hits(const typename Trait::StationID iStation,
                            const typename Trait::LayerID iLayer) const ;

    // Load the ST hits for a given region of interest
    virtual STHitRange hits(const typename Trait::StationID iStation,
                            const typename Trait::LayerID iLayer,
                            const typename Trait::RegionID iRegion) const ;

    // Load the ST hits for a given region of interest
    virtual STHitRange hits(const typename Trait::StationID iStation,
                            const typename Trait::LayerID iLayer,
                            const typename Trait::RegionID iRegion,
                            const double xmin,
                            const double xmax) const ;

    // Load the hits for a given region of interest
    virtual STHitRange hits(const typename Trait::StationID iStation,
                            const typename Trait::LayerID iLayer,
                            const typename Trait::RegionID iRegion,
                            const double xmin,
                            const double xmax,
                            const double ymin,
                            const double ymax) const ;

    // Retrieve the STRegion for a certain region ID. The region
    virtual const STRegion* region(const typename Trait::StationID iStation,
                                   const typename Trait::LayerID iLayer,
                                   const typename Trait::RegionID  iRegion) const ;

  private:

    /// Load the hits
    void loadHits() const ;

  private:
    
    mutable HitCreatorGeom::STDetector* m_detectordata ;
    std::string m_clusterLocation;
    std::string m_detectorLocation;
  };

}

#endif // TFKERNEL_STHitCreator_H

