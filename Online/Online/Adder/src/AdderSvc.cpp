#include "Gaucho/ProcessMgr.h"
#include "Gaucho/DimInfoServers.h"
#include "Gaucho/Misc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/Incident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "Gaucho/BaseServiceMap.h"
#include "Gaucho/DimTimerProcess.h"
#include "Gaucho/DimInfoServices.h"
#include "AdderSvc.h"

#include <ctime>

#include "RTL/rtl.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"

DECLARE_NAMESPACE_SERVICE_FACTORY(LHCb, AdderSvc)

AdderSvc::AdderSvc(const std::string& name, ISvcLocator* ploc) : Service(name, ploc),/*m_monitorSvc(0), */m_incidentSvc(0)
{
  //declareProperty("partname",m_partName); adders doesn't consider services with partname
  declareProperty("taskname",m_taskName);
  declareProperty("subfarmname",m_subfarmName);
  declareProperty("algorithmname",m_algorithmName);
  declareProperty("objectname",m_objectName);
  declareProperty("refreshTime",  m_refreshTime=10);
  declareProperty("dimclientdns",m_dimClientDns);
  declareProperty("publishRates",m_publishRates=0);

  //declareProperty("savedir", m_saveDir);
  m_enablePostEvents = true;
}

AdderSvc::~AdderSvc() {}

StatusCode AdderSvc::initialize() {
  StatusCode sc = Service::initialize(); // must be executed first
 // lib_rtl_sleep(1000);
  MsgStream msg(msgSvc(), name());
  if ( !sc.isSuccess() )  {
    msg << MSG::ERROR << "Cannot initialize service base class." << endmsg;
    return StatusCode::FAILURE;
  }

  m_utgid = RTL::processName();
  std::size_t first_us = m_utgid.find("_");
  std::size_t second_us = m_utgid.find("_", first_us + 1);
  m_nodeName = m_utgid.substr(0, first_us);

  std::string adderType;
  if (m_nodeName.size() == 8) adderType = "First Level"; 
  else if ((m_nodeName.size() == 6)&&(m_nodeName.substr(0,4)!="PART")) adderType = "Second Level"; 
  else if ((m_nodeName.size() == 6)&&(m_nodeName.substr(0,4)=="PART")) adderType = "Third Level"; 
  else {
    msg << MSG::ERROR << "This is not an Adder because the nodeName do not correspond at any case" << endreq;
    msg << MSG::ERROR << "1rst level ==> HLTA0101" << endreq;
    msg << MSG::ERROR << "2nd  level ==> HLTA01" << endreq;
    msg << MSG::ERROR << "3rd  level ==> PARTxx" << endreq;
    return StatusCode::FAILURE;
  }

  std::string taskName = m_utgid.substr(first_us + 1, second_us - first_us - 1);

  if ((taskName != "Adder")&&(taskName != "ADDER")) {
    msg << MSG::ERROR << "This is not an Adder !" << endreq;
    msg << MSG::ERROR << "Please try nodeName_Adder_1" << endreq;
    return StatusCode::FAILURE;
  }

  sc = service("IncidentSvc",m_incidentSvc,true);
  if ( !sc.isSuccess() )  {
    msg << MSG::ERROR << "Cannot access incident service." << endmsg;
    return StatusCode::FAILURE;
  }

  msg << MSG::DEBUG << "***************************************************** " << endreq;

  msg << MSG::DEBUG << "****************** Welcome to "<<adderType<<" Adder********* " << endreq;
  msg << MSG::DEBUG << "***************************************************** " << endreq;
  msg << MSG::DEBUG << "This Adder will add data published in : " << m_dimClientDns << endreq;

  msg << MSG::DEBUG << "Consider node " << m_nodeName << endreq;

  msg << MSG::DEBUG << "TaskNames: " << endreq;

  std::vector<std::string>::iterator it;

  //for(it = m_taskName.begin(); it != m_taskName.end(); ++it) {
    //msg << MSG::DEBUG << "         taskName: " << *it << endreq;
  //}
  msg << MSG::DEBUG << "         taskName: " << m_taskName << endreq;

  msg << MSG::DEBUG << "Properties: " << endreq;
  for(unsigned int i=0; i < m_objectName.size();++i) {
    msg << MSG::DEBUG << "         objectName: " << m_objectName[i] << endreq;
    msg << MSG::DEBUG << "         algorithmName: " << m_algorithmName[i] << endreq;
    msg << MSG::DEBUG << endreq;
  }

  msg << MSG::DEBUG << "Adder will publish data every " << m_refreshTime << " seconds"<< endreq;
  msg << MSG::DEBUG << "***************************************************** " << endreq;
  msg << MSG::DEBUG << "***************************************************** " << endreq;

  msg << MSG::DEBUG << "creating ProcessMgr" << endreq;
  m_processMgr = new ProcessMgr (s_Adder, msgSvc(), this, m_refreshTime);
  m_processMgr->setSubFarmVector(m_subfarmName);
  m_processMgr->setTaskName(m_taskName);
  m_processMgr->setAlgorithmVector(m_algorithmName);
  m_processMgr->setObjectVector(m_objectName);
  m_processMgr->setUtgid(m_utgid);
  if (m_publishRates == 1) m_processMgr->setPublishRates(true);

  m_processMgr->createInfoServers();

  m_processMgr->createTimerProcess();

  msg << MSG::DEBUG << "Activing PostEvent to StartTimer............." << endreq;
  IocSensor::instance().send(this, s_startTimer, this); //start Timer*/

  msg << MSG::DEBUG << "Finishing the initialize method." << endreq;
  return StatusCode::SUCCESS;
}

