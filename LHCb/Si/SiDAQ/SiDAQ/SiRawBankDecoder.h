// $Id: SiRawBankDecoder.h,v 1.1 2006-02-22 17:21:16 krinnert Exp $
#ifndef SIRAWBANKDECODER_H 
#define SIRAWBANKDECODER_H 1

#include <vector>

#include "SiDAQ/SiHeaderWord.h"
#include "SiDAQ/SiADCWord.h"
#include "SiDAQ/SiRawBufferWord.h"
#include "SiDAQ/SiADCBankTraits.h"

// forward declaration needed for friendship with iterators
template<class CLUSTERWORD>
class SiRawBankDecoder;

/** @class SiRawBankDecoder SiRawBankDecoder.h
 *  
 *  Decoder for raw Si banks
 *
 *  This class provides two ways to decode the velo raw buffer.
 *  Either you can iterate over the cluster positions only, 
 *  using the pos_iterator,
 *  or you can iterate over cluster positions bundled with
 *  the ADC values of the strips contributing to the cluster.
 *  The latter method uses more CPU and memory.  
 *  So only use it when you really need the ADCs.  
 *  Otherwise (e.g. in the trigger) stick to the first method.
 *
 *  One instance of this class can decode exactly one bank, the
 *  one provided in the constructor argument.
 *
 *  This class is intentionally as independent from the LHCb/Gaudi
 *  framework as possible (it only uses some classes in Si/SiDAQ
 *  SiDAQ which are in turn independent of the framework).  
 *  This is because it will be used in standalone testing of Velo DAQ. 
 *  Don't break it.
 *
 *  @see SiCLusterWord
 *  @see SiHeaderWord
 *  @see SiADCWord
 * 
 *  @author Kurt Rinnert
 *  @date   2006-02-22
 */
template<class CLUSTERWORD>
class SiRawBankDecoder {
public: 

  // live and death

  /** Constructor  
   *  Takes constant pointer to beginning of data content
   *  of a raw Si bank.  The Constructor is non trrivial,
   *  it will imediately decode the bank header bytes.  So
   *  if you don't need it, don't create it.
   */
  SiRawBankDecoder(const SiDAQ::buffer_word* bank) :
    m_bank(bank),
    m_header(SiHeaderWord(bank[0])),
    m_nClusters(m_header.nClusters()),
    m_posBegin(pos_iterator(0,this)),
    m_posEnd(pos_iterator(m_nClusters,this)), 
    m_posAdcBegin(posadc_iterator(0,this)),
    m_posAdcEnd(posadc_iterator(m_nClusters,this))
  { 
    ;
  } 

  ~SiRawBankDecoder() { ; } 

  // shortcuts
  
  /** @class SiRawBankDecoder::SiDecodedCluster SiRawBankDecoder.h
   *  
   *  Simple type wrapper
   *
   *  This is just a simple type wrapper for a pair of a
   *  SiClusterWord and a vector of corresponding SiADCWords.
   *
   *  @see SiClusterWord
   *  @see SiADCWord
   * 
   *  @author Kurt Rinnert
   *  @date   2006-02-22
   */
  typedef std::pair<CLUSTERWORD,std::vector<SiADCWord> > SiDecodedCluster;

  // decoding iterators

  /** @class SiRawBankDecoder::pos_iterator SiRawBankDecoder.h
   *  
   *  Decoding iterator for cluster positions only
   *
   *  This smart iterator allows you to traverse a the cluster
   *  positions in the raw bank and decode them at the same
   *  time.  Use this if you do not need the ADC counts and 
   *  speed is a concern (e.g. in the trigger).
   *
   *  @see SiClusterWord
   * 
   *  @author Kurt Rinnert
   *  @date   2006-02-22
   */
  class pos_iterator
  {
  public:
    
    // live and death
    
    pos_iterator() { ; }
    pos_iterator(const pos_iterator& init) : 
      m_pos(init.m_pos),
      m_decoder(init.m_decoder),
      m_cluster(init.m_cluster)
    { ; } 

    ~pos_iterator() { ; }

    // operators

