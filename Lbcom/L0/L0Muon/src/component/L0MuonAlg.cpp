// $Id: L0MuonAlg.cpp,v 1.27 2010-02-04 22:55:35 jucogan Exp $
#include <algorithm>
#include <math.h>
#include <set>

#include "L0MuonAlg.h"
#include "SubstituteEnvVarInPath.h"

// Gaudi interfaces
#include "GaudiKernel/IChronoStatSvc.h"

// Gaudi factories for algorithms
#include "GaudiKernel/AlgFactory.h"

// from Event
#include "Event/MuonDigit.h"
#include "Event/L0MuonData.h"

// Registers (to fill optical links)
#include "ProcessorKernel/RegisterFactory.h"
#include "ProcessorKernel/TileRegister.h"

// Emulator
#include "L0MuonKernel/UnitFactory.h"
#include "L0MuonKernel/L0MuonKernelFromXML.h"

DECLARE_ALGORITHM_FACTORY( L0MuonAlg );

L0MuonAlg::L0MuonAlg(const std::string& name,
                     ISvcLocator* pSvcLocator)
  :L0AlgBase(name, pSvcLocator)
  , m_l0CondCtrl( 0 )
  , m_l0CondProc( 0 )
{

  std::vector<int> time_slots;
  //  for (int i=-7;i<=7;++i) time_slots.push_back(i);
  time_slots.push_back(0);
  declareProperty( "TimeSlots"  , m_time_slots = time_slots);

  m_muonBuffer = 0;

  m_foiXSize.clear();

  m_foiXSize.push_back(4); // 0-> Xfoi in M1
  m_foiXSize.push_back(5); // 1-> Xfoi in M2
  m_foiXSize.push_back(0); // 2-> Xfoi in M3
  m_foiXSize.push_back(2); // 3-> Xfoi in M4
  m_foiXSize.push_back(4); // 4-> Xfoi in M5

  m_foiYSize.push_back(0); // 0-> Yfoi in M1
  m_foiYSize.push_back(0); // 1-> Yfoi in M2
  m_foiYSize.push_back(0); // 2-> Yfoi in M3
  m_foiYSize.push_back(1); // 3-> Yfoi in M4
  m_foiYSize.push_back(1); // 4-> Yfoi in M5

  declareProperty("IgnoreCondDB"   , m_ignoreCondDB = false);

  declareProperty("Version"        , m_version = 3 );

  declareProperty("ConfigFile"     , m_configfile= "$PARAMFILESROOT/data/L0MuonKernel.xml")  ;
  declareProperty("IgnoreM1"       , m_ignoreM1 = false );
  declareProperty("ForceM3"        , m_forceM3  = false );

  declareProperty("FoiXSize"       , m_foiXSize);
  declareProperty("FoiYSize"       , m_foiYSize);

  declareProperty("DebugMode"      , m_debug       = false );

  // Default for Boole :
  declareProperty("InputSource"    , m_inputSource = 0);
  declareProperty("DAQMode"        , m_mode        = 1 );
  declareProperty("Compression"    , m_compression = true );

}


