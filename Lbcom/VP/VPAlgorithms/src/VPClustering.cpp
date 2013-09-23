// Gaudi
#include "GaudiKernel/AlgFactory.h"
// LHCb
// Kernel/LHCbKernel
#include "Kernel/SiDataFunctor.h"
// Local
#include "VPClustering.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : VPClustering
//
// 2013-08-13 : Daniel Hynds
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY(VPClustering)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VPClustering::VPClustering(const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiAlgorithm (name , pSvcLocator) {

  declareProperty("DigitContainer",       m_digitLocation       = LHCb::VPDigitLocation::VPDigitLocation);
  declareProperty("LiteClusterContainer", m_liteClusterLocation = LHCb::VPLiteClusterLocation::Default);
  declareProperty("VPClusterContainer",   m_clusterLocation     = LHCb::VPClusterLocation::VPClusterLocation);

}

//=============================================================================
// Destructor
//=============================================================================
VPClustering::~VPClustering() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode VPClustering::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); 
  if (sc.isFailure()) return sc;
  m_debug = msgLevel(MSG::DEBUG);
  if (m_debug) debug() << "==> Initialize" << endmsg;
  // Get detector element
  m_vpDet = getDet<DeVP>(DeVPLocation::Default);
  return StatusCode::SUCCESS;

}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VPClustering::execute() {

  if (m_debug) debug() << "==> Execute" << endmsg;

  // Check if LiteClusters already decoded. 
  if (exist<LHCb::VPLiteCluster::VPLiteClusters>(LHCb::VPLiteClusterLocation::Default)) {
    // If lite clusters are decoded there is no need to build clusters.
    if (m_debug) {
      debug() << "Lite clusters already exist. No clustering taking place" << endmsg;
    }
    return StatusCode::SUCCESS;
  }  
  // If no lite clusters, then cluster the digits.
  return clusterDigits();

}

