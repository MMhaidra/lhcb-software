
//-----------------------------------------------------------------------------
/** @file RichTrackSelector.cpp
 *
 *  Implementation file for RICH reconstruction tool : RichTrackSelector
 *
 *  CVS Log :-
 *  $Id: RichTrackSelector.cpp,v 1.4 2006-08-28 11:34:42 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   12/08/2006
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "RichTrackSelector.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_NAMESPACE_TOOL_FACTORY( Rich, RichTrackSelector );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Rich::RichTrackSelector::RichTrackSelector( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : RichTrackSelectorBase ( type, name, parent ) { }

//=============================================================================
// Destructor
//=============================================================================
Rich::RichTrackSelector::~RichTrackSelector() { }
