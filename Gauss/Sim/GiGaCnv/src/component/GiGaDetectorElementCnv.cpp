// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.5  2001/07/25 17:19:31  ibelyaev
/// all conversions now are moved from GiGa to GiGaCnv
///
/// Revision 1.4  2001/07/24 11:13:55  ibelyaev
/// package restructurization(III) and update for newer GiGa
///
/// Revision 1.3  2001/07/15 20:45:09  ibelyaev
/// the package restructurisation
/// 
//  ===========================================================================
#define GIGACNV_GIGADETECTORELEMENTCNV_CPP
// ============================================================================
/// STL
#include <string>
#include <vector>
/// Gaudi 
#include "GaudiKernel/CnvFactory.h" 
#include "GaudiKernel/DataObject.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/IDataSelector.h" 
#include "GaudiKernel/IAddressCreator.h" 
/// DetDesc 
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/CLIDDetectorElement.h" 
/// Geant4
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PVPlacement.hh"
#include "G4PhysicalVolumeStore.hh"
/// GiGa & GiGaCnv 
#include "GiGaCnv/IGiGaGeomCnvSvc.h"
/// local 
#include "GiGaDetectorElementCnv.h"

/// factory 
static const CnvFactory<GiGaDetectorElementCnv> 
s_GiGaDetectorElementCnvFactory ;
const       ICnvFactory&GiGaDetectorElementCnvFactory = 
s_GiGaDetectorElementCnvFactory ;

// ============================================================================
/** standard constructor 
 *  @param svc pointer to Service Locator 
 */
// ============================================================================
GiGaDetectorElementCnv::GiGaDetectorElementCnv( ISvcLocator* Locator ) 
  : GiGaCnvBase( storageType() , classID() , Locator ) 
{
  setNameOfGiGaConversionService( "GiGaGeomCnvSvc" ); 
  setConverterName              ( "GiGaDECnv"      ); 
}; 

// ============================================================================
/// destructor 
// ============================================================================
GiGaDetectorElementCnv::~GiGaDetectorElementCnv(){}; 

// ============================================================================
/// Class ID
// ============================================================================
const CLID&         GiGaDetectorElementCnv::classID     () 
{ return CLID_DetectorElement ; }

// ============================================================================
/// StorageType 
// ============================================================================
const unsigned char GiGaDetectorElementCnv::storageType () 
{ return GiGaGeom_StorageType ; } 

// ============================================================================
/** create the representation
 *  @param Object  pointer to data object 
 *  @param Address address 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaDetectorElementCnv::createRep( DataObject*     Object  , 
                                              IOpaqueAddress*& Address ) 
{
  ///
  Address = 0 ; 
  if( 0 == Object                 ) 
    { return Error("CreateRep::DataObject* points to NULL"); } 
  ///
  IDetectorElement* de = 0 ; 
  try        { de = dynamic_cast<IDetectorElement*>( Object ) ; } 
  catch(...) { de =                                 0 ; } 
  if( 0 == de       ) 
    { return Error("createRep::Bad cast to IDetectorElement*"); }
  if( 0 == geoSvc() )
    { return Error("createRep::Conversion Service is unavailable"); }   
  /// 
  IGeometryInfo* gi = de->geometry(); 
  if( 0 == gi || 0 == gi->lvolume() )
    { 
      MsgStream log( msgSvc() , name() ); 
      log << MSG::WARNING 
          << " IGeometryInfo*/ILVolume* is not available for DE="      
          << de->name()    
          << " Consider it just as a holder of daughter DEs"  << endreq; 
      /// geometry information is not available, 
      /// consider DE just as a collection of DetectorElements
      IDataSelector dS; 
      for( IDetectorElement::IDEContainer::iterator ic = de->childBegin() ; 
           de->childEnd() != ic ; ++ic )
        {
          /// retrieve objects from the store
          DataObject* obj = 0 ; 
          SmartDataPtr<DataObject> so( detSvc() , (*ic)->name() ); 
          obj = (DataObject*)(so.operator->()) ; 
          if( 0 == obj ) 
            { return Error("createRep:: DataObject is not availablel for " + 
                           (*ic)->name() ) ;}
          dS.push_back( obj ); 
        } 
      StatusCode sc = geoSvc()->createReps( &dS );
      if( sc.isFailure() ) 
        { return Error("createRep:: could not convert daughter elements") ; }
      ///
      return StatusCode::SUCCESS;
      ///
    }  
  /// here it is an ordinary detector element 
  /// create IOpaqueAddress
  IAddressCreator* addrCreator = 0 ; 
  try        { addrCreator = dynamic_cast<IAddressCreator*> ( cnvSvc() ) ; } 
  catch(...) { addrCreator =                                           0 ; } 
  if( 0 == addrCreator   ) 
    { return Error("createRep::Address Creator is unavailable"); } 
  StatusCode status = 
    addrCreator->createAddress( repSvcType() , 
                                classID   () , 
                                "GiGaGeom"   , 
                                "GiGaDetectorElementObject" , 
                                -1 , Address );   
  if( status.isFailure() ) 
    { return Error("createRep::Error in Address Creation",status); }
  if( 0 == Address       ) 
    { return Error("createRep::Error Address is created"        ); }
  ///
  return updateRep( Object , Address ) ; 
  /// 
};

