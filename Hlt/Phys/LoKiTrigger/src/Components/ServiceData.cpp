// $Id: ServiceData.cpp,v 1.1 2009-03-19 13:16:12 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// Local
// ============================================================================
#include "Service.h"
// ============================================================================
/** @file 
 *  Implementation file for class Hlt::Service
 *  The methods from Hlt::IData are implemented here 
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2009-03-18
 */
// ============================================================================
/*  retrieve the selection by name  (non-const access, only by producer)
 *  @param alg the producer
 *  @param key the selection key/name 
 *  @return pointer to the selection (0 for missing selections)
 */
// ============================================================================
Hlt::Selection* Hlt::Service::retrieve
( const IAlgorithm* producer ,                           //        the producer
  const Key&        key      ) const                     //  selection key/name
{
  // be a little bit paranoic:
  Assert ( 0 != producer            , 
           "retrieve: invalid producer"  , Retrieve_Invalid_Producer   ) ; 
  // find all registered output selections by the given producer:
  OutputMap::iterator ifind = m_outputs.find ( producer ) ;
  Assert ( m_outputs.end() != ifind , 
           "retrieve: unknown producer"  , Retrieve_Unknown_Producer   ) ; 
  // find the output selection by key:
  const SelMap& outs = ifind->second ;
  SelMap::iterator isel = outs.find ( key ) ;
  Assert ( outs.end() != isel , 
           "retrieve: unknown selection"  , Retrieve_Unknown_Selection ) ;
  //
  return isel->second ;
}
// ============================================================================
/*  retrieve the selection by name  (conts-access by consumer)
 *  @param key the selection key/name 
 *  @param alg the consumer 
 *  @return pointer to the selection (0 for missing selections)
 */
// ============================================================================
const Hlt::Selection* Hlt::Service::selection 
( const Key&        key      ,                            // selection key/name
  const IAlgorithm* consumer ) const                      //       the consumer 
{
  /// a bit exceptional case: anonymous access 
  if ( 0 == consumer && m_anonymous ) 
  {
    SelMap::iterator isel = m_selections.find ( key ) ;
    if ( m_selections.end() == isel ) 
    {
      if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) ) 
      { Warning ( "Unknown selection '" 
                  + key + "', return NULL for anonymous consumer" , 
                  Selection_Unknown_Selection ) ; }
      return 0 ;                                                      // RETURN 
    }
    //
    if ( m_spy || msgLevel ( MSG::DEBUG ) ) { /* spy here! */ }
    //
    return isel->second ;                                             // RETURN
  }
  // ==========================================================================
  // find the consumer 
  InputMap::iterator icon = m_inputs.find ( consumer ) ;
  if ( m_inputs.end() == icon ) 
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) ) 
    { Warning ( "Consumer '" + consumer->name() +
                "' is not registered for any input" ,  
                Selection_Unknown_Consumer ) ; }
    return 0 ;                                                        // RETURN
  }
  // ==========================================================================
  // finally find the selection 
  const SelMap& ins = icon->second ;
  SelMap::iterator isel = ins.find ( key ) ;
  if ( ins.end() == isel ) 
  {
    if ( m_pedantic || m_spy || msgLevel ( MSG::DEBUG ) ) 
    { Warning ( "Unknown selection '" + key + 
                "', return NULL for consumer '" 
                + consumer->name() + "'" , 
                Selection_Unknown_Selection ) ; }
    return 0 ;                                                         // RETURN
  }
  //
  if ( m_spy || msgLevel ( MSG::DEBUG ) ) {  /* spy here */ }
  //
  return isel->second ;
}
// ============================================================================



// ============================================================================
// The END 
// ============================================================================
