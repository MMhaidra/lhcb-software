
//=============================================================================================
/** @file RichDAQHeaderPDBase.h
 *
 *  Header file for RICH DAQ utility class : Rich::DAQ::HeaderPDBase
 *
 *  CVS Log :-
 *  $Id: RichDAQHeaderPDBase.h,v 1.7 2007-03-26 11:21:40 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
//=============================================================================================

#ifndef RICHDAQ_RichDAQHeaderPDBase_H
#define RICHDAQ_RichDAQHeaderPDBase_H 1

// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// numberings
#include "RichDet/RichDAQDefinitions.h"

// RichKernel
#include "RichKernel/BoostMemPoolAlloc.h"

// boost
//#include "boost/pool/pool_alloc.hpp"

namespace Rich
{
  namespace DAQ
  {

    /** @class HeaderPDBase RichDAQHeaderPDBase.h
     *
     *  Base class for RICH HPD headers. Implements basic common functions
     *
     *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
     *  @date   2003-11-06
     */
    class HeaderPDBase
    {

    public:

      /// Type for header words
      typedef std::vector<LongType > HeaderWords;
      //typedef std::vector<LongType, boost::pool_allocator<LongType> > HeaderWords;

    public:

      /// Constructor with number of header words
      explicit HeaderPDBase ( const ShortType nWords   = 1,
                              const LongType  wordInit = 0 )
        : m_headerWords(nWords,wordInit) { }

      /// Copy constructor
      HeaderPDBase ( const HeaderPDBase & header )
        : m_headerWords(header.headerWords()) { }

      /// Constructor from raw header word(s)
      explicit HeaderPDBase( const HeaderWords & words )
        : m_headerWords(words) { }

      /// Destructor
      virtual ~HeaderPDBase ( ) { }

    public: // methods

      /// Read only access to header words
      inline const HeaderWords & headerWords() const { return m_headerWords; }

      /// Returns the number of data words in the header
      inline unsigned int nHeaderWords() const
      {
        return headerWords().size();
      }

    public: // reading and writing

      /// Read correct number of data words from given stream
      /// Note, after this call data pointer is incremented to the next word after the header
      virtual void readFromDataStream( const LongType *& data )
      {
        for ( unsigned int i = 0; i < nHeaderWords(); ++i )
        {
          headerWords()[i] = *(data++);
        }
      }

      /// Write this head to a RAWBank of data words
      inline void fillRAWBank ( RAWBank & rawData ) const
      {
        for ( HeaderWords::const_iterator iH = headerWords().begin();
              iH != headerWords().end(); ++iH )
        {
          rawData.push_back( *iH );
        }
      }

    protected: // methods

      /// Read/Write access to header words
      inline HeaderWords & headerWords() { return m_headerWords; }

      /// Set the data value using the given mask and shift values
      inline bool set( const ShortType value,
                       const ShortType shift,
                       const LongType  mask,
                       const ShortType wordNumber = 0 )
      {
        headerWords()[wordNumber] = ( ((value << shift) & mask) | (headerWords()[0] & ~mask) );
        return true;
      }

      /// tests whether a given value is in range for a given data field
      inline bool dataInRange( const ShortType value,
                               const ShortType max ) const
      {
        return ( value <= max );
      }

    private: // data

      /// The header words
      HeaderWords m_headerWords;

    };

  }
}

#endif // RICHDAQ_RichDAQHeaderPDBase_H
