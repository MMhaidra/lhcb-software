
//-----------------------------------------------------------------------------
/** @file RichNonZeroSuppALICEData_V2.h
 *
 *  Header file for RICH DAQ utility class : RichNonZeroSuppALICEData
 *
 *  CVS Log :-
 *  $Id: RichNonZeroSuppALICEData_V2.h,v 1.2 2007-03-01 19:39:07 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-07
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RichNonZeroSuppALICEDataV2_H
#define RICHDAQ_RichNonZeroSuppALICEDataV2_H 1

// local
#include "RichHPDDataBank.h"
#include "RichZSPacked_V2.h"

// RichKernel
#include "RichKernel/BoostMemPoolAlloc.h"

//===================================================================================

namespace Rich
{
  namespace DAQ
  {

    /** @namespace Rich::DAQ::RichNonZeroSuppALICEDataV2
     *
     *  Namespace for version 2 of the RichNonZeroSuppALICEData object.
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   2004-12-17
     */
    namespace RichNonZeroSuppALICEDataV2
    {

      /** @class RichNonZeroSuppALICEData RichNonZeroSuppALICEData_V2.h
       *
       *  The RICH HPD non zero suppressed data format for ALICE mode.
       *
       *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
       *  @date   2003-11-07
       */
      template< class Version, class Header, class Footer >
      class RichNonZeroSuppALICEData : public HPDDataBankImp<Version,Header,Footer>,
                                       public Rich::BoostMemPoolAlloc< RichNonZeroSuppALICEDataV2::RichNonZeroSuppALICEData<Version,Header,Footer> >
      {

      public:

        /// Default constructor
        RichNonZeroSuppALICEData() :
          HPDDataBankImp<Version,Header,Footer> ( 0,
                                                  Header(),
                                                  Footer(),
                                                  MaxDataSizeALICE ),
          m_nHits(-1)
        { }

        /** Constructor from a RichSmartID HPD identifier and a vector of RichSmartIDs
         *
         *  @param l0ID   L0 board hardware identifier
         *  @param digits Vector of RichSmartIDs listing the active channels in this HPD
         */
        explicit RichNonZeroSuppALICEData( const Level0ID l0ID,
                                           const LHCb::RichSmartID::Vector & digits )
          : HPDDataBankImp<Version,Header,Footer> ( Header ( false, // Non-ZS
                                                             true,  // Is ALICE mode
                                                             false, // normal data format
                                                             false, // No GT inhibit
                                                             l0ID,  // The L0 ID
                                                             EventID(0), // Event ID (TO BE DONE?)
                                                             0 // filled by buildData call
                                                             ),
                                                    Footer(),
                                                    0, MaxDataSizeALICE ),
            m_nHits ( -1 )
        {
          buildData( digits );
        }

        /** Constructor from a block of raw data
         *
         *  @param data Pointer to the start of the data block
         */
        explicit RichNonZeroSuppALICEData( const LongType * data )
          : HPDDataBankImp<Version,Header,Footer> ( data, // start of data
                                                    Header(),
                                                    Footer(),
                                                    MaxDataSizeALICE,
                                                    MaxDataSizeALICE ),
            m_nHits( -1 )
        { }

        /// Destructor
        ~RichNonZeroSuppALICEData() { }

        // Returns the hit count for this HPD
        virtual ShortType hitCount() const;

        // Fill a vector with RichSmartIDs for hit pixels
        virtual ShortType fillRichSmartIDs( LHCb::RichSmartID::Vector & ids,
                                            const LHCb::RichSmartID hpdID ) const;

      private: // methods

        /// Build data array from vector of RichSmartIDs
        void buildData( const LHCb::RichSmartID::Vector & digits );

        /// Calculates number of 8-bit words in the data
        ShortType calcEightBitword( const LHCb::RichSmartID::Vector & digits ) const;

        /// Set a pixel as active
        inline void setPixelActive( const ShortType row,
                                    const ShortType col )
        {
          setBit( this->data()[this->maxDataSize()-(row+1)], col );
        }

        /// Is a given pixel active ?
        inline bool isPixelActive( const ShortType row,
                                   const ShortType col ) const
        {
          return isBitOn( this->data()[this->maxDataSize()-(row+1)], col );
        }

      private: // data

        /// Decoded number of hits
        mutable int m_nHits;

      };

    } // RichNonZeroSuppALICEDataV2 namespace

  }
}

#endif // RICHDAQ_RichNonZeroSuppALICEDataV2_H
