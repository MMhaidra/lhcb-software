// $Id: CaloAdcFromRaw.cpp,v 1.1 2009-08-17 12:11:15 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "CaloAdcFromRaw.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloAdcFromRaw
//
//  generic algorithm to put Calo(L0)Adc and L0PrsSpdBit on TES from rawEvent
//  Possibility to apply condDB calibration factor on ADC
//  ---> to be used to produce mis-calibrated data from calibrated ADC !!
//
// 2009-04-07 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CaloAdcFromRaw );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloAdcFromRaw::CaloAdcFromRaw( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ){
  declareProperty("AdcLocation"        , m_location = "");
  declareProperty("L0AdcLocation"      , m_l0Location = "");
  declareProperty("L0BitLocation"      , m_l0BitLocation = "");
  declareProperty("DeCalibration"      , m_calib = false); // Expert usage

  // set default detectorName
  int index = name.find_last_of(".") +1 ; // return 0 if '.' not found --> OK !!
  m_detectorName = name.substr( index, 4 ); 
  if ( name.substr(index,3) == "Prs" ) m_detectorName = "Prs";
  if ( name.substr(index,3) == "Spd" ) m_detectorName = "Spd";

 if( "Ecal" == m_detectorName ) {
   m_location      = LHCb::CaloAdcLocation::Ecal    ;
   m_l0Location    = LHCb::L0CaloAdcLocation::Ecal  ;
   m_l0BitLocation = "";
   m_caloName    = DeCalorimeterLocation::Ecal;
   m_offset = +256;
 }else if( "Hcal" == m_detectorName ) {
   m_location    = LHCb::CaloAdcLocation::Hcal    ;
   m_l0Location  = LHCb::L0CaloAdcLocation::Hcal ;
   m_l0BitLocation = "";
   m_caloName = DeCalorimeterLocation::Hcal;
   m_offset = +256;
 }else if( "Prs" == m_detectorName ) {
   m_location       = LHCb::CaloAdcLocation::Prs ;
   m_l0Location     = "";
   m_l0BitLocation  = LHCb::L0PrsSpdHitLocation::Prs;
   m_caloName = DeCalorimeterLocation::Prs;
   m_offset = 0;
 }else if( "Spd" == m_detectorName ) {
   m_location       = "";
   m_l0Location     = "";
   m_l0BitLocation  = LHCb::L0PrsSpdHitLocation::Spd;
   m_caloName = DeCalorimeterLocation::Spd;
   m_offset = 0;
 }else{
   m_location = "";
   m_l0Location = "";
   m_l0BitLocation = "";
   m_caloName = "";
   m_offset = 0;
 }
 


}
//=============================================================================
// Destructor
//=============================================================================
CaloAdcFromRaw::~CaloAdcFromRaw() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CaloAdcFromRaw::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;


  // get detector elements
  if( m_caloName == "")return Error("Unknown calo detector name " + m_detectorName,StatusCode::FAILURE);
  m_calo  = getDet<DeCalorimeter>( m_caloName );
  // get data provider tools
  m_data = tool<ICaloDataProvider>("CaloDataProvider", m_detectorName + "DataProvider",this);
  m_l0data = tool<ICaloL0DataProvider>("CaloL0DataProvider",m_detectorName + "L0DataProvider",this);

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloAdcFromRaw::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // ADCs (ecal/hcal/prs)
  if( m_location != ""  && m_data->getBanks()){
    LHCb::CaloAdcs* outs = new LHCb::CaloAdcs();
    put( outs , m_location);
    CaloVector<LHCb::CaloAdc>& adcs = m_data->adcs();

    debug() << " #ADCS " << adcs.size() << endreq;
    for(CaloVector<LHCb::CaloAdc>::iterator iadc = adcs.begin();adcs.end()!=iadc;++iadc){
      LHCb::CaloAdc adc = *iadc;
      LHCb::CaloCellID id = adc.cellID();
      int value = adc.adc();
      double calib = 1.;
      if( m_calib){
        const CellParam& cParam = m_calo->cellParam( id );
        calib  = cParam.calibration();
        value = (calib > 0) ? int( (double) adc.adc() / calib) : 0 ;
        if( m_calo->isDead( id ))value = 0;
      }
      const unsigned int max = m_calo->adcMax();
      int satur = max - m_offset;
      if( value > satur      )value = satur;
      if( value < -m_offset  )value = -m_offset;
      LHCb::CaloAdc* out = new LHCb::CaloAdc( id, value);
      if ( msgLevel( MSG::DEBUG) )
        debug() << "Inserting : " << id << " adc = " << value << "  =  " << adc.adc() << " / " << calib 
                << "  (dead channel ? " << m_calo->isDead( id ) << ")" << endreq;
      outs->insert(out);
    }    
  }
  // L0ADCs (ecal/hcal)
  if( m_l0Location != ""  && m_l0data->getBanks() ){
    LHCb::L0CaloAdcs* outs = new LHCb::L0CaloAdcs();
    put( outs , m_l0Location);
    CaloVector<LHCb::L0CaloAdc>& adcs = m_l0data->l0Adcs();
    for(CaloVector<LHCb::L0CaloAdc>::iterator iadc = adcs.begin();adcs.end()!=iadc;++iadc){
      LHCb::L0CaloAdc adc = *iadc;
      LHCb::CaloCellID id = adc.cellID();
      int value = adc.adc();
      double calib = 1.;
      if( m_calib){
        CellParam cParam = m_calo->cellParam( id );
        calib  = cParam.calibration();
        value = ( calib > 0) ? int( (double) adc.adc() / calib) : 0;
        if( m_calo->isDead( id ))value = 0;
      }
      int satur = 255; // 8 bit L0ADC
      if( value > satur)value = satur;
      if( value < 0  )value = 0;
      LHCb::L0CaloAdc* out = new LHCb::L0CaloAdc( id, value);
      if ( msgLevel( MSG::DEBUG) )
        debug() << "Inserting : " << id << " l0adc = " << value << "  =  " << adc.adc() << " / " << calib 
                << "  (dead channel ? " << m_calo->isDead( id ) << ")" << endreq;
      outs->insert(out);
    }
  }
  // Trigger bits (prs/spd)
  if( m_l0BitLocation != ""  && m_l0data->getBanks() ){
    LHCb::L0PrsSpdHits* outs = new LHCb::L0PrsSpdHits();
    put( outs , m_l0BitLocation);
    CaloVector<LHCb::L0CaloAdc>& adcs = m_l0data->l0Adcs();
    for(CaloVector<LHCb::L0CaloAdc>::iterator iadc = adcs.begin();adcs.end()!=iadc;++iadc){
      LHCb::L0CaloAdc adc = *iadc;
      LHCb::CaloCellID id = adc.cellID();
      int value = adc.adc();
      if( m_calib & m_calo->isDead( id ))value = 0;
      int satur = 1; // 1 bit trigger
      if(value > satur)value = satur;
      if(value < 0)value = 0;
      if( 1 == value){
        LHCb::L0PrsSpdHit* out = new LHCb::L0PrsSpdHit( id );
        if ( msgLevel( MSG::DEBUG) )
          debug() << "Inserting : " << id << " bit = " << value 
                  << "  (dead channel ? " << m_calo->isDead( id ) << ")" << endreq;
        outs->insert(out);
      }
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloAdcFromRaw::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
