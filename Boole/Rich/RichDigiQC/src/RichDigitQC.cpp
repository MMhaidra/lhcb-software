// $Id: RichDigitQC.cpp,v 1.3 2003-11-25 15:02:32 jonrob Exp $

// local
#include "RichDigitQC.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichDigitQC
//
// 2003-09-08 : Chris Jones   (Christopher.Rob.Jones@cern.ch)
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichDigitQC>          s_factory ;
const        IAlgFactory& RichDigitQCFactory = s_factory ;

// Standard constructor, initializes variables
RichDigitQC::RichDigitQC( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : RichAlgBase ( name, pSvcLocator ) {

  // Declare job options
  declareProperty( "InputDigits", m_digitTDS = MCRichDigitLocation::Default );
  declareProperty( "HistoPath", m_histPth = "RICH/DIGI/DIGITQC" );

}

// Destructor
RichDigitQC::~RichDigitQC() {};

// Initialisation
StatusCode RichDigitQC::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Initialize base class
  if ( !RichAlgBase::initialize() ) return StatusCode::FAILURE;

  // Book histograms
  if ( !bookHistograms() ) return StatusCode::FAILURE;

  // Initialise mult counts
  m_hitMult[Rich::Rich1] = 0;
  m_hitMult[Rich::Rich2] = 0;
  m_eventCount = 0;

  msg << MSG::DEBUG
      << " Histogram location   = " << m_histPth << endreq;

  return StatusCode::SUCCESS;
};

// Main execution
StatusCode RichDigitQC::execute() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Execute" << endreq;

  // Locate MCRichDigits
  SmartDataPtr<MCRichDigits> richDigits( eventSvc(), m_digitTDS );
  if ( !richDigits ) {
    msg << MSG::WARNING << "Cannot locate MCRichDigits at " << m_digitTDS << endreq;
    return StatusCode::FAILURE;
  } else {
    msg << MSG::DEBUG << "Successfully located " << richDigits->size()
        << " RichDigits at " << m_digitTDS << endreq;
  }

  // Loop over all digits
  int nR1 = 0; int nR2 =0;
  for ( MCRichDigits::const_iterator iDigit = richDigits->begin();
        iDigit != richDigits->end();
        ++iDigit ) {

    if ( (*iDigit)->key().rich() == Rich::Rich1 ) {
      ++nR1;
    } else {
      ++nR2;
    }

  }

  m_rich1->fill( nR1 );
  m_rich2->fill( nR2 );
  ++m_eventCount;
  m_hitMult[ Rich::Rich1 ] += nR1;
  m_hitMult[ Rich::Rich2 ] += nR2;

  return StatusCode::SUCCESS;
};

//  Finalize
StatusCode RichDigitQC::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Finalize" << endreq;

  double avR1    = ( m_eventCount > 0 ? m_hitMult[Rich::Rich1]/m_eventCount : 0 );
  double avR2    = ( m_eventCount > 0 ? m_hitMult[Rich::Rich2]/m_eventCount : 0 );
  double avR1Err = ( m_eventCount > 0 ? sqrt(m_hitMult[Rich::Rich1])/m_eventCount : 0 );
  double avR2Err = ( m_eventCount > 0 ? sqrt(m_hitMult[Rich::Rich2])/m_eventCount : 0 );  
  msg << MSG::INFO 
      << "RICH1 digit multiplicity = " << avR1 <<" +- " << avR1Err << endreq
      << "RICH2 digit multiplicity = " << avR2 <<" +- " << avR2Err << endreq;

  // finalize base class
  return RichAlgBase::finalize();
}


StatusCode RichDigitQC::bookHistograms() {

  std::string title;
  int nBins = 100;

  title = "RICH1 Occupancy";
  m_rich1 = histoSvc()->book( m_histPth, 1, title, nBins, 0, 5000 );

  title = "RICH2 Occupancy";
  m_rich2 = histoSvc()->book( m_histPth, 2, title, nBins, 0, 2000 );

  return StatusCode::SUCCESS;
}
