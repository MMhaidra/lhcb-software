// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.4  2001/07/25 17:19:32  ibelyaev
/// all conversions now are moved from GiGa to GiGaCnv
///
/// Revision 1.3  2001/07/24 11:13:55  ibelyaev
/// package restructurization(III) and update for newer GiGa
///
/// Revision 1.2  2001/07/15 20:45:11  ibelyaev
/// the package restructurisation
/// 
// ============================================================================
#ifndef      GIGACNV_GIGAKINECNVSVC_H 
#define      GIGACNV_GIGAKINECNVSVC_H  1 
// ============================================================================
/// from STL 
#include <string> 
/// from GiGaCnv 
#include "GiGaCnv/IGiGaKineCnvSvc.h" 
#include "GiGaCnv/GiGaCnvSvcBase.h" 
#include "GiGaCnv/GiGaKineRefTable.h" 
///
template <class SERVICE> 
class SvcFactory; 
///
class    IGiGaSvc; 
class    IParticlePropertySvc; 
class    IDataSelector;

/** @class GiGaKineCnvSvc GiGaKineCnvSvc.h 
 *    
 *  conversion service  for converting of Gaudi 
 *  MCParticle/MCVertex structure into 
 *  Geant4 Primary Event conversion of  
 *  Geant4 Trajectories into Gaudi MCVertex/MCParticle structure 
 *  
 *  @author  Vanya Belyaev
 *  @date    07/08/2000
 */

class GiGaKineCnvSvc: public          GiGaCnvSvcBase ,
                      virtual public IGiGaKineCnvSvc  
{
  ///
  friend class SvcFactory<GiGaKineCnvSvc>;
  ///
protected: 

  /** standard constructor
   *  @param name  name of the service 
   *  @param loc   pointer to service locator 
   */
  GiGaKineCnvSvc( const std::string& name , 
                  ISvcLocator* loc );
  /// virtual destructor
  virtual ~GiGaKineCnvSvc();
  /// 
public: 
  
  /** initialization 
   *  @return status code 
   */
  virtual StatusCode            initialize() ;

  /** finalization 
   *  @return status code 
   */
  virtual StatusCode            finalize  () ;
  
  /** retrieve the relation table between Geant4 track/trajectory 
   *  identifiers and the converted MCParticle objects 
   *  @return the reference to relation table  
   */
  virtual GiGaKineRefTable&     table()        { return m_table;} ;
  
  /** access to particle properties service 
   *  @return pointer to particle properties service 
   */
  virtual IParticlePropertySvc* ppSvc () const { return m_ppSvc; }

  /** query the interface
   *  @param ID unique interface identifier 
   *  @param II placeholder for interface 
   *  @return status code 
   */
  virtual StatusCode queryInterface( const InterfaceID& ID , 
                                     void**             II ) ;

private:
  
  /// name of particle property service
  std::string            m_ppSvcName ;
  /// pointer to particle property service 
  IParticlePropertySvc*  m_ppSvc     ;
  /// reference table 
  GiGaKineRefTable       m_table     ;

};

// ============================================================================
#endif  ///<  GIGACNV_GIGAKINECNVSVC_H
// ============================================================================


























