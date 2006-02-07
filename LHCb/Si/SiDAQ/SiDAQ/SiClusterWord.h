// $Id: SiClusterWord.h,v 1.2 2006-02-07 08:48:06 mneedham Exp $
#ifndef _SiClusterWord_H
#define _SiClusterWord_H 1

/** @class SiClusterWord SiClusterWord.h "STDAQ/SiClusterWord.h"
 *
 *  Class for encapsulating word in RAW data format
 *  for the Si detectors. 
 *
 *  @author M.Needham
 *  @date   18/10/2005
 */


#include <iostream>

template <class bitPolicy>
class SiClusterWord{

public:

  /** constructer with chan + number of ADC values
  @param channel
  @param interstrip position
  @param cluster size
  @param high threshold bit
  */
  SiClusterWord(unsigned int aChan,
                double interStripPos, 
                unsigned int clusterSize,
                bool highThreshold );

  /** constructer with int
  @param value 
  */
  explicit SiClusterWord(unsigned int value):
  m_value(value){}

  /** Default Constructor */
  SiClusterWord()
    : m_value(0) {}

  /** Destructor */
  virtual ~SiClusterWord() {}

  /** cast to short*/
  operator short() const;
 
  /** Retrieve value
  @return value
  */
  unsigned int value() const;

  /** return channel 
  @return channelID
  */
  unsigned int channelID() const;


  /** cluster size
  can be 1, 2, 3 (= >2) 
  @return size
  */
  unsigned int pseudoSize() const;

  /** interstrip position 
  @return fraction of strip
  */
  double interStripPosition() const;

  /** high threshold
  @return fraction of strip
  */
  bool hasHighThreshold() const;

  /** Operator overloading for stringoutput */
  friend std::ostream& operator<< (std::ostream& s, const SiClusterWord& obj)
  {
    return obj.fillStream(s);
  }

  /** Fill the ASCII output stream */
  virtual std::ostream& fillStream(std::ostream& s) const;

  /** fracStrip Bits  - the actual value in the bits 1,2,3,4,,,*/
  unsigned int fracStripBits() const; 
 
  /** size Bits  - the actual value in the bits 0,1 */
  unsigned int pseudoSizeBits() const; 
 
private:

  short m_value; /// Packed word
};

template <class bitPolicy>
SiClusterWord<bitPolicy>::SiClusterWord(unsigned int aChan,
                             double interStripPos,
                             unsigned int clusterSize,
                             bool highThreshold )
:m_value(0){
 
  // constructer
  unsigned int position = (unsigned int)(interStripPos*(1<<bitPolicy::interStripPrecision));
  unsigned int sBit =0;
  if (clusterSize > 2) sBit = 1;
  unsigned int hThreshold = (unsigned int)(highThreshold);
 
  m_value  = (position << bitPolicy::positionBits) +
             (aChan << bitPolicy::channelBits) +
             (sBit  << bitPolicy::sizeBits) +
             (hThreshold << bitPolicy::thresBits);
 }

template <class bitPolicy>
inline  SiClusterWord<bitPolicy>::operator short() const
{
  return m_value;
}

template <class bitPolicy>
inline unsigned int SiClusterWord<bitPolicy>::value() const
{
  return m_value;
}

template <class bitPolicy>
inline unsigned int SiClusterWord<bitPolicy>::fracStripBits() const{
  return (m_value & bitPolicy::positionMask) >> bitPolicy::positionBits;
}

template <class bitPolicy>
inline unsigned int SiClusterWord<bitPolicy>::pseudoSizeBits() const{
  return (m_value & bitPolicy::sizeMask) >> bitPolicy::sizeBits;
}

template <class bitPolicy>
inline double SiClusterWord<bitPolicy>::interStripPosition() const{
  return fracStripBits()/double(1<<(bitPolicy::interStripPrecision));
}

template <class bitPolicy>
inline unsigned int SiClusterWord<bitPolicy>::channelID() const{
  return (m_value & bitPolicy::channelMask) >> bitPolicy::channelBits;
}

template <class bitPolicy>
inline unsigned int SiClusterWord<bitPolicy>::pseudoSize() const{
  unsigned int cSize  = 1 + ((m_value & bitPolicy::sizeMask) >> bitPolicy::sizeBits);
  if (cSize == 1){
    if (fracStripBits() > 0) cSize = 2;
  }
  else {
    cSize = 3;
  }
  return cSize;
}

template <class bitPolicy>
inline bool SiClusterWord<bitPolicy>::hasHighThreshold() const{
  return ((m_value & bitPolicy::thresMask) >>bitPolicy::thresBits);
}

template <class bitPolicy>
inline std::ostream& SiClusterWord<bitPolicy>::fillStream(std::ostream& s) const
{
  s << "{ "
    << " channelID:\t" << channelID() << std::endl
    << " pseudo cluster size:\t" << pseudoSize() << std::endl 
    << " interStrip position :\t" << interStripPosition() << std::endl 
    << " high threshold:\t" << hasHighThreshold() << " } ";
  return s;
}

#endif // _SiClusterWord_H