    /// assignment
    const pos_iterator& operator=(const pos_iterator& rhs) 
    { 
      m_pos     = rhs.m_pos; 
      m_decoder = rhs.m_decoder;
      m_cluster = rhs.m_cluster; 

      return *this;
    }

    /**  Increment
     *   The implementatio of this increment operator
     *   is non-trivial.  It reads the next cluster
     *   position word from the raw bank and decodes it.
     *
     */
    const pos_iterator& operator++() const; 

    /** Equal 
     *  True if positions match 
     */
    bool operator==(const pos_iterator& rhs) const { return m_pos == rhs.m_pos; }

    /** Not Equal 
     *  True if positions do not match 
     */
    bool operator!=(const pos_iterator& rhs) const { return m_pos != rhs.m_pos; }

    // dereferencing

    /**  dereference to reference to SiCLusterWord instance
     *   Gives access to the cluster word.  Note that the return value
     *   is a const reference.  The object it refers to does no longer
     *   exist once the iterator goes out of scope.  So if you need to hold
     *   a copy longer than that you have to initialise an instance. 
     *   I am well aware that this is a somewhat dangerous behaviour.  However,
     *   this is performance critical code.  So please simply refrain from
     *   creating instances and be aware of the lifetime of the returned
     *   reference.
     *
     * @see SiClusterWord
     */
    const CLUSTERWORD& operator*() const { return m_cluster; } 

    /**  dereference to pointer
     *   Gives access to pointer to cluster word
     *
     * @see SiClusterWord
     */
    const CLUSTERWORD* operator->() const { return &m_cluster; } 

  private:

    /**  Construct with position in raw bank reference to decoder
     *   Only friends (i.e. the decoder class) are allowed
     *   to do that.  If applicable, the position will be
     *   decoded.
     *
     * @see SiRawBankDecoder
     */
    pos_iterator(unsigned int pos, const SiRawBankDecoder* decoder);

    /** Decode cluster position
     *  This method decodes the cluster position from
     *  the raw bank into a SiClusterWord.
     *
     * @see SiClusterWord
     */
    void decode() const;

  private:
    mutable unsigned int m_pos;
    const SiRawBankDecoder* m_decoder;
    mutable CLUSTERWORD m_cluster; 

    friend class SiRawBankDecoder;
  };
  friend class pos_iterator;

  /** @class SiRawBankDecoder::posadc_iterator SiRawBankDecoder.h
   *  
   *  Decoding iterator for cluster positions only
   *
   *  This smart iterator allows you to traverse a the cluster
   *  positions and ADC counts in the raw bank and decode them at the same
   *  time.  Use this only if you really need the ADC counts an speed
   *  is not a concern.
   *
   *  @see SiDecodedCluster
   *  @see SiClusterWord
   *  @see SiADCWord
   * 
   *  @author Kurt Rinnert
   *  @date   2006-02-22
   */
  class posadc_iterator
  {
  public:
    
    // live and death
    
    posadc_iterator() { ; }
    posadc_iterator(const posadc_iterator& init) : 
      m_pos(init.m_pos),
      m_nADC(init.m_nADC),
      m_ADC32(init.m_ADC32),
      m_offset(init.m_offset),
      m_decoder(init.m_decoder),
      m_cluster(init.m_cluster)
    { ; } 

    ~posadc_iterator() { ; }

    // operators

    /// assignment
    const posadc_iterator& operator=(const posadc_iterator& rhs) 
    { 
      m_pos     = rhs.m_pos; 
      m_nADC    = rhs.m_nADC;
      m_ADC32   = rhs.m_ADC32;
      m_offset  = rhs.m_offset; 
      m_decoder = rhs.m_decoder;
      m_cluster = rhs.m_cluster; 

      return *this;
    }

    /**  Increment
     *   The implementatio of this increment operator
     *   is non-trivial.  It reads the next cluster
     *   position word from the raw bank and decodes it.
     *
     */
    const posadc_iterator& operator++() const; 

    /** Equal 
     *  True if positions match 
     */
    bool operator==(const posadc_iterator& rhs) const { return m_pos == rhs.m_pos; }

