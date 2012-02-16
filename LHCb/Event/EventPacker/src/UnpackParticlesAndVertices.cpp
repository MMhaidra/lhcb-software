// $Id: $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Event/RecVertex.h"
// local
#include "UnpackParticlesAndVertices.h"

//-----------------------------------------------------------------------------
// Implementation file for class : UnpackParticlesAndVertices
//
// 2012-01-23 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( UnpackParticlesAndVertices )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  UnpackParticlesAndVertices::UnpackParticlesAndVertices( const std::string& name,
                                                          ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputStream", m_inputStream = "/Event/" );
  declareProperty( "PostFix",     m_postFix     = "" );
}

//=============================================================================
// Destructor
//=============================================================================
UnpackParticlesAndVertices::~UnpackParticlesAndVertices() {}

//=========================================================================
//  Initializer: Set the class ids for the various types to be packed
//=========================================================================
StatusCode UnpackParticlesAndVertices::initialize( ) 
{
  const StatusCode sc = GaudiAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;  
  
  info() << "Unpacking in " << m_inputStream << endmsg;

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode UnpackParticlesAndVertices::execute()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute"  << endmsg;
  
  int nbContainer = 0;
  int nbPart = 0;
  int prevLink = -1;

  LHCb::Particles* parts = NULL;
  static const LHCb::ParticlePacker pPacker;
  if ( exist<LHCb::PackedParticles>(m_inputStream+LHCb::PackedParticleLocation::InStream) )
  {
    LHCb::PackedParticles* pparts =
      get<LHCb::PackedParticles>( m_inputStream + LHCb::PackedParticleLocation::InStream );
    for ( std::vector<LHCb::PackedParticle>::iterator itP = pparts->data().begin();
          pparts->data().end() != itP; ++itP )
    {
      const LHCb::PackedParticle& ppart = *itP;
      const int key    = ppart.key & 0x0000FFFF;
      const int linkID = ppart.key >> 16;
      if ( linkID != prevLink ) 
      {
        prevLink = linkID;
        const std::string & containerName = pparts->linkMgr()->link( linkID )->path() + m_postFix;
        // Check to see if container alrady exists. If it does, unpacking has already been run this
        // event so quit
        if ( exist<LHCb::Particles>(containerName) )
        {
          if ( msgLevel(MSG::DEBUG) ) 
            debug() << " -> " << containerName << " exists" << endmsg;
          return StatusCode::SUCCESS;
        }
        parts = new LHCb::Particles();
        put( parts, containerName );
        ++nbContainer;
      }

      // Make new object and insert into the output container
      LHCb::Particle* part = new LHCb::Particle();
      parts->insert( part, key );
      ++nbPart;

      // Unpack the physics info
      pPacker.unpack( ppart, *part, *pparts, *parts );

    }
  }

  //=================================================================
  //== Process the vertices
  //=================================================================
  int nbVertContainer = 0;
  int nbVert = 0;
  prevLink = -1;
  LHCb::Vertices* verts = NULL;
  static const LHCb::VertexPacker vPacker;
  if ( exist<LHCb::PackedVertices>( m_inputStream + LHCb::PackedVertexLocation::InStream ) )
  {
    LHCb::PackedVertices* pverts = 
      get<LHCb::PackedVertices>( m_inputStream + LHCb::PackedVertexLocation::InStream );
    for ( std::vector<LHCb::PackedVertex>::iterator itV = pverts->data().begin();
          pverts->data().end() != itV; ++itV )
    {
      const LHCb::PackedVertex& pvert = *itV;
      const int key    = pvert.key & 0x0000FFFF;
      const int linkID = pvert.key >> 16;
      if ( linkID != prevLink ) 
      {
        prevLink = linkID;
        const std::string & containerName = pverts->linkMgr()->link( linkID )->path() + m_postFix;
        verts = new LHCb::Vertices();
        put( verts, containerName );
        ++nbVertContainer;
      }
      //== Construct with verticle ID and key.
      LHCb::Vertex* vert = new LHCb::Vertex();
      verts->insert( vert, key );
      ++nbVert;

      // unpack the physics info
      vPacker.unpack( pvert, *vert, *pverts, *verts );
    }
  }

  //=================================================================
  //== Process the RecVertices
  //=================================================================
  int nbRecVertContainer = 0;
  int nbRecVert = 0;
  prevLink = -1;
  LHCb::RecVertices* recVerts = NULL;
  static const LHCb::RecVertexPacker rvPacker;
  if ( exist<LHCb::PackedRecVertices>( m_inputStream + LHCb::PackedRecVertexLocation::InStream ) )
  {
    LHCb::PackedRecVertices* pRecVerts =
      get<LHCb::PackedRecVertices>( m_inputStream + LHCb::PackedRecVertexLocation::InStream );
    for ( std::vector<LHCb::PackedRecVertex>::iterator itV = pRecVerts->vertices().begin();
          pRecVerts->vertices().end() != itV; ++itV ) 
    {
      const LHCb::PackedRecVertex& pRecVert = *itV;
      int key    = pRecVert.key & 0x0000FFFF;
      int linkID = pRecVert.key >> 16;
      if ( linkID != prevLink ) 
      {
        prevLink = linkID;
        const std::string & containerName = pRecVerts->linkMgr()->link( linkID )->path() + m_postFix;
        recVerts = new LHCb::RecVertices();
        put( recVerts, containerName );
        ++nbRecVertContainer;
      }

      //== Construct with RecVertex ID and key.
      LHCb::RecVertex* recVert = new LHCb::RecVertex( key );
      recVerts->add( recVert );
      ++nbRecVert;

      // Physics Info
      rvPacker.unpack( pRecVert, *recVert, *pRecVerts, *recVerts );
    }
  }

  //=================================================================
  //== Process the relations
  //=================================================================
  int nbRelContainer = 0;
  int nbRel = 0;
  RELATION* rels = NULL;
  if ( exist<LHCb::PackedRelations>( m_inputStream + LHCb::PackedRelationsLocation::InStream ) )
  {
    LHCb::PackedRelations* prels =
      get<LHCb::PackedRelations>( m_inputStream + LHCb::PackedRelationsLocation::InStream );
    for ( std::vector<LHCb::PackedRelation>::iterator itR = prels->relations().begin();
          prels->relations().end() != itR; ++itR )
    {
      const LHCb::PackedRelation& prel = *itR;
      int indx = prel.container >> 16;
      const std::string & containerName = prels->linkMgr()->link( indx )->path() + m_postFix;
      rels = new RELATION();
      put( rels, containerName );
      nbRelContainer++;
      LHCb::Particles* srcContainer = NULL;
      int prevSrcLink = -1;
      DataObject* dstContainer = NULL;
      int prevDstLink = -1;
      for ( int kk = prel.start;  prel.end > kk; ++kk ) 
      {
        int srcLink = prels->sources()[kk] >> 16;
        int srcKey  = prels->sources()[kk] & 0xFFFF;
        if ( srcLink != prevSrcLink ) {
          prevSrcLink = srcLink;
          const std::string & srcName = prels->linkMgr()->link( srcLink )->path();
          srcContainer = get<LHCb::Particles>( srcName );
        }
        LHCb::Particle* from = srcContainer->object( srcKey );
        int dstLink = prels->dests()[kk] >> 16;
        int dstKey  = prels->dests()[kk] & 0xFFFF;
        if ( dstLink != prevDstLink ) {
          prevDstLink = dstLink;
          const std::string & dstName = prels->linkMgr()->link( dstLink )->path();
          dstContainer = get<DataObject>( dstName );
        }
        const LHCb::RecVertices * _to = dynamic_cast<LHCb::RecVertices*>(dstContainer);
        LHCb::VertexBase* to = ( _to ? _to->object(dstKey) : NULL );
        if ( !to ) info() << "Unknown objec: Container type " << (dstContainer->clID()>>16)
                          << "+" << (dstContainer->clID()&0xFFFF)
                          << " key " << dstKey << endmsg;
        rels->relate( from, to );
        ++nbRel;
      }
    }
  }

  //=================================================================
  //== Process the Particle2LHCbID
  //=================================================================
  int nbPartIdContainer = 0;
  int nbPartId = 0;
  prevLink = -1;
  DaVinci::Map::Particle2LHCbIDs* partIds = NULL;
  LHCb::Particles* partContainer = NULL;
  int prevPartLink = -1;
  if ( exist<LHCb::PackedParticle2Ints>( m_inputStream + LHCb::PackedParticle2IntsLocation::InStream ) )
  {
    LHCb::PackedParticle2Ints* pPartIds =
      get<LHCb::PackedParticle2Ints>( m_inputStream +
                                      LHCb::PackedParticle2IntsLocation::InStream );
    for ( std::vector<LHCb::PackedParticle2Int>::iterator itL = pPartIds->relations().begin();
          pPartIds->relations().end() != itL; ++itL )
    {
      const LHCb::PackedParticle2Int& pPartId = *itL;
      const int linkID = pPartId.key >> 16;
      if ( linkID != prevLink ) 
      {
        prevLink = linkID;
        const std::string & containerName = pPartIds->linkMgr()->link( linkID )->path() + m_postFix;
        partIds = new DaVinci::Map::Particle2LHCbIDs();
        put( partIds, containerName );
        ++nbPartIdContainer;
      }
      ++nbPartId;
      const int partLink = pPartId.container >> 16;
      const int partKey  = pPartId.container & 0xFFFF;
      if ( partLink != prevPartLink ) 
      {
        prevPartLink = partLink;
        const std::string & partName = pPartIds->linkMgr()->link( partLink )->path();
        partContainer = get<LHCb::Particles>( partName );
      }
      LHCb::Particle* part = partContainer->object( partKey );
      std::vector<LHCb::LHCbID> temp;
      for ( int kk = pPartId.start; pPartId.end != kk; ++kk ) 
      {
        temp.push_back( LHCb::LHCbID( pPartIds->ints()[kk] ) );
      }
      partIds->insert( part, temp );
    }
  }

  if ( msgLevel(MSG::DEBUG) )
    debug() << "== Retrieved " << nbPart << " Particles in " << nbContainer << " containers, "
            << nbVert << " vertices in " << nbVertContainer << " containers, "
            << nbRecVert << " RecVerticess in " << nbRecVertContainer << " containers,"
            << nbRel << " relations in " << nbRelContainer << " containers, "
            << nbPartId << " Part-LHCbID in " << nbPartIdContainer << " containers, "
            <<endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
