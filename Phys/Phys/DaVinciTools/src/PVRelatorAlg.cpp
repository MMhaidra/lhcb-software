// $Id: PVRelatorAlg.cpp,v 1.5 2009-05-25 08:48:16 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from LHCb
#include "Kernel/IOnOffline.h"
#include "Kernel/IRelatedPVFinder.h"
#include "Event/RecVertex.h"
#include "Event/Particle.h"
#include "Kernel/Particle2Vertex.h"
#include "Relations/Get.h"
// local
#include "PVRelatorAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PVRelatorAlg
//
// 2009-05-23 : Juan Palacios
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PVRelatorAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PVRelatorAlg::PVRelatorAlg( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : 
  GaudiAlgorithm ( name , pSvcLocator ),
  m_particleInputLocation(""),
  m_PVInputLocation(LHCb::RecVertexLocation::Primary),
  m_P2PVInputLocation(""),
  m_P2PVOutputLocation(""),
  m_useTable(false),
  m_OnOffline(0),
  m_pvRelator(0)
{
  declareProperty("ParticleInputLocation",  m_particleInputLocation);
  declareProperty("PrimaryVertexInputLocation",  m_PVInputLocation);
  declareProperty("P2PVRelationsInputLocation",  m_P2PVInputLocation);
  declareProperty("P2PVRelationsOutputLocation",  m_P2PVOutputLocation);
  
}
//=============================================================================
// Destructor
//=============================================================================
PVRelatorAlg::~PVRelatorAlg() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PVRelatorAlg::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  if (m_P2PVInputLocation!="") m_useTable=true;
  
  if ( m_useTable && 
       ( m_particleInputLocation!="" ||
         m_PVInputLocation!=LHCb::RecVertexLocation::Primary) ) {
    return Error("You have set P2PVRelationsLocation and one of ParticleInputLocation and PrimaryVertexInputLocation.",
                 StatusCode::FAILURE);    
  }

  if (""==m_P2PVOutputLocation) return Error("P2PVRelationsOutputLocation not set");
  
  m_OnOffline = tool<IOnOffline>("OnOfflineTool",this);

  if (0==m_OnOffline) return Error("Failed to get IOnOffline tool");
  
  m_pvRelator = tool<IRelatedPVFinder>(m_OnOffline->relatedPVFinderType(), 
                                       this);
 
  if (0==m_pvRelator) return Error("Failed to get IRelatedPVFinder tool");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PVRelatorAlg::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  Particle2Vertex::Table* table = (m_useTable) ? this->tableFromTable() : this->table();
  
  if (0!=table) {
    if ( msgLevel(MSG::DEBUG) ) {
      debug() << "Storing relations table in " 
              << m_P2PVOutputLocation << endmsg;
      put(table, m_P2PVOutputLocation);
      } else {
      Error("No table created!",10).ignore();
      }
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
Particle2Vertex::Table* PVRelatorAlg::table() const
{
  typedef LHCb::Particle::Container Particles;
  typedef LHCb::RecVertex::Container Vertices;
  typedef Particle2Vertex::LightTable RelTable;  

  const Particles* particles = i_get<Particles>(m_particleInputLocation);

  const Vertices* vertices = i_get<Vertices>(m_PVInputLocation);

  Particle2Vertex::Table* table = new Particle2Vertex::Table();

  if (0==particles || 0==vertices) return table;

  for (Particles::const_iterator iPart = particles->begin();
       iPart != particles->end(); ++ iPart) {
    const RelTable::Range range = 
      m_pvRelator->relatedPVs(*iPart, *vertices).relations();
    table->merge(range);
  }  
  return table;
 
}
//=============================================================================
//=============================================================================
Particle2Vertex::Table* PVRelatorAlg::tableFromTable() const
{
  typedef LHCb::Particle::ConstVector Particles;
  typedef LHCb::VertexBase::ConstVector Vertices;
  typedef LHCb::RecVertex::ConstVector PVs;
  typedef Particle2Vertex::LightTable RelTable;
  typedef Particle2Vertex::Table Table;
  typedef Particle2Vertex::Table::InvType InvTable;
  
  const Table* inputTable = i_get<Table>(m_P2PVInputLocation);

  Table* table = new Table();

  if (0==inputTable) return table;

  const InvTable invTable(*inputTable, 1);

  InvTable::Range range = invTable.relations();

  Particles particles;

  Relations::getUniqueTo(range.begin(), range.end(), particles);

  for (Particles::const_iterator iPart = particles.begin();
       iPart != particles.end(); ++iPart) {

    const Table::Range range = inputTable->relations(*iPart);

    Vertices vertices;
    PVs pvs;
    Relations::getUniqueTo(range.begin(), range.end(), vertices);
    /// @todo uncomment with LHCb v27r1
    //    const RelTable::Range rel = 
    //      m_pvRelator->relatedPVs(*iPart, vertices).relations();
    /// @todo remove with LHCb v27r1
    getPVsFromVertexBases(vertices, pvs);
    const RelTable::Range rel = 
      m_pvRelator->relatedPVs(*iPart, pvs).relations();

    table->merge(rel);       
    
  }

  return table;


}
//=============================================================================
/// @todo remove with LHCb v27r1
void 
PVRelatorAlg::getPVsFromVertexBases(const LHCb::VertexBase::ConstVector& v,
                                    LHCb::RecVertex::ConstVector& pvs) const
{
  LHCb::VertexBase::ConstVector::const_iterator iVtx = v.begin();
  for ( ; iVtx != v.end() ; ++ iVtx ) {
    const LHCb::RecVertex* pv = dynamic_cast<const LHCb::RecVertex*>(*iVtx);
    if (pv) {
      std::cout << "Inserting RecVertex cast from VertexBase" << std::endl;
      pvs.push_back(pv);
    } else {
      Error("VertexBase -> RecVertex dynamic_cast failed!", 10).ignore();
    }
  }
  
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode PVRelatorAlg::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}
//=============================================================================
