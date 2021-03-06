#ifndef TF_HITCREATORGEOM_H
#define TF_HITCREATORGEOM_H

#include <vector>
#include <algorithm>
#include <boost/checked_delete.hpp>
#include "TfKernel/Region.h"

namespace Tf
{

  /** @namespace HitCreatorGeom
   *
   *  Namespace for various utilities used by the hit creators
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   08/07/2004
   */
  namespace HitCreatorGeom
  {

    /** @class IndexedVector HitCreatorGeom.h
     *
     *  A container that gives access to elements by iterator and
     *  by index. It keeps to containers internally. No hashing or
     *  anything smart.
     *
     *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
     *  @date   08/07/2004
     */
    template < class T , size_t offset=0 >
    class IndexedVector : public std::vector<T*>
    {
    public:
      /// Constructor
      IndexedVector() : m_isowner(true) { };

      /// Copy constructor passes ownership
      IndexedVector( IndexedVector& rhs) : m_isowner(true),
                                           std::vector<T*>(rhs),
                                           m_pointers(rhs.m_pointers) {
        rhs.m_isowner = false ;
      }

      /// Destructor
      ~IndexedVector() {
        if(m_isowner) std::for_each( std::vector<T*>::begin(),
                                     std::vector<T*>::end(),
                                     boost::checked_delete<T> ) ; }

      /// Const operator []
      const T* operator[](size_t index) const {
        index-=offset;
        return index < m_pointers.size() ? m_pointers[index] : 0;
      };

      /// Non const operator []
      T* operator[](size_t index) {
        index-=offset;
        return index < m_pointers.size() ? m_pointers[index] : 0;
      };

      /// Insert method
      void insert(size_t index, T* object) {
        assert(m_isowner) ;
        std::vector<T*>::push_back( object ) ;
        index-=offset;
        if (index >= m_pointers.size()) m_pointers.resize(index + 1,0);
        m_pointers[index] = object ;
      };

      /// clear vector
      void clear() { std::vector<T*>::clear(); m_pointers.clear() ; };

    private:
      bool m_isowner ;
      std::vector<T*> m_pointers ; // these can be accessed by index, but contain zeros
    } ;


    /////////////////////////////////////////////////////////////
    /** @class Layer HitCreatorGeom.h
     *
     * Templated helper class for creating a hierarchy of patreco regions
     *
     *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
     *  @date   08/07/2004
     */
    /////////////////////////////////////////////////////////////

    template < typename RegionType >
    struct Layer
    {
      typedef IndexedVector<RegionType> RegionContainer ;
      RegionContainer regions ;
      typename RegionType::HitRangeType hits() const {
        return { regions.front()->hits().begin(),
                 regions.back()->hits().end() } ; }
      void insert( RegionType* region) {
        RegionType* altregion = regions[region->id().region()] ;
        if( altregion ) throw std::runtime_error("region already exists!") ;
        regions.insert( region->id().region(), region ) ;
      }
      RegionType* region( const RegionID& id) { return regions[id.region()] ; }
    } ;

    /////////////////////////////////////////////////////////////
    /** @class Station HitCreatorGeom.h
     *
     * Templated helper class for creating a hierarchy of patreco regions
     *
     *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
     *  @date   08/07/2004
     */
    /////////////////////////////////////////////////////////////
    template < typename RegionType >
    struct Station
    {
      typedef IndexedVector<Layer<RegionType> > LayerContainer ;
      LayerContainer layers ;
      typename RegionType::HitRangeType hits() const {
        return { layers.front()->hits().begin(),
                 layers.back()->hits().end()} ; };
      void insert( RegionType* region) {
        Layer<RegionType>* layer = layers[region->id().layer()] ;
        if( !layer ) {
          layer = new Layer<RegionType>() ;
          layers.insert( region->id().layer(), layer ) ;
        }
        layer->insert( region ) ;
      }
      RegionType* region( const RegionID& id) {
        return layers[id.layer()] ? layers[id.layer()]->region(id) : 0 ; }

    } ;

    /////////////////////////////////////////////////////////////
    /** @class Detector HitCreatorGeom.h
     *
     * Templated helper class for creating a hierarchy of patreco regions
     *
     *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
     *  @date   08/07/2004
     */
    /////////////////////////////////////////////////////////////
    template < typename RegionType >
    class Detector
    {
    public:
      typedef IndexedVector<Station<RegionType> > StationContainer  ;
      typedef std::vector<RegionType*> RegionContainer ;
      typedef std::vector<const typename RegionType::HitType*> HitContainer ;