StatusCode L0MuonAlg::initialize()
{
  StatusCode sc = L0AlgBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  //   IChronoStatSvc * svc = chronoSvc();
  //   svc->chronoStart("L0MuonTrigger Initialize");

  // Set the layouts used to fill the OLs
  setLayouts();

  // Instanciate the MuonTrigger Units and Registers
  L0Muon::RegisterFactory::selectInstance(0);
  std::string xmlFileName = L0MuonUtils::SubstituteEnvVarInPath(m_configfile);
  info() <<  "XML file = " << xmlFileName << endmsg;
  L0Muon::L0MuonKernelFromXML(xmlFileName);
  L0Muon::UnitFactory* ufactory = L0Muon::UnitFactory::instance();
  m_muontriggerunit = ufactory->topUnit();
  if( msgLevel(MSG::DEBUG) ) debug() <<  "MuonTrigger build from xml "<< endmsg;

  // Set debug mode
  if (m_debug) {
    m_muontriggerunit->setDebugMode();
    info() <<  "MuonTrigger debug mode "<< endmsg;
  }

  // Set properties
  if ( m_ignoreCondDB ) {
    // Use properties from options
    m_muontriggerunit->setProperties(l0MuonProperties());
  } else {
    // Use CondDB
    // - processor version 
    try {
      registerCondition( "Conditions/Private/L0MUON/Q1/CB" ,
                         m_l0CondCtrl ,
                         &L0MuonAlg::updateL0CondCtrl ) ;
    } catch ( GaudiException &e ) {
      error() << e << endmsg ;
      error() << "Cannot access L0MUON/Q1/CB condition"<< endmsg ;
      error() << "Emulation will run with the processor version defined in options." << endmsg ;
      m_muontriggerunit->setProperties(l0MuonProperties());
    }
    // - FOI 
    try {
      registerCondition( "Conditions/Private/L0MUON/Q1/PB1" ,
                         m_l0CondProc ,
                         &L0MuonAlg::updateL0CondProc ) ;
    } catch ( GaudiException &e ) {
      error() << e << endmsg ;
      error() << "Cannot access L0MUON/Q1/PB1 condition"<< endmsg ;
      error() << "Emulation will run with the FOI defined in options." << endmsg ;
      m_muontriggerunit->setProperties(l0MuonProperties());
    }
  }
  
  // Initialize
  m_muontriggerunit->initialize();

  //   svc->chronoStop("L0MuonTrigger Initialize");
  //   svc->chronoDelta("L0MuonTrigger Initialize",IChronoStatSvc::KERNEL);
  //   svc->chronoPrint("L0MuonTrigger Initialize");

  // L0MuonOutputs tool
  m_outputTool =  tool<L0MuonOutputs>( "L0MuonOutputs" , "OutputTool" , this );

  m_totEvent = 0;

  return StatusCode::SUCCESS;
}