//=============================================================================
// Loop over track container
//=============================================================================
StatusCode VPClustering::clusterDigits (){

  if (m_debug) debug() << "Clustering digits" << endmsg;

  // Pick up pixel hits (digits) to cluster
  LHCb::VPDigits* raw_hits = getIfExists<LHCb::VPDigits>(m_digitLocation);
  if (!raw_hits) {
    error() << "No digits founds in this event" << endmsg;
    return StatusCode::FAILURE;
  }

  // Make space on the TES for the lite clusters and VP clusters
  VPLiteCluster::VPLiteClusters* liteClusters = new VPLiteCluster::VPLiteClusters();
  VPClusters* clusters = new VPClusters();

  // Make a temporary storage space for the pixels, and delete when used
  std::vector<LHCb::VPDigit*> digitCont;
  LHCb::VPDigits::const_iterator iHit;
  for (iHit = raw_hits->begin(); iHit != raw_hits->end(); ++iHit) {
    digitCont.push_back(*iHit);
  }
  // Sort pixels by channelID
  std::sort(digitCont.begin(), digitCont.end(), less_than_channelID()); 
  
  // Cluster while there are still pixels in the temporary storage
  unsigned int oldsize = 0;
  while (digitCont.size() > 0) {
    // Create a new cluster.
    std::vector<LHCb::VPDigit*> cluster;
    std::vector<std::pair<LHCb::VPChannelID,int> > totVec;     
    // Add the first hit in the container. 
    cluster.push_back(digitCont[0]);
    totVec.push_back(std::make_pair(digitCont[0]->channelID(),digitCont[0]->ToTValue()));
    // Remove hit from container.
    digitCont.erase(digitCont.begin());
 
    // Add hits to cluster until it no longer changes size
    do {
      oldsize = cluster.size();
      for (unsigned int i = 0; i < cluster.size(); i++) {
        if (digitCont.empty()) break; // No more hits in temp. storage
        
        if (cluster[i]->channelID().module() != digitCont[0]->channelID().module()) break;  // Next hit not on same module
        if (sensorNumber(cluster[i]) != sensorNumber(digitCont[0]) ) break; // Next hit not on same sensor
        // Loop down the stored hits until new pixels cannot be part of cluster       
        for (unsigned int hit = 0; hit < digitCont.size(); hit++) { 
          if (!isEdge(cluster[i])) {
            // Not edge
            if (cluster[i]->channelID().chip() != digitCont[hit]->channelID().chip()) break;  // Next hit not on same chip
            if (abs(cluster[i]->channelID().row() - digitCont[hit]->channelID().row()) > 1) continue; // Not neighbouring in y
            if (abs(cluster[i]->channelID().col() - digitCont[hit]->channelID().col()) > 1) break; // Too far away to be added
            addToCluster(cluster, totVec, digitCont, hit); 
            continue;
          } else {
            // Deal with edge pixels
            if (cluster[i]->channelID().chip() == digitCont[hit]->channelID().chip()) {
              // On the same chip
              if (abs(cluster[i]->channelID().row() - digitCont[hit]->channelID().row()) > 1) continue; // Not neighbouring in y
              if (abs(cluster[i]->channelID().col() - digitCont[hit]->channelID().col()) > 1) break; // Too far away to be added
              addToCluster(cluster, totVec, digitCont, hit); 
              continue;
            } else {
              // Not on the same chip
              if (!isEdge(digitCont[hit])) break; // No hits on neighbouring edge
              if (abs(cluster[i]->channelID().row() - digitCont[hit]->channelID().row()) > 1) continue; // Not neighbouring in y
              addToCluster(cluster, totVec, digitCont, hit); 
              continue;
            }
          } // End of edge pixel check
        }
      }
    } while (cluster.size() != oldsize); // no more hits to be added to cluster

    unsigned int module = cluster[0]->channelID().module();
    
    double x = 0; //temp
    double y = 0; //temp
    double z = 0; //temp
    const DeVPSensor* vp_sensor = m_vpDet->sensorOfChannel(cluster[0]->channelID()); //temp
    int clustToT = 0;
    double clustRow = 0.;
    double clustCol = 0.;    
    for (unsigned int it = 0;it < cluster.size(); it++) { 
      const double tot = cluster[it]->ToTValue();
      clustToT += tot;
      clustRow += tot * (cluster[it]->channelID().col() + 0.5);
      clustCol += tot * (cluster[it]->channelID().row() + 0.5);
      Gaudi::XYZPoint pixel = vp_sensor->channelToPoint(cluster[it]->channelID()); //temp
      x += (pixel.x() * tot); //temp
      y += (pixel.y() * tot); //temp
      z += (pixel.z() * tot); //temp
    }
    clustRow /= clustToT;
    clustCol /= clustToT;
    
    x /= clustToT; //temp
    y /= clustToT; //temp
    z /= clustToT; //temp
    Gaudi::XYZPoint cluster_point(x, y, z); //temp
    LHCb::VPChannelID temp_id; //temp
    std::pair<double, double> temp_frac; //temp
    StatusCode mycode = vp_sensor->pointToChannel(cluster_point, temp_id, temp_frac); //temp
    if (mycode == StatusCode::FAILURE) continue;
    // Make the barycentre channel
    LHCb::VPChannelID baryCenterChID(module, cluster[0]->channelID().chip(), floor(clustRow), floor(clustCol));
        
    // Get the interpixel fraction
    std::pair<unsigned int, unsigned int> xyFrac;
    xyFrac.first = int(ceil((clustRow-floor(clustRow)) * 7.));
    xyFrac.second = int(ceil((clustCol-floor(clustCol)) * 7.));
    if (xyFrac.first > 7.) xyFrac.first = int(7.);
    if (xyFrac.second > 7.) xyFrac.second = int(7.);
    
    // const VPLiteCluster newLiteCluster(baryCenterChID,1,xyFrac,isLong);
    // LHCb::VPCluster* newCluster = new LHCb::VPCluster(newLiteCluster,totVec);
    // clusters->insert(newCluster, baryCenterChID);
    // liteClusters->push_back(newLiteCluster); 
    if (m_debug) {
      debug() << "New cluster with temp frac: " << temp_frac.first << "," << temp_frac.second 
              << " and cluster id: " << temp_id << endmsg;
    }
    bool isLong = false; //temp
    const VPLiteCluster newLiteCluster(temp_id, 1, temp_frac, isLong); //temp
    LHCb::VPCluster* newCluster = new LHCb::VPCluster(newLiteCluster, totVec); //temp
    clusters->insert(newCluster, temp_id); //temp
    liteClusters->push_back(newLiteCluster); //temp
  }
  // Sort lite clusters.
  std::sort(liteClusters->begin(), liteClusters->end(), 
            SiDataFunctor::Less_by_Channel<LHCb::VPLiteCluster>()); 
  // Store clusters and lite clusters on TS.
  put(liteClusters, m_liteClusterLocation);
  put(clusters, m_clusterLocation);
  return StatusCode::SUCCESS;

}

//=============================================================================
//  Get sensor number for a given digit
//=============================================================================
int VPClustering::sensorNumber(LHCb::VPDigit* digit) {
  return floor(digit->channelID().chip() / 3);
}


//=============================================================================
//  Check if pixel is on the edge of a chip
//=============================================================================
bool VPClustering::isEdge(LHCb::VPDigit* digit) {
  
  const int ladder_position = digit->channelID().chip() % 3;
  if ((ladder_position == 0 && digit->channelID().col() == 255) ||
      (ladder_position == 1 && (digit->channelID().col() == 255 || digit->channelID().col() == 0)) ||
      (ladder_position == 2 && digit->channelID().col() == 0)) {
    return true;  
  }
  return false;
  
}

//=============================================================================
//  Add pixel to cluster
//=============================================================================
void VPClustering::addToCluster(std::vector<LHCb::VPDigit*>& cluster, 
                                std::vector<std::pair<LHCb::VPChannelID, int> >& totVec,
                                std::vector<LHCb::VPDigit*>& digitCont, 
                                unsigned int hit) {
  // Add hit to cluster
  totVec.push_back(std::make_pair(digitCont[hit]->channelID(), digitCont[hit]->ToTValue()));
  cluster.push_back(digitCont[hit]);                 
  // Remove hit from container
  digitCont.erase(digitCont.begin() + hit);            
}

