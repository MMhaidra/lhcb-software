// $Id: STErrorMonitor.cpp,v 1.3 2009-03-24 16:11:20 nchiapol Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from STTELL1Event
#include "Event/STTELL1BoardErrorBank.h"

// from STKernel
#include "Kernel/STDAQDefinitions.h"
#include "Kernel/STBoardMapping.h"

// Boost
#include "boost/lexical_cast.hpp"

// local
#include "STErrorMonitor.h"

using namespace LHCb;
using namespace STDAQ;
using namespace STBoardMapping;

//-----------------------------------------------------------------------------
// Implementation file for class : STErrorMonitor
//
// 2009-03-20 : Jeroen Van Tilburg
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( STErrorMonitor );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
STErrorMonitor::STErrorMonitor( const std::string& name,
                                ISvcLocator* pSvcLocator) :
  ST::HistoAlgBase(name, pSvcLocator)
{
  declareSTConfigProperty("ErrorLocation", m_errorLocation, 
                          STTELL1BoardErrorBankLocation::TTErrors );
  declareProperty("ExpertHisto", m_expertHisto = true );
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode STErrorMonitor::initialize() 
{  
  // Initialize ST::HistoAlgBase
  StatusCode sc = ST::HistoAlgBase::initialize();
  if (sc.isFailure()) return sc;

  if (detType() == "TT") m_TELL1Mapping = &TTSourceIDToNumberMap();
  else m_TELL1Mapping = &ITSourceIDToNumberMap();

  // Get the maximum number of Tell1s to determine number of histogram bin
  m_maxTell1s = m_TELL1Mapping->size();

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode STErrorMonitor::execute()
{
  // Get the error banks
  if ( !exist<STTELL1BoardErrorBanks>(m_errorLocation) ) {
    return Error("No STTELL1BoardErrorBanks in TES. Run the STErrorDecoding "
                 "first.", StatusCode::FAILURE);
  }
  STTELL1BoardErrorBanks* errors = get<STTELL1BoardErrorBanks>(m_errorLocation);

  // Loop over the error banks
  STTELL1BoardErrorBanks::const_iterator iterBank = errors->begin();
  for ( ; iterBank != errors->end();  ++iterBank){

    // Print out the error bank information
    if (msgLevel(MSG::DEBUG)) debug() << **iterBank << endmsg;

    // Get the number of the tell1
    unsigned int sourceID = (*iterBank)->Tell1ID();
    unsigned int tellNum = (m_TELL1Mapping->find(sourceID))->second;
    
    // Plot the number of error banks versus sequential tell number
    plot1D(tellNum, "Error banks per Tell1", 0.5, m_maxTell1s+0.5, m_maxTell1s);

    if ( !m_expertHisto ) continue;

    // Convert the tell1 number to a string
    std::string strTellNum  = boost::lexical_cast<std::string>(tellNum);

    // Loop over the ErrorInfo objects (one for each FPGA-PP)
    const std::vector<STTELL1Error*> errorInfo = (*iterBank) -> errorInfo();
    std::vector<STTELL1Error*>::const_iterator errorIt = errorInfo.begin();    
    for( ; errorIt != errorInfo.end(); ++errorIt) {

      // Get the PP number [0:3]
      unsigned int pp = errorIt - errorInfo.begin();

      // Get the majority vote for this PP
      unsigned int pcn = (*errorIt)->pcnVote();

      // Loop over the Beetles and ports
      for (unsigned int beetle = 0u; beetle < nBeetlesPerPPx; ++beetle) {
        for (unsigned int port = 0u; port < nports; ++port) {
          
          // Fill 2D histogram with the error types versus Beetle port
          unsigned int errorType = (*errorIt) -> linkInfo(beetle, port, pcn);
          double portBin = double(port)/double(nports)+beetle+pp*nBeetlesPerPPx;
          plot2D(portBin, errorType, "error-types_$tell"+strTellNum, 
	         "Error types tell"+strTellNum, 0, noptlinks, 0, 10, 
		 nports*noptlinks, 10);
        }
      }
    }
  }  //iterBanks

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode STErrorMonitor::finalize()
{
  return ST::HistoAlgBase::finalize();// must be called after all other actions
}
//=============================================================================
