// $Log: not supported by cvs2svn $
// Revision 1.7  2001/06/28 14:24:15  ibelyaev
// remove 'inline' attributes (IDetectorElement.h)
//
#ifndef  DETDESC_IDETECTORELEMENT_H 
#define  DETDESC_IDETECTORELEMENT_H 1 

// Include files
#include <iostream>
#include <string>

#include "GaudiKernel/IInspectable.h"

// Forward declarations
class IGeometryInfo;
class IAlignment; 
class ICalibration;
class IReadOut;
class ISlowControl;
class IFastControl; 

/** @class IDetectorElement IDetectorElement.h DetDesc/IDetectorElement.h
    
    An abstract Interface accees 
    the node in DetectorDescription tree.
    Just delegates all questions to right guys. 
*/

class IDetectorElement: virtual public IInspectable
{
  ///
 public: 
  ///
  typedef std::vector<IDetectorElement*>   IDEContainer; 
  ///
 public:
  ///
  /// "accessor":  name/identifier of the Detector Element
  virtual          const std::string&   name       ()   const = 0;
  ///
  /// delegations:
  ///
  /// "accessor":  delegates the IGeometryInfo Interface 
  virtual                IGeometryInfo* geometry   ()         = 0; 
  /// "accessor":  delegates the IGeometryInfo Interface (const)
  virtual          const IGeometryInfo* geometry   ()   const = 0; 
  /// "accessor":  delegates the IAlignment Interface 
  virtual                IAlignment*    alignment  ()         = 0; 
  /// "accessor":  delegates the IAlignment Interface (const) 
  virtual          const IAlignment*    alignment  ()   const = 0; 
  /// "accessor":  delegates the ICalibration Interface 
  virtual                ICalibration*  calibration()         = 0; 
  /// "accessor":  delegates the ICalibration Interface (const)
  virtual          const ICalibration*  calibration()   const = 0; 
  /// "accessor":  delegates the IReadOut Interface 
  virtual                IReadOut*      readOut    ()         = 0; 
  /// "accessor":  delegates the IReadOut Interface (const)
  virtual          const IReadOut*      readOut    ()   const = 0; 
  /// "accessor":  delegates the ISlowControl Interface 
  virtual                ISlowControl*  slowControl()         = 0; 
  /// "accessor":  delegates the ISlowControl Interface (const)
  virtual          const ISlowControl*  slowControl()   const = 0; 
  /// "accessor":  delegates the IFastControl Interface                      
  virtual                IFastControl*  fastControl()         = 0; 
  // "accessor":  delegates the IFastControl Interface (const)
  virtual          const IFastControl*  fastControl()   const = 0; 
  /// Another way of accessiong the information: "pseudo-conversion"
  /// cast to       IGeometryInfo*
  virtual operator       IGeometryInfo*()                     = 0; 
  /// cast to const IGeometryInfo*
  virtual operator const IGeometryInfo*()               const = 0; 
  /// cast to       IAlignment*
  virtual operator       IAlignment*   ()                     = 0; 
  /// cast to const IAlignment*
  virtual operator const IAlignment*   ()               const = 0; 
  /// cast to       ICalibration*
  virtual operator       ICalibration* ()                     = 0; 
  /// cast to const ICalibration*
  virtual operator const ICalibration* ()               const = 0; 
  /// cast to       IReadOut*
  virtual operator       IReadOut*     ()                     = 0; 
  /// cast to const IReadOut*
  virtual operator const IReadOut*     ()               const = 0; 
  /// cast to       ISlowControl*
  virtual operator       ISlowControl* ()                     = 0; 
  /// cast to const ISlowControl*
  virtual operator const ISlowControl* ()               const = 0; 
  /// cast to       IFastControl*
  virtual operator       IFastControl* ()                     = 0; 
  /// cast to const IFastControl*
  virtual operator const IFastControl* ()               const = 0; 
  
  /// more convinient set of conversion operators:

  /// cast to       IGeometryInfo&
  virtual operator       IGeometryInfo&()                     = 0; 
  /// cast to const IGeometryInfo&
  virtual operator const IGeometryInfo&()               const = 0; 
  /// cast to       IAlingment&
  virtual operator       IAlignment&   ()                     = 0; 
  /// cast to const IAlignment&
  virtual operator const IAlignment&   ()               const = 0; 
  /// cast to       ICalibration&
  virtual operator       ICalibration& ()                     = 0; 
  /// cast to const ICalibration&
  virtual operator const ICalibration& ()               const = 0; 
  /// cast to       IReadOut&
  virtual operator       IReadOut&     ()                     = 0; 
  /// cast to const IReadOut&
  virtual operator const IReadOut&     ()               const = 0; 
  /// cast to       ISlowControl&
  virtual operator       ISlowControl& ()                     = 0; 
  /// cast to const ISlowControl&
  virtual operator const ISlowControl& ()               const = 0; 
  /// cast to       IFastControl&
  virtual operator       IFastControl& ()                     = 0; 
  /// cast to const IFastControl&
  virtual operator const IFastControl& ()               const = 0; 

