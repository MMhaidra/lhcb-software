// $Id: MuonChamberGrid.h,v 1.1 2005-10-25 06:54:28 asarti Exp $
#ifndef MUONDET_MUONCHAMBERGRID_H 
#define MUONDET_MUONCHAMBERGRID_H 1

#include "DetDesc/Condition.h"

// Include files
#include "MuonDet/MuonFrontEndID.h"

/** @class MuonChamberGrid MuonChamberGrid.h MuonDet/MuonChamberGrid.h
 *  
 *
 *  @author Alessia(o) Sat(r)ta(i)
 *  @date   2004-01-07
 */

/// Class ID of the Muon Grid
static const CLID& CLID_MuonChamberGrid = 11094;  

class MuonChamberGrid: public Condition {
public: 
  /// Standard constructor
  MuonChamberGrid( ); 

  virtual ~MuonChamberGrid( ); ///< Destructor

  StatusCode initialize();
  

  std::vector<MuonFrontEndID> listOfPhysChannels(double x_enter,double y_enter,
						 double x_exit,double y_exit);
  
  double retLenght(int nLx,  std::vector<double> my_list);

  // Class ID of this class
  inline static  const CLID& classID() {
    return CLID_MuonChamberGrid;
  };

  inline unsigned int MuonChamberGrid::getNGrid(){
    return m_number_of_grid;
  }

  inline std::vector<double> MuonChamberGrid::getReadoutGrid(){
    return m_readoutType;
  }

protected:

private:

  int m_number_of_grid;
  std::vector<double> m_readoutType;
  std::vector<double> m_x_pad_rdout1;
  std::vector<double> m_y_pad_rdout1;
  std::vector<double> m_x_pad_rdout2;
  std::vector<double> m_y_pad_rdout2;

};
#endif // MUONDET_MUONCHAMBERGRID_H

