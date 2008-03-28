#include "Gaucho/DimInfoMonObject.h"
#include "Gaucho/DimMonObjectManager.h"
#include "Gaucho/MonObject.h"
#include "GaudiKernel/IMessageSvc.h"
#include "GaudiKernel/MsgStream.h"

#include <iostream>
#include <string>
#include <stdlib.h>
#include <malloc.h>
#ifdef WIN32
namespace win {
#include <windows.h>
}
# define mysleep win::Sleep
#else
# define mysleep usleep
#endif


DimInfoMonObject::DimInfoMonObject(DimMonObjectManager* manager, std::string svcName): DimInfo((svcName).c_str(),-1.0){
  m_name = "DimInfoMonObject";
  IMessageSvc* msgSvc = manager->msgSvc();
  MsgStream mes(msgSvc, m_name);
  mes << MSG::INFO << "DimInfoMonObject(" + svcName + ") called with no refresh time. Using 5 s" << endreq;
  DimInfoMonObject(manager, svcName, 5);
}

DimInfoMonObject::DimInfoMonObject(DimMonObjectManager* manager, std::string svcName, int refreshTime): DimInfo((svcName).c_str(),refreshTime,-1.0), m_hasData(false){
  m_dimMonObjectManager  = manager;

  char buffer[3];
  sprintf(buffer, "%d", refreshTime);
  std::string strRefreshTime(buffer);

  m_svcname = svcName;
  m_rtime = refreshTime;
  m_monObject=0;
}

DimInfoMonObject::~DimInfoMonObject() {
}

void DimInfoMonObject::infoHandler() 
{
  if (m_monObject==0) createMonObject();
  else loadMonObject();
}

void DimInfoMonObject::createMonObject(){

  int tmpStringSize = -1;
  while ( (tmpStringSize = getSize()) <=0 ){usleep(10000);}

  m_StringSize = tmpStringSize;

  char* c = const_cast<char *>((const char*) getData());
  MonObject *monObjectBase;

  monObjectBase = m_dimMonObjectManager->createMonObject("MonObject");
  m_dimMonObjectManager->loadMonObject(c, m_StringSize, monObjectBase);

  std::string monObjectTypeName  = monObjectBase->typeName();
  m_dimMonObjectManager->deleteMonObject(monObjectBase);
  m_monObject = m_dimMonObjectManager->createMonObject(monObjectTypeName);
  m_dimMonObjectManager->loadMonObject(c, m_StringSize, m_monObject);
}

void DimInfoMonObject::deleteMonObject(){
  if (m_monObject==0) return;
  m_dimMonObjectManager->deleteMonObject(m_monObject);
}

void DimInfoMonObject::loadMonObject(){

  int tmpStringSize = -1;

  while ( (tmpStringSize = getSize()) <0 ){usleep(10000);}

  m_StringSize = tmpStringSize;

  char* c = const_cast<char *>((const char*) getData());

  m_monObject->reset();

  m_dimMonObjectManager->loadMonObject(c, m_StringSize, m_monObject);
}

MonObject *DimInfoMonObject::monObject() {
  return m_monObject;
}