  /// some functions to simplify the navigation 
  /// (according to feedback after release 3) 
  /// pointer to parent IDetectorElement
  virtual       IDetectorElement*  parentIDetectorElement()        = 0 ;  
  /// pointer to parent IDetectorElement (const version)
  virtual const IDetectorElement*  parentIDetectorElement() const  = 0 ; 
  /// (reference to) container of pointers to child detector elements 
  virtual       IDetectorElement::IDEContainer&  childIDetectorElements() = 0 ;
  /// (reference to) container of pointers to child detector elements 
  virtual const IDetectorElement::IDEContainer&
  childIDetectorElements() const = 0;
  /// iterators for manipulation of daughter elements 
  /// begin iterator 
  virtual IDetectorElement::IDEContainer::iterator childBegin() = 0;
  virtual IDetectorElement::IDEContainer::const_iterator childBegin() const = 0;
  /// end   iterator 
  virtual IDetectorElement::IDEContainer::iterator childEnd() = 0 ;
  virtual IDetectorElement::IDEContainer::const_iterator childEnd() const = 0 ;
  /// functions for  listing of objects, used in overloaded << operations
  virtual std::ostream&  printOut      ( std::ostream& ) const = 0;  
  /// reset to the initial state 
  virtual       IDetectorElement* reset()       = 0 ; 
  
  /// destructor
  virtual ~IDetectorElement(){};

  /**
   * This method initializes the detector element. It should be overridden
   * and used for computation purposes. This is a kind of hook for adding
   * user code easily in the initialization of a detector element.
   */
  virtual StatusCode initialize() = 0;

  /**
   * this gets the type of a parameter.
   * If this parameter does not exist, it raises a DetectorElementException.
   * @param name the name of the parameter
   * @return its type
   */
  virtual std::string userParameterType (std::string name) = 0;
  
  /**
   * this gets the comment of a parameter
   * If this parameter does not exist, it raises a DetectorElementException.
   * @param name the name of the parameter
   * @return its comment
   */
  virtual std::string userParameterComment (std::string name) = 0;
  
  /**
   * this gets the value of a parameter, as a string
   * If this parameter does not exist, it raises a DetectorElementException.
   * @param name the name of the parameter
   * @return its value, as a string
   */
  virtual std::string userParameterAsString (std::string name) = 0;
  
  /**
   * this gets the value of a parameter, as an int
   * If the value is not an int, it raises a DetectorElementException.
   * If this parameter does not exist, it raises a DetectorElementException.
   * @param name the name of the parameter
   * @return its value, as a string
   */
  virtual int userParameterAsInt (std::string name) = 0;
  
  /**
   * this gets the value of a parameter, as a double
   * If the value is not a number, it raises a DetectorElementException.
   * If this parameter does not exist, it raises a DetectorElementException.
   * @param name the name of the parameter
   * @return its value, as a double
   */
  virtual double userParameterAsDouble (std::string name) = 0;
  
  /**
   * this gets the value of the parameter as a double. This actually is an
   * equivalent of userParameterAsDouble.
   *  If the value is not a double, it raises a DetectorElementException.
   * If this parameter does not exist, it raises a DetectorElementException.
   * @param name the name of the parameter
   * @return its value, as a double
   */
  virtual double userParameter (std::string name) = 0;
  
  /**
   * this gets the type of a parameter vector
   * If this parameter does not exist, it raises a DetectorElementException.
   * @param name the name of the parameter vector
   * @return its type
   */
  virtual std::string userParameterVectorType (std::string name) = 0;
  
  /**
   * this gets the comment of a parameter vector
   * If this parameter does not exist, it raises a DetectorElementException.
   * @param name the name of the parameter vector
   * @return its comment
   */
  virtual std::string userParameterVectorComment (std::string name) = 0;
  
  /**
   * this gets the value of a parameter vector, as a vector of string
   * If this parameter does not exist, it raises a DetectorElementException.
   * @param name the name of the parameter vector
   * @return its value, as a string
   */
  virtual std::vector<std::string>
  userParameterVectorAsString (std::string name) = 0;
  
  /**
   * this gets the value of a parameter vector, as a vector of int
   * If the parameter vector is not made of ints, it raises a
   * DetectorElementException.
   * If this parameter does not exist, it raises a DetectorElementException.
   * @param name the name of the parameter vector
   * @return its value, as a string
   */
  virtual std::vector<int>
  userParameterVectorAsInt (std::string name) = 0;
  
  /**
   * this gets the value of a parameter as a vector of double.
   * If the parameter vector is not made of numbers, it raises a
   * DetectorElementException.
   * If this parameter does not exist, it raises a DetectorElementException.
   * @param name the name of the parameter
   * @return its value, as a vector of double
   */
  virtual std::vector<double>
  userParameterVectorAsDouble (std::string name) = 0;
  
  /**
   * this gets the value of a parameter as a vector of double. This actually is
   * an equivalent of userParameterVectorAsDouble.
   * If the parameter vector is not made of doubles, it raises a
   * DetectorElementException.
   * If this parameter does not exist, it raises a DetectorElementException.
   * @param name the name of the parameter
   * @return its value, as a vector of double
   */
  virtual std::vector<double> userParameterVector (std::string name) = 0;

  /**
   * this returns the list of existing userParameters as a vector of their
   * names
   * @return a list of userParameter names
   */
  virtual std::vector<std::string> userParameters() = 0;
  
  /**
   * this returns the list of existing userParameterVectors as a vector of
   * their names
   * @return a list of userParameter names
   */
  virtual std::vector<std::string> userParameterVectors() = 0;  

};
///
inline std::ostream& operator<<( std::ostream&           os , 
                                 const IDetectorElement& de ) 
{
  return de.printOut(os);
};
///
inline std::ostream& operator<<( std::ostream&           os , 
				 const IDetectorElement* de )
{ 
  return 
    ( (0 == de) ? (os << "IDetectorElement* points to NULL") : (os << *de) ) ; 
};

#endif  //    GAUDIKERNEL_IDETECTORELEMENT_H









