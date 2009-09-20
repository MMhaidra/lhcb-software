#include <vector>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/bind.hpp>

#include "dim/dic.hxx"
#include "presenter.h"
#include "PresenterMainFrame.h"
#include "Archive.h"
#include "ParallelWait.h"
#include "OnlineHistDB/OnlineHistDB.h"
#include "DbRootHist.h"
// #include "TThread.h"

using namespace pres;
using namespace std;
using namespace boost;

boost::mutex listMutex;
boost::recursive_mutex oraMutex;
boost::recursive_mutex dimMutex;
boost::recursive_mutex rootMutex;


DbRootHist* getPageHistogram(PresenterMainFrame * presenter, 
                         const std::string & identifier,
                         const std::string & currentPartition,
                         int refreshTime,
                         int instance,
                         OnlineHistDB* histogramDB,
                         OMAlib* analysisLib,
                         OnlineHistogram* onlineHist,
                         pres::MsgLevel verbosity,
                         DimBrowser* dimBrowser)
{
  std::vector<std::string*> tasksNotRunning;
  DbRootHist* dbRootHist = new DbRootHist(identifier,
                                          currentPartition,
                                          refreshTime, instance,
                                          histogramDB, analysisLib,
                                          onlineHist,
                                          verbosity,
                                          dimBrowser,oraMutex,
                                          dimMutex,
                                          tasksNotRunning,
                                          rootMutex);
  if (0 != presenter) {
    dbRootHist->setPresenter(presenter);
    dbRootHist->initHistogram();
// _addFromDim?    
    if (histogramDB) { dbRootHist->setTH1FromDB();}
  }

//    if (0 != presenter &&
//        0 != presenter->archive() &&
//        ((History == presenter->presenterMode()) ||
//         (EditorOffline == presenter->presenterMode() && presenter->canWriteToHistogramDB()))) {
//      presenter->archive()->fillHistogram(dbRootHist,
//                               presenter->rw_timePoint,
//                               presenter->rw_pastDuration);
//   }
                      
  std::vector<std::string*>::const_iterator tasksNotRunningIt;
  for (tasksNotRunningIt = tasksNotRunning.begin();
       tasksNotRunningIt != tasksNotRunning.end();
       ++tasksNotRunningIt) {
    delete *tasksNotRunningIt;
  }
  tasksNotRunning.clear();                                  
  return dbRootHist;
}

void getHistogramsFromLists(PresenterMainFrame * presenter,
                            OnlineHistoOnPage* onlineHistosOnPage,
                            std::vector<DbRootHist*> * dbHistosOnPage,
                            std::vector<std::string*> & m_tasksNotRunning)
{
          DbRootHist* dbRootHist;
          if( (History == presenter->presenterMode()) ||
              (EditorOffline == presenter->presenterMode())) { // no need for DIM
            dbRootHist = new DbRootHist(onlineHistosOnPage->histo->identifier(),
                                        std::string(""),
                                        2, onlineHistosOnPage->instance,
                                        presenter->histogramDB(),
                                        presenter->analysisLib(),
                                        onlineHistosOnPage->histo,
                                        presenter->verbosity(),
                                        NULL,
                                        oraMutex,
                                        dimMutex,
                                        m_tasksNotRunning,
                                        rootMutex);
          } else if( (Online == presenter->presenterMode()) ||
                     (EditorOnline == presenter->presenterMode())) {
            DimBrowser* dimBrowser = NULL;
//          std::string currentPartition("");

            dimBrowser = presenter->dimBrowser();
//          currentPartition = presenter->currentPartition();
            dbRootHist = new DbRootHist(onlineHistosOnPage->histo->identifier(),
                                        presenter->currentPartition(),
                                        2, onlineHistosOnPage->instance,
                                        presenter->histogramDB(),
                                        presenter->analysisLib(),
                                        onlineHistosOnPage->histo,
                                        presenter->verbosity(),
                                        dimBrowser,
                                        oraMutex,
                                        dimMutex,
                                        m_tasksNotRunning,
                                        rootMutex);

//          if (dbRootHist->isEmptyHisto()) {
//            m_tasksNotRunning.push_back(onlineHistosOnPage->histo->task());
//          }
          }

   boost::recursive_mutex::scoped_lock lock(rootMutex);
   if (lock) {
     if (0 != presenter) {
       dbRootHist->setPresenter(presenter);
       dbRootHist->initHistogram();
       dbRootHist->setTH1FromDB();
     }
     if (0 != presenter->archive() &&
         ( (History == presenter->presenterMode()) ||
           (EditorOffline == presenter->presenterMode() &&
       presenter->canWriteToHistogramDB())) ) {
       presenter->archive()->fillHistogram(dbRootHist,
                                           presenter->rw_timePoint,
                                           presenter->rw_pastDuration);
     }
//              if (dbRootHist && dbRootHist->isEmptyHisto()) {
//                presenter->setResumeRefreshAfterLoading(false);
//              }
   }
//        dbHistosOnPage->push_back(dbRootHist);
   boost::mutex::scoped_lock listLock(listMutex);
   if (listLock) {
     dbHistosOnPage->push_back(dbRootHist);
   }
}

