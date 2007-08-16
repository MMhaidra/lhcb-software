// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "HltSummaryTool.h"
#include "Event/HltNames.h"
#include "Event/HltEnums.h"
#include "HltBase/HltFunctions.h"
#include "HltBase/HltConfigurationHelper.h"
#include "HltBase/HltSummaryHelper.h"
#include "HltBase/IHltFunctionFactory.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltSummaryTool
//
// 2006-07-20 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------
using namespace LHCb;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HltSummaryTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltSummaryTool::HltSummaryTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  m_summary = NULL;
  
  declareInterface<IHltConfSummaryTool>(this);
  declareInterface<IHltSummaryTool>(this);
  
  declareProperty("SummaryLocation",
                  m_summaryLocation = LHCb::HltSummaryLocation::Default);
  
}
//=============================================================================
// Destructor
//=============================================================================
HltSummaryTool::~HltSummaryTool() {}

StatusCode HltSummaryTool::initialize() {

 IDataProviderSvc* hltsvc = NULL;
  StatusCode sc = serviceLocator()->service("HltDataSvc/EventDataSvc",hltsvc);
  if (!hltsvc) fatal() << " not able to create Hlt Svc provider " << endreq;

  m_conf = NULL;
 //  Hlt::DataHolder<Hlt::Configuration>* holder = 
//     get<Hlt::DataHolder<Hlt::Configuration> >(hltsvc,"Hlt/Configuration");
//   if (!holder) fatal() << " not able to retrieve configuration " << endreq;

//   m_conf = &(holder->object());

  //  IHltFunctionFactory* ctool;
  // ctool = tool<IHltFunctionFactory>("HltFunctionFactory");
  
  return sc;
} 

void HltSummaryTool::getSummary() {
  m_summary = NULL;
  std::string loca = m_summaryLocation;
  m_summary = get<LHCb::HltSummary >(loca);  
  if (! m_summary) error() << " No summaryy in TES! " << endreq;

  loca = m_summaryLocation+"/Configuration";
  Hlt::DataHolder< Hlt::Configuration >* m_holder = 
    get< Hlt::DataHolder<Hlt::Configuration> >(loca);
  m_conf = &(m_holder->object());
 
}

const HltSummary& HltSummaryTool::summary() {
  getSummary();
  return *m_summary;
}

bool HltSummaryTool::decision() {
  getSummary();
  return m_summary->decision();
}

bool HltSummaryTool::decisionType(const std::string& name) {
  getSummary();
  int id = (unsigned int) HltNames::decisionTypeID(name);
  return m_summary->checkDecisionType(id);
}

bool HltSummaryTool::hasSelection(const std::string& name) {
  getSummary();
  int id = HltConfigurationHelper::getID(*m_conf,"SelectionID",name);
  return m_summary->hasSelectionSummary(id);
}

bool HltSummaryTool::selectionDecision(const std::string& name) {
  getSummary();
  int id = HltConfigurationHelper::getID(*m_conf,"SelectionID",name);
  bool ok =  m_summary->hasSelectionSummary(id);
  if (!ok) return ok;
  return m_summary->selectionSummary(id).decision();
}

bool HltSummaryTool::selectionDecisionType(const std::string& name,
                                           const std::string& type) {
  getSummary();
  int id = HltConfigurationHelper::getID(*m_conf,"SelectionID",name);
  bool ok =  m_summary->hasSelectionSummary(id);
  if (!ok) return ok;
  HltSelectionSummary& sum = m_summary->selectionSummary(id);
  int itype = (unsigned int) HltNames::decisionTypeID(type);
  return sum.checkDecisionType(itype);
}


std::vector<std::string> HltSummaryTool::selections() {
  getSummary();
  std::vector<int> ids = m_summary->selectionSummaryIDs();
  std::vector<std::string> names;
  for (std::vector<int>::iterator it = ids.begin(); it!= ids.end(); ++it) {
    std::string name = HltConfigurationHelper::getName(*m_conf,"SelectionID",*it);
    names.push_back(name);
  }
  return names;  
}

