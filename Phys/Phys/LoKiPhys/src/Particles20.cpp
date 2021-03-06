// $Id$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
#include "GaudiKernel/SmartIF.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/ILifetimeFitter.h"
#include "Kernel/IDistanceCalculator.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Constants.h"
#include "LoKi/Vertices0.h"
#include "LoKi/Particles4.h"
#include "LoKi/Particles16.h"
#include "LoKi/Particles20.h"
#include "LoKi/PhysSources.h"
// ============================================================================
#include  "LoKi/GetTools.h"
// ============================================================================
/** @file 
 *  The implementation file for functions form the file LoKi/Particles20.h
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2008-01-16
 */
// ============================================================================
/// anonymous namespace to hide some pure technical stuff
// ============================================================================
namespace
{
  // ==========================================================================
  /// "invalid" pointer 
  const IDistanceCalculator* const s_IGEO    = 0 ;
  /// "invalid" pointer 
  const ILifetimeFitter*     const s_LTIME   = 0 ;
  // ==========================================================================
  /** @var s_IPTOOL 
   *  the instance of "invalid" IP-tool
   */
  const LoKi::Vertices::ImpactParamTool s_IPTOOL ( s_IGEO ) ;
  // ==========================================================================
  /// "invalid" vertex 
  const LHCb::VertexBase* const s_VERTEX = 0 ;
  // ==========================================================================
  /// the finder for the primary vertices:
  const LoKi::Vertices::IsPrimary s_PRIMARY = LoKi::Vertices::IsPrimary () ;
  // ==========================================================================
  /// the default name of Lifetime fitter:
  const std::string s_LIFETIME = "LoKi::LifetimeFitter/lifetime:PUBLIC" ;
  // ==========================================================================
} // end of anonymous namespace 
// ============================================================================
// the default constructor: create the object in invalid state
// ============================================================================
LoKi::Particles::CosineDirectionAngleWithTheBestPV::
CosineDirectionAngleWithTheBestPV()
  : LoKi::AuxDesktopBase                  (          )
  , LoKi::Particles::CosineDirectionAngle ( s_VERTEX ) 
{}
// ============================================================================
// MANDATORY: the clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::CosineDirectionAngleWithTheBestPV* 
LoKi::Particles::CosineDirectionAngleWithTheBestPV::clone() const 
{ return new CosineDirectionAngleWithTheBestPV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::CosineDirectionAngleWithTheBestPV::result_type 
LoKi::Particles::CosineDirectionAngleWithTheBestPV::operator() 
  ( LoKi::Particles::CosineDirectionAngleWithTheBestPV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return -1000" ) ;
    return -1000 ;                                                     // RETURN 
  }
  //
  // get the best vertex from desktop and use it 
  setVertex ( bestVertex ( p ) ) ;
  //
  return dira ( p ) ;
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::CosineDirectionAngleWithTheBestPV::fillStream 
( std::ostream& s ) const { return s << "BPVDIRA" ; }
// ============================================================================
// the default constructor, creates the object in invalid state 
// ============================================================================
LoKi::Particles::ImpParWithTheBestPV::ImpParWithTheBestPV 
( const std::string& geo ) 
  : LoKi::AuxFunBase        ( std::tie ( geo )    ) 
  , LoKi::AuxDesktopBase    (                     )
  , LoKi::Particles::ImpPar ( s_VERTEX , s_IPTOOL ) 
  , m_geo ( geo )   
{
  if ( 0 == tool() && gaudi() ) 
  { setTool ( LoKi::GetTools::distanceCalculator ( *this , geo ) ) ; }
}
// ============================================================================
// MANDATORY: the clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::ImpParWithTheBestPV*
LoKi::Particles::ImpParWithTheBestPV::clone() const 
{ return  new ImpParWithTheBestPV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ImpParWithTheBestPV::result_type 
LoKi::Particles::ImpParWithTheBestPV::operator() 
  ( LoKi::Particles::ImpParWithTheBestPV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return -1000" ) ;
    return -1000 ;                                                     // RETURN 
  }
  // get the IDistanceCalculator from IDVAlgorithm 
  if ( 0 == tool() ) 
  { setTool ( LoKi::GetTools::distanceCalculator ( *this , geo() ) ) ; }
  // check it!
  Assert ( 0 != tool() , "No valid IDistanceCalculator is found" ) ;
  // get the best vertex from desktop and use it 
  setVertex ( bestVertex ( p ) ) ;
  //
  return ip ( p ) ;
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::ImpParWithTheBestPV::fillStream 
( std::ostream& s ) const { return s << "BPVIP ('" <<  m_geo << "')"; }

// ============================================================================
// the default constructor, creates the object in invalid state 
// ============================================================================
LoKi::Particles::ImpParChi2WithTheBestPV::ImpParChi2WithTheBestPV 
( const std::string& geo ) 
  : LoKi::AuxFunBase    ( std::tie ( geo ) ) 
  , LoKi::Particles::ImpParWithTheBestPV ( geo ) 
{}
// ============================================================================
// MANDATORY: the clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::ImpParChi2WithTheBestPV*
LoKi::Particles::ImpParChi2WithTheBestPV::clone() const 
{ return  new ImpParChi2WithTheBestPV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::ImpParChi2WithTheBestPV::result_type 
LoKi::Particles::ImpParChi2WithTheBestPV::operator() 
  ( LoKi::Particles::ImpParChi2WithTheBestPV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return -1000" ) ;
    return -1000 ;                                                     // RETURN 
  }
  // get the IDistanceCalculator from IDVAlgorithm 
  if ( 0 == tool() )
  { setTool ( LoKi::GetTools::distanceCalculator ( *this , geo() ) ) ; }
  // check it!
  Assert ( 0 != tool() , "No valid IDistanceCalculator is found" ) ;
  // get the best vertex from desktop and use it 
  setVertex ( bestVertex ( p ) ) ;
  //
  return ipchi2 ( p ) ;
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::ImpParChi2WithTheBestPV::fillStream 
( std::ostream& s ) const { return s << "BPVIPCHI2 ('" <<  geo() << "')"; }
// ============================================================================
/*  constructor from the source and nickname or full type/name of 
 *  IDistanceCalculator tool
 *  @see IDVAlgorithm::distanceCalculator 
 *  @param source the source 
 *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParWithSource::MinImpParWithSource 
( const LoKi::BasicFunctors<const LHCb::VertexBase*>::Source& source ,
  const std::string&                                          geo    ) 
  : LoKi::AuxFunBase    ( std::tie ( source , geo ) ) 
  , LoKi::Particles::MinImpPar ( LHCb::VertexBase::ConstVector() , s_IPTOOL ) 
  , m_source ( source )   
  , m_geo    ( geo    )
{}
// ============================================================================
// MANDATORY: the clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinImpParWithSource*
LoKi::Particles::MinImpParWithSource::clone() const 
{ return  new MinImpParWithSource ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MinImpParWithSource::result_type 
LoKi::Particles::MinImpParWithSource::operator() 
  ( LoKi::Particles::MinImpParWithSource::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return -1000" ) ;
    return -1000 ;                                                     // RETURN 
  }
  // get the IDistanceCalculator from IDVAlgorithm 
  if ( 0 == tool() )
  { setTool ( LoKi::GetTools::distanceCalculator ( *this , geo() ) ) ; }
  // check it!
  Assert ( 0 != tool() , "No valid IDistanceCalculator is found" ) ;      
  // check the event 
  if ( !sameEvent() ) 
  {
    typedef LoKi::UniqueKeeper<LHCb::VertexBase> KEEPER ;
    const KEEPER& keep1 = *this ;
    KEEPER&       keep2 = const_cast<KEEPER&> ( keep1 ) ;
    
    
    // clear the list of vertices 
    keep2.clear() ;
    // get the primary vertices from the source 
    LHCb::VertexBase::ConstVector primaries = source()() ;  // NB! 
    // fill the functor with primary vertices:
    keep2.addObjects ( primaries.begin() , primaries.end () ) ;
    if ( empty() ) 
    { Warning ( "Empty list of vertices is loaded!" ) ; }
    // update the event:
    setEvent () ;
  }
  // use the functor 
  return mip ( p ) ;
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::MinImpParWithSource::fillStream 
( std::ostream& s ) const 
{ return s << "MIPSOURCE ('" <<  source() << ", " << geo() << "')"; }
// ============================================================================
/* the "default" constructor,
 *  gets the IDistanceCalculator tool from IDVAlgorithm by nickname or 
 *  by full type/name
 *  @see IDVAlgorithm::distanceCalculator 
 *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParDV::MinImpParDV 
( const std::string& geo    ) 
  : LoKi::AuxFunBase    ( std::tie ( geo ) ) 
  , LoKi::Particles::MinImpParWithSource ( LoKi::Vertices::SourceDesktop ( s_PRIMARY ) , geo )  
{}
// ============================================================================
/*  the constructor form the vertex selection functor and 
 *  the name/nickname of IDistanceCalculator tool from IDVAlgorithm
 *  @see IDVAlgorithm::distanceCalculator 
 *  @param 
 *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParDV::MinImpParDV 
( const LoKi::PhysTypes::VCuts& cuts , 
  const std::string&            geo )
  : LoKi::AuxFunBase    ( std::tie ( cuts , geo ) ) 
  , LoKi::Particles::MinImpParWithSource ( LoKi::Vertices::SourceDesktop ( cuts ) , geo )
{}
// ============================================================================
/*  the constructor form the vertex selection functor and 
 *  the name/nickname of IDistanceCalculator tool from IDVAlgorithm
 *  @see IDVAlgorithm::distanceCalculator 
 *  @param 
 *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParDV::MinImpParDV 
( const std::string&            geo , 
  const LoKi::PhysTypes::VCuts& cuts ) 
  : LoKi::AuxFunBase    ( std::tie ( geo , cuts ) ) 
  , LoKi::Particles::MinImpParWithSource ( LoKi::Vertices::SourceDesktop ( cuts ) , geo )
{}
// ============================================================================
// MANDATORY: the clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinImpParDV*
LoKi::Particles::MinImpParDV::clone() const 
{ return  new MinImpParDV ( *this ) ; }
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::MinImpParDV::fillStream 
( std::ostream& s ) const 
{ return s << "MIPDV('" << geo() << "')"; }
// ============================================================================
/* the constructor,
 *  gets the IDistanceCalculator tool from IDVAlgorithm by nickname or 
 *  by full type/name
 *  @see IDVAlgorithm::distanceCalculator 
 *  @param path the location of vertices in TES 
 *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParTES::MinImpParTES 
( const std::string& path   , 
  const std::string& geo    ) 
  : LoKi::AuxFunBase    ( std::tie ( path , geo ) ) 
  , LoKi::Particles::MinImpParWithSource 
( LoKi::Vertices::SourceTES ( path , s_PRIMARY ) , geo )  
  , m_path ( 1 , path ) 
{}
// ============================================================================
/* the constructor,
 *  gets the IDistanceCalculator tool from IDVAlgorithm by nickname or 
 *  by full type/name
 *  @see IDVAlgorithm::distanceCalculator 
 *  @param path the locations of vertices in TES 
 *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParTES::MinImpParTES 
( const std::vector<std::string>& path   , 
  const std::string&              geo    ) 
  : LoKi::AuxFunBase    ( std::tie ( path , geo ) ) 
  , LoKi::Particles::MinImpParWithSource ( LoKi::Vertices::SourceTES ( path , s_PRIMARY ) , geo )  
  , m_path ( path ) 
{}
// ============================================================================
/* the constructor,
 *  gets the IDistanceCalculator tool from IDVAlgorithm by nickname or 
 *  by full type/name
 *  @see IDVAlgorithm::distanceCalculator 
 *  @param path the locations of vertices in TES 
 *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParTES::MinImpParTES 
( const LoKi::PhysTypes::VCuts&   cuts ,
  const std::vector<std::string>& path , 
  const std::string&              geo  ) 
  : LoKi::AuxFunBase    ( std::tie ( cuts , path , geo ) ) 
  , LoKi::Particles::MinImpParWithSource ( LoKi::Vertices::SourceTES ( path , cuts  ) , geo )  
  , m_path ( path ) 
{}
// ============================================================================
/* the constructor,
 *  gets the IDistanceCalculator tool from IDVAlgorithm by nickname or 
 *  by full type/name
 *  @see IDVAlgorithm::distanceCalculator 
 *  @param path the location of vertices in TES 
 *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParTES::MinImpParTES 
( const LoKi::PhysTypes::VCuts& cuts ,
  const std::string&            path , 
  const std::string&            geo  ) 
  : LoKi::AuxFunBase    ( std::tie ( cuts , path , geo ) ) 
  , LoKi::Particles::MinImpParWithSource ( LoKi::Vertices::SourceTES ( path , cuts  ) , geo )  
  , m_path ( 1 , path ) 
{}
// ============================================================================
/*  the constructor,
 *  gets the IDistanceCalculator tool from IDVAlgorithm by nickname or 
 *  by full type/name
 *  @see IDVAlgorithm::distanceCalculator 
 *  @param path the locations of vertices in TES 
 *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParTES::MinImpParTES 
( const std::vector<std::string>& path , 
  const LoKi::PhysTypes::VCuts&   cuts ,
  const std::string&              geo  ) 
  : LoKi::AuxFunBase    ( std::tie ( path , cuts , geo ) ) 
  , LoKi::Particles::MinImpParWithSource ( LoKi::Vertices::SourceTES ( path , cuts  ) , geo )  
  , m_path ( path ) 
{}
// ============================================================================
/*  the constructor,
 *  gets the IDistanceCalculator tool from IDVAlgorithm by nickname or 
 *  by full type/name
 *  @see IDVAlgorithm::distanceCalculator 
 *  @param path the location of vertices in TES 
 *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParTES::MinImpParTES 
( const std::string&            path , 
  const LoKi::PhysTypes::VCuts& cuts ,
  const std::string&            geo  ) 
  : LoKi::AuxFunBase    ( std::tie ( path , cuts , geo ) ) 
  , LoKi::Particles::MinImpParWithSource ( LoKi::Vertices::SourceTES ( path , cuts  ) , geo )  
  , m_path ( 1 , path ) 
{}
// ============================================================================
/*  the constructor,
 *  gets the IDistanceCalculator tool from IDVAlgorithm by nickname or 
 *  by full type/name
 *  @see IDVAlgorithm::distanceCalculator 
 *  @param path the locations of vertices in TES 
 *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParTES::MinImpParTES 
( const std::vector<std::string>& path , 
  const std::string&              geo  , 
  const LoKi::PhysTypes::VCuts&   cuts ) 
  : LoKi::AuxFunBase    ( std::tie ( path , geo , cuts ) ) 
  , LoKi::Particles::MinImpParWithSource ( LoKi::Vertices::SourceTES ( path , cuts  ) , geo )  
  , m_path ( path ) 
{}
// ============================================================================
/** the constructor,
 *  gets the IDistanceCalculator tool from IDVAlgorithm by nickname or 
 *  by full type/name
 *  @see IDVAlgorithm::distanceCalculator 
 *  @param path the location of vertices in TES 
 *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParTES::MinImpParTES 
( const std::string&            path , 
  const std::string&            geo  ,
  const LoKi::PhysTypes::VCuts& cuts ) 
  : LoKi::AuxFunBase    ( std::tie ( path , geo , cuts ) ) 
  , LoKi::Particles::MinImpParWithSource ( LoKi::Vertices::SourceTES ( path , cuts  ) , geo )  
  , m_path ( 1 , path ) 
{}
// ============================================================================
// MANDATORY: the clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinImpParTES*
LoKi::Particles::MinImpParTES::clone() const 
{ return  new MinImpParTES ( *this ) ; }
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::MinImpParTES::fillStream 
( std::ostream& s ) const 
{ return s << "MIPTES(" << Gaudi::Utils::toString ( m_path ) 
           << "," << geo() << "')"; }
// ============================================================================
/*  constructor from the source and nickname or full type/name of 
 *  IDistanceCalculator tool
 *  @see IDVAlgorithm::distanceCalculator 
 *  @param source the source 
 *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParChi2WithSource::MinImpParChi2WithSource 
( const LoKi::BasicFunctors<const LHCb::VertexBase*>::Source& source ,
  const std::string&                                          geo    ) 
  : LoKi::AuxFunBase    ( std::tie ( source , geo ) ) 
  , LoKi::Particles::MinImpParWithSource ( source , geo ) 
{}
// ============================================================================
// MANDATORY: the clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinImpParChi2WithSource*
LoKi::Particles::MinImpParChi2WithSource::clone() const 
{ return  new MinImpParChi2WithSource ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MinImpParChi2WithSource::result_type 
LoKi::Particles::MinImpParChi2WithSource::operator() 
  ( LoKi::Particles::MinImpParChi2WithSource::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return -1000" ) ;
    return -1000 ;                                                     // RETURN 
  }
  // get the IDistanceCalculator from IDVAlgorithm 
  if ( 0 == tool() ) 
  { setTool ( LoKi::GetTools::distanceCalculator ( *this , geo() ) ) ; }
  // check it!
  Assert ( 0 != tool() , "No valid IDistanceCalculator is found" ) ;
  // check the event 
  if ( !sameEvent() ) 
  {
    typedef LoKi::UniqueKeeper<LHCb::VertexBase> KEEPER ;
    const KEEPER& keep1 = *this ;
    KEEPER&       keep2 = const_cast<KEEPER&> ( keep1 ) ;
    
    
    // clear the list of vertices 
    keep2.clear() ;
    // get the primary vertices from the source 
    LHCb::VertexBase::ConstVector primaries = source()() ; // NB!!
    // fill the functor with primary vertices:
    keep2.addObjects ( primaries.begin() , primaries.end () ) ;
    if ( empty() ) { Error ( "Empty list of vertices is loaded!" ) ; }
    // update the event:
    setEvent () ;
  }
  // use the functor 
  return mipchi2 ( p ) ;
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::MinImpParChi2WithSource::fillStream 
( std::ostream& s ) const 
{ return s << "MIPCHI2SOURCE ('" <<  source() << ", " << geo() << "')"; }
// ============================================================================
/* the "default" constructor,
 *  gets the IDistanceCalculator tool from IDVAlgorithm by nickname or 
 *  by full type/name
 *  @see IDVAlgorithm::distanceCalculator 
 *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParChi2DV::MinImpParChi2DV 
( const std::string& geo    ) 
  : LoKi::AuxFunBase    ( std::tie ( geo ) ) 
  , LoKi::Particles::MinImpParChi2WithSource ( LoKi::Vertices::SourceDesktop ( s_PRIMARY ) , geo )  
{}
// ============================================================================
/*  the constructor,
 *  gets the IDistanceCalculator tool from IDVAlgorithm by nickname or 
 *  by full type/name
 *  @see IDVAlgorithm::distanceCalculator 
 *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParChi2DV::MinImpParChi2DV 
( const LoKi::PhysTypes::VCuts& cuts , 
  const std::string&            geo  ) 
  : LoKi::AuxFunBase    ( std::tie ( cuts , geo ) ) 
  , LoKi::Particles::MinImpParChi2WithSource ( LoKi::Vertices::SourceDesktop ( cuts  ) , geo )  
{}
// ============================================================================
/*  the  constructor,
 *  gets the IDistanceCalculator tool from IDVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::distanceCalculator 
 *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParChi2DV::MinImpParChi2DV 
( const std::string&            geo  , 
  const LoKi::PhysTypes::VCuts& cuts ) 
  : LoKi::AuxFunBase    ( std::tie ( geo , cuts ) ) 
  , LoKi::Particles::MinImpParChi2WithSource ( LoKi::Vertices::SourceDesktop ( cuts  ) , geo )  
{}
// ============================================================================
// MANDATORY: the clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinImpParChi2DV*
LoKi::Particles::MinImpParChi2DV::clone() const 
{ return  new MinImpParChi2DV ( *this ) ; }
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::MinImpParChi2DV::fillStream 
( std::ostream& s ) const 
{ return s << "MIPCHI2DV('" << geo() << "')"; }
// ============================================================================
/* the constructor,
 *  gets the IDistanceCalculator tool from IDVAlgorithm by nickname or 
 *  by full type/name
 *  @see IDVAlgorithm::distanceCalculator 
 *  @param path the location of vertices in TES 
 *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParChi2TES::MinImpParChi2TES 
( const std::string& path   , 
  const std::string& geo    ) 
  : LoKi::AuxFunBase    ( std::tie ( path , geo ) ) 
  , LoKi::Particles::MinImpParChi2WithSource ( LoKi::Vertices::SourceTES ( path , s_PRIMARY ) , geo )  
  , m_path ( 1 , path ) 
{}
// ============================================================================
/* the constructor,
 *  gets the IDistanceCalculator tool from IDVAlgorithm by nickname or 
 *  by full type/name
 *  @see IDVAlgorithm::distanceCalculator 
 *  @param path the locations of vertices in TES 
 *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParChi2TES::MinImpParChi2TES 
( const std::vector<std::string>& path   , 
  const std::string&              geo    ) 
  : LoKi::AuxFunBase    ( std::tie ( path , geo ) ) 
  , LoKi::Particles::MinImpParChi2WithSource ( LoKi::Vertices::SourceTES ( path , s_PRIMARY ) , geo )  
  , m_path ( path ) 
{}
// ============================================================================
/*  the constructor,
 *  gets the IDistanceCalculator tool from IDVAlgorithm by nickname or 
 *  by full type/name
 *  @see IDVAlgorithm::distanceCalculator 
 *  @param path the location of vertices in TES 
 *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParChi2TES::MinImpParChi2TES 
( const LoKi::PhysTypes::VCuts& cuts , 
  const std::string&            path , 
  const std::string&            geo  ) 
  : LoKi::AuxFunBase    ( std::tie ( cuts , path , geo ) ) 
  , LoKi::Particles::MinImpParChi2WithSource ( LoKi::Vertices::SourceTES ( path , cuts ) , geo )  
  , m_path ( 1 , path ) 
{}
// ============================================================================
/*  the constructor,
 *  gets the IDistanceCalculator tool from IDVAlgorithm by nickname or 
 *  by full type/name
 *  @see IDVAlgorithm::distanceCalculator 
 *  @param path the locations of vertices in TES 
 *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParChi2TES::MinImpParChi2TES 
( const LoKi::PhysTypes::VCuts&   cuts , 
  const std::vector<std::string>& path , 
  const std::string&              geo  ) 
  : LoKi::AuxFunBase    ( std::tie ( cuts , path , geo ) ) 
  , LoKi::Particles::MinImpParChi2WithSource ( LoKi::Vertices::SourceTES ( path , cuts ) , geo )  
  , m_path ( path ) 
{}
// ============================================================================
/*  the constructor,
 *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::distanceCalculator 
 *  @param path the location of vertices in TES 
 *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParChi2TES::MinImpParChi2TES 
( const std::string&            path , 
  const LoKi::PhysTypes::VCuts& cuts , 
  const std::string&            geo  ) 
  : LoKi::AuxFunBase    ( std::tie ( path , cuts , geo ) ) 
  , LoKi::Particles::MinImpParChi2WithSource ( LoKi::Vertices::SourceTES ( path , cuts ) , geo )  
  , m_path ( 1 , path ) 
{}
// ============================================================================
/*  the constructor,
 *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::distanceCalculator 
 *  @param path the locations of vertices in TES 
 *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
 */
LoKi::Particles::MinImpParChi2TES::MinImpParChi2TES 
( const std::vector<std::string>& path , 
  const LoKi::PhysTypes::VCuts&   cuts , 
  const std::string&              geo  )  
  : LoKi::AuxFunBase    ( std::tie ( path , cuts , geo ) ) 
  , LoKi::Particles::MinImpParChi2WithSource ( LoKi::Vertices::SourceTES ( path , cuts ) , geo )  
  , m_path ( path ) 
{}
// ============================================================================
/*  the constructor,
 *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::distanceCalculator 
 *  @param path the location of vertices in TES 
 *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
 */
// ============================================================================
LoKi::Particles::MinImpParChi2TES::MinImpParChi2TES 
( const std::string&            path , 
  const std::string&            geo  ,
  const LoKi::PhysTypes::VCuts& cuts ) 
  : LoKi::AuxFunBase    ( std::tie ( path , geo , cuts ) ) 
  , LoKi::Particles::MinImpParChi2WithSource ( LoKi::Vertices::SourceTES ( path , cuts ) , geo )  
  , m_path ( 1 , path ) 
{}
// ============================================================================
/*  the constructor,
 *  gets the IDistanceCalculator tool from DVAlgorithm by nickname or 
 *  by full type/name
 *  @see DVAlgorithm::distanceCalculator 
 *  @param path the locations of vertices in TES 
 *  @param geo the nickname (or type/name)  of IDistanceCalculator tool
 */
LoKi::Particles::MinImpParChi2TES::MinImpParChi2TES 
( const std::vector<std::string>& path , 
  const std::string&              geo  , 
  const LoKi::PhysTypes::VCuts&   cuts )
  : LoKi::AuxFunBase    ( std::tie ( path , geo , cuts ) ) 
  , LoKi::Particles::MinImpParChi2WithSource ( LoKi::Vertices::SourceTES ( path , cuts ) , geo )  
  , m_path ( path ) 
{}
// ============================================================================
// MANDATORY: the clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinImpParChi2TES*
LoKi::Particles::MinImpParChi2TES::clone() const 
{ return  new MinImpParChi2TES ( *this ) ; }
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::MinImpParChi2TES::fillStream 
( std::ostream& s ) const 
{ return s << "MIPCHI2TES(" << Gaudi::Utils::toString ( m_path ) 
           << "," << geo() << "')" ; 
}
// ============================================================================
// the default constructor 
// ============================================================================
LoKi::Particles::VertexDistanceDV::VertexDistanceDV ()
  : LoKi::AuxDesktopBase ()
  , LoKi::Particles::VertexDistance ( s_VERTEX ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::VertexDistanceDV*
LoKi::Particles::VertexDistanceDV::clone() const 
{ return new LoKi::Particles::VertexDistanceDV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::VertexDistanceDV::result_type
LoKi::Particles::VertexDistanceDV::operator() 
  ( LoKi::Particles::VertexDistanceDV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* poiunts to NULL, return InvalidDistance" ) ;
    return LoKi::Constants::InvalidDistance ;                    // RETURN 
  }
  // get the best vertex from desktop and use it 
  setVertex ( bestVertex ( p ) ) ;
  //
  return distance ( p ) ;                                           // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::VertexDistanceDV::fillStream 
( std::ostream& s ) const { return s << "BPVVD" ; }
// ============================================================================
// the default constructor 
// ============================================================================
LoKi::Particles::VertexSignedDistanceDV::VertexSignedDistanceDV ()
  : LoKi::AuxDesktopBase ()
  , LoKi::Particles::VertexSignedDistance ( s_VERTEX ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::VertexSignedDistanceDV*
LoKi::Particles::VertexSignedDistanceDV::clone() const 
{ return new LoKi::Particles::VertexSignedDistanceDV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::VertexSignedDistanceDV::result_type
LoKi::Particles::VertexSignedDistanceDV::operator() 
  ( LoKi::Particles::VertexSignedDistanceDV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* poiunts to NULL, return InvalidDistance" ) ;
    return LoKi::Constants::InvalidDistance ;                    // RETURN 
  }
  // get the best vertex from desktop and use it 
  setVertex ( bestVertex ( p ) ) ;
  //
  return signedDistance ( p ) ;                                      // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::VertexSignedDistanceDV::fillStream 
( std::ostream& s ) const { return s << "BPVVDSIGN" ; }
// ============================================================================
// the default constructor 
// ============================================================================
LoKi::Particles::VertexDotDistanceDV::VertexDotDistanceDV ()
  : LoKi::AuxDesktopBase ()
  , LoKi::Particles::VertexDotDistance ( s_VERTEX ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::VertexDotDistanceDV*
LoKi::Particles::VertexDotDistanceDV::clone() const 
{ return new LoKi::Particles::VertexDotDistanceDV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::VertexDotDistanceDV::result_type
LoKi::Particles::VertexDotDistanceDV::operator() 
  ( LoKi::Particles::VertexDotDistanceDV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* poiunts to NULL, return InvalidDistance" ) ;
    return LoKi::Constants::InvalidDistance ;                    // RETURN 
  }
  // get the best vertex from desktop and use it 
  setVertex ( bestVertex ( p ) ) ;
  //
  return distance ( p ) ;                                           // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::VertexDotDistanceDV::fillStream 
( std::ostream& s ) const { return s << "BPVVDDOT" ; }

// ============================================================================
// the default constructor 
// ============================================================================
LoKi::Particles::VertexChi2DistanceDV::VertexChi2DistanceDV ()
  : LoKi::AuxDesktopBase ()
  , LoKi::Particles::VertexChi2Distance ( s_VERTEX ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::VertexChi2DistanceDV*
LoKi::Particles::VertexChi2DistanceDV::clone() const 
{ return new LoKi::Particles::VertexChi2DistanceDV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::VertexChi2DistanceDV::result_type
LoKi::Particles::VertexChi2DistanceDV::operator() 
  ( LoKi::Particles::VertexChi2DistanceDV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* poiunts to NULL, return InvalidDistance" ) ;
    return LoKi::Constants::InvalidDistance ;                    // RETURN 
  }
  // get the best vertex from desktop and use it 
  setVertex ( bestVertex ( p ) ) ;
  //
  return chi2 ( p ) ;                                           // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::VertexChi2DistanceDV::fillStream 
( std::ostream& s ) const { return s << "BPVVDCHI2" ; }
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeDV::LifeTimeDV( const double chi2 ) 
  : LoKi::AuxFunBase    ( std::tie ( chi2 ) ) 
  , LoKi::AuxDesktopBase ()
  , LoKi::Particles::LifeTime ( s_LTIME , s_VERTEX , chi2 ) 
  , m_fit ( s_LIFETIME         )
{
  // check the tool 
  if ( 0 == tool() && gaudi() ) 
  { setTool ( LoKi::GetTools::lifetimeFitter ( *this , fitter() ) ) ; }
}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeDV::LifeTimeDV
( const std::string& fit  , 
  const double       chi2 ) 
  : LoKi::AuxFunBase    ( std::tie ( fit , chi2 ) ) 
  , LoKi::AuxDesktopBase ()
  , LoKi::Particles::LifeTime ( s_LTIME , s_VERTEX , chi2 ) 
  , m_fit ( fit  ) 
{
  // check the tool 
  if ( 0 == tool() && gaudi() ) 
  { setTool ( LoKi::GetTools::lifetimeFitter ( *this , fitter() ) ) ; }
}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::LifeTimeDV*
LoKi::Particles::LifeTimeDV::clone() const 
{ return  new LoKi::Particles::LifeTimeDV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::LifeTimeDV::result_type
LoKi::Particles::LifeTimeDV::operator() 
  ( LoKi::Particles::LifeTimeDV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidTime") ;
    return LoKi::Constants::InvalidTime ;
  }    
  // check the tool 
  if ( 0 == tool() ) 
  { setTool ( LoKi::GetTools::lifetimeFitter ( *this , fitter() ) ) ; }
  // check the fitter 
  Assert ( 0 != tool() , "No Valid ILifetimeFitter is availabe" ) ;
  // get the vertex from desktop
  const LHCb::VertexBase* vx = bestVertex ( p ) ;
  if ( 0 == vx ) 
  {
    Error ( "LHCb::VertexBase* points to NULL, return InvalidTime") ;
    return LoKi::Constants::InvalidTime ;
  }
  //
  setVertex ( vx ) ;
  // use the functor 
  return lifeTime ( p ) ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Particles::LifeTimeDV::fillStream ( std::ostream& s ) const 
{ 
  s << " BPVLTIME('" << fitter() << "'" ;
  if ( 0 < chi2cut() ) { s << "," << chi2cut() ; }
  return s << ") ";
}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeChi2DV::LifeTimeChi2DV( const double chi2 ) 
  : LoKi::AuxFunBase    ( std::tie ( chi2 ) ) 
  , LoKi::Particles::LifeTimeDV ( chi2 ) 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeChi2DV::LifeTimeChi2DV
( const std::string& fit  , 
  const double       chi2 )
  : LoKi::AuxFunBase    ( std::tie ( fit , chi2 ) ) 
  , LoKi::Particles::LifeTimeDV ( fit , chi2 ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::LifeTimeChi2DV*
LoKi::Particles::LifeTimeChi2DV::clone() const 
{ return  new LoKi::Particles::LifeTimeChi2DV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::LifeTimeChi2DV::result_type
LoKi::Particles::LifeTimeChi2DV::operator() 
  ( LoKi::Particles::LifeTimeChi2DV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidChi2") ;
    return LoKi::Constants::InvalidChi2 ;
  }    
  // check the tool 
  if ( 0 == tool() ) 
  { setTool ( LoKi::GetTools::lifetimeFitter ( *this , fitter() ) ) ; }
  // check the fitter 
  Assert ( 0 != tool() , "No Valid ILifetimeFitter is availabe" ) ;
  // get the vertex from desktop 
  const LHCb::VertexBase* vx = bestVertex ( p ) ;
  if ( 0 == vx ) 
  {
    Error ( "LHCb::VertexBase* points to NULL, return InvalidChi2") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  setVertex ( vx ) ;
  // use the functor 
  return lifeTimeChi2 ( p ) ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Particles::LifeTimeChi2DV::fillStream ( std::ostream& s ) const
{
  s << " BPVLTCHI2('" << fitter() << "'" ;
  if ( 0 < chi2cut() ) { s << "," << chi2cut() ; }
  return s << ") ";
}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeSignedChi2DV::LifeTimeSignedChi2DV
( const double chi2 ) 
  : LoKi::AuxFunBase    ( std::tie ( chi2 ) ) 
  , LoKi::Particles::LifeTimeChi2DV ( chi2 ) 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeSignedChi2DV::LifeTimeSignedChi2DV
( const std::string& fit  ,
  const double       chi2 ) 
  : LoKi::AuxFunBase    ( std::tie ( fit , chi2 ) ) 
  , LoKi::Particles::LifeTimeChi2DV ( fit , chi2 ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::LifeTimeSignedChi2DV*
LoKi::Particles::LifeTimeSignedChi2DV::clone() const 
{ return  new LoKi::Particles::LifeTimeSignedChi2DV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::LifeTimeSignedChi2DV::result_type
LoKi::Particles::LifeTimeSignedChi2DV::operator() 
  ( LoKi::Particles::LifeTimeSignedChi2DV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidChi2") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  // check the tool 
  if ( 0 == tool() ) 
  { setTool ( LoKi::GetTools::lifetimeFitter ( *this , fitter() ) ) ; }
  // check the fitter 
  Assert ( 0 != tool() , "No Valid ILifetimeFitter is availabe" ) ;
  // get the vertex from desktop 
  const LHCb::VertexBase* vx = bestVertex ( p ) ;
  if ( 0 == vx ) 
  {
    Error ( "LHCb::VertexBase* points to NULL, return InvalidChi2") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  setVertex ( vx ) ;
  // use the functor 
  return lifeTimeSignedChi2 ( p ) ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Particles::LifeTimeSignedChi2DV::fillStream 
( std::ostream& s ) const 
{
  s << " BPVLTSIGNCHI2('" << fitter() << "'" ;
  if ( 0 < chi2cut() ) { s << "," << chi2cut() ; }
  return s << ") ";
}
// ============================================================================

// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeFitChi2DV::LifeTimeFitChi2DV() 
  : LoKi::Particles::LifeTimeDV() 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeFitChi2DV::LifeTimeFitChi2DV
( const std::string& fit ) 
  : LoKi::AuxFunBase    ( std::tie ( fit ) ) 
  , LoKi::Particles::LifeTimeDV( fit ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::LifeTimeFitChi2DV*
LoKi::Particles::LifeTimeFitChi2DV::clone() const 
{ return  new LoKi::Particles::LifeTimeFitChi2DV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::LifeTimeFitChi2DV::result_type
LoKi::Particles::LifeTimeFitChi2DV::operator() 
  ( LoKi::Particles::LifeTimeFitChi2DV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidChi2") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  // check the tool 
  if ( 0 == tool() ) 
  { setTool ( LoKi::GetTools::lifetimeFitter ( *this , fitter() ) ) ; }
  // check the fitter 
  Assert ( 0 != tool() , "No Valid ILifetimeFitter is availabe" ) ;
  // get the vertex from desktop 
  const LHCb::VertexBase* vx = bestVertex ( p ) ;
  if ( 0 == vx ) 
  {
    Error ( "LHCb::VertexBase* points to NULL, return InvalidChi2") ;
    return LoKi::Constants::InvalidChi2 ;
  }
  setVertex ( vx ) ;
  // use the functor 
  return lifeTimeFitChi2 ( p ) ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Particles::LifeTimeFitChi2DV::fillStream 
( std::ostream& s ) const { return s << "BPVLTFITCHI2('" << fitter ()  << "')" ; }
// ============================================================================



// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeErrorDV::LifeTimeErrorDV() 
  : LoKi::Particles::LifeTimeDV() 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Particles::LifeTimeErrorDV::LifeTimeErrorDV
( const std::string& fit ) 
  : LoKi::AuxFunBase    ( std::tie ( fit ) ) 
  , LoKi::Particles::LifeTimeDV( fit ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::LifeTimeErrorDV*
LoKi::Particles::LifeTimeErrorDV::clone() const 
{ return  new LoKi::Particles::LifeTimeErrorDV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::LifeTimeErrorDV::result_type
LoKi::Particles::LifeTimeErrorDV::operator() 
  ( LoKi::Particles::LifeTimeErrorDV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidTime") ;
    return LoKi::Constants::InvalidTime ;
  }    
  // check the tool 
  if ( 0 == tool() ) 
  { setTool ( LoKi::GetTools::lifetimeFitter ( *this , fitter() ) ) ; }
  // check the fitter 
  Assert ( 0 != tool() , "No Valid ILifetimeFitter is availabe" ) ;
  // get the vertex from desktop 
  const LHCb::VertexBase* vx = bestVertex ( p ) ;
  if ( 0 == vx ) 
  {
    Error ( "LHCb::VertexBase* points to NULL, return InvalidTime") ;
    return LoKi::Constants::InvalidTime ;
  }
  setVertex ( vx ) ;
  // use the functor 
  return lifeTimeError ( p ) ;
}
// ============================================================================
// OPTIONAL: the specific printout 
// ============================================================================
std::ostream& LoKi::Particles::LifeTimeErrorDV::fillStream 
( std::ostream& s ) const { return s << "BPVLTERR('" << fitter ()  << "')" ; }
// ============================================================================



// ============================================================================
// the default constructor 
// ============================================================================
LoKi::Particles::VertexZDistanceWithTheBestPV::VertexZDistanceWithTheBestPV ()
  : LoKi::AuxDesktopBase ()
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::VertexZDistanceWithTheBestPV*
LoKi::Particles::VertexZDistanceWithTheBestPV::clone() const 
{ return new LoKi::Particles::VertexZDistanceWithTheBestPV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::VertexZDistanceWithTheBestPV::result_type
LoKi::Particles::VertexZDistanceWithTheBestPV::operator() 
  ( LoKi::Particles::VertexZDistanceWithTheBestPV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidDistance" ) ;
    return LoKi::Constants::InvalidDistance ;                    // RETURN 
  }
  const LHCb::VertexBase* ev = p->endVertex() ;
  if ( 0 == ev ) 
  {
    Error ( "LHCb::Particle::endVertex points to NULL, return InvalidDistance" ) ;
    return LoKi::Constants::InvalidDistance ;                    // RETURN 
  }
  // get the best vertex from desktop and use it 
  const LHCb::VertexBase* bpv = bestVertex ( p ) ; 
  if ( 0 == bpv ) 
  {
    Error ( "Related points to NULL, return InvalidDistance" ) ;
    return LoKi::Constants::InvalidDistance ;                    // RETURN 
  }
  // 
  return ev->position().Z() - bpv->position().Z() ;    // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::VertexZDistanceWithTheBestPV::fillStream 
( std::ostream& s ) const { return s << "BPVVDZ" ; }


// ============================================================================
// the default constructor 
// ============================================================================
LoKi::Particles::VertexRhoDistanceWithTheBestPV::VertexRhoDistanceWithTheBestPV ()
  : LoKi::AuxDesktopBase ()
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::VertexRhoDistanceWithTheBestPV*
LoKi::Particles::VertexRhoDistanceWithTheBestPV::clone() const 
{ return new LoKi::Particles::VertexRhoDistanceWithTheBestPV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::VertexRhoDistanceWithTheBestPV::result_type
LoKi::Particles::VertexRhoDistanceWithTheBestPV::operator() 
  ( LoKi::Particles::VertexRhoDistanceWithTheBestPV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return InvalidDistance" ) ;
    return LoKi::Constants::InvalidDistance ;                    // RETURN 
  }
  const LHCb::VertexBase* ev = p->endVertex() ;
  if ( 0 == ev ) 
  {
    Error ( "LHCb::Particle::endVertex points to NULL, return InvalidDistance" ) ;
    return LoKi::Constants::InvalidDistance ;                    // RETURN 
  }
  // get the best vertex from desktop and use it 
  const LHCb::VertexBase* bpv = bestVertex ( p ) ; 
  if ( 0 == bpv ) 
  {
    Error ( "Related points to NULL, return InvalidDistance" ) ;
    return LoKi::Constants::InvalidDistance ;                    // RETURN 
  }
  // 
  return ( ev->position() - bpv->position() ) . Rho() ;    // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::VertexRhoDistanceWithTheBestPV::fillStream 
( std::ostream& s ) const { return s << "BPVVDRHO" ; }


// ============================================================================
// constructor from the source
// ============================================================================
LoKi::Particles::MinVertexDistanceWithSource::MinVertexDistanceWithSource
( const LoKi::Particles::MinVertexDistanceWithSource::_Source& s ) 
  : LoKi::AuxFunBase    ( std::tie ( s ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( s ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinVertexDistanceWithSource*
LoKi::Particles::MinVertexDistanceWithSource::clone() const 
{ return new LoKi::Particles::MinVertexDistanceWithSource(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MinVertexDistanceWithSource::result_type
LoKi::Particles::MinVertexDistanceWithSource::operator() 
  ( LoKi::Particles::MinVertexDistanceWithSource::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return 'HugeDistance'") ;
    return LoKi::Constants::HugeDistance ;                            // RETURN 
  }
  const LHCb::VertexBase* v = p -> endVertex() ;
  if ( 0 == v ) 
  {
    Error ( "LHCb::VertexBase* points to NULL, return 'HugeDistance'") ;
    return LoKi::Constants::HugeDistance ;                             // RETURN 
  }
  // use the functor!
  return m_fun.minvd ( v ) ;                                           // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::MinVertexDistanceWithSource::fillStream 
( std::ostream& s ) const 
{ return s << "MINVDSOURCE(" << m_fun.source() << ")" ; }
// ============================================================================


// ============================================================================
// constructor from the source
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceWithSource::MinVertexChi2DistanceWithSource
( const LoKi::Particles::MinVertexChi2DistanceWithSource::_Source& s ) 
  : LoKi::AuxFunBase    ( std::tie ( s ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( s ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceWithSource*
LoKi::Particles::MinVertexChi2DistanceWithSource::clone() const 
{ return new LoKi::Particles::MinVertexChi2DistanceWithSource(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceWithSource::result_type
LoKi::Particles::MinVertexChi2DistanceWithSource::operator() 
  ( LoKi::Particles::MinVertexChi2DistanceWithSource::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return 'HugeChi2'") ;
    return LoKi::Constants::HugeChi2 ;                            // RETURN 
  }
  const LHCb::VertexBase* v = p -> endVertex() ;
  if ( 0 == v ) 
  {
    Error ( "LHCb::VertexBase* points to NULL, return 'HugeChi2'") ;
    return LoKi::Constants::HugeChi2 ;                             // RETURN 
  }
  // use the functor!
  return m_fun.minvdchi2 ( v ) ;                                    // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::MinVertexChi2DistanceWithSource::fillStream 
( std::ostream& s ) const 
{ return s << "MINVDCHI2SOURCE(" << m_fun.source() << ")" ; }
// ============================================================================

 

// ============================================================================
// the default constructor
// ============================================================================
LoKi::Particles::MinVertexDistanceDV::MinVertexDistanceDV ()
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun ( s_PRIMARY ) 
{}
// ============================================================================
// the constructor from the vertex filter
// ============================================================================
LoKi::Particles::MinVertexDistanceDV::MinVertexDistanceDV 
( const LoKi::PhysTypes::VCuts& cut )
  : LoKi::AuxFunBase    ( std::tie ( cut ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun ( cut ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinVertexDistanceDV*
LoKi::Particles::MinVertexDistanceDV::clone() const 
{ return new LoKi::Particles::MinVertexDistanceDV(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MinVertexDistanceDV::result_type 
LoKi::Particles::MinVertexDistanceDV::operator()
  ( LoKi::Particles::MinVertexDistanceDV::argument p ) const
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return 'HugeDistance'") ;
    return LoKi::Constants::HugeDistance ;                            // RETURN 
  }
  const LHCb::VertexBase* v = p -> endVertex() ;
  if ( 0 == v ) 
  {
    Error ( "LHCb::VertexBase* points to NULL, return 'HugeDistance'") ;
    return LoKi::Constants::HugeDistance ;                             // RETURN 
  }
  // use the functor!
  return m_fun.minvd ( v ) ;                                           // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::MinVertexDistanceDV::fillStream 
( std::ostream& s ) const 
{ return s << "MINVDDV(" << m_fun.cut() << ")" ; }
// ============================================================================


// ============================================================================
// the default constructor
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceDV::MinVertexChi2DistanceDV ()
  : LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun ( s_PRIMARY ) 
{}
// ============================================================================
// the constructor from the vertex filter
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceDV::MinVertexChi2DistanceDV 
( const LoKi::PhysTypes::VCuts& cut )
  : LoKi::AuxFunBase    ( std::tie ( cut ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function () 
  , m_fun ( cut ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceDV*
LoKi::Particles::MinVertexChi2DistanceDV::clone() const 
{ return new LoKi::Particles::MinVertexChi2DistanceDV(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceDV::result_type 
LoKi::Particles::MinVertexChi2DistanceDV::operator()
  ( LoKi::Particles::MinVertexChi2DistanceDV::argument p ) const
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return 'HugeChi2'") ;
    return LoKi::Constants::HugeChi2 ;                            // RETURN 
  }
  const LHCb::VertexBase* v = p -> endVertex() ;
  if ( 0 == v ) 
  {
    Error ( "LHCb::VertexBase* points to NULL, return 'HugeChi2'") ;
    return LoKi::Constants::HugeChi2 ;                             // RETURN 
  }
  // use the functor!
  return m_fun.minvdchi2 ( v ) ;                                    // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::MinVertexChi2DistanceDV::fillStream 
( std::ostream& s ) const 
{ return s << "MINVDCHI2DV(" << m_fun.cut() << ")" ; }
// ============================================================================


// ============================================================================
// the constructor from the TES location
// ============================================================================
LoKi::Particles::MinVertexDistanceTES::MinVertexDistanceTES
( const std::string& path ) 
  : LoKi::AuxFunBase    ( std::tie ( path ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( path ) 
{}
// ============================================================================
// the constructor from the TES locations
// ============================================================================
LoKi::Particles::MinVertexDistanceTES::MinVertexDistanceTES
( const std::vector<std::string>& path ) 
  : LoKi::AuxFunBase    ( std::tie ( path ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( path ) 
{}
// ============================================================================
// the constructor from the TES location  & selector 
// ============================================================================
LoKi::Particles::MinVertexDistanceTES::MinVertexDistanceTES
( const std::string&            path , 
  const LoKi::PhysTypes::VCuts& cut  ) 
  : LoKi::AuxFunBase    ( std::tie ( path , cut ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( path , cut ) 
{}
// ============================================================================
// the constructor from the TES locations & selector 
// ============================================================================
LoKi::Particles::MinVertexDistanceTES::MinVertexDistanceTES
( const std::vector<std::string>& path , 
  const LoKi::PhysTypes::VCuts&   cut  ) 
  : LoKi::AuxFunBase    ( std::tie ( path , cut ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( path , cut ) 
{}
// ============================================================================
// the constructor from the TES location  & selector 
// ============================================================================
LoKi::Particles::MinVertexDistanceTES::MinVertexDistanceTES
( const LoKi::PhysTypes::VCuts& cut  ,
  const std::string&            path ) 
  : LoKi::AuxFunBase    ( std::tie ( cut , path ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( cut , path ) 
{}
// ============================================================================
// the constructor from the TES locations & selector 
// ============================================================================
LoKi::Particles::MinVertexDistanceTES::MinVertexDistanceTES
( const LoKi::PhysTypes::VCuts&   cut  ,
  const std::vector<std::string>& path ) 
  : LoKi::AuxFunBase    ( std::tie ( cut , path ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( cut , path ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinVertexDistanceTES*
LoKi::Particles::MinVertexDistanceTES::clone() const 
{ return new LoKi::Particles::MinVertexDistanceTES(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MinVertexDistanceTES::result_type 
LoKi::Particles::MinVertexDistanceTES::operator() 
  ( LoKi::Particles::MinVertexDistanceTES::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return 'HugeDistance'") ;
    return LoKi::Constants::HugeDistance ;                            // RETURN 
  }
  const LHCb::VertexBase* v = p -> endVertex() ;
  if ( 0 == v ) 
  {
    Error ( "LHCb::VertexBase* points to NULL, return 'HugeDistance'") ;
    return LoKi::Constants::HugeDistance ;                             // RETURN 
  }
  // use the functor!
  return m_fun.minvd ( v ) ;                                           // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::MinVertexDistanceTES::fillStream 
( std::ostream& s ) const 
{ 
  s << "MINVDTES("  ;
  //
  if ( 1 == m_fun.path().size() ) 
  { s << Gaudi::Utils::toString ( m_fun.path().front() ) ; }
  else 
  { s << Gaudi::Utils::toString ( m_fun.path()         ) ; }
  //
  return s << "," << m_fun.cut() << ")" ;
}
// ============================================================================


// ============================================================================
// the constructor from the TES location
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceTES::MinVertexChi2DistanceTES
( const std::string& path ) 
  : LoKi::AuxFunBase    ( std::tie ( path ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( path ) 
{}
// ============================================================================
// the constructor from the TES locations
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceTES::MinVertexChi2DistanceTES
( const std::vector<std::string>& path ) 
  : LoKi::AuxFunBase    ( std::tie ( path ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( path ) 
{}
// ============================================================================
// the constructor from the TES location  & selector 
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceTES::MinVertexChi2DistanceTES
( const std::string&            path , 
  const LoKi::PhysTypes::VCuts& cut  ) 
  : LoKi::AuxFunBase    ( std::tie ( path , cut ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( path , cut ) 
{}
// ============================================================================
// the constructor from the TES locations & selector 
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceTES::MinVertexChi2DistanceTES
( const std::vector<std::string>& path , 
  const LoKi::PhysTypes::VCuts&   cut  ) 
  : LoKi::AuxFunBase    ( std::tie ( path , cut ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( path , cut ) 
{}
// ============================================================================
// the constructor from the TES location  & selector 
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceTES::MinVertexChi2DistanceTES
( const LoKi::PhysTypes::VCuts& cut  ,
  const std::string&            path ) 
  : LoKi::AuxFunBase    ( std::tie ( cut , path ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( cut , path ) 
{}
// ============================================================================
// the constructor from the TES locations & selector 
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceTES::MinVertexChi2DistanceTES
( const LoKi::PhysTypes::VCuts&   cut  ,
  const std::vector<std::string>& path ) 
  : LoKi::AuxFunBase    ( std::tie ( cut , path ) ) 
  , LoKi::BasicFunctors<const LHCb::Particle*>::Function() 
  , m_fun ( cut , path ) 
{}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceTES*
LoKi::Particles::MinVertexChi2DistanceTES::clone() const 
{ return new LoKi::Particles::MinVertexChi2DistanceTES(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::MinVertexChi2DistanceTES::result_type 
LoKi::Particles::MinVertexChi2DistanceTES::operator() 
  ( LoKi::Particles::MinVertexDistanceTES::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, return 'HugeChi2'") ;
    return LoKi::Constants::HugeChi2 ;                            // RETURN 
  }
  const LHCb::VertexBase* v = p -> endVertex() ;
  if ( 0 == v ) 
  {
    Error ( "LHCb::VertexBase* points to NULL, return 'HugeChi2'") ;
    return LoKi::Constants::HugeChi2 ;                             // RETURN 
  }
  // use the functor!
  return m_fun.minvdchi2 ( v ) ;                                    // RETURN 
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::MinVertexChi2DistanceTES::fillStream 
( std::ostream& s ) const 
{ 
  s << "MINVDCHI2TES("  ;
  //
  if ( 1 == m_fun.path().size() ) 
  { s << Gaudi::Utils::toString ( m_fun.path().front() ) ; }
  else 
  { s << Gaudi::Utils::toString ( m_fun.path()         ) ; }
  //
  return s << "," << m_fun.cut() << ")" ;
}
// ============================================================================


// ============================================================================
// the default constructor: create the object in invalid state
// ============================================================================
LoKi::Particles::TrgPointingScoreWithBestPV::
TrgPointingScoreWithBestPV()
  : LoKi::AuxDesktopBase()
  , LoKi::Particles::TrgPointingScore ( s_VERTEX ) 
{}
// ============================================================================
// MANDATORY: the clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::TrgPointingScoreWithBestPV* 
LoKi::Particles::TrgPointingScoreWithBestPV::clone() const 
{ return new TrgPointingScoreWithBestPV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Particles::TrgPointingScoreWithBestPV::result_type 
LoKi::Particles::TrgPointingScoreWithBestPV::operator() 
  ( LoKi::Particles::TrgPointingScoreWithBestPV::argument p ) const 
{
  if ( 0 == p ) 
  {
    Error ( "LHCb::Particle* points to NULL, retuen -1000" ) ;
    return -1000 ;                                                     // RETURN 
  }
  // get the best vertex from desktop and use it 
  setVertex ( bestVertex ( p ) ) ;
  //
  return pointing ( p ) ;
}
// ============================================================================

// ============================================================================
// the default constructor: create the object in invalid state
// ============================================================================
LoKi::Particles::PseudoRapidityWithTheBestPV::PseudoRapidityWithTheBestPV()
  : LoKi::AuxDesktopBase                      (          )
  , LoKi::Particles::PseudoRapidityFromVertex ( s_VERTEX )
{}
// ============================================================================
// MANDATORY: the clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::PseudoRapidityWithTheBestPV*
LoKi::Particles::PseudoRapidityWithTheBestPV::clone() const
{ return new PseudoRapidityWithTheBestPV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Particles::PseudoRapidityWithTheBestPV::result_type
LoKi::Particles::PseudoRapidityWithTheBestPV::operator()
  ( LoKi::Particles::PseudoRapidityWithTheBestPV::argument p ) const
{
  if ( 0 == p )
  {
    Error ( "LHCb::Particle* points to NULL, return -1000" ) ;
    return -1000 ;                                                     // RETURN
  }
  //
  // get the best vertex from desktop and use it
  setVertex ( bestVertex ( p ) ) ;
  //
  return eta ( p ) ;
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::PseudoRapidityWithTheBestPV::fillStream
( std::ostream& s ) const { return s << "BPVETA" ; }
 



// ============================================================================
// the default constructor: create the object in invalid state
// ============================================================================
LoKi::Particles::PhiWithTheBestPV::PhiWithTheBestPV()
  : LoKi::AuxDesktopBase                        ()
  , LoKi::Particles::PseudoRapidityWithTheBestPV()
{}
// ============================================================================
// MANDATORY: the clone method ("virtual constructor")
// ============================================================================
LoKi::Particles::PhiWithTheBestPV*
LoKi::Particles::PhiWithTheBestPV::clone() const
{ return new PhiWithTheBestPV ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method
// ============================================================================
LoKi::Particles::PhiWithTheBestPV::result_type
LoKi::Particles::PhiWithTheBestPV::operator()
  ( LoKi::Particles::PhiWithTheBestPV::argument p ) const
{
  if ( 0 == p )
  {
    Error ( "LHCb::Particle* points to NULL, return -1000" ) ;
    return -1000 ;                                                     // RETURN
  }
  //
  // get the best vertex from desktop and use it
  setVertex ( bestVertex ( p ) ) ;
  //
  return phi ( p ) ;
}
// ============================================================================
// OPTIONAL: the specific printout
// ============================================================================
std::ostream& LoKi::Particles::PhiWithTheBestPV::fillStream
( std::ostream& s ) const { return s << "BPVPHI" ; }


// ============================================================================
// The END 
// ============================================================================
