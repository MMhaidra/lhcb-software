// $Id: ICameraTool.h,v 1.2 2009-08-26 10:50:11 rogers Exp $
#ifndef RICHKERNEL_ICAMERATOOL_H 
#define RICHKERNEL_ICAMERATOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/IHistogramSvc.h"

// Forward declarations
#include <iosfwd>
class TH1D;
class TH2D;

static const InterfaceID IID_ICameraTool ( "ICameraTool", 1, 0 );

/** @class ICameraTool ICameraTool.h ICameraTool.h
 *   
 *
 *  @author Claus Buszello
 *  @date   2007-05-30
 */
class ICameraTool : virtual public IAlgTool {
 public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICameraTool; }
  
  enum MessageLevel { NONE, INFO, WARNING, ERROR };

  virtual int SendAndClear(MessageLevel l,const std::string& who,const std::string& what)=0;
  virtual int SendAndClearTS(MessageLevel l,const std::string& who,const std::string& what)=0; 

  virtual int Append(TH2D * H,const char *opts=NULL)=0;
  virtual int Append(TH1D * H,const char *opts=NULL)=0;

  virtual int Append(const char * T,const char * C)=0;
  virtual int Append(const char * T,void * C, int sz)=0;

  virtual int Append(const char * C)=0;
  virtual int Append(const std::string & C)=0;

};

std::ostream& operator<<(std::ostream &, ICameraTool::MessageLevel);

#endif // RICHKERNEL_ICAMERATOOL_H