StatusCode L0MuonAlg::execute()
{
  L0Muon::RegisterFactory::selectInstance(0);

  if( msgLevel(MSG::DEBUG) )  {
    debug() << "-----------------------------------------------------------------" << endreq;
    debug() << "-- Start execution:" << endreq;
  }

  //IChronoStatSvc * svc = chronoSvc();
  //svc->chronoStart("L0MuonTrigger Execute");

  StatusCode sc;

  // Loop over time slots
  for (std::vector<int>::iterator it_ts=m_time_slots.begin(); it_ts<m_time_slots.end(); ++it_ts){

    sc = setProperty("RootInTes",timeSlot(*it_ts));
    if( sc.isFailure() ) return Error( "Unable to set RootInTES property of L0MuonAlg", sc );

    if (!exist<LHCb::RawEvent>( LHCb::RawEventLocation::Default )) continue;

    // Set rootInTES to point to the current TimeSlot
    sc = m_outputTool->setProperty( "RootInTES", rootInTES() );
    if ( sc.isFailure() ) continue;// error printed already by GaudiAlgorithm

    // Get list of hits used as input for the emulator
    if ( m_inputSource>0 ) { 
      // Use hits from L0MuonDatas
      sc = getDigitsFromL0Muon();
      if ( sc.isFailure() ) return Error( "Failed to get Hits from L0Muon ... abort",StatusCode::SUCCESS,50);
    } else if ( m_inputSource==0 ) {
      // Use Muon Digits or Muon ZS raw data
      sc = getDigitsFromMuon();
      if ( sc.isFailure() ) return Error( "Failed to get Hits from Muon ... abort",StatusCode::SUCCESS,50);
    } else if ( m_inputSource<0 ) {
      // Use NZS muon raw data
      sc = getDigitsFromMuonNZS();
      if ( sc.isFailure() ) return Error( "Failed to get Hits from Muon NZS bank ... abort",StatusCode::SUCCESS,50);
    }
    
    // Fill the OL register with the input hits
    sc = fillOLsfromDigits();
    if( sc.isFailure() ) {
      return Error( "Failed to load OLs ... abort",StatusCode::SUCCESS,50);
    }

    // Preexecution phase: data exchange between PUs
    if( msgLevel(MSG::DEBUG) ) debug() << "Preexecute of MuonKernel units ..." << endreq;
    m_muontriggerunit->preexecute();

    // Execution phase: search for candidates and fill output registers
    if( msgLevel(MSG::DEBUG) ) debug() << "Execution of MuonKernel units ..." << endreq;
    m_muontriggerunit->execute();

    // Specify the version parameters to the output tool
    m_outputTool->setVersion(m_version, m_mode, m_compression);

    // Fill the Raw Event container
    if ( m_writeBanks ) {
      if( msgLevel(MSG::DEBUG) ) debug() << "Fill RawEvent ..." << endreq;
      sc = m_outputTool->writeRawBanks();
      if ( sc.isFailure() ) return sc;
    }

    // Write on TES
    if ( m_writeOnTES) {
      if( msgLevel(MSG::DEBUG) ) debug() << "Write on TES ..." << endreq;
      sc = m_outputTool->writeOnTES(m_l0context);
      if ( sc.isFailure() ) return sc;
    }

    // Fill the container for the L0DU (L0ProcessorData)
    if( msgLevel(MSG::DEBUG) ) debug() << "Fill L0ProcessorData ..." << endreq;
    sc = m_outputTool->writeL0ProcessorData();
    if ( sc.isFailure() ) return sc;

    // Postexecution phase: reset registers
    if( msgLevel(MSG::DEBUG) ) debug() << "Postexecution of MuonKernel units ..." << endreq;
    m_muontriggerunit->postexecute();

  } // End of loop over time slots

  //svc->chronoStop("L0MuonTrigger Execute");
  //svc->chronoDelta("L0MuonTrigger Execute", IChronoStatSvc::KERNEL);
  //if( MSG::DEBUG >= log.level() ) svc->chronoPrint("L0MuonTrigger Execute");


  ++m_totEvent;

  if( msgLevel(MSG::DEBUG) ){
    debug() << "-- Execution done." << endreq;
    debug() << "-----------------------------------------------------------------" << endreq;
  }

  return StatusCode::SUCCESS;
}


StatusCode L0MuonAlg::finalize()
{

  L0Muon::UnitFactory* ufactory = L0Muon::UnitFactory::instance();
  delete(ufactory);

  info() << "- ------------------------------------------------------------------"<<endmsg;
  info() << "- ========> Final summary of the L0Muon trigger (emulator) <========"<<endmsg;
  info() << "- Total number of events processed           : "
         <<format("%8d",m_totEvent)<<endmsg;
  m_outputTool->statTot(info());
  info() << "- ------------------------------------------------------------------"<<endmsg;

  return L0AlgBase::finalize();  // must be called after all other actions
}


