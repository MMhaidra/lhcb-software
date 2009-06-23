// $Id: VeloClusterMonitor.h,v 1.11 2009-06-23 12:49:26 krinnert Exp $
#ifndef VELORECMONITORS_VELOCLUSTERMONITOR_H 
#define VELORECMONITORS_VELOCLUSTERMONITOR_H 1

// Include files
// -------------
#include "TH1D.h"
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

#include "Event/VeloCluster.h"

#include "VeloMonitorBase.h"

/** @class VeloClusterMonitor VeloClusterMonitor.h
 *  
 *  @author Eduardo Rodrigues
 *  @date   2008-08-18
 *
 *  @author Mark Tobin, Kazu Akiba
 *  @date   2008-06-28
 *
 *  Based on original version by
 *  @author Aras Papadelis, Thijs Versloot
 *  @date   2006-04-30
 *
 */

namespace Velo
{
  
  class VeloClusterMonitor : public VeloMonitorBase {
    
  public:
    /// Standard constructor
    VeloClusterMonitor( const std::string& name, ISvcLocator* pSvcLocator );
    
    virtual ~VeloClusterMonitor( );     ///< Destructor
    
    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization
    
  protected:
    
  private:

    // Retrieve the VeloClusters
    StatusCode veloClusters();

    // Monitor the VeloClusters
    void monitorClusters();

    // R_i-R_j difference distributions
    void rDifferences( unsigned int sensorNumber1,
                       double localR1,
                       unsigned int sensorNumber2 );

    // R_i-R_j correlation distributions
    void rCorrelations( unsigned int sensorNumber1, double localR1 );

    // Data members
    std::string m_tae;

    LHCb::VeloClusters* m_clusters;
    std::vector<unsigned int> m_nClustersPerSensor;
    
    AIDA::IHistogram1D* m_hNCluEvt;
    AIDA::IHistogram1D* m_hCluSize;
    AIDA::IHistogram2D* m_hCluSizeSens;
    AIDA::IHistogram1D* m_hCluADC;
    AIDA::IHistogram2D* m_hCluADCSens;
    AIDA::IHistogram1D* m_hCluADCR;
    AIDA::IHistogram1D* m_hCluADCPhi;
    AIDA::IHistogram1D* m_hSeedADC;
    AIDA::IHistogram1D* m_hSeedADCR;
    AIDA::IHistogram1D* m_hSeedADCPhi;
    AIDA::IHistogram1D* m_hIncADC;
    AIDA::IHistogram1D* m_hIncADCR;
    AIDA::IHistogram1D* m_hIncADCPhi;
    AIDA::IHistogram2D* m_hActiveLinkSens;
    
    std::vector<AIDA::IHistogram1D*> m_hNCluSens;
    std::map<unsigned int, AIDA::IHistogram1D*> m_hRDiff; 
    std::map<unsigned int, AIDA::IHistogram2D*> m_hRCorr; 
    
    TH1D* m_histOccSpectAll;
    TH1D* m_histOccSpectLow;
    TH1D* m_histAvrgSensor;
    std::vector< TH1D* > m_stripOccupancyHistPerSensor;
    std::vector< TH1D* > m_channelOccupancyHistPerSensor;
    unsigned int m_occupancyDenom;
    
    
    // Job options
    std::string m_clusterCont;
    std::vector<unsigned int> m_rSensorNumbers;
    std::vector<unsigned int> m_phiSensorNumbers;
    bool m_rCorrelationPlots;
    bool m_perSensorPlots;
    bool m_occupancyPlots;
    unsigned int m_occupancyResetFreq;	
  };
}

#endif // VELORECMONITORS_VELOCLUSTERMONITOR_H
