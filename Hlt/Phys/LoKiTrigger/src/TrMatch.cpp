// $Id: TrMatch.cpp,v 1.1 2008-11-17 17:38:49 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/HltUtils.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TrMatch.h"
// ============================================================================
// Local 
// ============================================================================
#include "LTTools.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Hlt1::TrMatch
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2008-11-17
 */
// ============================================================================
// constructor 
// ============================================================================
LoKi::Hlt1::TrMatch::TrMatch 
( const std::string&                   output  ,   // output selection name/key 
  const LoKi::Hlt1::TrMatch::TrSource& tracks2 ,   // tracks to be matched with 
  const LoKi::Hlt1::MatchConf&         config  )   //        tool configuration 
  : LoKi::BasicFunctors<LHCb::Track*>::Pipe ()
  , m_tracks2    ( tracks2 ) 
  , m_config     ( config  )
  , m_sink       ( output  ) 
  , m_qualityID  ( 0       ) 
  , m_qualityID2 ( 0       )
  , m_match      (         )
  , m_recoID     ( 0       )
  , m_alg        ( 0       ) 
{
  init() ; 
}
// ============================================================================
// the only one important method 
// ============================================================================
LoKi::Hlt1::TrMatch::result_type 
LoKi::Hlt1::TrMatch::operator() 
  ( LoKi::Hlt1::TrMatch::argument a ) const 
{
  if ( !m_match || 0 == alg() ) { init () ; }
  Assert ( !(!m_match) && 0 != alg() ,  "Inavlid setup!" ) ;
  
  
  typedef LHCb::Track::Container    Tracks ;  
  typedef std::vector<LHCb::Track*> TRACKS ;
  
  /// get TES container 
  Tracks* otracks = alg() -> getOrCreate<Tracks,Tracks> ( address() ) ;
  
  // get the tracks from the second source 
  TRACKS tracks2 = m_tracks2() ;
  
  // the output selection 
  TRACKS output ;

  // double loop over all 2-tracks combinations
  
  for ( TRACKS::const_iterator itrk1 = a.begin() ; a.end() != itrk1 ; ++itrk1 ) 
  {
    const LHCb::Track* trk1 = *itrk1 ;
    if ( 0 == trk1 ) { continue ; }                                // CONTINUE 
    
    for ( TRACKS::const_iterator itrk2 = tracks2.begin() ; tracks2.end() != itrk2 ; ++itrk2 ) 
    {
      const LHCb::Track* trk2 = *itrk2 ;
      if ( 0 == trk2 ) { continue ; }                              // CONTINUE  
      //
      // ======================================================================
      std::auto_ptr<LHCb::Track> track3 ( new LHCb::Track() ) ;
      // the actual track matching
      double q1 = 0 ;
      double q2 = 0 ;
      StatusCode sc = match() -> match
        ( *trk1 , *trk2 , *track3 , q1 , q2 ) ;
      if ( sc.isFailure() ) { continue ; }                         // CONTINUE 
      
      // good matching ?
      if ( q1 < maxQualityCut() && q2 < maxQuality2Cut() ) 
      {
        if ( 0 != qualityID  () ) { track3 -> addInfo ( qualityID  () , q1 ) ; }
        if ( 0 != qualityID2 () ) { track3 -> addInfo ( qualityID2 () , q2 ) ; }
        // move info ? 
        if ( moveInfo () ) 
        {
          Hlt::MergeInfo ( *trk1 , *track3 ) ;
          Hlt::MergeInfo ( *trk2 , *track3 ) ;          
        }        
        /// add the track to the selection 
        output.push_back ( track3.release() ) ;
        /// add the track to the TES container 
        otracks->insert ( output.back() ) ;
      }
      // ======================================================================
    } // end of the loop over the second conatiner of tracks 
  } // end of the loop over the first constainer of tarcks
  // final... 
  // register the selection in Hlt Data Service 
  return m_sink ( output ) ; // RETURN 
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Hlt1::TrMatch::fillStream ( std::ostream& s ) const 
{
  return 
    s << "TrMatch(" 
      << "'" << address()  << "',"
      <<        m_tracks2  << "," 
      <<  config()         << ")" ;  
}
// ============================================================================
void LoKi::Hlt1::TrMatch::init () const 
{
  // get GaudiAlgorithm 
  GaudiAlgorithm* alg = LoKi::Hlt1::Utils::getGaudiAlg ( *this ) ;
  Assert ( 0 != alg , "GaudiAlgorithm points to NULL!" ) ;
  // get IAlgorithm 
  IAlgorithm* ialg = LoKi::Hlt1::Utils::getAlg ( *this ) ;
  Assert ( alg == ialg , "GaudiAlgorithm is not *my* IAlgorithm" ) ;
  //
  m_alg = alg ;
  //
  // retrive the tool 
  m_match = m_alg->tool<ITrackMatch> ( mTool() , alg ) ;
  /// it must be private tool! 
  Assert ( m_match->parent() == m_alg , "ITrackMatch tool must be private!");  
  //
  /// get the service 
  SmartIF<IANNSvc> ann = LoKi::Hlt1::Utils::annSvc( *this ) ;
  //
  { // recoID 
    boost::optional<IANNSvc::minor_value_type> info = 
      ann->value( "InfoID" , mTool()  );
    Assert( info , " request for unknown Info ID : " + mTool() );
    //
    m_recoID = info->second ;
  }
  //
  m_qualityID = 0 ;
  if ( !quality().empty() ) 
  {
    boost::optional<IANNSvc::minor_value_type> info = 
      ann->value( "InfoID" , quality() );
    Assert( info , " request for unknown Info ID : " + quality() );
    //
    m_qualityID  = info->second ; 
  }
  //
  m_qualityID2 = 0 ;
  if ( !quality2().empty() ) 
  {
    boost::optional<IANNSvc::minor_value_type> info = 
      ann->value( "InfoID" , quality2() );
    Assert( info , " request for unknown Info ID : " + quality2() );
    //
    m_qualityID2  = info->second ; 
  }
  //
}

// ============================================================================


// ============================================================================
// The END 
// ============================================================================


