
//-----------------------------------------------------------------------------
/** @file RichRawDataFormatTool.cpp
 *
 *  Implementation file for class : RichRawDataFormatTool
 *
 *  CVS Log :-
 *  $Id: RichRawDataFormatTool.cpp,v 1.17 2005-11-15 12:57:48 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 2004-12-18
 */
//-----------------------------------------------------------------------------

// local
#include "RichRawDataFormatTool.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichRawDataFormatTool>          s_factory ;
const        IToolFactory& RichRawDataFormatToolFactory = s_factory ;

// Standard constructor
RichRawDataFormatTool::RichRawDataFormatTool( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : RichToolBase    ( type, name , parent ),
    m_detNumTool    ( 0                   ),
    m_rawEvent      ( 0                   ),
    m_evtCount      ( 0                   ),
    m_hasBeenCalled ( false               )
{
  // interface
  declareInterface<IRichRawDataFormatTool>(this);

  // job opts
  declareProperty( "ZeroSuppressHitCut", m_zeroSuppresCut = 96 );
  declareProperty( "RawEventLocation",
                   m_rawEventLoc = RawEventLocation::Default );
  declareProperty( "RawBufferLocation",
                   m_rawBuffLoc = RawBufferLocation::Default );
  declareProperty( "PrintSummary", m_summary = true );
}

// Destructor
RichRawDataFormatTool::~RichRawDataFormatTool() {}

StatusCode RichRawDataFormatTool::initialize()
{
  // Initialise base class
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // acquire tools
  acquireTool( "RichDetNumberingTool", m_detNumTool, 0, true );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );
  incSvc()->addListener( this, IncidentType::EndEvent   );

  return sc;
}

// Method that handles various Gaudi "software events"
void RichRawDataFormatTool::handle ( const Incident& incident )
{
  // Update prior to start of new event.
  if ( IncidentType::BeginEvent == incident.type() )
  {
    InitEvent();
  }
  // End of event
  else if ( IncidentType::EndEvent == incident.type() )
  {
    FinishEvent();
  }
}

StatusCode RichRawDataFormatTool::finalize()
{

  // Print out L1 information
  if ( m_summary )
  {
    printL1Stats( m_l1decodeSummary, "RICH Level 1 : Decoding Summary" );
    printL1Stats( m_l1encodeSummary, "RICH Level 1 : Encoding Summary" );
  }

  // base class finalize
  return RichToolBase::finalize();
}

void
RichRawDataFormatTool::printL1Stats( const L1TypeCount & count,
                                     const std::string & title ) const
{

  // See if any summary info has been collected
  if ( !count.empty() )
  {

    // Statistical calculators
    const RichStatDivFunctor occ1("%7.2f +-%5.2f"), occ2("%8.2f +-%5.2f");

    // Printout
    info() << "=========================================================================================================" << endreq
           << "                              " << title << " : " << m_evtCount << " events" << endreq;

    debug() << "---------------------------------------------------------------------------------------------------------" << endreq;
    std::vector< unsigned long > totWordSize(Rich::NRiches,0), totBanks(Rich::NRiches,0), totHits(Rich::NRiches,0);
    for ( L1TypeCount::const_iterator iL1C = count.begin(); iL1C != count.end(); ++iL1C )
    {
      const RichDAQ::Level1ID      L1ID   = (*iL1C).first.second;
      const RichDAQ::BankVersion version  = (*iL1C).first.first;
      const Rich::DetectorType rich       = m_detNumTool->richDetector( L1ID );
      const unsigned long nBanks          = (*iL1C).second.first;
      totBanks[rich]                     += nBanks;
      const unsigned long words           = (*iL1C).second.second.first;
      totWordSize[rich]                  += words;
      const unsigned long hits            = (*iL1C).second.second.second;
      totHits[rich]                      += hits;
      debug() << "  Board " << format("%3i",L1ID.dataValue()) << " Ver" << version << " | L1 size ="
              << occ1(nBanks,m_evtCount) << " hpds :"
              << occ2(words,m_evtCount) << " words :"
              << occ2(hits,m_evtCount) << " hits / event" << endreq;
    }

    info() << "---------------------------------------------------------------------------------------------------------" << endreq;
    info() << "  RICH1 Average  | L1 size =" << occ1(totBanks[Rich::Rich1],m_evtCount) << " hpds :"
           << occ2(totWordSize[Rich::Rich1],m_evtCount) << " words :"
           << occ2(totHits[Rich::Rich1],m_evtCount) << " hits / event" << endreq;
    info() << "  RICH2 Average  | L1 size =" << occ1(totBanks[Rich::Rich2],m_evtCount) << " hpds :"
           << occ2(totWordSize[Rich::Rich2],m_evtCount) << " words :"
           << occ2(totHits[Rich::Rich2],m_evtCount) << " hits / event" << endreq;

    info() << "=========================================================================================================" << endreq;

  } // end stats available

}

