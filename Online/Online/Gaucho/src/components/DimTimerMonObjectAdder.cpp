#include "Gaucho/DimTimerMonObjectAdder.h"
#include "Gaucho/DimInfoMonObject.h"
#include "Gaucho/DimServiceMonObject.h"
#include "Gaucho/MonObject.h"
#include "Gaucho/DimMonObjectManager.h"

DimTimerMonObjectAdder::DimTimerMonObjectAdder (DimMonObjectManager* manager, int ttime, std::vector<std::vector<DimInfoMonObject*> > dimInfo2D, std::vector<DimServiceMonObject*> dimService1D) : DimTimer(ttime) {
  m_dimMonObjectManager  = manager;
  m_dimInfoMonObjectVect2D    = dimInfo2D;
  m_dimServiceMonObjectVect1D = dimService1D;
  m_time=ttime;
  start(m_time);
}

void DimTimerMonObjectAdder::timerHandler(){
  for (int i = 0; i < (int)m_dimServiceMonObjectVect1D.size(); i++) {
    MonObject* monObjectAdder = m_dimServiceMonObjectVect1D[i]->monObject();
    monObjectAdder->reset();
    for (int j = 0; j < (int)m_dimInfoMonObjectVect2D[i].size(); j++) {
      MonObject* monObject = m_dimInfoMonObjectVect2D[i][j]->monObject();
      monObjectAdder->combine(monObject);
    }
  }
  start(m_time);
}


