// $Id: lcgStl.h,v 1.3 2004-05-03 15:15:38 cattanem Exp $
#ifndef KERNEL_LCGSTL_H 
#define KERNEL_LCGSTL_H 1

// Dummy use of complex STL classes used in LHCb event model
// For lcg dictionary generation

// Include files
#include <vector>
#include <map>
#include <utility>
#include "Kernel/VeloChannelID.h"
#include "Kernel/ITChannelID.h"
#include "Kernel/OTChannelID.h"

namespace {
  std::vector<std::pair<int,int> > a;
  std::pair<VeloChannelID,int>     b;
  std::pair<ITChannelID,int>       c;
  std::pair<OTChannelID,int>       d;
  std::map<int,int>                e;
  std::vector<std::pair<long,double> > f;
  std::vector<std::pair<int,double> > g;
}
#endif // KERNEL_LCGSTL_H