// Create data bank of correct version
// This function knows which bank objects to create for each version number
const RichHPDDataBank *
RichRawDataFormatTool::createDataBank( const RichSmartID::Collection & smartIDs,
                                       const RichDAQ::BankVersion version ) const
{

  // flag the tool as having been used this event
  m_hasBeenCalled = true;

  // pointer to HPD data block
  RichHPDDataBank * dataBank = 0;

  // Check bank is not empty
  if ( smartIDs.empty() )
  {
    Warning( "RichSmartID vector is empty -> No data bank" );
    return dataBank;
  }

  if ( RichDAQ::LHCb2 == version )
  {
    // Third iteration of bank format

    // Level 0 ID
    const RichDAQ::Level0ID l0ID = m_detNumTool->level0ID( smartIDs.front().pdID() );

    // First try the ZS format
    RichZeroSuppDataV2::RichZeroSuppData * zsData = new RichZeroSuppDataV2::RichZeroSuppData( l0ID, smartIDs );

    // If too big, use non ZS instead
    if ( zsData->tooBig() )
    {
      delete zsData;
      dataBank = (RichHPDDataBank*) new RichNonZeroSuppDataV1::RichNonZeroSuppData( l0ID, smartIDs );
    }
    else
    {
      dataBank = (RichHPDDataBank*)zsData;
    }

  }
  else if ( RichDAQ::LHCb1 == version )
  {
    // Second iteration of bank format

    // Level 0 ID
    const RichDAQ::Level0ID l0ID = m_detNumTool->level0ID( smartIDs.front().pdID() );

    // Decide to zero suppress or not depending on number of hits
    if ( smartIDs.size() < m_zeroSuppresCut )
    {
      dataBank = (RichHPDDataBank*) new RichZeroSuppDataV1::RichZeroSuppData( l0ID, smartIDs );
    }
    else
    {
      dataBank = (RichHPDDataBank*) new RichNonZeroSuppDataV1::RichNonZeroSuppData( l0ID, smartIDs );
    }

  } 
  else if ( RichDAQ::LHCb0 == version )
  {
    // Version 0 of data banks (DC04 compatibility)

    // HPD software ID
    const RichSmartID pdID = smartIDs.front().pdID();

    // Decide to zero suppress or not depending on number of hits
    if ( smartIDs.size() < m_zeroSuppresCut )
    {
      dataBank = (RichHPDDataBank*) new RichZeroSuppDataV0::RichZeroSuppData( pdID, smartIDs );
    }
    else
    {
      dataBank = (RichHPDDataBank*) new RichNonZeroSuppDataV0::RichNonZeroSuppData( pdID, smartIDs );
    }

  }
  else
  {
    Warning ( "Unknown RICH Raw Buffer version " +
              boost::lexical_cast<std::string>(version) + " -> No data bank" );
    return NULL;
  }

  if ( msgLevel(MSG::VERBOSE) )
  {
    // Print out SmartIDs to encode
    verbose() << " Creating data bank from " << smartIDs.size() << " RichSmartIDs :-" << endreq;
    for ( RichSmartID::Collection::const_iterator iID = smartIDs.begin();
          iID != smartIDs.end(); ++iID )
    {
      verbose() << "   " << *iID << endreq;
    }
    // Printout this bank
    verbose() << *dataBank << endreq;
  }

  return dataBank;
}

