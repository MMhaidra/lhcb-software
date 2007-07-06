// $Id: ReadLHE.cpp,v 1.1 2007-07-06 08:10:57 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// Gaudi
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiKernel/ToolFactory.h"
// ============================================================================
// Generators  
// ============================================================================
#include "Generators/F77Utils.h"
// ============================================================================
// LbPythia
// ============================================================================
#include "LbPythia/Pypars.h"
#include "LbPythia/Pythia.h"
#include "LbPythia/PythiaProduction.h"
// ============================================================================
// Local 
// ============================================================================
#include "ReadLHE.h"
// ============================================================================
/** @file
 *  Implementation file for class LbPythia::ReadLHE 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2007-06-29
 */
// ============================================================================
/* standard constructor 
 *  @param type tool type (?)
 *  @param name tool name 
 *  @param parent tool parent 
 */
// ============================================================================
LbPythia::ReadLHE::ReadLHE
( const std::string& type,
  const std::string& name,
  const IInterface* parent ) 
  : LbPythia::ReadFile ( type , name , parent ) 
  , m_LUN ( 0 )
{
  // see LHEREADPROCESS
  PythiaProduction::m_userProcess    = 3 ; ///< see LHEREADPROCESS
}
// ============================================================================
// destructor 
// ============================================================================
LbPythia::ReadLHE::~ReadLHE(){}
// ============================================================================
// tool initialization 
// ============================================================================
StatusCode LbPythia::ReadLHE::initialize () 
{
  // check input file name 
  if ( file().empty() ) { return Error("Input fiel name is not specified!") ; }
  // get free fortran Unit 
  m_LUN = F77Utils::getUnit() ;
  if ( 0 >= m_LUN ) { return Error("No free FORTRAN unit available ") ; }    
  // open input LHE file 
  StatusCode sc = F77Utils::openOld ( m_LUN , file() ) ;
  if ( sc.isFailure() ) 
  { return Error ( "Could not open the file '" + file() + "'" ) ; }
  //
  Pythia::pypars().mstp ( 161 ) = m_LUN ;                                // ATTENTION!
  Pythia::pypars().mstp ( 162 ) = m_LUN ;                                // ATTENTION!
  //
  return LbPythia::ReadFile::initialize () ;
}
// ============================================================================
// tool finalization 
// ============================================================================
StatusCode LbPythia::ReadLHE::finalize   () 
{
  // close input LHE file 
  StatusCode sc = F77Utils::close ( m_LUN ) ;
  if ( sc.isFailure() ) 
  { Error ( "Error in closing '" + file() + "'" , sc ) ; } // NO RETURN !
  m_LUN = 0 ;
  Pythia::pypars().mstp ( 161 ) = 0 ;                      // ATTENTION!
  Pythia::pypars().mstp ( 162 ) = 0 ;                      // ATTENTION!
  //
  return LbPythia::ReadFile::finalize() ;
} 
// ============================================================================
// Declaration of the Tool Factory
// ============================================================================
DECLARE_NAMESPACE_TOOL_FACTORY(LbPythia,ReadLHE)
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
