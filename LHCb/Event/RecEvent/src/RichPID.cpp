
//-----------------------------------------------------------------------------
/** @file RichPID.cpp
 *
 *  Implementation file for class : RichPID
 *
 *  CVS Log :-
 *  $Id: RichPID.cpp,v 1.3 2006-06-14 17:38:39 jonrob Exp $
 *
 *  @author  Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date    2002-06-10
 */
//-----------------------------------------------------------------------------

// Include to suppress "debug information too long" warning on Windows
#include "GaudiKernel/Kernel.h"

// RichEvent includes
#include "Event/RichPID.h"

std::string LHCb::RichPID::pidType() const
{
  std::string hist;
  int cnt = 0;
  if ( this->global()     ) { hist+=(cnt>0?"+":""); hist+="Global";     ++cnt; }
  if ( this->local()      ) { hist+=(cnt>0?"+":""); hist+="Local";      ++cnt; }
  if ( this->ringRefit()  ) { hist+=(cnt>0?"+":""); hist+="RingRefit";  ++cnt; }
  if ( this->trigGlobal() ) { hist+=(cnt>0?"+":""); hist+="TrigGlobal"; ++cnt; }
  if ( this->trigLocal()  ) { hist+=(cnt>0?"+":""); hist+="TrigLocal";  ++cnt; }
  return hist;
}

bool LHCb::RichPID::isAboveThreshold(const Rich::ParticleIDType type) const
{
  switch ( type )
  {
  case Rich::Pion        :  return this->pionHypoAboveThres();
  case Rich::Electron    :  return this->electronHypoAboveThres();
  case Rich::Muon        :  return this->muonHypoAboveThres();
  case Rich::Kaon        :  return this->kaonHypoAboveThres();
  case Rich::Proton      :  return this->protonHypoAboveThres();
  default                :  return false;
  }
}

bool LHCb::RichPID::traversedRadiator(const Rich::RadiatorType radiator) const
{
  switch ( radiator )
  {
  case Rich::Aerogel   : return this->usedAerogel();
  case Rich::Rich1Gas  : return this->usedRich1Gas();
  case Rich::Rich2Gas  : return this->usedRich2Gas();
  default              : return false;
  }
}