void refreshHisto(DbRootHist* dbHistoOnPage)
{

  if (false == gROOT->IsInterrupted()) { // false == dbHistoOnPage->isEmptyHisto() &&
    dbHistoOnPage->fillHistogram();
    dbHistoOnPage->normalizeReference();
//      } else {
//    dbHistoOnPage->beRegularHisto();
//    dbHistoOnPage->resetRetryInit();
//    dbHistoOnPage->initHistogram();
//              if (false == dbHistoOnPage->isEmptyHisto()) {
//                      dbHistoOnPage->fillHistogram();
//                dbHistoOnPage->setTH1FromDB();
//      dbHistoOnPage->normalizeReference();
//              }
  }
}

ParallelWait::ParallelWait(PresenterMainFrame* presenter):
  m_presenterApp(presenter)
{
}
ParallelWait::~ParallelWait()
{
  std::vector<std::string*>::const_iterator m_tasksNotRunningIt;
  for (m_tasksNotRunningIt = m_tasksNotRunning.begin();
       m_tasksNotRunningIt != m_tasksNotRunning.end();
       ++m_tasksNotRunningIt) {
    delete *m_tasksNotRunningIt;
  }
  m_tasksNotRunning.clear();  
}

void ParallelWait::loadHistograms(const std::vector<OnlineHistoOnPage*>* onlineHistosOnPage,
                                  std::vector<DbRootHist*> * dbHistosOnPage) {
//  TThread::Lock();
        m_onlineHistosOnPageIt = onlineHistosOnPage->begin();

//  int num_threads = onlineHistosOnPage->nhs=();
  boost::thread_group thrds;

//      m_tasksNotRunning.clear();
  while (m_onlineHistosOnPageIt != onlineHistosOnPage->end()) {
//            startBenchmark((*m_onlineHistosOnPageIt)->histo->identifier());
    thrds.create_thread(boost::bind(&getHistogramsFromLists,
                                    m_presenterApp,
                                    *m_onlineHistosOnPageIt,
                                    dbHistosOnPage,
                                    m_tasksNotRunning));
          m_onlineHistosOnPageIt++;
  }
  thrds.join_all();
//      TThread::UnLock();

}

void ParallelWait::refreshHistograms(std::vector<DbRootHist*>* dbHistosOnPage)
{
//  TThread::Lock();

  std::vector<std::string*>::const_iterator m_tasksNotRunningIt;
  for (m_tasksNotRunningIt = m_tasksNotRunning.begin();
       m_tasksNotRunningIt != m_tasksNotRunning.end();
       ++m_tasksNotRunningIt) {
    delete *m_tasksNotRunningIt;
  }
  m_tasksNotRunning.clear();
    
  boost::thread_group thrds;

  std::vector<DbRootHist*>::iterator refresh_dbHistosOnPageIt;
  refresh_dbHistosOnPageIt = dbHistosOnPage->begin();
  while (refresh_dbHistosOnPageIt != dbHistosOnPage->end()) {
    if ( (pres::TCKinfo == (*refresh_dbHistosOnPageIt)->effServiceType()) &&
         (Batch != m_presenterApp->presenterMode()) ) {
        m_presenterApp->setTCK(((*refresh_dbHistosOnPageIt)->rootHistogram)->GetTitle());
        m_presenterApp->setStatusBarText(((*refresh_dbHistosOnPageIt)->rootHistogram)->GetTitle(), 2);
      }
    thrds.create_thread(boost::bind(&refreshHisto,
                                    *refresh_dbHistosOnPageIt));
                                    
    refresh_dbHistosOnPageIt++;
  }
  
  thrds.join_all();
  
//  TThread::UnLock();    
}