    /** Not Equal 
     *  True if positions do not match 
     */
    bool operator!=(const posadc_iterator& rhs) const { return m_pos != rhs.m_pos; }

    // dereferencing

    /**  dereference to reference to SiDecodedCluster instance
     *   Gives access to the decoded cluster.  Note that the return value
     *   is a const reference.  The object it refers to does no longer
     *   exist once the iterator goes out of scope.  So if you need to hold
     *   a copy longer than that you have to initialise an instance. 
     *   I am well aware that this is a somewhat dangerous behaviour.  However,
     *   this is performance critical code.  So please simply refrain from
     *   creating instances and be aware of the lifetime of the returned
     *   reference.
     * 
     * @see SiDecodedCluster
     */
    const SiDecodedCluster& operator*() const { return m_cluster; } 

    /**  dereference to pointer
     *   Gives access to pointer to decoded cluster
     *
     * @see SiDecodedCluster
     */
    const SiDecodedCluster* operator->() const { return &m_cluster; } 

  private:

    /**  Construct with position in raw bank and reference to decoder
     *   Only friends (i.e. the decoder class) are allowed
     *   to do that.  If applicable, the position is decoded.  
     *
     * @see SiRawBankDecoder
     */
    posadc_iterator(unsigned int pos, const SiRawBankDecoder* decoder);

    /** Decode cluster position and ADC counts
     *  This method wraps the call to the bank type specific
     *  doDecode() methods.
     *
     * @see SiDecodedCluster
     */
    void decode() const;

    /** Decoder for ADC bank with ADC counts only
     *  The actual decoding for ADC only banks like
     *  specified for the Velo
     */
    void doDecode(SiDAQ::adc_only_bank_tag) const;

    /** Decoder for ADC bank with neighboursum  
     *  The actual decoding for ADC banks with prepended 
     *  neighbour sums like specified for the ST
     */
    void doDecode(SiDAQ::adc_neighboursum_bank_tag) const;

    /** Deconding common to all ADC bank types
     *  The part of the decoding for ADC banks which
     *  independent of the specific ADC bank type, i.e.
     *  the common between Velo and ST
     */
    void doDecodeCommon() const;



  private:
    mutable unsigned int m_pos;
    mutable unsigned int m_nADC;
    mutable unsigned int m_ADC32;
    unsigned int m_offset;
    const SiRawBankDecoder* m_decoder;
    mutable  SiDecodedCluster m_cluster; 

    friend class SiRawBankDecoder;
  };
  friend class posadc_iterator;


  // accessors

  /** Access decoded bank header  
   *
   * @see SiHeaderWord
   */
  const SiHeaderWord& header() const { return m_header; }

  /** Access number of clusters  
   *  Yields the total number of clusters encoded
   *  in the raw bank.
   * 
   * @return numbert of clusters in raw bank
   */
  unsigned int nClusters() const { return m_nClusters; }

  /// start iterator for cluster positions
  const pos_iterator& posBegin() const { return m_posBegin; }

  /// end iterator for cluster positions
  const pos_iterator& posEnd() const { return m_posEnd; }

  /// start iterator for decoded clusters with ADC values
  const posadc_iterator& posAdcBegin() const { return m_posAdcBegin; }

  /// end iterator for decoded clusters with ADC values
  const posadc_iterator& posAdcEnd() const { return m_posAdcEnd; }

private:
  const SiDAQ::buffer_word*  m_bank;
  const SiHeaderWord  m_header;
  const unsigned int m_nClusters;

  const pos_iterator    m_posBegin;
  const pos_iterator    m_posEnd;
  const posadc_iterator m_posAdcBegin;
  const posadc_iterator m_posAdcEnd;

};


//-----------------------------------------------------------------------------
// Implementations for class template: SiRawBankDecoder<CLUSTERWORD>
//
// 2006-02-22 : Kurt Rinnert
//-----------------------------------------------------------------------------

