// $Id: HltSelectionToTES.cpp,v 1.8 2009-05-30 11:46:11 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "HltBase/HltSelection.h"
#include "HltSelectionToTES.h"

using namespace LHCb;


template <class CONT>
CONT* copy(const Hlt::Selection& sel) 
{
  typedef typename Hlt::TSelection<typename CONT::contained_type> TSelection; 
  typedef typename TSelection::const_iterator TSelectionIterator;
  CONT* cont = new CONT();
  const TSelection& tsel = dynamic_cast<const TSelection&>(sel);
  for (TSelectionIterator it = tsel.begin(); it != tsel.end(); ++it) {
     cont->insert((*it)->clone());
  }
  return cont;
}

//-----------------------------------------------------------------------------
// Implementation file for class : HltSelectionToTES
//
// 2006-09-01 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltSelectionToTES );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltSelectionToTES::HltSelectionToTES( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : HltBaseAlg ( name , pSvcLocator )
{
  declareProperty("CopyAll",m_copyAll = false);
  declareProperty("Copy", m_AselectionNames);
  declareProperty("OutputTrackLocationPrefix", 
                  m_trackLocation = "Hlt/Selection/Track/");
  declareProperty("OutputVertexLocationPrefix", 
                  m_vertexLocation = "Hlt/Selection/Vertex/");

}
//=============================================================================
// Destructor
//=============================================================================
HltSelectionToTES::~HltSelectionToTES() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltSelectionToTES::initialize() {
  
  StatusCode sc = HltBaseAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  m_selectionIDs.clear();
  if (m_copyAll) {
    std::vector<stringKey> selections = dataSvc().selectionKeys();      
    for (std::vector<stringKey>::const_iterator i  = selections.begin(); 
         i != selections.end(); ++i) m_selectionIDs.push_back(i->str());
  } else {
    std::vector<std::string> cromos = m_AselectionNames.value();
    for (std::vector<std::string>::iterator it = cromos.begin();
         it != cromos.end(); ++it) {
      stringKey name(*it);
      if (!dataSvc().hasSelection(name)) {
        error() << " Not a valid selection name " << name << endreq;
      } else {
        m_selectionIDs.push_back(name);
      }
    }
  }
  
  for (std::vector<stringKey>::iterator it = m_selectionIDs.begin();
       it != m_selectionIDs.end(); ++it) {
    info() << " will copy selection " << it->str() << " into TES "  << endreq;
  }
  
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================

StatusCode HltSelectionToTES::execute() {

  StatusCode sc = StatusCode::SUCCESS;
  
  for (std::vector<stringKey>::iterator it = m_selectionIDs.begin();
       it != m_selectionIDs.end(); ++it) {
    Hlt::Selection* sel = dataSvc().selection(*it,this);
    if (sel==0) {
      warning() << " no hlt selection to put in TES " << it->str() << endreq;
      continue;
    }
    if (sel->classID() == LHCb::Track::classID()) {
      LHCb::Tracks* tracks = 
          sel->decision() ? copy<LHCb::Tracks>(*sel)
                          : new LHCb::Tracks();
      put(tracks,m_trackLocation+it->str());
    } else if (sel->classID() == LHCb::RecVertex::classID()) {
      LHCb::RecVertices* vertices = 
          sel->decision() ? copy<LHCb::RecVertices>(*sel)
                          : new LHCb::RecVertices();
      put(vertices,m_vertexLocation+it->str());
    }
  }
  setFilterPassed(true);  
  return sc;
}

