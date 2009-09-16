// $Id: CaloEnergyFromRaw.h,v 1.11 2009-09-16 16:02:46 odescham Exp $
#ifndef CALOENERGYFROMRAW_H 
#define CALOENERGYFROMRAW_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDAQ/ICaloEnergyFromRaw.h"            // Interface
#include "CaloReadoutTool.h"

/** @class CaloEnergyFromRaw CaloEnergyFromRaw.h
 *  Decode the calorimeter energies, accodrding to version
 *
 *  @author Olivier Callot
 *  @date   2005-01-10
 */
class CaloEnergyFromRaw : public CaloReadoutTool , public virtual ICaloEnergyFromRaw {
public: 
  /// Standard constructor
  CaloEnergyFromRaw( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~CaloEnergyFromRaw( ); ///< Destructor

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual std::vector<LHCb::CaloAdc>& adcs( );// decode all banks
  virtual std::vector<LHCb::CaloAdc>& adcs( LHCb::RawBank* bank ); // decode a single bank
  virtual std::vector<LHCb::CaloAdc>& adcs( int source ); // decode a single Tell1 bank
  virtual std::vector<LHCb::CaloAdc>& pinAdcs( );
  //
  virtual std::vector<LHCb::CaloDigit>& digits( ); // decode all banks
  virtual std::vector<LHCb::CaloDigit>& digits( LHCb::RawBank* bank );// decode a single bank
  virtual std::vector<LHCb::CaloDigit>& digits(  int source );// decode a single Tell1 bank  
  virtual void clear();
  virtual void cleanData(int feb);
  
  
protected:
  bool getData ( LHCb::RawBank* bank );
  bool getDigits ();

private:
  double   m_pedShift;
  std::vector<LHCb::CaloAdc>    m_pinData;
  std::vector<LHCb::CaloAdc>    m_data;
  std::vector<LHCb::CaloDigit>  m_digits;
};
#endif // CALOENERGYFROMRAW_H