void L0MuonAlg::setLayouts()
{
  m_layout=MuonSystemLayout(MuonStationLayout(MuonLayout(24,8)),
                            MuonStationLayout(MuonLayout(48,8)),
                            MuonStationLayout(MuonLayout(48,8)),
                            MuonStationLayout(MuonLayout(12,8)),
                            MuonStationLayout(MuonLayout(12,8)));

  m_ollayout = MuonSystemLayout(MuonStationLayout(MuonLayout(2,4)),
                                MuonStationLayout(MuonLayout(4,1),
                                                  MuonLayout(4,2),
                                                  MuonLayout(2,2),
                                                  MuonLayout(2,2)),
                                MuonStationLayout(MuonLayout(4,1),
                                                  MuonLayout(4,2),
                                                  MuonLayout(2,2),
                                                  MuonLayout(2,2)),
                                MuonStationLayout(MuonLayout(2,2)),
                                MuonStationLayout(MuonLayout(2,2)));


  m_lulayout=MuonSystemLayout(MuonStationLayout(MuonLayout(0,0)),
                              MuonStationLayout(MuonLayout(8,1),
                                                MuonLayout(4,2),
                                                MuonLayout(2,2),
                                                MuonLayout(2,2)),
                              MuonStationLayout(MuonLayout(8,1),
                                                MuonLayout(4,2),
                                                MuonLayout(2,2),
                                                MuonLayout(2,2)),
                              MuonStationLayout(MuonLayout(0,0),
                                                MuonLayout(4,2),
                                                MuonLayout(2,2),
                                                MuonLayout(2,2)),
                              MuonStationLayout(MuonLayout(0,0),
                                                MuonLayout(4,2),
                                                MuonLayout(2,2),
                                                MuonLayout(2,2)));


  m_stripH =MuonSystemLayout(MuonStationLayout(MuonLayout(0,0)),
                             MuonStationLayout(MuonLayout(8,8),
                                               MuonLayout(4,8),
                                               MuonLayout(2,8),
                                               MuonLayout(2,8)),
                             MuonStationLayout(MuonLayout(8,8),
                                               MuonLayout(4,8),
                                               MuonLayout(2,8),
                                               MuonLayout(2,8)),
                             MuonStationLayout(MuonLayout(0,0),
                                               MuonLayout(4,8),
                                               MuonLayout(2,8),
                                               MuonLayout(2,8)),
                             MuonStationLayout(MuonLayout(0,0),
                                               MuonLayout(4,8),
                                               MuonLayout(2,8),
                                               MuonLayout(2,8)));

  m_stripV =MuonSystemLayout(MuonStationLayout(MuonLayout(0,0)),
                             MuonStationLayout(MuonLayout(48,1),
                                               MuonLayout(48,2),
                                               MuonLayout(48,2),
                                               MuonLayout(48,2)),
                             MuonStationLayout(MuonLayout(48,1),
                                               MuonLayout(48,2),
                                               MuonLayout(48,2),
                                               MuonLayout(48,2)),
                             MuonStationLayout(MuonLayout(0,0),
                                               MuonLayout(12,2),
                                               MuonLayout(12,2),
                                               MuonLayout(12,2)),
                             MuonStationLayout(MuonLayout(0,0),
                                               MuonLayout(12,2),
                                               MuonLayout(12,2),
                                               MuonLayout(12,2)));
}


std::map<std::string,L0Muon::Property>  L0MuonAlg::l0MuonProperties()
{

  // Translate the gaudi Algorithms properties into the L0MuonKernel properties:

  std::map<std::string,L0Muon::Property> properties;
  std::string prop;
  char buf[128];

  // xFOI
  prop="";
  for (std::vector<int>::iterator ifoi =  m_foiXSize.begin();ifoi!= m_foiXSize.end();ifoi++) {
    sprintf(buf,"%d",(*ifoi));
    prop+=buf;
    ifoi++;
    if( ifoi!=  m_foiXSize.end()) prop+=",";
    ifoi--;
  }
  properties["foiXSize"]       = L0Muon::Property(prop);

  // yFOI
  prop="";
  for (std::vector<int>::iterator ifoi =  m_foiYSize.begin();ifoi!= m_foiYSize.end();ifoi++) {
    sprintf(buf,"%d",(*ifoi));
    prop+=buf;
    ifoi++;
    if( ifoi!=  m_foiYSize.end()) prop+=",";
    ifoi--;
  }
  properties["foiYSize"]       = L0Muon::Property(prop);

  // ignoreM1
  properties["ignoreM1"]       = m_ignoreM1 ? L0Muon::Property("1") : L0Muon::Property("0");

  // ProcVersion
  sprintf(buf,"%d",m_version);
  prop=buf;
  properties["procVersion"]    = L0Muon::Property(prop);;

  info() << "MuonTriggerUnit properties are:"<<endreq;
  for (std::map<std::string,L0Muon::Property>::iterator ip= properties.begin(); ip!=properties.end();ip++){
    info() << " "<< (*ip).first << " = "<< ((*ip).second).value() <<endreq;
  }

  return properties;
}