// Create data bank of correct version
// This function knows which bank objects to create for each version number
const RichHPDDataBank *
RichRawDataFormatTool::createDataBank( const RichDAQ::LongType * dataStart,
                                       const unsigned int dataSize,
                                       const RichDAQ::BankVersion version ) const
{

  RichHPDDataBank * dataBank = 0;

  // Check data size
  if ( dataSize < 1 || dataSize > RichDAQ::MaxDataSize )
  {
    Exception( "Invalid HPD data block size : " + boost::lexical_cast<std::string>(dataSize) );
  }

  if ( RichDAQ::LHCb2 == version )
  {
    // Third iteration of bank format

    // Header word
    const RichDAQHeaderPDBase header( *dataStart );

    // Decide to zero suppress or not depending on number of hits
    if ( header.zeroSuppressed() )
    {
      dataBank = (RichHPDDataBank*) new RichZeroSuppDataV2::RichZeroSuppData( dataStart, dataSize );
    }
    else
    {
      dataBank = (RichHPDDataBank*) new RichNonZeroSuppDataV1::RichNonZeroSuppData( dataStart );
    }

  } 
  else if ( RichDAQ::LHCb1 == version )
  {
    // Second iteration of bank format

    // Header word
    const RichDAQHeaderPDBase header( *dataStart );

    // Decide to zero suppress or not depending on number of hits
    if ( header.zeroSuppressed() )
    {
      dataBank = (RichHPDDataBank*) new RichZeroSuppDataV1::RichZeroSuppData( dataStart, dataSize );
    }
    else
    {
      dataBank = (RichHPDDataBank*) new RichNonZeroSuppDataV1::RichNonZeroSuppData( dataStart );
    }

  } 
  else if ( RichDAQ::LHCb0 == version )
  {
    // Version 0 of data banks (DC04 compatibility)

    // Header word
    const RichDAQHeaderV0::RichDAQHeaderPD header( *dataStart );

    // Decide to zero suppress or not depending on number of hits
    if ( header.zeroSuppressed() )
    {
      dataBank = (RichHPDDataBank*) new RichZeroSuppDataV0::RichZeroSuppData( dataStart, dataSize );
    }
    else
    {
      dataBank = (RichHPDDataBank*) new RichNonZeroSuppDataV0::RichNonZeroSuppData( dataStart );
    }

  }
  else
  {
    Exception ( "Unknown RICH Raw Buffer version " +
                boost::lexical_cast<std::string>(version) + " -> No data bank" );
  }

  // Printout this bank
  if ( msgLevel(MSG::VERBOSE) ) verbose() << *dataBank << endreq;

  return dataBank;
}

