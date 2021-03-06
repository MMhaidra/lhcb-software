#ifndef CALODAQ_ICALOREADOUTTOOL_H 
#define CALODAQ_ICALOREADOUTTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/RawEvent.h"
#include "Event/RawBankReadoutStatus.h"
#include "CaloDet/DeCalorimeter.h"

static const InterfaceID IID_ICaloReadoutTool ( "ICaloReadoutTool", 4, 0 );

/** @class ICaloReadoutTool ICaloReadoutTool.h CaloDAQ/ICaloReadoutTool.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2007-07-30
 */
class ICaloReadoutTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICaloReadoutTool; }

  virtual std::string _rootInTES()=0;
  virtual StatusCode  _setProperty(const std::string& p,const std::string& v)=0;
  virtual bool getBanks()=0;
  virtual void setBanks(const std::vector<LHCb::RawBank*>* bank )=0;
  virtual void clear()=0;
  virtual void cleanData(int feb)=0;
  virtual LHCb::RawBankReadoutStatus& status()=0;
  virtual void putStatusOnTES()=0;
  virtual bool ok()=0;
  virtual DeCalorimeter* deCalo()=0;

  /// virtual destructor
  virtual ~ICaloReadoutTool() {}
  
protected:

private:

};
#endif // CALODAQ_ICALOREADOUTTOOL_H
