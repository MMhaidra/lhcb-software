// $Id: TsaSeedSelector.cpp,v 1.1 2006-12-06 14:35:01 mneedham Exp $

// GaudiKernel
#include "GaudiKernel/ToolFactory.h"

#include "TsaSeedSelector.h"
#include "TsaKernel/SeedFunctor.h"
#include "TsaKernel/SeedTrack.h"

#include <algorithm>
#include <utility>

#include "Event/State.h"

DECLARE_TOOL_FACTORY( TsaSeedSelector );

TsaSeedSelector::TsaSeedSelector(const std::string& type,
                               const std::string& name,
                                const IInterface* parent):
TsaSelectorBase(type, name, parent)
{

}

TsaSeedSelector::~TsaSeedSelector(){
  // destructer
}

StatusCode TsaSeedSelector::execute(LHCb::State& , std::vector<SeedTrack*>& seeds, std::vector<SeedHit*>*  ){
  return execute(seeds);
}

StatusCode TsaSeedSelector::execute(std::vector<SeedTrack*>& seeds, std::vector<SeedHit*>*  ){

//-------------------------------------------------------------------------
//  Select tracks in 3-D
//-------------------------------------------------------------------------

  // Sort seeds according to their likelihood
  std::sort( seeds.begin(), seeds.end(), SeedFunctor::decreasingLikelihood<const SeedTrack*>() );
  return select(seeds);
}