      //Detector( const typename RegionType::DetectorType& detector) ;
      Detector() : m_isloaded(false) {}

      const StationContainer& stations() const { return m_stations ; }
      HitContainer& hits() { return m_hits ; }
      RegionContainer& regions() { return m_regions ; }

      //typename RegionType::ModuleType* module( size_t istation, size_t ilayer, size_t iregion, size_t imodule ) {
      //return m_stations[istation]->layers[ilayer]->regions[iregion]->modules()[imodule] ; }

      const RegionType* region( size_t istation, size_t ilayer, size_t iregion ) const {
        return m_stations[istation]->layers[ilayer]->regions[iregion] ; }

      const RegionType* region( const RegionID& id) const {
        return m_stations[id.station()]->layers[id.layer()]->regions[id.region()] ; }

      RegionType* region( const RegionID& id) {
        return m_stations[id.station()] ? m_stations[id.station()]->region(id) : 0 ; }

      void insert( RegionType* region) {
        Station<RegionType>* station = m_stations[region->id().station()] ;
        if( !station ) {
          station = new Station<RegionType>() ;
          m_stations.insert( region->id().station(), station ) ;
        }
        station->insert(region) ;
        m_regions.push_back(region) ;
      }

      void clearEvent() {
        m_isloaded = false ;
        m_hits.clear() ;
        std::for_each(  std::begin(m_regions),  std::end(m_regions), [](const RegionType* r) { r->clearEvent(); } );
      }

      void setIsLoaded(bool b=true) { m_isloaded = b ; }

      bool isLoaded() const { return m_isloaded ; }

      // We put the template specialization inside the region class. Ugly, but pragmatic.
      void loadHits() { if(!m_isloaded) m_regions.front()->loadAllHits() ; }

    private:
      StationContainer m_stations ;  // the stations
      RegionContainer  m_regions ;   // shortcut to regions, to speed up the loops
      mutable HitContainer m_hits ;          // sorted list of hits
      mutable bool m_isloaded ;
    } ;

    template<typename T> bool compareX(const T* lhs, const T* rhs) { return lhs->x() < rhs->x() ; }

    /** @class RegionOfModules
     *
     * An (templated) implementation of a region class that represents
     * a collection of detector elements (called 'modules') ModuleType
     * is a wrapper for the lowest level DetectorElements in OT or
     * ST. The module owns the hits. The region implements the
     * functionality to search in certain windows and decode on
     * demand.
     *
     *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
     *  @date   08/07/2004
     */
    template<class ModuleType, class HitLoader>
    class RegionOfModules : public Tf::Region<typename ModuleType::HitType>
    {
    public:
      typedef typename Tf::Region<typename ModuleType::HitType>::HitRangeType HitRangeType ;
      typedef typename ModuleType::HitType HitType ;
      typedef IndexedVector<ModuleType> ModuleContainer ;       // for indexed access. can contain 0 pointers

      RegionOfModules( const RegionID& id,  HitLoader& parent )
        : Tf::Region<typename ModuleType::HitType>(id), m_parent(&parent) {}

      virtual ~RegionOfModules() {}

      ModuleContainer& modules() { return m_modules ; }
      const ModuleContainer& modules() const { return m_modules ; }

      /// Add a module
      void insert(size_t index, ModuleType* module) {
        m_modules.insert(index,module) ;
        std::sort(m_modules.begin(),m_modules.end(),compareX<ModuleType>) ;
        this->add(module->detelement()) ;
      }

      /// Clear event data
      void clearEvent() const {
        std::for_each( std::begin(m_modules), std::end(m_modules), [](ModuleType* m) { m->clearEvent(); });
      }

      /// Get all hits
      virtual HitRangeType hits() const {
	if( !m_parent->isLoaded() )
	  (const_cast<RegionOfModules<ModuleType,HitLoader>*>(this))->m_parent->loadHits(m_modules.begin(),
											 m_modules.end()) ;
        return HitRangeType(m_modules.front()->hits().begin(), m_modules.back()->hits().end()) ; }
      
      /// find the hits in a region of interest with a bounding box in X
      virtual HitRangeType hitsLocalXRange(double xmin, double xmax) const ;

