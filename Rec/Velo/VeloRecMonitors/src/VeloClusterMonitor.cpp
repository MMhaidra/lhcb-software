// $Id: VeloClusterMonitor.cpp,v 1.13 2009-05-14 15:28:36 krinnert Exp $
// Include files 
// -------------

#include <string.h>

/// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiUtils/Aida2ROOT.h"

// from VeloDet
#include "VeloDet/DeVeloSensor.h"

// local
#include "VeloClusterMonitor.h"

// from Boost
#include <boost/assign/list_of.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

//-----------------------------------------------------------------------------
// Implementation file for class : VeloClusterMonitor
//
// VELO clusters monitoring algorithm.
// Produces a set of histograms from the clusters bank in the TES.
//
// 2008-08-18 : Eduardo Rodrigues
// 2008-06-28 : Mark Tobin, Kazu Akiba
// 2008-04-30 : Aras Papadelis, Thijs Versloot
//
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
namespace Velo {
  DECLARE_ALGORITHM_FACTORY( VeloClusterMonitor );
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Velo::VeloClusterMonitor::VeloClusterMonitor( const std::string& name,
					      ISvcLocator* pSvcLocator)
  : Velo::VeloMonitorBase ( name , pSvcLocator )
  , m_nClustersPerSensor(256,0)
  , m_occupancyDenom(0)
{
  m_rSensorNumbers = boost::assign::list_of(1)(3)(5)(7)(9)(20)(24)(26)(28)(30);
  m_phiSensorNumbers = boost::assign::list_of(65)(67)(69)(71)(73)(84)(88)(90)(92)(94);
  
  declareProperty( "VeloClusterLocation",
		   m_clusterCont = LHCb::VeloClusterLocation::Default );
  declareProperty( "RSensorNumbersForPlots",   m_rSensorNumbers );
  declareProperty( "PhiSensorNumbersForPlots", m_phiSensorNumbers );
  declareProperty( "PerSensorPlots", m_perSensorPlots=false );
  declareProperty( "OccupancyPlots", m_occupancyPlots=false );
  declareProperty( "OccupancyResetFrequency", m_occupancyResetFreq=10000 );
}

//=============================================================================
// Destructor
//=============================================================================
Velo::VeloClusterMonitor::~VeloClusterMonitor() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode Velo::VeloClusterMonitor::initialize() {

  StatusCode sc = VeloMonitorBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;

  m_tae = name();
  m_tae.erase(0,3);
  
  if ( m_occupancyPlots ) {
    m_histOccSpectAll = Gaudi::Utils::Aida2ROOT::aida2root(book1D("OccSpectAll", "Occupancy Spectrum", -0.5, 100.5, 202)); 
    m_histOccSpectLow = Gaudi::Utils::Aida2ROOT::aida2root(book1D("OccSpectMaxLow", "Occupancy Spectrum", -0.5, 20.5, 210));
    m_histAvrgSensor  = Gaudi::Utils::Aida2ROOT::aida2root(book1D("OccAvrgSens", "Avrg. Occupancy vs. Sensor", -0.5, 131.5, 132));
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode Velo::VeloClusterMonitor::execute() {

  // Count the number of events monitored
  // ------------------------------------
  counter( "# events" ) += 1;

  // Increment the occpancy denominator, reset at configurable
  // frequency
  if ( m_occupancyPlots ) {
    if ( 0 == m_occupancyDenom % m_occupancyResetFreq ) {
      m_occupancyDenom = 1;
      for (  std::map< unsigned int,  TH1D* >::iterator hIt = m_occupancyHistPerSensor.begin();
          hIt != m_occupancyHistPerSensor.end(); 
          ++hIt ) {
        hIt->second->Reset();
      }
      m_histOccSpectAll->Reset();
      m_histOccSpectLow->Reset();
    } else {
      ++m_occupancyDenom;
      for (  std::map< unsigned int,  TH1D* >::iterator hIt = m_occupancyHistPerSensor.begin();
          hIt != m_occupancyHistPerSensor.end(); 
          ++hIt ) {
        hIt->second->Scale((m_occupancyDenom-1.0)/m_occupancyDenom);
      }
    }
  }

  monitorClusters();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode Velo::VeloClusterMonitor::finalize() {
  
  if ( m_debugLevel ) debug() << "==> Finalize" << endmsg;
  
  return VeloMonitorBase::finalize(); // must be called after all other actions
}

//=============================================================================
// Retrieve the VeloClusters
//=============================================================================
StatusCode Velo::VeloClusterMonitor::veloClusters() {
  
  if ( m_debugLevel )
    debug() << "Retrieving VeloClusters from " << m_clusterCont << endmsg;
  
  if ( !exist<LHCb::VeloClusters>( m_clusterCont ) ) {
    debug() << "No VeloClusters container found for this event !" << endmsg;
   return StatusCode::FAILURE;
  }
  else {
    m_clusters = get<LHCb::VeloClusters>( m_clusterCont );
    if ( m_debugLevel ) debug() << "  -> number of clusters found in TES: "
				<< m_clusters->size() <<endmsg;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Monitor the VeloClusters
//=============================================================================
void Velo::VeloClusterMonitor::monitorClusters() {

  if ( m_perSensorPlots ) {
    memset(&m_nClustersPerSensor[0],0,m_nClustersPerSensor.size()*sizeof(unsigned int));
  }

  // Retrieve the VeloClusters
  // -------------------------
  StatusCode sc = veloClusters();
  if( sc.isFailure() ) return;

  // Number of clusters per event
  // ----------------------------
  unsigned int nclus = m_clusters -> size();
  counter( "# VeloClusters" ) += nclus;
  if ( nclus > 0 )
    plot1D( nclus, "# VELO clusters", "Number of VeloClusters per event",
	    -0.5, 20000.5, 2000 );
  
  // Loop over the VeloClusters
  LHCb::VeloClusters::const_iterator itVC;
  for ( itVC = m_clusters -> begin(); itVC != m_clusters -> end(); ++itVC ) {
    
    LHCb::VeloCluster* cluster = (*itVC);
    
    // Number of strips and total charge
    // -----------------------------------
    unsigned int nstrips = cluster -> size();    
    double adc           = cluster -> totalCharge();
    
    plot1D( nstrips, "Cluster size", "Number of strips per cluster",
	    -0.5, 5.5, 6 );
    plot1D( adc, "Cluster ADC value", "ADC value per cluster",
	    -0.5, 128*4+0.5, 128*4+1 );
    
    if( cluster -> isRType() )
      plot1D( adc, "Cluster ADC value (R)", "ADC value per cluster (R)",
	      -0.5, 128*4+0.5, 128*4+1 );
    
    if( cluster -> isPhiType() )
      plot1D( adc, "Cluster ADC value (Phi)", "ADC value per cluster (Phi)",
	      -0.5, 128*4+0.5, 128*4+1 );
    
    // Number of strips and total charge versus the sensor number
    // ----------------------------------------------------------
    unsigned int sensorNumber = cluster -> channelID().sensor();
    
    ++m_nClustersPerSensor[sensorNumber];

    plot2D( sensorNumber, nstrips, "Cluster size vs sensor",
	    "Number of strips per cluster versus sensor",
	    -0.5, 131.5, -0.5, 5.5, 132, 6 );
    plot2D( sensorNumber, adc, "Cluster ADC values vs sensor",
	    "Cluster ADC values versus sensor",
	    -0.5, 131.5, 0, 515, 132, 103 );
    

    // Active chip links versus sensor number
    // --------------------------------------
    const DeVeloSensor* veloSensor = m_veloDet -> sensor( sensorNumber );
    
    unsigned int stripNumber   = cluster -> channelID().strip();
    unsigned int chipChannel   = veloSensor -> StripToChipChannel( stripNumber ); // 0 -> 2047
    unsigned int chip          = veloSensor -> ChipFromChipChannel( chipChannel ); // 0 -> 15
    unsigned int channelInChip = chipChannel % 128; // 0 -> 127
    unsigned int linkInChip    = static_cast<int>( channelInChip / 32 ); // 0 -> 3
    // number 6 introduced to have 2 bins separating every 4 links per chip
    unsigned int activeLink = chip*6 + linkInChip;
    plot2D( sensorNumber, activeLink, "Active chip links vs sensor",
	    "Active chip links versus sensor",
	    -0.5, 131.5, -0.5, 95.5, 132, 96 );
    
    // Produce occupancy plots
    // -----------------------
    if ( m_occupancyPlots ) {
      TH1D* occHist = 0;
      if ( m_occupancyHistPerSensor.end() == m_occupancyHistPerSensor.find(sensorNumber) ) {
        boost::format fmtName ( "OccPerStripSens%d" ) ;
        fmtName % sensorNumber;
        boost::format fmtTitle ( "Occupancy, Sensor %d, " ) ;
        fmtTitle % sensorNumber;

        occHist = m_occupancyHistPerSensor[sensorNumber] 
          = Gaudi::Utils::Aida2ROOT::aida2root(book1D(fmtName.str(), fmtTitle.str()+m_tae, -0.5, 2047.5, 2048)); 
      } else {
        occHist = m_occupancyHistPerSensor[sensorNumber];
      }
      double occ = occHist->GetBinContent(stripNumber+1)+(1.0/m_occupancyDenom);
      occHist->SetBinContent(stripNumber+1,occ);
      m_histOccSpectAll->Fill(occ*100);
      if ( occ <= 0.2 ) {
        m_histOccSpectLow->Fill(occ*100);
      }
    }    

    // Produce the R correlation plots
    // -------------------------------
    if ( m_rSensorNumbers.end()
	 != std::find( m_rSensorNumbers.begin(), m_rSensorNumbers.end(), sensorNumber )
	 && veloSensor -> isR() ) {
      
      const DeVeloRType* rSensor1 = dynamic_cast<const DeVeloRType*>( veloSensor );
      double localR1 = rSensor1 -> rOfStrip( stripNumber );
      rDifferences( sensorNumber, localR1, sensorNumber+2 );
      
      if ( sensorNumber == 24 ) rCorrelations( sensorNumber, localR1 );
    }
  }

  // plot number of clusters seperately for each sensor
  if ( m_perSensorPlots ) {
    for (unsigned int s=0; s<m_nClustersPerSensor.size(); ++s) {
      if ( 0 == m_nClustersPerSensor[s] ) continue;

      boost::format fmtEvt ( "# clusters sensor %d" ) ;
      fmtEvt % s ;
      const std::string hName = fmtEvt.str() ;
      plot1D( m_nClustersPerSensor[s], hName, hName, -0.5, 200.5, 201 );

    }
  }
      
  if ( m_occupancyPlots ) {
    for ( std::map< unsigned int,  TH1D* >::iterator hIt = m_occupancyHistPerSensor.begin();
        hIt != m_occupancyHistPerSensor.end(); 
        ++hIt ) {
      m_histAvrgSensor->SetBinContent(hIt->first + 1, hIt->second->Integral()/hIt->second->GetNbinsX());
    }
  }
  
}

//=============================================================================
//  R_i -R_j difference distributions
//=============================================================================
void Velo::VeloClusterMonitor::rDifferences( unsigned int sensorNumber1,
					     double localR1,
					     unsigned int sensorNumber2 ) {
  
  LHCb::VeloClusters::const_iterator itVC;
  
  std::string sn  = boost::lexical_cast<std::string>( sensorNumber1 )
                    + "-"
                    + boost::lexical_cast<std::string>( sensorNumber2 );
  
  for ( itVC = m_clusters -> begin(); itVC != m_clusters -> end(); ++itVC ) {
    
    unsigned int sensorNumber = (*itVC) -> channelID().sensor();
    const DeVeloSensor* veloSensor = m_veloDet -> sensor( sensorNumber );
    if ( ! veloSensor -> isR() ) continue;  // only consider R sensors!

    if ( sensorNumber==sensorNumber2 ) {
      const DeVeloRType* rSensor = dynamic_cast<const DeVeloRType*>( veloSensor );
      unsigned int stripNumber = (*itVC) -> channelID().strip(); 
      double localR2           = rSensor -> rOfStrip( stripNumber );      

      plot1D( localR1-localR2, "Sensors R diff. (" + sn + ")",
          "Sensors R difference (sensors " + sn + ")", -10.5, 10.5, 41 );
    }
  }
}

//=============================================================================
//  R_i -R_j difference distributions
//=============================================================================
void Velo::VeloClusterMonitor::rCorrelations( unsigned int sensorNumber1,
					      double localR1 ) {
  
  // check whether this all makes sense ;-)!
  const DeVeloSensor* veloSensor2 = m_veloDet -> sensor( sensorNumber1+2 );
  const DeVeloSensor* veloSensor4 = m_veloDet -> sensor( sensorNumber1+4 );
  if ( ! veloSensor2 -> isR() ) return;  // only consider R sensors!
  if ( ! veloSensor4 -> isR() ) return;  // only consider R sensors!
  
  LHCb::VeloClusters::const_iterator itVC;

  std::vector<LHCb::VeloCluster*> clusters2;
  std::vector<LHCb::VeloCluster*> clusters4;

  for ( itVC = m_clusters -> begin(); itVC != m_clusters -> end(); ++itVC ) {

    unsigned int sensorNumber = (*itVC) -> channelID().sensor();

    if      ( sensorNumber == sensorNumber1+2 ) clusters2.push_back( (*itVC) );
    else if ( sensorNumber == sensorNumber1+4 ) clusters4.push_back( (*itVC) );

  }

  std::string sn = boost::lexical_cast<std::string>( sensorNumber1 );

  std::vector<LHCb::VeloCluster*>::const_iterator it2, it4;

  for ( it2 = clusters2.begin(); it2 != clusters2.end(); ++it2 ) {

    for ( it4 = clusters4.begin(); it4 != clusters4.end(); ++it4 ) {

      const DeVeloSensor* veloSensor2 = m_veloDet -> sensor( sensorNumber1 + 2 );
      const DeVeloSensor* veloSensor4 = m_veloDet -> sensor( sensorNumber1 + 4 );
      const DeVeloRType* rSensor2 = dynamic_cast<const DeVeloRType*>( veloSensor2 );
      const DeVeloRType* rSensor4 = dynamic_cast<const DeVeloRType*>( veloSensor4 );
      unsigned int stripNumber2 = (*it2) -> channelID().strip();
      unsigned int stripNumber4 = (*it4) -> channelID().strip();
      double localR2 = rSensor2 -> rOfStrip( stripNumber2 );
      double localR4 = rSensor4 -> rOfStrip( stripNumber4 );

      plot2D( localR1-localR2, localR2-localR4,
          "R-Sensors R_s(i+2)-R_s(i+4) vs R_s(i)-R_s(i+2), i=" + sn,
          "R-Sensors R_s(i+2)-R_s(i+4) versus R_s(i)-R_s(i+2), i=" + sn,
          -10.5, 10.5, -10.5, 10.5, 41, 41 );

    }

  }

}

//=============================================================================