StatusCode L0MuonAlg::getDigitsFromMuon()
{

  m_digits.clear();
  
  // First try the digits on the TES if there (Boole)
  if (exist<LHCb::MuonDigits>( LHCb::MuonDigitLocation::MuonDigit ) ) {

    if( msgLevel(MSG::DEBUG) ) debug() << "fillOLsfromDigits:  Getting hits from muon digits"<<m_muonBuffer<<endmsg;

    LHCb::MuonDigits* digits = get<LHCb::MuonDigits>( LHCb::MuonDigitLocation::MuonDigit );
    LHCb::MuonDigits::const_iterator did;
    for( did = digits->begin() ; did != digits->end() ; did++ ){
      LHCb::MuonTileID mkey = (*did)->key();
      m_digits.push_back(mkey);
    }
    return StatusCode::SUCCESS;
  }

  // Otherwise, decode the Muon bank (ZS)
  if( msgLevel(MSG::DEBUG) ) debug() << "Getting hits from Muon Raw buffer (ZS)"<<endmsg;
  
  if(!m_muonBuffer) {
    // First call: initialize the pointer to the Muon Raw Buffer Interface
    m_muonBuffer=tool<IMuonRawBuffer>("MuonRawBuffer","MuonRaw", this);
    if(!m_muonBuffer) {
      error() << "No pointer to muon raw buffer interface tool"<<m_muonBuffer<<endmsg;
      return StatusCode::FAILURE;
    }
  }
  
  IProperty* prop = dynamic_cast<IProperty*>( m_muonBuffer );
  if( prop ) {
    StatusCode sc = prop->setProperty( "RootInTES", rootInTES() );
    if( sc.isFailure() ) return Error( "Unable to set RootInTES property of MuonRawBuffer", sc );
  } else {
    return Error( "Unable to locate IProperty interface of MuonRawBuffer" );
  }
  StatusCode sc = m_muonBuffer->getTile(m_digits);
  if( sc.isFailure() ) {
    m_digits.clear();
    return Error( "Unable to get the tiles from the MuonRawBuffer", StatusCode::FAILURE ,50 );
  }
  
  m_muonBuffer->forceReset();

  return StatusCode::SUCCESS;
}

StatusCode L0MuonAlg::getDigitsFromMuonNZS()
{
  if( msgLevel(MSG::DEBUG) ) debug() << "Getting hits from Muon Raw buffer "<<endmsg;
  
  m_digits.clear();

  if(!m_muonBuffer) {
    // First call: initialize the pointer to the Muon Raw Buffer Interface
    m_muonBuffer=tool<IMuonRawBuffer>("MuonRawBuffer","MuonRaw", this);
    if(!m_muonBuffer) {
      error() << "No pointer to muon raw buffer interface tool"<<m_muonBuffer<<endmsg;
      return StatusCode::FAILURE;
    }
  }

  IProperty* prop = dynamic_cast<IProperty*>( m_muonBuffer );
  if( prop ) {
    StatusCode sc = prop->setProperty( "RootInTES", rootInTES() );
    if( sc.isFailure() ) return Error( "Unable to set RootInTES property of MuonRawBuffer", sc);
  } else {
    return Error( "Unable to locate IProperty interface of MuonRawBuffer" , StatusCode::FAILURE );
  }
 
  std::vector<std::pair<LHCb::MuonTileID,unsigned int> >  tileAndTDC;
  StatusCode sc = m_muonBuffer->getNZSupp(tileAndTDC);
  if( sc.isFailure() ) {
    m_digits.clear();
    return Error( "Unable to get the tiles from the NZS MuonRawBuffer", StatusCode::FAILURE ,50 );
  }

  std::vector<std::pair<LHCb::MuonTileID,unsigned int> >::iterator itileAndTDC;
  for (itileAndTDC=tileAndTDC.begin(); itileAndTDC<tileAndTDC.end(); ++itileAndTDC) {
    m_digits.push_back(itileAndTDC->first);
  }

  m_muonBuffer->forceReset();
  
  return StatusCode::SUCCESS;
}