void RichRawDataFormatTool::createDataBank( const RichDAQ::L1Map & L1Data,
                                            const RichDAQ::BankVersion version ) const
{

  // Retrieve the RawBuffer
  RawBuffer * rawBuffer = get<RawBuffer>( m_rawBuffLoc );

  // DC04 bug fix hack
  if ( RichDAQ::LHCb0 == version )
  {
    // Special handling for this format, due to bug in header word....
    // Also, each HPD is in its own bank...
    // To be removed once DC04 format no loner in use...

    // Loop over all L1 groups
    for ( RichDAQ::L1Map::const_iterator iL1 = L1Data.begin(); iL1 != L1Data.end(); ++iL1 )
    {
      // loop over all HPDs
      for ( RichDAQ::PDMap::const_iterator iHPD = (*iL1).second.begin();
            iHPD != (*iL1).second.end(); ++iHPD )
      {

        // Make a new data bank
        RichDAQ::RAWBank dataBank;
        // Get raw data bank for this HPD, and fill into RAWBank
        const RichHPDDataBank * hpdData = createDataBank( (*iHPD).second, version );
        hpdData->fillRAWBank( dataBank );
        delete hpdData;
        // Add this bank to the Raw buffer
        rawBuffer->addBank( (*iL1).first.dataValue(), RawBuffer::Rich, dataBank, version );

        if ( m_summary )
        {
          // Count the number of banks and size
          L1CountAndSize & cands = m_l1encodeSummary[ L1IDandV(version,(*iL1).first) ];
          // Increment bank size
          cands.second.first += 2+dataBank.size(); // 2 L1 headers + data words
          // Increment hit occupancy
          cands.second.second += (*iHPD).second.size();
        }

      }

      // Count number of HPD banks
      if ( m_summary )
      {
        m_l1encodeSummary[L1IDandV(version,(*iL1).first)].first += (*iL1).second.size();
      }

    }

  }
  else
  {
    // Proper handling...

    // Loop over Level1 board and make a RAWBank for each
    for ( RichDAQ::L1Map::const_iterator iL1 = L1Data.begin(); iL1 != L1Data.end(); ++iL1 )
    {

      // Make a new data bank
      RichDAQ::RAWBank dataBank;

      // Loop over each active HPD for this Level1 board and fill RAWBank
      unsigned nHits(0);
      if ( !(*iL1).second.empty() )
      {
        for ( RichDAQ::PDMap::const_iterator iHPD = (*iL1).second.begin();
              iHPD != (*iL1).second.end(); ++iHPD )
        {

          // Get raw data bank for this HPD, and fill into RAWBank
          const RichHPDDataBank * hpdData = createDataBank( (*iHPD).second, version );
          hpdData->fillRAWBank( dataBank );
          delete hpdData;
          nHits += (*iHPD).second.size();

        } // end photon detector loop
      }

      // Add this bank to the Raw buffer
      rawBuffer->addBank( (*iL1).first.dataValue(), RawBuffer::Rich, dataBank, version );

      if ( m_summary )
      {
        // Count the number of banks and size
        L1CountAndSize & cands = m_l1encodeSummary[ L1IDandV(version,(*iL1).first) ];
        // Increment bank size
        cands.second.first += 2+dataBank.size(); // 2 L1 headers + data words
        // Increment hit occupancy
        cands.second.second += nHits;
        // Count number of HPD banks
        cands.first += (*iL1).second.size();
      }

      if ( msgLevel(MSG::DEBUG) )
      {
        debug() << "Encoded " << format("%2i",(*iL1).second.size()) << " HPDs into Level1 Bank "
                << format("%2i",(*iL1).first.dataValue()) << " : Size " << format("%4i",2+dataBank.size())
                << " words : Version " << version << endreq;
      }

    } // Level1 loop

  } // end version if

}

void RichRawDataFormatTool::decodeToSmartIDs( const RawBank & bank,
                                              RichSmartID::Collection & smartIDs ) const
{

  // Check this is a RICH bank
  if ( bank.bankType() != RawBuffer::Rich )
  {
    Exception( "BankType is not RICH : type = " +
               boost::lexical_cast<std::string>(bank.bankType()) );
  }

  // Get bank version and ID
  const RichDAQ::Level1ID L1ID       = static_cast< RichDAQ::Level1ID >    ( bank.bankSourceID() );
  const RichDAQ::BankVersion version = static_cast< RichDAQ::BankVersion > ( bank.version()      );

  // HPD count
  unsigned int nHPDbanks(0), startSize(smartIDs.size());

  // Is this an empty bank ?
  if ( bank.dataSize() > 0 )
  {

    // ... otherwise, must have at least 2 entries
    if ( bank.dataSize() < 2 )
    {
      Exception( "Non-empty RICH Bank size is less than 2 !" );
    }

    // flag the tool as having been used this event
    m_hasBeenCalled = true;

    // DC04 bug fix hack
    if ( RichDAQ::LHCb0 == version )
    {
      // Special handling for this format, due to bug in header word....

      // Create data bank and decode into RichSmartIDs
      const RichHPDDataBank * hpdBank ( createDataBank( &bank.data()[0],
                                                        bank.dataSize()-1,
                                                        version ) );
      hpdBank->fillRichSmartIDs( smartIDs, m_detNumTool );
      delete hpdBank;
      ++nHPDbanks;

    }
    else
    {
      // Proper handling...

      // Loop over bank, find headers and produce a data bank for each
      // Fill data into RichSmartIDs
      long lineC(0);
      while ( lineC < bank.dataSize() )
      {

        // Find HPD bank start
        const RichDAQHeaderPDBase header ( bank.data()[lineC] );

        // Is this a true header
        if ( header.startPD() )
        {

          if ( msgLevel(MSG::VERBOSE) )
            verbose() << " Found HPD header at line " << lineC << " of " << bank.dataSize() << endreq;

          // Store start line for header
          const long lineHeader = lineC;

          // Find last line of block
          long lineLast = lineC;
          if ( header.zeroSuppressed() )
          {
            // For ZS blocks, have to search for the hext header to define the block length

            bool cont = true;
            while ( cont && lineC < bank.dataSize() )
            {
              ++lineC;
              // Test if this is a new header
              const RichDAQHeaderPDBase testheader ( bank.data()[lineC] );
              if ( testheader.startPD() || lineC == bank.dataSize() )
              {
                lineLast = lineC-1;
                cont = false;
              }
            }

            if ( msgLevel(MSG::VERBOSE) )
              verbose() << "  -> Bank is zero surpressed : ends at " << lineLast << endreq;

          }
          else
          {
            // non-ZS blocks have fixed length, so skip straight to the end

            lineC   += 1+RichDAQ::MaxDataSize; // data block + header
            lineLast = lineC-1;

            if ( msgLevel(MSG::VERBOSE) )
              verbose() << "  -> Bank is non zero surpressed : ends at " << lineLast << endreq;

          }

          // Create data bank and decode into RichSmartIDs
          const RichHPDDataBank * hpdBank ( createDataBank( &bank.data()[lineHeader],
                                                            lineLast-lineHeader,
                                                            version ) );
          hpdBank->fillRichSmartIDs( smartIDs, m_detNumTool );
          ++nHPDbanks;
          delete hpdBank;

        }
        else
        {
          ++lineC;
        }

      } // bank while loop

    } // DC04 bug fix if...

  } // data bank not empty

  if ( m_summary )
  {
    // Count the number of banks and size
    L1CountAndSize & cands = m_l1decodeSummary[ L1IDandV(version,L1ID) ];
    // Increment bank size
    cands.second.first += 2+bank.dataSize(); // 2 L1 headers + data words
    // Increment hit occupancy
    cands.second.second += smartIDs.size()-startSize;
    // Count number of HPD banks
    cands.first += nHPDbanks;
  }

  // debug printout
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Decoded " << format("%2i",nHPDbanks) << " HPDs from Level1 Bank " 
            << format("%2i",L1ID.dataValue())
            << " : Size " << format("%4i",2+bank.dataSize()) << " words : Version " 
            << version << endreq;

    // Print out decoded smartIDs
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << " Decoded RichSmartIDs :-" << endreq;
      for ( RichSmartID::Collection::const_iterator iID = smartIDs.begin();
            iID != smartIDs.end(); ++iID )
      {
        verbose() << "   " << *iID << endreq;
      }
    }

  }

}