      /// find the hits in a region of interest with a bounding box in X
      virtual HitRangeType hits(double xmin, double xmax) const ;

      /// find the hits in a region of interest with a bounding box in X and Y
      virtual HitRangeType hits(double thisxmin, double thisxmax, double thisymin, double thisymax) const ;

    private:
      double xT( double cosT, double sinT, double x, double y) const {
        return x*cosT + y*sinT ; }
      double xTMin( double cosT, double sinT, double x, double ymin, double ymax) const {
        return x*cosT + std::min( ymin*sinT, ymax*sinT ) ; }
      double xTMax( double cosT, double sinT, double x, double ymin, double ymax) const {
        return x*cosT + std::max( ymin*sinT, ymax*sinT ) ; }

    private:
      HitLoader* m_parent ;
      ModuleContainer m_modules ;
    } ;


    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // find the hits in a region of interest with a bounding box in global X 
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    template<class ModuleType, class HitLoader>
    typename RegionOfModules<ModuleType,HitLoader>::HitRangeType
    RegionOfModules<ModuleType,HitLoader>::hits(double xmin, double xmax) const
    {
      auto first = std::begin(modules()) ;
      auto last = std::end(modules()); ; --last ;

      if( xmin > (*last)->xmax() || xmax < (*first)->xmin()) return HitRangeType() ;
      
      // FIXME. we could use a binary search, but there are so few
      // modules, that it might be overkill. bullshit ... this might
      // be slower than anything that follows!

      while( first<= last  && (*first)->xmax() < xmin ) ++first ;
      while( last >= first && (*last)->xmin() > xmax )  --last ;

      if( !m_parent->isLoaded() )
        (const_cast<RegionOfModules<ModuleType,HitLoader>*>(this))->m_parent->loadHits(first,last+1) ;

      // binary search to find first hit in first module
      //const HitRangeType& firstrange = (*first)->hits() ;
      const HitRangeType& firstrange = (*first)->hits() ;
      auto beginhit = firstrange.begin() ;
      if( firstrange.size()>0)
        beginhit = std::lower_bound(firstrange.begin(), firstrange.end(), xmin, 
                                    [](const HitType* hit, double x) { return hit->xMax() < x; });

      // binary search to find last hit in last module
      const HitRangeType& lastrange = (*last)->hits() ;
      auto endhit = lastrange.end() ;
      if( lastrange.size()>0)
        endhit = std::lower_bound(lastrange.begin(), lastrange.end(), xmax,
                                  [](const HitType* hit, double x) { return hit->xMin() <  x ; });

      // let's at least check that all the hits that we return are actually in this window
      //  for( OTHitRange::const_iterator ihit = beginhit ; ihit != endhit; ++ihit) {
      //       bool isinwindow = xmin <= (*ihit)->x() && (*ihit)->x() <= xmax ;
      //     }

      //std::cout << "Number of hits in window: " << OTHitRange( beginhit, endhit ).size() << std::endl ;
      //assert( xmin <= (*ihit)->x() && (*ihit)->x() <= xmax ) ;

      return HitRangeType( beginhit, endhit ) ;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // find the hits in a region of interest with a bounding box in X and Y
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    template<class ModuleType, class HitLoader>
    typename RegionOfModules<ModuleType,HitLoader>::HitRangeType
    RegionOfModules<ModuleType,HitLoader>::hits(double thisxmin, double thisxmax, double thisymin, double thisymax) const
    {
      HitRangeType rc ;
      // first check in global coordinates if the ranges overlap at all
      if( ! ( thisymax < Region<typename ModuleType::HitType>::ymin() || thisymin > Region<typename ModuleType::HitType>::ymax() ||
              thisxmax < Region<typename ModuleType::HitType>::xmin() || thisxmin > Region<typename ModuleType::HitType>::xmax() ) ) {

        //  std::cout << "Window in global coordinates: "
        //     << thisxmin << " " << thisxmax << " " << thisymin << " " << thisymax << std::endl ;
        //  std::cout << "Stereo-angle: " << asin(RegionOfModules<ModuleType,HitLoader>::sinT()) << std::endl ;

        auto beginmodule = modules().begin() ;
        auto endmodule   = modules().end() ;
        // if there is more than one module, we need to find the right
        // module(s) first.
        if( modules().size()>1) {
          // First find the first module. A binary search would help here. FIXME.
          while( beginmodule<modules().end() &&
                 (*beginmodule)->xmaxT() < xTMin( (*beginmodule)->cosT(), (*beginmodule)->sinT(), thisxmin, thisymin,thisymax) )
            ++beginmodule ;

          // Now find the last module. Don't use binary search
          // here. (Usually, the next module will fail the window.)
          endmodule = beginmodule+1 ;
          while( endmodule<modules().end() &&
                 (*endmodule)->xminT() < xTMax( (*endmodule)->cosT(), (*endmodule)->sinT(), thisxmax, thisymin,thisymax) )
            ++endmodule ;
        }
        //std::cout << "Number of overlapping modules: " << std::distance(beginmodule,endmodule) << std::endl ;

        if( beginmodule < modules().end() ) {

          // Load the data for these modules
          if( !m_parent->isLoaded() )
            (const_cast<RegionOfModules<ModuleType,HitLoader>*>(this))->m_parent->loadHits(beginmodule,endmodule) ;


          // binary search to find first hit in first module
          const HitRangeType& firstrange = (*beginmodule)->hits() ;
          auto beginhit = firstrange.begin() ;
          double thisxTmin = xTMin( (*beginmodule)->cosT(), (*beginmodule)->sinT(), thisxmin, thisymin,thisymax) ;
          if( firstrange.size()>0)
            beginhit = std::lower_bound(firstrange.begin(), firstrange.end(),thisxTmin ,
                                        [](const HitType* hit, double x) { return hit->xT() < x; } );

          // binary search to find last hit in last module
          auto lastmodule = endmodule - 1 ;
          const HitRangeType& lastrange = (*lastmodule)->hits() ;
          double thisxTmax = xTMax( (*lastmodule)->cosT(), (*lastmodule)->sinT(), thisxmax, thisymin,thisymax) ;
          auto endhit = lastrange.end() ;
          if( lastrange.size()>0)
            endhit = std::lower_bound(lastrange.begin(), lastrange.end(), thisxTmax,
                                      [](const HitType* hit, double x) { return hit->xT() < x; } );

          rc = HitRangeType( beginhit, endhit ) ;
          //    std::cout << "X-window in local coordinates: " << thisxTmin << " " << thisxTmax << std::endl ;
          //    std::cout << "Number of hits in window: " << rc.size() << std::endl ;
          //    std::cout << "Total number of hits in region: " << hits().size() << std::endl ;
        }
      }
      return rc ;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // find the hits in a region of interest with a bounding box in local X
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    template<class ModuleType, class HitLoader>
    typename RegionOfModules<ModuleType,HitLoader>::HitRangeType
    RegionOfModules<ModuleType,HitLoader>::hitsLocalXRange(double xmin, double xmax) const
    {
 
      auto first = modules().begin() ;
      auto last = modules().end() ; --last ;

      if( xmin > (*last)->xmaxT() || xmax < (*first)->xminT()) return HitRangeType() ;
      
      // FIXME. we could use a binary search, but there are so few
      // modules, that it might be overkill. bullshit ... this might
      // be slower than anything that follows!
     
      while( first<= last  && (*first)->xmaxT() < xmin ) ++first ;
      while( last >= first && (*last)->xminT() > xmax )  --last ;

      if( !m_parent->isLoaded() )
        (const_cast<RegionOfModules<ModuleType,HitLoader>*>(this))->m_parent->loadHits(first,last+1) ;

      // binary search to find first hit in first module
      //const HitRangeType& firstrange = (*first)->hits() ;
      const HitRangeType& firstrange = (*first)->hits() ;
      auto beginhit = firstrange.begin() ;
      if( firstrange.size()>0)
        beginhit = std::lower_bound(firstrange.begin(), firstrange.end(), xmin,
                                    [](const HitType* hit, double x) { return hit->xT() < x; } );

      // binary search to find last hit in last module
      const HitRangeType& lastrange = (*last)->hits() ;
      auto endhit = lastrange.end() ;
      if( lastrange.size()>0)
        endhit = std::lower_bound(lastrange.begin(), lastrange.end(), xmax,
                                  [](const HitType* hit, double x) { return hit->xT() < x; } );
      return HitRangeType( beginhit, endhit ) ;
    }

  }
}
#endif