StatusCode L0MuonAlg::getDigitsFromL0Muon()
{
  if (!exist<LHCb::L0MuonDatas>( LHCb::L0MuonDataLocation::Default) ) {
    return Error("L0MuonDatas not found",StatusCode::FAILURE,10);
  }
  
  if( msgLevel(MSG::DEBUG) ) debug() << "Getting hits from L0Muon itself"<<endmsg;

  m_digits.clear();
  
  LHCb::L0MuonDatas* pdatas = get<LHCb::L0MuonDatas>( LHCb::L0MuonDataLocation::Default);
  LHCb::L0MuonDatas::const_iterator itdata;
  for (itdata = pdatas->begin() ; itdata!=pdatas->end() ; ++itdata){
    LHCb::MuonTileID mkey = (*itdata)->key();
    std::vector<LHCb::MuonTileID> ols = (*itdata)->ols();
    if (ols.size()>0) {
      if( msgLevel(MSG::VERBOSE) ) {
        verbose()  <<"  -  PU: "<<mkey.toString()
                   <<" => "<<ols.size()
                   <<" hits found"<<endmsg;
      }
      for (std::vector<LHCb::MuonTileID>::iterator itol=ols.begin(); itol!=ols.end(); ++itol){
        //              if( msgLevel(MSG::DEBUG) )debug()  <<"fillOLsfromDigits:       "<<(*itol).toString()<<endmsg;
        m_digits.push_back(*itol);
      }
    }

  }

  return StatusCode::SUCCESS;
}


StatusCode L0MuonAlg::fillOLsfromDigits()
{

  if( msgLevel(MSG::DEBUG) ) debug() << "fillOLsfromDigits:  m_digits.size()= "<<m_digits.size()<<endmsg;

  if( msgLevel(MSG::VERBOSE) ) {
    std::vector<LHCb::MuonTileID>::const_iterator id;
    for( id = m_digits.begin() ; id != m_digits.end() ; id++ ){
      LHCb::MuonTileID mkey = *id;
      verbose() << "fillOLsfromDigits:     mkey: "<<mkey.toString()<<endmsg;
    }
  }

  // -- Force M3 (add all M3 hits!!)
  if (m_forceM3) {
    if( msgLevel(MSG::DEBUG) ) debug() << "fillOLsfromDigits:  FORCING M3 TO 1 "<<endmsg;
    std::vector<LHCb::MuonTileID> m3StripV=m_stripV.stationLayout(2).tiles(2); // stripV in M3 Q3
    std::vector<LHCb::MuonTileID> m3StripH=m_stripH.stationLayout(2).tiles(2); // stripH in M3 Q3
    std::vector<LHCb::MuonTileID>::iterator itstrip;
    for (itstrip=m3StripV.begin();itstrip<m3StripV.end();++itstrip) itstrip->setStation(2);
    for (itstrip=m3StripH.begin();itstrip<m3StripH.end();++itstrip) itstrip->setStation(2);
    m_digits.insert(m_digits.begin(),m3StripV.begin(),m3StripV.end());
    m_digits.insert(m_digits.begin(),m3StripH.begin(),m3StripH.end());
    if( msgLevel(MSG::DEBUG) ) debug() << "fillOLsfromDigits:  m_digits.size()= "<<m_digits.size()<<endmsg;
  }// -- force M3 done

  std::vector<LHCb::MuonTileID>::const_iterator id;
  for( id = m_digits.begin() ; id != m_digits.end() ; id++ ){

    LHCb::MuonTileID mkey = *id;

    if (!mkey.isValid()) {
      warning()<<mkey.toString()<<"is not valid ... skip tile"<<endmsg;
      continue;
    }
    
    // Skip M1 tile
    if ( m_ignoreM1 && mkey.station()==0 ) continue; 

    // MuonTileID of the OL containing the hit
    LHCb::MuonTileID olID = m_ollayout.contains(mkey); 

    // Name of the OL register
    char bufnm[1024];
    sprintf(bufnm,"(Q%d,R%d,%d,%d)",
            olID.quarter(),olID.region(),olID.nX(),olID.nY());
    char buf[4096];
    sprintf(buf,"OL_%d_%s",mkey.station(),bufnm);

    // Get OL register from the register factory
    L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();
    L0Muon::TileRegister* pReg = rfactory->createTileRegister(buf,0);
    
    // Add the hit to the register
    pReg->setTile(mkey);

    if( msgLevel(MSG::VERBOSE) ) {
      verbose() << "fillOLsfromDigits:  mkey: "<<mkey.toString()
                << " olID: "<<olID.toString()
                << " buf: "<<buf<<endmsg;
    }

  }

  if( msgLevel(MSG::DEBUG) ) {
    debug() << "fillOLsfromDigits:     DONE "<<endmsg;
  }

  return StatusCode::SUCCESS;
}

