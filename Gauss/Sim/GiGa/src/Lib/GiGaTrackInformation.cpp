// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/src/Lib/GiGaTrackInformation.cpp,v 1.4 2001-04-23 11:53:34 ibelyaev Exp $
#define GIGA_GIGATRACKINFORMATION_CPP 1

// GiGa 
#include "GiGa/GiGaTrackInformation.h"


/////////////////////////////////////////////////////////////////////////////////////
G4Allocator<GiGaTrackInformation> s_GiGaTrackInformationAllocator;
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
GiGaTrackInformation::~GiGaTrackInformation() {}; 
////////////////////////////////////////////////////////////////////////////////////
void*      GiGaTrackInformation::operator new    ( size_t    )
{ return (void*) s_GiGaTrackInformationAllocator.MallocSingle(); };
////////////////////////////////////////////////////////////////////////////////////
void       GiGaTrackInformation::operator delete ( void*  trackInfo  )
{ s_GiGaTrackInformationAllocator.FreeSingle((GiGaTrackInformation*) trackInfo ); };
////////////////////////////////////////////////////////////////////////////////////
