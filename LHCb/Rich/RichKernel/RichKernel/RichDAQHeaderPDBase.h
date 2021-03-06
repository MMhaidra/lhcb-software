
//=============================================================================================
/** @file RichDAQHeaderPDBase.h
 *
 *  Header file for RICH DAQ utility class : Rich::DAQ::HeaderPDBase
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
//=============================================================================================

#ifndef RICHDAQ_RichDAQHeaderPDBase_H
#define RICHDAQ_RichDAQHeaderPDBase_H 1

// Gaudi
#include "GaudiKernel/GaudiException.h"

// numberings
#include "RichKernel/RichDAQDefinitions.h"

// Kernel
#include "Kernel/MemPoolAlloc.h"
#include "Kernel/FastAllocVector.h"

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
      using HeaderWords = LHCb::Boost::PoolAllocVector<LongType>;

    public:

      /// Constructor with number of header words and init value
      explicit HeaderPDBase ( const ShortType nWords,
                              const LongType  wordInit = 0 )
        : m_headerWords(nWords,wordInit) { }
      
      /// Constructor from raw header word(s)
      explicit HeaderPDBase( const HeaderWords & words )
        : m_headerWords(words) { }
      
    public:

      /// Default constructor
      HeaderPDBase( ) : m_headerWords(1,0) { }

      /// Destructor
      virtual ~HeaderPDBase ( ) = default;

      /// Default Copy Constructor
      HeaderPDBase( const HeaderPDBase& ) = default;

      /// Default Copy operator
      HeaderPDBase& operator=( const HeaderPDBase& ) = default;

      /// Default Move Constructor
      HeaderPDBase( HeaderPDBase&& ) = default;

      /// Default Move operator
      HeaderPDBase& operator=( HeaderPDBase&& ) = default;

    public: // methods

      /// Read only access to header words
      inline const HeaderWords &  headerWords() const & noexcept { return m_headerWords; }

      /// Move access to header words
      inline HeaderWords && headerWords() && noexcept { return std::move(m_headerWords); }

      /// Returns the number of data words in the header
      inline HeaderWords::size_type nHeaderWords() const noexcept
      {
        return headerWords().size();
      }

    public: // reading and writing

      /// Read correct number of data words from given stream
      /// Note, after this call data pointer is incremented to the next word after the header
      virtual void readFromDataStream( const LongType *& data );

      /// Write this head to a RAWBank of data words
      inline void fillRAWBank ( RAWBank & rawData ) const
      {
        for ( const auto& H : headerWords() ) { rawData.push_back(H); }
      }

    protected: // methods

      /// Read/Write access to header words
      inline HeaderWords & headerWords() & noexcept { return m_headerWords; }

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
                               const ShortType max ) const noexcept
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
