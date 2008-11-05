// $Id: RawBankToSTLiteClusterAlg.cpp,v 1.34 2008-11-05 15:31:15 mneedham Exp $


#include <algorithm>
#include <vector>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "RawBankToSTLiteClusterAlg.h"
#include "Kernel/ISTReadoutTool.h"

#include "LHCbMath/LHCbMath.h"

// Event
#include "Event/RawEvent.h"
#include "Event/STLiteCluster.h"


#include "Kernel/STTell1Board.h"
#include "Kernel/STTell1ID.h"
#include "Kernel/STDAQDefinitions.h"
#include "Kernel/STDecoder.h"
#include "Kernel/STDetSwitch.h"
#include "Kernel/STDataFunctor.h"
#include "Kernel/StripRepresentation.h"

#include "Kernel/FastClusterContainer.h"

#include "boost/lexical_cast.hpp"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : RawBufferToSTLiteClusterAlg
//
// 2004-01-07 : Matthew Needham   
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( RawBankToSTLiteClusterAlg );

RawBankToSTLiteClusterAlg::RawBankToSTLiteClusterAlg( const std::string& name,
                                           ISvcLocator* pSvcLocator ):
STDecodingBaseAlg (name , pSvcLocator){
 

 // Standard constructor, initializes variables
  declareSTConfigProperty( "clusterLocation", m_clusterLocation , STLiteClusterLocation::TTClusters);
  declareSTConfigProperty("BankType", m_bankTypeString , detType() );
}

RawBankToSTLiteClusterAlg::~RawBankToSTLiteClusterAlg() {
  // Destructor
};

StatusCode RawBankToSTLiteClusterAlg::initialize() {

  // Initialization
  StatusCode sc = STDecodingBaseAlg::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  STDetSwitch::flip(detType(),m_clusterLocation); 
  
  return StatusCode::SUCCESS;
}
    
StatusCode RawBankToSTLiteClusterAlg::execute() {

  STLiteCluster::STLiteClusters* fCont = new STLiteCluster::STLiteClusters();
  fCont->reserve(5000);  
  put(fCont, m_clusterLocation);

  if (!validSpill()) {
    return Warning("Not a valid spill",StatusCode::SUCCESS, 1);
  }

  // Retrieve the RawEvent:
  RawEvent* rawEvt = get<RawEvent>(m_rawEventLocation);

 
  // decode banks
  StatusCode sc = decodeBanks(rawEvt, fCont);   
  if (sc.isFailure()){
    return Error("Problems in decoding event skipped", sc);
  }

  return sc;
};


StatusCode RawBankToSTLiteClusterAlg::decodeBanks(RawEvent* rawEvt,STLiteCluster::STLiteClusters* fCont) const{

  const std::vector<RawBank* >&  tBanks = rawEvt->banks(bankType()); 
  std::vector<unsigned int> missing = missingInAction(tBanks);
  if ( missing.empty() == false ){
    counter("lost Banks") += missing.size() ;
    if (tBanks.size() == 0){
      ++counter("no banks found");
      return StatusCode::SUCCESS;
    }
  } 


  const unsigned int pcn = pcnVote(tBanks);
  debug() << "PCN was voted to be " << pcn << endmsg;
  if (pcn == STDAQ::inValidPcn) {
    counter("skipped Banks") += tBanks.size();
    return Warning("PCN vote failed", StatusCode::SUCCESS,2);
  }
  
  // loop over the banks of this type..
  std::vector<RawBank* >::const_iterator iterBank =  tBanks.begin();
  for (; iterBank != tBanks.end() ; ++iterBank){

    ++counter("# valid banks");

    // get the board and data
    STTell1Board* aBoard = readoutTool()->findByBoardID(STTell1ID((*iterBank)->sourceID()));
    if (!aBoard && !m_skipErrors){
      std::string invalidSource = "Invalid source ID --> skip bank"+
	 boost::lexical_cast<std::string>((*iterBank)->sourceID());  
      Warning(invalidSource,StatusCode::SUCCESS,2); 
      ++counter("skipped Banks");
      continue;
    } 

   ++counter("# valid source ID");

   if ((*iterBank)->magic() != RawBank::MagicPattern) {
      std::string pattern = "wrong magic pattern "+
	 boost::lexical_cast<std::string>((*iterBank)->sourceID());  
      Warning(pattern, StatusCode::SUCCESS,2);
      counter("skipped Banks") += tBanks.size();
      continue;
    }
 
    // make a SmartBank of shorts...
    STDecoder decoder((*iterBank)->data());
   
    // get number of clusters..
    if (decoder.hasError() == true && !m_skipErrors){
      std::string errorBank = "bank has errors, skip, sourceID"+
	boost::lexical_cast<std::string>((*iterBank)->sourceID());
      Warning(errorBank, StatusCode::SUCCESS,2);
      ++counter("skipped Banks");
      continue;
    }

    const unsigned bankpcn = decoder.header().pcn();
    if (pcn != bankpcn && !m_skipErrors){
      std::string errorBank = "PCNs out of sync sourceID "+
	boost::lexical_cast<std::string>((*iterBank)->sourceID());
      debug() << "Expected " << pcn << " found " << bankpcn << endmsg;
      Warning(errorBank, StatusCode::SUCCESS,2);
      ++counter("skipped Banks");
      continue; 
    }

    const STDAQ::version bankVersion = forceVersion() ? STDAQ::version(m_forcedVersion): STDAQ::version((*iterBank)->version());

    // check the integrity of the bank --> always skip if not ok
    if (!m_skipErrors && checkDataIntegrity(decoder, aBoard , (*iterBank)->size() , bankVersion) == false) continue;

    // read in the first half of the bank
    STDecoder::pos_iterator iterDecoder = decoder.posBegin();
    for ( ;iterDecoder != decoder.posEnd(); ++iterDecoder){
      
      STClusterWord aWord = *iterDecoder;
      unsigned int fracStrip = aWord.fracStripBits();
      
      STTell1Board::chanPair chan = aBoard->DAQToOffline(fracStrip, bankVersion, STDAQ::StripRepresentation(aWord.channelID()));

      STLiteCluster liteCluster(chan.second,
                                aWord.pseudoSizeBits(),
                                aWord.hasHighThreshold(),
                                chan.first);

      fCont->push_back(liteCluster);

    } //decoder
      
  } // iterBank

  // sort and remove any duplicates
  std::stable_sort(fCont->begin(),fCont->end(), Less_by_Channel());
  STLiteCluster::STLiteClusters::iterator iter =  std::unique(fCont->begin(), fCont->end(), Equal_Channel()); 
  if (iter != fCont->end()){
    const unsigned int nvalid = iter - fCont->begin();
    fCont->resize(nvalid);    
    return Warning("Removed duplicate clusters in the decoding", StatusCode::SUCCESS, 100);
  }

  return StatusCode::SUCCESS;

}


StatusCode RawBankToSTLiteClusterAlg::finalize() {

  const double failed = counter("skipped Banks").flag();
  const double processed = counter("# valid banks").flag();
  double eff = 0.0;
  if (!LHCb::Math::Equal_To<double>()(processed, 0.0) == true){   
    eff = 1.0 - (failed/processed); 
  }
  info() << "Successfully processed " << 100* eff << " %"  << endmsg;
    
  return STDecodingBaseAlg::finalize();
}