// position iterator
template<class CLUSTERWORD>
inline SiRawBankDecoder<CLUSTERWORD>::pos_iterator::pos_iterator
(unsigned int pos, const SiRawBankDecoder<CLUSTERWORD>* decoder) : 
  m_pos(pos),
  m_decoder(decoder)
{ 
  decode(); 
}

template<class CLUSTERWORD>
inline void SiRawBankDecoder<CLUSTERWORD>::pos_iterator::decode() const
{
  if (m_pos < m_decoder->m_nClusters) m_cluster = CLUSTERWORD((m_decoder->m_bank[1+m_pos/2] >> ((m_pos%2) << 4) & 0x0000FFFF));
  return;
}

template<class CLUSTERWORD>
const typename SiRawBankDecoder<CLUSTERWORD>::pos_iterator& 
SiRawBankDecoder<CLUSTERWORD>::pos_iterator::operator++() const
{
  if (*this != m_decoder->m_posEnd) {
    ++m_pos;
    decode();
  }

  return *this; 
}

// position and ADC cluster iterator

template<class CLUSTERWORD>
inline SiRawBankDecoder<CLUSTERWORD>::posadc_iterator::posadc_iterator
(unsigned int pos, const SiRawBankDecoder<CLUSTERWORD>* decoder) : 
      m_pos(pos),
      m_nADC(0),
      m_ADC32(0),
      m_offset(1+ decoder->m_nClusters/2 + decoder->m_nClusters%2),
      m_decoder(decoder)
{ 
  decode(); 
}

template<class CLUSTERWORD>
inline void SiRawBankDecoder<CLUSTERWORD>::posadc_iterator::decode() const
{
  if (m_pos < m_decoder->m_nClusters) doDecode(typename CLUSTERWORD::adc_bank_type());
  return;
}

template<class CLUSTERWORD>
inline void SiRawBankDecoder<CLUSTERWORD>::posadc_iterator::doDecode(SiDAQ::adc_only_bank_tag) const
{
  m_cluster.second.clear();
  doDecodeCommon();
  return;
}

template<class CLUSTERWORD>
inline void SiRawBankDecoder<CLUSTERWORD>::posadc_iterator::doDecode(SiDAQ::adc_neighboursum_bank_tag) const
{
  m_cluster.second.clear();

 // fetch the neighbour sum first
  m_ADC32 = (m_nADC%4 ? m_ADC32 : m_decoder->m_bank[m_offset+m_nADC/4]);
  m_cluster.second.push_back(SiADCWord((m_ADC32 >> ((m_nADC%4)<<3)) & 0x000000FF));
  ++m_nADC;

  doDecodeCommon();
  return;
}

template<class CLUSTERWORD>
void SiRawBankDecoder<CLUSTERWORD>::posadc_iterator::doDecodeCommon() const
{
  // get the first adc count *without* checking the end-of-cluster bit
  m_ADC32 = (m_nADC%4 ? m_ADC32 : m_decoder->m_bank[m_offset+m_nADC/4]);
  m_cluster.second.push_back(SiADCWord((m_ADC32 >> ((m_nADC%4)<<3)) & 0x000000FF));
  ++m_nADC;

  // only move on if the end-of-cluster bit is not set
  while (!m_cluster.second.back().endCluster()) {
    m_ADC32 = (m_nADC%4 ? m_ADC32 : m_decoder->m_bank[m_offset+m_nADC/4]);
    m_cluster.second.push_back(SiADCWord((m_ADC32 >> ((m_nADC%4)<<3)) & 0x000000FF));
    ++m_nADC;
  }

  // get cluster position
  m_cluster.first = CLUSTERWORD((m_decoder->m_bank[1+m_pos/2] >> ((m_pos%2) << 4)) & 0x0000FFFF);

  return;
}

template<class CLUSTERWORD>
const typename SiRawBankDecoder<CLUSTERWORD>::posadc_iterator& 
SiRawBankDecoder<CLUSTERWORD>::posadc_iterator::operator++() const
{
  if (*this != m_decoder->m_posAdcEnd) {
    ++m_pos;
    decode();
  }
  
  return *this; 
}

#endif // SIRAWBANKDECODER_H