size_t HltSummaryTool::selectionNCandidates(const std::string& name) {
  getSummary();
  int id = HltConfigurationHelper::getID(*m_conf,"SelectionID",name);
  return HltSummaryHelper::ncandidates(*m_summary,id);
}

std::vector<std::string> 
HltSummaryTool::selectionFilters(const std::string& name) {
  getSummary();
  std::vector<std::string> filters;
  std::string key = name+"/Filters";
  if (m_conf->has_key(key))
    filters = m_conf->retrieve<std::vector<std::string> >(key);
  return filters;
}

std::vector<std::string> 
HltSummaryTool::selectionInputSelections(const std::string& name) {
  getSummary();
  std::vector<std::string> inputs;
  std::string key = name+"/InputSelections";
  if (m_conf->has_key(key))
    inputs = m_conf->retrieve<std::vector<std::string> >(key);
  return inputs;
}

std::string 
HltSummaryTool::selectionType(const std::string& name) {
  getSummary();
  std::string type = "unknown";
  std::string key = name+"/SelectionType";
  if (m_conf->has_key(key)) type = m_conf->retrieve<std::string >(key);
  return type;
}

std::vector<Track*> 
HltSummaryTool::selectionTracks(const std::string& name) {
  std::vector<Track*> tracks;
  getSummary();
  int id = HltConfigurationHelper::getID(*m_conf,"SelectionID",name);
  if (!HltSummaryHelper::has<std::vector<Track*> >(*m_summary,id)) 
    return tracks;
  tracks = HltSummaryHelper::retrieve<std::vector<Track*> >(*m_summary,id);
  return tracks;
}

std::vector<RecVertex*> 
HltSummaryTool::selectionVertices(const std::string& name) {
  std::vector<RecVertex*> vertices;
  getSummary();
  int id = HltConfigurationHelper::getID(*m_conf,"SelectionID",name);
  if (!HltSummaryHelper::has< std::vector<RecVertex*> >(*m_summary,id)) 
    return vertices;
  vertices = 
    HltSummaryHelper::retrieve<std::vector<RecVertex*> >(*m_summary,id);
  return vertices;
}

std::vector<Particle*> 
HltSummaryTool::selectionParticles(const std::string& name) {
  std::vector<Particle*> pars;
  getSummary();
  int id = HltConfigurationHelper::getID(*m_conf,"SelectionID",name);
  return pars;
}

bool HltSummaryTool::isInSelection(const std::string& name,
                                   const Track& track) {
  bool ok = false;
  getSummary();
  std::vector<Track*> tracks = selectionTracks(name);
  for (std::vector<Track*>::const_iterator it = tracks.begin(); 
       it != tracks.end(); it++) {
    const Track& otrack = *(*it);
    if (HltUtils::matchIDs(track,otrack)) ok = true;
  }
  return ok;
}

bool HltSummaryTool::isInSelection( const Track& track, int id) {
  std::string name = HltConfigurationHelper::getName(*m_conf,"SelectionID",id);
  return isInSelection(name,track);
}


std::vector<std::string> HltSummaryTool::confKeys() {
  return m_conf->keys();
}

int HltSummaryTool::confInt(const std::string& name) {
  int val = -1;
  if (m_conf->has_key(name)) val = m_conf->retrieve<int>(name);
  return val;
}

double HltSummaryTool::confDouble(const std::string& name) {
  double val = 0.;
  if (m_conf->has_key(name)) val = m_conf->retrieve<double>(name);
  return val;
}

std::string HltSummaryTool::confString(const std::string& name) {
  std::string val = "unknown";
  if (m_conf->has_key(name)) val = m_conf->retrieve<std::string>(name);
  return val;
}

std::vector<std::string> HltSummaryTool::confStringVector(const std::string& name) 
{
  std::vector<std::string> val;
  if (m_conf->has_key(name)) 
    val = m_conf->retrieve<std::vector<std::string> >(name);
  return val;
}


