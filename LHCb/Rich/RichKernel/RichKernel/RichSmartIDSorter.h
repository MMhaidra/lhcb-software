
//-----------------------------------------------------------------------------
/** @file RichSmartIDSorter.h
 *
 *  Header file for RichSmartID utility class : RichSmartIDSorter
 *
 *  $Id: RichSmartIDSorter.h,v 1.5 2005-02-09 09:13:04 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-10
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHSMARTIDSORTER_H
#define RICHKERNEL_RICHSMARTIDSORTER_H 1

// STL
#include <functional>

// Kernel
#include "Kernel/RichSmartID.h"

/** @class RichSmartIDSorter RichSmartIDSorter.h RichKernel/RichSmartIDSorter.h
 *
 *  Utility class to sort RichSmartIDs according to various criteria
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-01-10
 */
class RichSmartIDSorter {

private: // utility classes

  /// Functor to sort RichSmartIDs by Rich then panel numbers
  class SortByRegion
    : std::binary_function<const RichSmartID,const RichSmartID,bool>
  {

  public:

    /** Sort operator
     *
     *  Sorts the RichSmartIDs into order of RICH/Panel etc.
     *
     *  @param p1 First RichSmartID
     *  @param p2 Second RichSmartID
     *
     *  @return bool indicating if p1 should be listed before p2
     *
     *  @todo Rewrite more efficiently when RichSmartID bit packing is better arranged
     */
    bool operator() ( const RichSmartID p1, const RichSmartID p2 ) const
    {
      // Until RichSmartID is updated, use explicit slow sorting...
      //return ( p1.dataBitsOnly().key() < p2.dataBitsOnly().key() );
      return ( 100000000*p1.rich() + 10000000*p1.panel() +
               1000000*p1.pdRow() + 10000*p1.pdCol() +
               100*p1.pixelRow() + p1.pixelCol()  <
               100000000*p2.rich() + 10000000*p2.panel() +
               1000000*p2.pdRow() + 10000*p2.pdCol() +
               100*p2.pixelRow() + p2.pixelCol() );
    }

  };

public:

  /// Constructor
  RichSmartIDSorter() {};

  /// Destructor
  ~RichSmartIDSorter() {};

  /// Sort the list by detector region
  inline void sortByRegion( RichSmartID::Collection & smartIDs ) const
  {
    std::sort( smartIDs.begin(), smartIDs.end(), RichSmartIDSorter::SortByRegion() );
  }

};

#endif // RICHKERNEL_RICHSMARTIDSORTER_H
