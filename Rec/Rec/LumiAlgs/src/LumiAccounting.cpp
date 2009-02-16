// $Id: LumiAccounting.cpp,v 1.1.1.1 2009-02-16 16:28:12 panmanj Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IOpaqueAddress.h"

// event model
#include "Event/RawEvent.h"
#include "Event/ODIN.h"
#include "Event/HltLumiSummary.h"
#include "Event/LumiFSR.h"
#include "Event/LumiCounters.h"

// local
#include "LumiAccounting.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LumiAccounting
//
// 2009-01-19 : Jaap Panman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( LumiAccounting );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
LumiAccounting::LumiAccounting( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )

{
  declareProperty( "RawEventLocation"  ,  m_rawEventLocation = LHCb::RawEventLocation::Default );
  declareProperty( "InputDataContainer",  m_DataName = LHCb::HltLumiSummaryLocation::Default );
  // it is assumed that we are only called for a single BXType and that the
  // output data container gets this name
  declareProperty( "OutputDataContainer", m_FSRName = LHCb::LumiFSRLocation::Default );
  
}
//=============================================================================
// Destructor
//=============================================================================
LumiAccounting::~LumiAccounting() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode LumiAccounting::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  // get MsgStream
  sc = service("RunRecordDataSvc", m_runRecordSvc, true);
  if( sc.isFailure() ) {
    if ( msgLevel(MSG::ERROR) ) error() << "Unable to retrieve run records service" << endreq;
    return sc;
  }

  // file counting 
  m_current_fname = "";
  m_count_files = 0;

  // prepare TDS for FSR
  m_lumiFSRs = new LHCb::LumiFSRs();
  m_lumiFSR = 0;
  put(m_runRecordSvc, m_lumiFSRs, m_FSRName);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode LumiAccounting::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // registry from raw data - only correct if file catalogue used 
  std::string event_fname;
  if( !exist<LHCb::RawEvent>(m_rawEventLocation) ){
    return Warning("RawBank cannot be loaded", StatusCode::SUCCESS);
  } else {
    LHCb::RawEvent* event = get<LHCb::RawEvent>(m_rawEventLocation);
    IOpaqueAddress* eAddr = event->registry()->address();
    // obtain the fileID
    if ( eAddr ) {
      event_fname = eAddr->par()[0];
      if ( msgLevel(MSG::DEBUG) ) debug() << "RunInfo record from Event: " << event_fname << endmsg;
    } else {
      return Error("Registry cannot be loaded from Event", StatusCode::SUCCESS);
    }
  }

  // get data container
  if( !exist<LHCb::HltLumiSummary>(m_DataName) ){
    // not sure that this is an error - physics events do not have the lumi bank
    return Warning("LumiSummary cannot be loaded", StatusCode::SUCCESS);
  }
  if ( msgLevel(MSG::DEBUG) ) debug() << "LumiSummary loaded!!" << endmsg;
  LHCb::HltLumiSummary* the_hltLumiSummary = get<LHCb::HltLumiSummary>(m_DataName);

  // get ODIN
  LHCb::ODIN* odin;
  if( exist<LHCb::ODIN>(LHCb::ODINLocation::Default) ){
    odin = get<LHCb::ODIN> (LHCb::ODINLocation::Default);
  }else{
    // should remain flagged as an error and stop the run
    return Error("ODIN cannot be loaded", StatusCode::FAILURE);
  }
  // obtain the run number from ODIN
  unsigned int run = odin->runNumber();  
  if ( msgLevel(MSG::DEBUG) ) debug() << "ODIN RunNumber: " << run << endmsg;

  // check if the file ID is new
  std::string fname = "";
  if ( fname != event_fname ) {
    fname = event_fname;
    if ( fname != m_current_fname ) {
      // this is a new file
      m_count_files++;
      m_current_fname = fname;
      std::string runRecordName = fname+"/EOR";
      if ( msgLevel(MSG::DEBUG) ) debug() << "RunInfo record: " << fname << endmsg;
      // create a new FSR and append to TDS
      m_lumiFSR = new LHCb::LumiFSR();
      m_lumiFSRs->insert(m_lumiFSR);
    }
  }

  // insert new fileID only
  if ( !m_lumiFSR->hasFileID(fname) ) m_lumiFSR->addFileID(fname);
  // insert new run only
  if ( !m_lumiFSR->hasRunNumber(run) ) m_lumiFSR->addRunNumber(run);
  // increment FSR with summary info
  LHCb::HltLumiSummary::ExtraInfo::iterator summaryIter;
  LHCb::HltLumiSummary::ExtraInfo  summaryInfo = the_hltLumiSummary->extraInfo();
  for (summaryIter = summaryInfo.begin(); summaryIter != summaryInfo.end(); summaryIter++) {
    // get the key and value of the input info
    int key = summaryIter->first;
    int value = summaryIter->second;
    // increment!
    m_lumiFSR->incrementInfo(key, value);
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode LumiAccounting::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  // some printout of FSRs
  if ( msgLevel(MSG::INFO) ) {
    info() << "number of files seen: " << m_count_files << endmsg;
    // FSR - use the class method which prints it
    LHCb::LumiFSRs::iterator fsr;
    LHCb::LumiFSR* sumFSR = new LHCb::LumiFSR();
    for ( fsr = m_lumiFSRs->begin(); fsr != m_lumiFSRs->end(); fsr++ ) {
      // sum up the information
      *sumFSR = *sumFSR + *(*fsr);
      // print the individual FSR
      info() << "FSR: " << *(*fsr) << endmsg; 
    }
    // print the integral
    info() << "INTEGRAL: " << *sumFSR << endmsg; 
  }

  // check if the FSRs can be retrieved from the TS
  if ( msgLevel(MSG::DEBUG) ) {
    LHCb::LumiFSRs* readFSRs = get<LHCb::LumiFSRs>(m_runRecordSvc, m_FSRName);
    LHCb::LumiFSRs::iterator fsr;
    for ( fsr = readFSRs->begin(); fsr != readFSRs->end(); fsr++ ) {
      // print the FSR just retrieved from TS
      debug() << "READ FSR: " << *(*fsr) << endmsg; 

      // print also the contents using the builtin lookup tables
      LHCb::LumiFSR::ExtraInfo::iterator infoIter;
      LHCb::LumiFSR::ExtraInfo  fsrInfo = (*fsr)->extraInfo();
      for (infoIter = fsrInfo.begin(); infoIter != fsrInfo.end(); infoIter++) {
	// get the key and value of the input info
	int key = infoIter->first;
	LHCb::LumiFSR::ValuePair values = infoIter->second;
	int incr = values.first;
	longlong count = values.second;
	const std::string keyName = LHCb::LumiCounters::counterKeyToString(key);
	int keyInt = LHCb::LumiCounters::counterKeyToType(keyName);
	debug() << "SUM: key: " << key 
		<< " name: " << keyName << " KeyInt: " << keyInt 
		<< " increment: " << incr << " integral: " << count << endmsg;
      }
    }
  }

  // release service
  if ( m_runRecordSvc ) m_runRecordSvc->release();
  m_runRecordSvc = 0;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

