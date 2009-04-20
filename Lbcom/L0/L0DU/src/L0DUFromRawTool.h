// $Id: L0DUFromRawTool.h,v 1.12 2009-04-20 09:48:33 odescham Exp $
#ifndef L0DUFROMRAWTOOL_H 
#define L0DUFROMRAWTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// Interface
#include "L0Interfaces/IL0DUFromRawTool.h"            
#include "L0Interfaces/IL0DUConfigProvider.h"
#include "L0Interfaces/IL0DUEmulatorTool.h"
#include "L0Interfaces/IL0CondDBProvider.h"
// from Event
#include "Event/RawEvent.h"
#include "Event/RawBankReadoutStatus.h"


/** @class L0DUFromRawTool L0DUFromRawTool.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2007-10-19
 */
class L0DUFromRawTool : public GaudiTool, virtual public IL0DUFromRawTool {
public: 
  /// Standard constructor
  L0DUFromRawTool( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~L0DUFromRawTool( ); ///< Destructor
  virtual StatusCode initialize();
  bool decodeBank(int ibank=0 );

  unsigned int data(std::string name);
  virtual unsigned int version(){return m_vsn;};
  unsigned int tck(){  return m_tck; }
  unsigned int firmware(){  return m_pgaVsn; }
  const std::pair<unsigned int,unsigned int> bcid(){  return std::make_pair(m_bcid2,m_bcid3); }
  unsigned int rsda(){  return m_rsda; }
  unsigned int muonCleaningPattern(){  return m_muCleanPattern; }
  unsigned int status(){  return m_status; }
  unsigned int size(){return m_size;  }
  unsigned long roStatus(){return m_roStatus.status();  }
  void fillDataMap(bool fill = true){m_fill = fill;}
  

  LHCb::L0DUReport report(){return m_report;}
  LHCb::L0ProcessorDatas* L0ProcessorDatas(){return m_processorDatas;}
  
protected:

private:
  bool decoding(int ibank);
  bool getL0DUBanksFromRaw();
  inline void encode(std::string name, unsigned int data ,  const unsigned int base[L0DUBase::Index::Size]);
  inline void dataMap(std::string name, unsigned int data );
  void fillBCIDData();
  double scale(unsigned int base);
  inline bool nextData();
  void putStatusOnTES();
  //
  std::string m_rawLocation;
  std::string m_emulatorType;
  std::string m_configName;
  bool m_muonNoZsup;
  int m_force;
  int m_sumSize;
  

  IL0DUConfigProvider*   m_confTool;
  IL0DUEmulatorTool*     m_emuTool;
  IL0CondDBProvider*     m_condDB;  
  //
  const std::vector<LHCb::RawBank*>* m_banks;  
  std::map<std::string, unsigned int> m_dataMap;
  unsigned int m_vsn;
  unsigned int m_status;
  unsigned int m_pgaVsn;
  unsigned int m_bcid2;
  unsigned int m_bcid3;
  unsigned int m_rsda;
  unsigned int m_muCleanPattern;
  std::map<std::pair<int , unsigned int>, unsigned int > m_ecs;
  std::map<std::pair<int , unsigned int>, unsigned int > m_cds;
  std::map<std::pair<int , unsigned int>, unsigned int > m_tcs;
  std::map<int , unsigned int> m_sumEt;
  
  LHCb::L0DUReport m_report;
  LHCb::L0ProcessorDatas* m_processorDatas;
  unsigned int m_tck;  
  bool m_warning;
  unsigned int m_size;
  LHCb::RawBankReadoutStatus m_roStatus;
  //
  unsigned int* m_data;
  unsigned int  m_source;
  bool m_warn;
  bool m_fill;
  bool m_emu;
  bool m_encode;
  bool m_stat;
};
#endif // L0DUFROMRAWTOOL_H




inline void L0DUFromRawTool::encode(std::string name, unsigned int data ,  const unsigned int base[L0DUBase::Index::Size]){
  if(name != "")dataMap(name,data);
  if(!m_encode)return;
  LHCb::L0ProcessorData* fiber = m_processorDatas->object( base[ L0DUBase::Index::Fiber ]  )  ;
  unsigned int word = fiber->word();  
  word |= ( (data << base[L0DUBase::Index::Shift]) & base[L0DUBase::Index::Mask] );
  fiber->setWord( word);
  if( L0DUBase::Fiber::Empty != base[ L0DUBase::Index::Fiber2 ]  ) {
    fiber = m_processorDatas->object( base[ L0DUBase::Index::Fiber2 ]  )  ;
    word = fiber->word();
    unsigned int val = data >> base[L0DUBase::Index::Offset];
    word |= ( ( val << base[L0DUBase::Index::Shift2]) & base[L0DUBase::Index::Mask2] );
    fiber->setWord( word);
  }  
}

inline void L0DUFromRawTool::dataMap(std::string name, unsigned int data){
  if(m_fill)m_dataMap[name]=data;
}

inline bool L0DUFromRawTool::nextData(){
  if( NULL == ++m_data){
    Error("READOUTSTATUS : No more data in bank --> CORRUPTION",StatusCode::SUCCESS).ignore();
    m_roStatus.addStatus( m_source , LHCb::RawBankReadoutStatus::Corrupted );
    m_roStatus.addStatus( m_source , LHCb::RawBankReadoutStatus::Incomplete);
    return false;
  }else{
    if ( msgLevel( MSG::VERBOSE) )verbose() << "data = " <<  format("0x%04X", *m_data) << endreq;
  }
  return true;
}