void
RichRawDataFormatTool::decodeToSmartIDs( RichSmartID::Collection & smartIDs ) const
{

  // Get the banks for the Rich
  const RichDAQ::RAWBanks & richBanks = rawEvent()->banks( RawBuffer::Rich );

  // Purge SmartID container
  smartIDs.clear();

  // make a guess at a reserved size
  smartIDs.reserve( richBanks.size() * 250 );

  // Loop over data banks
  for ( RichDAQ::RAWBanks::const_iterator iBank = richBanks.begin();
        iBank != richBanks.end(); ++iBank )
  {

    // Decode this bank to RichSmartIDs
    decodeToSmartIDs( *iBank, smartIDs );

  } // end loop over data banks

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Decoded " << smartIDs.size()
            << " RichSmartIDs from " << richBanks.size() << " RICH L1 bank(s)" << endreq;
  }

}

RawEvent * RichRawDataFormatTool::rawEvent() const
{
  if ( !m_rawEvent ) 
  {

    // Try and load from TES. If it exists return, overwise try to create one
    SmartDataPtr<RawEvent> rawEventTES( evtSvc(), m_rawEventLoc );
    if ( rawEventTES )
    {
      m_rawEvent = rawEventTES;
    }
    else
    {
      debug() << "Creating RawEvent from RawBuffer" << endreq;

      // Retrieve the RawBuffer
      SmartDataPtr<RawBuffer> rawBuffer( evtSvc(), m_rawBuffLoc );
      if ( !rawBuffer ) { m_rawEvent = 0; Exception("Unable to locate RawBuffer"); }

      // make new RawEvent and put into TES
      m_rawEvent = new RawEvent( rawBuffer );
      if ( !m_rawEvent ) 
      {
        m_rawEvent = 0;
        Exception("Unable to allocate memory to RawEvent");
      }
      put( m_rawEvent, RawEventLocation::Default );
    }

  }

  return m_rawEvent;
}