// ============================================================================
/** update the  representation
 *  @param Object  pointer to data object 
 *  @param Address address 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaDetectorElementCnv::updateRep( DataObject*     Object  , 
                                              IOpaqueAddress* /* Address */ ) 
{
  ///
  { MsgStream log( msgSvc() , name() ); 
  log << MSG::VERBOSE << "updateRep::start" << endreq; } 
  ///
  if( 0 == Object                 ) 
    { return Error("updateRep::DataObject* points to NULL"); } 
  ///
  IDetectorElement* de = 0 ; 
  try        { de = dynamic_cast<IDetectorElement*>( Object ) ; } 
  catch(...) { de =                                 0 ; } 
  if( 0 == de        ) 
    { return Error("updateRep::Bad cast to IDetectorElement*"); }
  if( 0 == geoSvc()  )
    { return Error("updateRep::Conversion Service is unavailable"); } 
  ///
  IGeometryInfo* gi = de->geometry() ; 
  if( 0 == gi )
    { return Error("updateRep:: IGeometryInfo* is not available for " + 
                   de->name() ); }
  ILVolume*      lv = gi->lvolume () ;
  if( 0 == lv )
    { return Error("updateRep:: ILVolume*      is not available for " + 
                   de->name() ); }
  ///
  /// look at G4 physical volume store and check 
  ///  if it was converted exlicitely or imlicitely
  {
    std::string path ( de->name() );
    do
      {
        G4VPhysicalVolume* pv = 0; 
        G4PhysicalVolumeStore& store = *G4PhysicalVolumeStore::GetInstance();
        for( unsigned int indx = 0 ; indx < store.size() ; ++indx )
          { if( path == store[indx]->GetName() ) { pv = store[indx] ; break; } }
        /// it was converted EXPLICITELY or IMPLICITELY !!!
        if( 0 != pv ) 
          {
            MsgStream log( msgSvc() , name() ) ; 
            log << MSG::INFO << "DE=" << de->name() 
                << " was already EXPLICITELY/IMPLICITELY converted for PV=" 
                << pv->GetName() << endreq; 
            return StatusCode::SUCCESS; 
          }                          /// RETURN !!!
        ///
        path.erase( path.find_last_of('/') ); 
      } 
    while( std::string::npos != path.find_last_of('/') ); 
  }
  /// it have not been converted yet!!! convert it!
  G4LogicalVolume*     LV = geoSvc()->g4LVolume( lv->name() );
  if( 0 == LV ) 
    { return Error("updateRep:: could not convert LV="+lv->name() ); }
  G4VPhysicalVolume*   PV = geoSvc()->G4WorldPV() ; 
  if( 0 == PV ) 
    { return Error("updateRep:: G4WorldPV is not available!" ) ; }
  /// create the placement of Detector Element
  G4VPhysicalVolume*   pv = 
    new G4PVPlacement( gi->matrix().inverse() , 
                       LV , de->name() ,
                       PV->GetLogicalVolume() , false , 0 ) ;
  pv = pv ; /// just to please the compiler 
  /// look again in the store 
  {
    G4VPhysicalVolume* PV = 0; 
    G4PhysicalVolumeStore& store = *G4PhysicalVolumeStore::GetInstance();
    for( unsigned int indx = 0 ; indx < store.size() ; ++indx )
      { if( de->name() == store[indx]->GetName() ) 
        { PV = store[indx] ; break; } }
    if( 0 != PV ) { return StatusCode::SUCCESS; } /// RETURN !!!
  } 
  ///
  return Error("updateRep:: could not convert GetectorElement="+de->name()) ;
  ///
};

// ============================================================================












