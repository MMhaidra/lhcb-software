// ============================================================================
#ifndef CALOUTILS_CELLMATRIX_H 
#define CALOUTILS_CELLMATRIX_H 1
// Include files
#include <functional>
#include "GaudiKernel/Point3DTypes.h"
#include "Kernel/CaloCellID.h"
// forward declaration
class DeCalorimeter ;   

/** @class CellMatrix CellMatrix.h CaloUtils/CellMatrix.h
 *  
 *  The base class for implementation of utilities 
 *  for determination of relative posiiton of 2 cells 
 *   e.g. 3x3 matrix, swiss-cross, 2x2 submatrix etc  
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   07/11/2001
 */
class CellMatrix : 
  public std::binary_function<LHCb::CaloCellID,LHCb::CaloCellID,double>
{
  
 public:
  
  /** set detector
   *  @param Det  new pointer to detector element
   */
  inline void setDet( const DeCalorimeter* Det ) 
    { m_det = Det; }
  
 protected:
  
  /** Standard constructor
   *  @param Det pointer to calorimeter detector 
   */
  CellMatrix( const DeCalorimeter* Det = 0 )
    : m_det ( Det )
    {};
  
  virtual ~CellMatrix(); ///< Destructor
  
 protected:
  
  /** accessor to calorimeter 
   *  @return pointer to detector element 
   */
  const DeCalorimeter* det() const { return m_det ;}
  
  /** select minimum from 2 values 
   *  @param a the first parameter 
   *  @param b the second parameter
   *  @return minnimum value 
   */
  template<class TYPE>
    inline TYPE mini ( const TYPE& a , const TYPE& b ) const 
    { return a < b ? a : b ; }
  
  /** select maximum from  2 values 
   *  @param a the first parameter 
   *  @param b the second parameter
   *  @return maximum value 
   */
  template <class TYPE>
    inline TYPE maxi ( const TYPE& a , const TYPE& b ) const 
    { return a < b ? b : a ; }
  
  /** calculate the intersection area for 2 squares on the plane
   *  @param   center1   center point of the first  square
   *  @param   halfsize1 half   size  of the first  square 
   *  @param   center2   center point of the second square
   *  @param   halfsize2 half   size  of the second square 
   *  @return the intersection area
   */ 
  inline double area ( const Gaudi::XYZPoint& center1   , 
                       const double           halfsize1 , 
                       const Gaudi::XYZPoint& center2   , 
                       const double           halfsize2 ) const 
    {
      /// check the sizes 
      if( halfsize1 <= 0 || halfsize2 <= 0 ) { return 0 ; } ///< RETURN 
      const double xSize = 
        mini( center1.x() + halfsize1 , center2.x() + halfsize2 ) - 
        maxi( center1.x() - halfsize1 , center2.x() - halfsize2 ) ;
      if( xSize <= 0        )  { return 0 ; } ///< RETURN 
      const double ySize = 
        mini( center1.y() + halfsize1 , center2.y() + halfsize2 ) - 
        maxi( center1.y() - halfsize1 , center2.y() - halfsize2 ) ;
      if( ySize <= 0        )  { return 0 ; } ///< RETURN 
      ///
      return xSize * ySize ;
    };
  
  /** throw the exception 
   *  @exception CaloException 
   *  @param message exception message 
   */
  void Exception
    ( const std::string& message ) const ;
  
 private:
  
  /** copy constructor is private!
   */
  CellMatrix( const CellMatrix& );
  
  /** assignement operator is private!
   */
  CellMatrix& operator=( const CellMatrix& );
  
 private:

  const DeCalorimeter* m_det ;
  
};

// ============================================================================
#endif // CALOUTILS_CELLMATRIX_H
// ============================================================================