void AdderSvc::handle(const Event&  ev) {
  MsgStream msg(msgSvc(), name());
  
  if (!m_enablePostEvents) return;
    
  if(s_startTimer == ev.type) {
    msg << MSG::DEBUG << " We are inside a PostEvent to Start the Timer " << endreq;
    m_processMgr->dimTimerProcess()->start(m_refreshTime);
    msg << MSG::DEBUG << " End PostEvent to Start the Timer " << endreq;
  }
  else if(s_stopTimer == ev.type) {
    msg << MSG::DEBUG << " We are inside a PostEvent to Sop the Timer " << endreq;
    m_processMgr->dimTimerProcess()->stop();
    msg << MSG::DEBUG << " End PostEvent to Stop Timer " << endreq;
  }
  else if(s_createInfoServices == ev.type ){
    msg << MSG::DEBUG << " We are inside a PostEvent to Create the DimInfoServices " << endreq;
    msg << MSG::DEBUG << "Choosing Server to get ServicesSet.........." << endreq;

    std::string serverChoosed;
    while (true) {
      m_processMgr->dimInfoServers()->chooseServer();
      serverChoosed = m_processMgr->dimInfoServers()->serverChoosed();
      if ("" != serverChoosed) {
        msg << MSG::DEBUG << "Server choosen = " << m_processMgr->dimInfoServers()->serverChoosed() << endreq;
        break;
      }
    }
    msg << MSG::DEBUG << "Before createInfoServices............." << endreq;
    m_processMgr->createInfoServices(serverChoosed);

   // IocSensor::instance().send(this, s_updateServiceMap, ev.data); //start Timer*/
    msg << MSG::DEBUG << " End PostEvent to Create the DimInfoServices " << endreq;
  }
  else if(s_updateSvcMapFromInfoServer == ev.type) {
    msg << MSG::DEBUG << " We are inside a PostEvent to UpdateServiceMapFromInfoServer " << endreq;
    std::map<std::string, bool, std::less<std::string> > serverMap = m_processMgr->dimInfoServers()->serverMap();

//    std::pair<ProcessMgr*, std::map<std::string, bool, std::less<std::string> > >* data = (std::pair<ProcessMgr*, std::map<std::string, bool, std::less<std::string> > >*) ev.data;

//    std::map<std::string, bool, std::less<std::string> > serverMap = data->second;
        
    m_processMgr->serviceMap()->updateMap(serverMap);
    m_processMgr->serviceMap()->printMap();
    msg << MSG::DEBUG << " End PostEvent to UpdateServiceMap " << endreq;
  }
  else if(s_updateSvcMapFromInfoService == ev.type) {
    msg << MSG::DEBUG << " We are inside a PostEvent to UpdateServiceMapFromInfoService " << endreq;

    
    std::set<std::string> serviceSet = m_processMgr->dimInfoServices()->serviceSet();
 
   // std::pair<ProcessMgr*, std::set<std::string> >* data = (std::pair<ProcessMgr*, std::set<std::string> >*) ev.data;
   // std::set<std::string> serviceSet = data->second;

    m_processMgr->serviceMap()->setServiceSet(serviceSet);
    m_processMgr->updateMap();
    m_processMgr->serviceMap()->printMap();
    msg << MSG::DEBUG << " End PostEvent to UpdateServiceMapFromInfoService " << endreq;
  }
}

void AdderSvc::handle(const Incident& inc) {
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Got incident " << inc.type() << " from " << inc.source() <<endreq;
  //IocSensor::instance().send(this, s_COMMAND_IN_PROCESSMGR, this);
}

StatusCode AdderSvc::finalize() {
  MsgStream msg(msgSvc(), name());
  m_enablePostEvents = false;
  msg << MSG::DEBUG << "Finalize Adder..... " << endmsg;
  if (m_processMgr) {delete m_processMgr; m_processMgr=0;}
  return Service::finalize();
}

