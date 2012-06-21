#ifndef VL_RAWBANK_DECODER_H 
#define VL_RAWBANK_DECODER_H 1

// STL
#include <string>
// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
// Event/DigitEvent
#include "Event/VLCluster.h"
#include "Event/VLLiteCluster.h"
// Local
#include "VLRawBankConstants.h"

namespace LHCb {
  class RawBank;
}

class DeVL;

/** @class VLRawBankDecoder VLRawBankDecoder.h
 *
 */

class VLRawBankDecoder : public GaudiAlgorithm {
public:
  /// Standard constructor
  VLRawBankDecoder(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~VLRawBankDecoder() {}

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute();       ///< Algorithm execution
  virtual StatusCode finalize();      ///< Algorithm finalisation

  enum AlgStatusType { 
    OK              =  0,
    BadSensorNumber =  1,
    CorruptBuffer   =  2,
    HeaderErrorBit  =  3,
    Other           = 99
  };
    
private:

  /// Decode raw bank to clusters and lite clusters. 
  void decode(const LHCb::RawBank* rb);

  bool m_decodeToLiteClusters;
  bool m_decodeToClusters;

  std::string m_rawEventLocation;
  std::string m_liteClusterLocation;
  std::string m_clusterLocation;

  LHCb::VLLiteCluster::VLLiteClusters* m_liteClusters;
  LHCb::VLClusters* m_clusters;
  const DeVL* m_det;

};
#endif 
