
// local
#include "AddRelatedInfo.h"

//-----------------------------------------------------------------------
// Implementation file for class : AddRelatedInfo
//
// Author: Anton Poluektov
//-----------------------------------------------------------------------

using namespace LHCb ;
using namespace Gaudi::Units;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( AddRelatedInfo )

//=======================================================================
AddRelatedInfo::AddRelatedInfo( const std::string& name,
                                ISvcLocator* pSvcLocator )
: DaVinciAlgorithm ( name, pSvcLocator )
{
  declareProperty("Tool", m_toolName, "Name of RelatedInfoTool" );
  declareProperty("MaxLevel", m_maxLevel = 0, "Maximum recursion level");
  declareProperty("InfoLocations", m_infoLocations, "Locations of RelatedInfo objects"); 
  //setProperty( "OutputLevel", 1 );
}

//=======================================================================
StatusCode AddRelatedInfo::initialize()
{
  const StatusCode sc = DaVinciAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  m_tool = tool<IRelatedInfoTool>(m_toolName,this);

  // Make sure infoLocations start with /Event/
  InfoMap tmpMap;
  for ( const auto & loc : m_infoLocations )
  {
    std::string tmpLoc = loc.first;
    boost::erase_all( tmpLoc, "/Event/" );
    tmpLoc = "/Event/" + tmpLoc;
    tmpMap[tmpLoc] = loc.second;
  }
  m_infoLocations = tmpMap;

  info() << "Info Locations : " << m_infoLocations << endmsg;
  
  return sc;
}

AddRelatedInfo::~AddRelatedInfo() {}

//=======================================================================
// Main execution
//=======================================================================
StatusCode AddRelatedInfo::execute()
{
  setFilterPassed( true ); // Filter always passes

  m_relMap.clear(); 

  // Loop over input locations
  for ( const auto& loc : inputLocations() )
  {
    const std::string location = loc + "/Particles";
  
    const Particle::Range parts = getIfExists<Particle::Range>( location );
    if ( parts.empty() ) 
    {
      if (msgLevel(MSG::VERBOSE)) verbose() << "No particles found at " << location << endreq;
      continue;
    }

    if (msgLevel(MSG::VERBOSE)) verbose() << " Found "<< parts.size() << " particles" <<endreq;

    // Loop over particles in the locations
    for ( const Particle * p : parts )
    {
      Particle * c = const_cast<Particle*>(p);
      fill( c, c, 0, loc );
    }

  }

  return StatusCode::SUCCESS;
}

//==========================================================================

void AddRelatedInfo::fill( const Particle* top,
                           Particle* c, 
                           const int level, 
                           const std::string &top_location ) 
{
  const std::string c_location = ( c && c->parent() && c->parent()->registry() ?
                                   c->parent()->registry()->identifier() : "NotInTES" ); 
 
  bool isInLocations = false; 

  // check if the particle is in the list of info locations
  for ( const auto& infoLoc : m_infoLocations )
  {
    if ( c_location.compare(infoLoc.first) == 0 )
    {
      isInLocations = true; 
      break;
    }
  }

  if ( isInLocations )
  {

    if (msgLevel(MSG::DEBUG)) debug() << "Filling RelatedInfo for particle at " << c_location << endreq;

//    std::string key_location = c_location; 
//    boost::replace_all(key_location, "/Particles", ""); 

//    if (msgLevel(MSG::DEBUG)) debug() << "Key is " << key_location << ", info location is " << m_infoLocations[key_location] << endreq;

    const std::string map_location = top_location + "/" + m_infoLocations[c_location];
    
    if (msgLevel(MSG::DEBUG)) debug() << "GetOrCreate RelatedInfo at " << map_location << endreq;

    ParticleInfoRelation* relation = getOrCreate<ParticleInfoRelation, ParticleInfoRelation>(map_location); 

    const StatusCode sc = m_tool->calculateRelatedInfo( top, c );
    if ( sc.isFailure() ) 
    {
      Warning( "Problem calculating related info" ).ignore();
      return;
    }

    RelatedInfoMap* map = m_tool->getInfo(); 

    if (msgLevel(MSG::DEBUG))
    {
      debug() << "Got RelatedInfoMap : " << *map << endreq;
    }
      
    relation->i_relate(c, *map); 

  } else {
    if (msgLevel(MSG::VERBOSE)) verbose() << "Particle at " << c_location << " not in the list, skipping" << endreq; 
  }

  // If we reached the maximum recursion level, we're done
  if ( level >= m_maxLevel ) return;
  
  // -- if the candidate is not a stable particle, call the function recursively
  if( ! c->isBasicParticle() )
  {
    // loop over daughters
    for ( const auto& const_dau : c->daughters() )
    {
      if ( msgLevel(MSG::DEBUG) ) 
        debug() << " Filling RelatedInfo for daughters of ID " << const_dau->particleID().pid() << endmsg;
      fill( top, const_cast<Particle*>(&*const_dau), level+1, top_location );
    }
  }

}
