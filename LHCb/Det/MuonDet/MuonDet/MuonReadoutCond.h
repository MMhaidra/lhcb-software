// $Id: MuonReadoutCond.h,v 1.2 2002-01-31 10:00:09 dhcroft Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
//
// ============================================================================
#ifndef MUONDET_MUONREADOUTCOND_H
#define MUONDET_MUONREADOUTCOND_H 1

// Include files
#include <vector>
#include "GaudiKernel/DataObject.h"
#include "DetDesc/Condition.h"

#include "MuonDet/CLID_MuonReadoutCond.h"

/** @class MuonReadoutCond MuonReadoutCond.h MuonDet/MuonReadoutCond.h
 *  
 *  The muon readout parameters implemented as a Condition
 *
 *  Stores the numbers related to the electronics and chamber effects 
 *  required to convert GEANT entry/exit points into electronics channels
 *  fired for a given chamber readout
 *
 *  @author David Hutchcroft
 *  @date   21/01/2002
 */

class MuonReadoutCond: public Condition
{
public:
  /// Default Constructors
  MuonReadoutCond();

  /// Constructor 
  MuonReadoutCond( const ITime& since, const ITime& till );

  /// Copy constructor 
  MuonReadoutCond( MuonReadoutCond& obj );

  /// Update using another MuonReadoutCond: deep copy all contents, 
  /// except for the properties of a generic DataObject
  virtual void update ( MuonReadoutCond& obj );

  /// Destructor
  ~MuonReadoutCond();

public:

  /// Sets the cumlative cross talk arrays
  virtual StatusCode initialize();


  // Re-implemented from DataObject

  /// Class ID of this instance
  inline virtual const CLID& clID() const { 
    return classID(); 
  } 

  /// Class ID of this class
  inline static  const CLID& classID() { 
    return CLID_MuonReadoutCond; 
  }

public:

  //local setters and getters

  /// get number of readouts associated to this chamber
  inline int number() const {
    return m_RList.size();
  }

  /// adds another readout of either "Anode" or "Cathode" as a std::string,
  /// returns the index of the readout created in index.
  StatusCode addReadout(const std::string &rType, int &index);

  /// get readoutType (Anode=0, Cathode=1)
  inline int readoutType(const int &i) const {
    return m_RList[i].ReadoutType;
  }

  /// get Efficiency as a fraction (mean value)
  inline double efficiency(const int &i) const {
    return m_RList[i].Efficiency;
  }
  /// set efficiency
  void setEfficiency(const double &eff, const int &i){
    m_RList[i].Efficiency = eff;
  }
  
  /// get ID of time jitter histogram (will be replaced sometime)
  inline std::string timeJitterID(const int &i) const {
    return m_RList[i].TimeJitterID;
  }
  /// set Hoook ID of time jitter histogram 
  void setTimeJitterID(const std::string &jitter, const int &i){
    m_RList[i].TimeJitterID=jitter;
  }
  
  /// get (maximum) synchronization imprecision (ns)
  inline double syncDrift(const int &i) const {
    return m_RList[i].SyncDrift;
  }
  /// set (maximum) synchronization imprecision (ns)
  void setSyncDrift(const double &sync, const int &i){
    m_RList[i].SyncDrift = sync;
  }
  
   /// get Chamber Noise rate (counts/sec/cm2)
  inline double chamberNoise(const int &i) const {
    return m_RList[i].ChamberNoise;
  }
  /// set Chamber Noise rate (counts/sec/cm2)
  void setChamberNoise(const double &chamNoise, const int &i){
    m_RList[i].ChamberNoise=chamNoise;
  }
  
  /// get Electronics noise rates (counts/sec/channel)
  inline double electronicsNoise(const int &i) const {
    return m_RList[i].ElectronicsNoise;
  }
  /// set Electronics noise rates (counts/sec/channel)
  void setElectronicsNoise(const double &elecNoise, const int &i){
    m_RList[i].ElectronicsNoise=elecNoise;
  }
  
  /// get average dead time of a channel (ns)
  inline double meanDeadTime(const int &i) const {
    return m_RList[i].MeanDeadTime;
  }
  /// set average dead time of a channel (ns)
  void setMeanDeadTime(const double &mDead, const int &i){
    m_RList[i].MeanDeadTime=mDead;
  }
  
  /// get RMS of the dead time (ns)
  inline double rmsDeadTime(const int &i) const {
    return  m_RList[i].RMSDeadTime;
  }
  /// set RMS of the dead time (ns)
  void setRMSDeadTime(const double &rmsDead, const int &i){
    m_RList[i].RMSDeadTime=rmsDead;
  }

  /// get time gate start relative to T0 (ns)
  inline double timeGateStart(const int &i) const {
    return m_RList[i].TimeGateStart;
  }
  /// set time gate start relative to T0 (ns)
  void setTimeGateStart(const double &tGate, const int &i){
    m_RList[i].TimeGateStart=tGate;
  }

  /// get size of double hit rate at pad edge in X
  void setPadEdgeSizeX(const double &pedge, const int &i){
    m_RList[i].PadEdgeSizeX=pedge;
  }
  /// get width (sigma) of pad edge effect in X (cm)
  void setPadEdgeSigmaX(const double psigma, const int &i){
    m_RList[i].PadEdgeSizeX=psigma;
  }
  /// get size of double hit rate at pad edge in Y
  void setPadEdgeSizeY(const double pedge, const int &i){
    m_RList[i].PadEdgeSizeY=pedge;
  }
  /// get width (sigma) of pad edge effect in Y (cm)
  void setPadEdgeSigmaY(const double psigma, const int &i){
    m_RList[i].PadEdgeSizeY=psigma;
  }

  /// function to add a cluster size with a corresponding probablilty in X
  void addClusterX(const int &size, const double &prob, const int &i);
  /// function to add a cluster size with a corresponding probablilty in Y
  void addClusterY(const int &size, const double &prob, const int &i);


  int singleGapClusterX(const double &randomNumber,
                        const double &xDistPadEdge, const int &i);
  
  int singleGapClusterY(const double &randomNumber,
                        const double &xDistPadEdge, const int &i);    

private: 
  // some typdefs for the structures to store the information in

  // used to keep the cluster size parameters before turning them into
  // cumlative probabilites
  typedef struct {
    int clSize;
    double clProb;
  } _clus;

  // this holds the details of a readout (Anode or Cathode)
  typedef struct  {
    int                 ReadoutType;
    double              Efficiency;
    std::string         TimeJitterID;
    double              SyncDrift;
    double              ChamberNoise;
    double              ElectronicsNoise;
    double              MeanDeadTime;
    double              RMSDeadTime;
    double              TimeGateStart;                 
    double              PadEdgeSizeX;
    double              PadEdgeSigmaX;
    double              PadEdgeSizeY;
    double              PadEdgeSigmaY;
    std::vector<_clus>  ClusterX;
    std::vector<_clus>  ClusterY;
    std::vector<double> CumProbX;
    std::vector<double> CumProbY;    
  } _readoutParameter;

private:
  /// used by the copy and update methods
  std::vector<_readoutParameter> getRList() const{
    return m_RList;
  }

  /// returns a single gap cluster size in x (0) or y (1) 
  /// should be given a random number between 0 and 1 and distance from the
  /// pad edge (cm)
  int singleGapCluster(const int &xy, const double &randomNumber,
                       const double &xpos, const int &i);


  int maxCluster(const _readoutParameter &readout, const char &xy);

  /// The list of the readouts 
  std::vector<_readoutParameter> m_RList;
};

#endif    // MUONDET_MUONREADOUTCOND_H
