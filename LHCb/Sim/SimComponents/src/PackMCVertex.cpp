// $Id: PackMCVertex.cpp,v 1.1 2006-02-20 08:21:40 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/MCVertex.h"
#include "Kernel/StandardPacker.h"
#include "Event/PackedMCVertex.h"
// local
#include "PackMCVertex.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PackMCVertex
//
// 2005-03-18 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PackMCVertex );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PackMCVertex::PackMCVertex( const std::string& name,
                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::MCVertexLocation::Default );
  declareProperty( "OutputName", m_outputName = LHCb::PackedMCVertexLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
PackMCVertex::~PackMCVertex() {}; 

//=============================================================================
// Main execution
//=============================================================================
StatusCode PackMCVertex::execute() {

  debug() << "==> Execute" << endmsg;

  LHCb::MCVertices* verts = get<LHCb::MCVertices>( m_inputName );
  debug() << m_inputName << " contains " << verts->size()
          << " MCVertices to convert." << endreq;
  
  StandardPacker pack;
  
  LHCb::PackedMCVertices* out = new LHCb::PackedMCVertices();
  put( out, m_outputName );

  for ( LHCb::MCVertices::const_iterator itV = verts->begin();
        verts->end() != itV; ++itV ) {
    LHCb::MCVertex* vert = (*itV);
    LHCb::PackedMCVertex newVert;

    newVert.key  = vert->key();
    newVert.x    = pack.position( vert->position().x() );
    newVert.y    = pack.position( vert->position().y() );
    newVert.z    = pack.position( vert->position().z() );
    newVert.tof  = vert->time();   // What scale ?
    newVert.type = vert->type();

    newVert.mother = -1;
    if ( 0 != vert->mother() ) {
      newVert.mother = pack.reference( out, vert->mother()->parent(), 
                                       vert->mother()->key() );
    }
    for ( SmartRefVector<LHCb::MCParticle>::const_iterator itP = vert->products().begin();
          vert->products().end() != itP; ++itP ) {
      int ref = pack.reference( out, (*itP)->parent(), (*itP)->key() );
      newVert.products.push_back( ref );
    }
    out->addEntry( newVert );
  }

  // Clear the registry address of the unpacked container, to prevent reloading
  verts->registry()->setAddress( 0 );
  
  return StatusCode::SUCCESS;
};

//=============================================================================
