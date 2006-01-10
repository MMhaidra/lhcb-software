// $Id: GaudiOnline.cpp,v 1.2 2006-01-10 18:06:42 frankb Exp $
//====================================================================
//  GaudiOnline_dll.cpp
//--------------------------------------------------------------------
//
//  Package    : Gaudi
//
//  Description: Implementation of DllMain routine.
//               The DLL initialisation must be done seperately for 
//               each DLL. 
//
//  Author     : M.Frank
//  Created    : 13/1/99
//  Changes    : 
//
//====================================================================
// Include Files
#include "GaudiKernel/LoadFactoryEntries.h"

LOAD_FACTORY_ENTRIES(GaudiOnline)

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_FACTORY_ENTRIES(GaudiOnline)  {
  DECLARE_NAMESPACE_SERVICE(   LHCb,MBMCnvSvc  );
  DECLARE_NAMESPACE_SERVICE(   LHCb,MEPCnvSvc  );
  DECLARE_NAMESPACE_SERVICE(   LHCb,MEPManager );
  DECLARE_OBJECT(              OnlineMessageSvc);
}