StatusCode L0MuonAlg::updateL0CondProc()
{

  if ( ! m_l0CondProc -> exists( "Q1PB1_PU0_FOIX" ) ) {
    Warning("Q1PB1_PU0_FOIX parameter does not exist in DB").ignore() ;
    Warning("Use default FOIX").ignore() ;
  } else {
    std::vector<int> cond_foix =m_l0CondProc -> paramAsIntVect( "Q1PB1_PU0_FOIX" ) ;
    if (cond_foix.size()!=2) {
      Warning("Q1PB1_PU0_FOIX parameter has wrong size").ignore() ;
      Warning("Use default FOIX").ignore() ;
    } else {
      m_foiXSize.clear();
      int sta = 0;
      for (std::vector<int>::iterator it = cond_foix.begin(); it<cond_foix.end() ; ++it) {
        int ifoi = (*it);
        for (int j=1; j>=0; --j) {
          int hardFOI = ( ( ifoi>>(j*4) ) & 0xF );
          int softFOI = hardFOI * hard2softFOIConversion(sta);
          m_foiXSize.push_back( softFOI );
          ++sta;
        }
        if (sta==2) m_foiXSize.push_back( 0 ); // M3
        ++sta; 
      }
    }
  }

  if ( ! m_l0CondProc -> exists( "Q1PB1_PU0_FOIY" ) ) {
    Warning("Q1PB1_PU0_FOIY parameter does not exist in DB").ignore() ;
    Warning("Use default FOIY").ignore() ;
  } else {
    std::vector<int> cond_foiy =m_l0CondProc -> paramAsIntVect( "Q1PB1_PU0_FOIY" ) ;
    if (cond_foiy.size()!=2) {
      Warning("Q1PB1_PU0_FOIY parameter has wrong size").ignore() ;
      Warning("Use default FOIY").ignore() ;
    } else {
      m_foiYSize.clear();
      int sta = 0;
      for (std::vector<int>::iterator it = cond_foiy.begin(); it<cond_foiy.end() ; ++it) {
        int ifoi = (*it);
        for (int j=1; j>=0; --j) {
          int FOI = ( ( ifoi>>(j*4) ) & 0xF );
          m_foiYSize.push_back( FOI );
          ++sta;
        }
        if (sta==2) m_foiYSize.push_back( 0 ); // M3
        ++sta; 
      }
    }
  }

  // Set the properties of the MuonTriggerUnit
  m_muontriggerunit->setProperties(l0MuonProperties());
  
  return StatusCode::SUCCESS;
}

StatusCode L0MuonAlg::updateL0CondCtrl()
{
  if ( ! m_l0CondCtrl -> exists( "Q1CB0.versionEMUL" ) ) {
    Warning("Q1CB0.versionEMUL parameter does not exist in DB").ignore() ;
    Warning("Use default processor version").ignore() ;
  } else {
    std::string s_version = m_l0CondCtrl -> paramAsString( "Q1CB0.versionEMUL" ) ;
    m_version = atoi( s_version.c_str() );
  }

  // Set the properties of the MuonTriggerUnit
  m_muontriggerunit->setProperties(l0MuonProperties());
  
  return StatusCode::SUCCESS;
}
