// $Id: STTell1Board.h,v 1.9 2008-08-15 08:01:06 mneedham Exp $
#ifndef _STTell1Board_H
#define _STTell1Board_H 1

#include "Kernel/STTell1ID.h"
#include "Kernel/STChannelID.h"
#include <vector>
#include <iostream>

#include "Event/STCluster.h"

/** @class STTell1Board STTell1Board.h "STDAQ/STTell1Board.h"
 *
 *  Class for converting from STChannelID to daq channel/STTell1ID
 *
 *  @author M.Needham
 *  @date   5/01/2004
 */


class STTell1Board{

public:

  typedef std::pair<LHCb::STChannelID,int> chanPair;

  /// constructer
  STTell1Board(const STTell1ID aBoard, 
               const unsigned int stripsPerHybrid);

  /// destructer
  virtual ~STTell1Board();

  /// add wafer
  void addSector(LHCb::STChannelID aOfflineChan, unsigned int orientation, const std::string& serviceBox);

  /// board indentifer
  STTell1ID boardID() const;

  /// same board id
  bool sameID(const STTell1ID& id) const;
 
  /// channel is in this board
  bool isInside(const LHCb::STChannelID aOfflineChan,
                unsigned int& sectorIndex) const;

  /// construct LHCb::STChannelID from DAQ Channel
  chanPair DAQToOffline(const unsigned int fracStrip,
			const int version,
                        const unsigned int aDAQChan) const;

    /** construct LHCb::STChannelID from 
    *   beetle [0 - 23]
    *   port [0 -3]
    *   strip [0 -31]
    */
    STTell1Board::chanPair BeetleRepDAQToOffline(const unsigned int fracStrip,
					const int version,
                                        const unsigned int beetle,
                                        unsigned int port = 0,
					 unsigned int strip = 0) const;

   /** construct LHCb::STChannelID from 
   *   pp [0-3]
   *   beetle [0 - 5]
   *   port [0 -3]
   *   strip [0 -31]
   */
   STTell1Board::chanPair PPRepDAQToOffline(const unsigned int fracStrip,
 			              const int version,
                                      const unsigned int pp,
                                      unsigned int beetle =0,
                                      unsigned int port = 0,
                                      unsigned int strip = 0) const;

  /// fill adc values offline 
    void ADCToOffline(const unsigned int aDAQChan,
		      LHCb::STCluster::ADCVector& adcs,
		      const int version,
		      const unsigned int offset,
		      const double interStripPos) const;
  

  /// construct DAQChannel from LHCb::STChannelID
  unsigned int offlineToDAQ(const LHCb::STChannelID aOfflineChan,
                            const unsigned int sectorIndex,
			    double isf) const;

  /// check channel is valid
  bool validChannel(const unsigned int daqChan) const;

  /// vector of sectors on the board
  const std::vector<LHCb::STChannelID>& sectorIDs() const;
  
  /// vector of hybrid orientations
  const std::vector<int>& orientation() const;

  /// vector of service boxes
  const std::vector<std::string>& serviceBoxes() const;

  /// service box
  std::string serviceBox(const LHCb::STChannelID& chan) const; 

  /// service box
  std::string BeetleRepServiceBox(const unsigned int beetle) const;

  /// service box
  std::string PPRepServiceBox(const unsigned int pp, const unsigned int beetle) const;

  /// number of readout sectors
  unsigned int nSectors() const;

  /// Operator overloading for stringoutput
  friend std::ostream& operator<< (std::ostream& s, const STTell1Board& obj)
  {
    return obj.fillStream(s);
  }


  // Fill the ASCII output stream
  virtual std::ostream& fillStream(std::ostream& s) const;

private:

  /// service box
  std::string serviceBox(const unsigned int& waferIndex) const; 

  STTell1ID m_boardID;
  unsigned int m_nStripsPerHybrid;
  std::vector<LHCb::STChannelID> m_sectorsVector;
  std::vector<int> m_orientation;
  std::vector<std::string> m_serviceBoxVector;
};

inline STTell1ID STTell1Board::boardID() const{
  return m_boardID;
}

inline bool STTell1Board::sameID(const STTell1ID& id) const{
  return m_boardID == id ? true : false;
}

inline const std::vector<int>& STTell1Board::orientation() const{
  return m_orientation;
}

inline const std::vector<std::string>& STTell1Board::serviceBoxes() const{
  return m_serviceBoxVector;
}

inline const std::vector<LHCb::STChannelID>& STTell1Board::sectorIDs() const{
  return m_sectorsVector;
}

inline std::string STTell1Board::serviceBox(const unsigned int& waferIndex) const {
  return (waferIndex < m_serviceBoxVector.size() ? m_serviceBoxVector[waferIndex] : std::string("Unknown"));
}

inline std::string STTell1Board::serviceBox(const LHCb::STChannelID& chan) const{
  unsigned int waferIndex;
  isInside(chan, waferIndex);
  return serviceBox(waferIndex);
}

inline bool STTell1Board::validChannel(const unsigned int daqChan) const{
  if (daqChan > m_nStripsPerHybrid*m_sectorsVector.size()) return false;
  const int index = daqChan/m_nStripsPerHybrid; 
  if (m_sectorsVector[index].sector() == 0 ) return false;
  return true; 
}
  
inline unsigned int STTell1Board::nSectors() const{
  return m_sectorsVector.size();
}

#endif // _STTell1Board_H
