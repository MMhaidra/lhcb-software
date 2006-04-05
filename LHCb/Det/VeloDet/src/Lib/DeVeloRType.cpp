// $Id: DeVeloRType.cpp,v 1.21 2006-04-05 09:06:07 mtobin Exp $
//==============================================================================
#define VELODET_DEVELORTYPE_CPP 1
//==============================================================================
// Include files 

// from Kernel
#include "Kernel/SystemOfUnits.h"

// From Gaudi
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/IJobOptionsSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/MsgStream.h"

// From LHCb
#include "Kernel/LHCbMath.h"

// From Velo
#include "VeloDet/DeVeloRType.h"

/** @file DeVeloRType.cpp
 *
 *  Implementation of class : DeVeloRType
 *
 *  @author Mark Tobin  Mark.Tobin@cern.ch
 *  @author Kurt Rinnert kurt.rinnert@cern.ch
 */

//==============================================================================
/// Standard constructor
//==============================================================================
DeVeloRType::DeVeloRType(const std::string& name) : DeVeloSensor(name)
{
}
//==============================================================================
/// Destructor
//==============================================================================
DeVeloRType::~DeVeloRType()
{
}
//==============================================================================
/// Object identification
//==============================================================================
const CLID& DeVeloRType::clID() 
  const { return DeVeloRType::classID(); }
//==============================================================================
/// Initialisation method
//==============================================================================
StatusCode DeVeloRType::initialize() 
{
  // Trick from old DeVelo to set the output level
  PropertyMgr* pmgr = new PropertyMgr();
  int outputLevel=0;
  pmgr->declareProperty("OutputLevel", outputLevel);
  IJobOptionsSvc* jobSvc;
  ISvcLocator* svcLoc = Gaudi::svcLocator();
  StatusCode sc = svcLoc->service("JobOptionsSvc", jobSvc);
  jobSvc->setMyProperties("DeVeloRType", pmgr);
  if ( 0 < outputLevel ) {
    msgSvc()->setOutputLevel("DeVeloRType", outputLevel);
  }
  delete pmgr;
  MsgStream msg(msgSvc(), "DeVeloRType");
  
  sc = DeVeloSensor::initialize();
  if(!sc.isSuccess()) {
    msg << MSG::ERROR << "Failed to initialise DeVeloSensor" << endreq;
    return sc;
  }

  m_numberOfZones = 4;
  m_stripsInZone = numberOfStrips() / m_numberOfZones;

  m_innerPitch = param<double>("InnerPitch");
  m_outerPitch = param<double>("OuterPitch");

  m_overlapInX = param<double>("ROverlapInX");
  
  // the resolution of the sensor
  m_resolution.first = param<double>("RResGrad");
  m_resolution.second = param<double>("RResConst");
  
  /// Calculate the strip radii/phi limits 
  calcStripLimits();

  /// Build up map of strips to routing lines
  BuildRoutingLineMap();
  
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Calculate the nearest channel to a 3-d point.
//==============================================================================
StatusCode DeVeloRType::pointToChannel(const Gaudi::XYZPoint& point,
                                       LHCb::VeloChannelID& channel,
                                       double& fraction,
                                       double& pitch) const
{
  MsgStream msg(msgSvc(), "DeVeloRType");
  Gaudi::XYZPoint localPoint(0,0,0);
  StatusCode sc = globalToLocal(point,localPoint);

  if(!sc.isSuccess()) return sc;

  // Check boundaries...
  sc = isInActiveArea(localPoint);
  if(!sc.isSuccess()) return sc;

  // work out closet channel....
  unsigned int closestStrip=0;
  double logarithm, strip;
  double radius=localPoint.Rho();
  logarithm = (m_pitchSlope*(radius - m_innerR)+m_innerPitch) / 
    m_innerPitch;
  strip = log(logarithm)/m_pitchSlope;
  closestStrip = LHCbMath::round(strip);
  fraction = strip - closestStrip; 
 
  // Which zone is the strip in?
  double phi=localPoint.phi();
  unsigned int zone=zoneOfPhi(phi);
  
  // minimum strip in zone
  closestStrip += firstStrip(zone);
  
  unsigned int sensor=sensorNumber();
  // set VeloChannelID....
  channel.setSensor(sensor);
  channel.setStrip(closestStrip);
  if(isR()) {
    channel.setType(LHCb::VeloChannelID::RType);
  } else if( isPileUp() ) {
    channel.setType(LHCb::VeloChannelID::PileUpType);
  }
  // calculate pitch....
  pitch = rPitch(channel.strip());
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Checks if local point is inside sensor
//==============================================================================
StatusCode DeVeloRType::isInActiveArea(const Gaudi::XYZPoint& point) const
{
  MsgStream msg(msgSvc(), "DeVeloRType");
  // check boundaries....  
  double radius=point.Rho();
  if(innerRadius() >= radius || outerRadius() <= radius) {
    msg << MSG::VERBOSE << "Outside active radii " << radius << endreq;
    return StatusCode::FAILURE;
  }
  // Dead region from bias line
  double y=point.y();
  if (m_phiGap > y && -m_phiGap < y) {
    msg << MSG::VERBOSE << "Inside dead region from bias line " << y << endreq;
    return StatusCode::FAILURE;
  }
  // corner cut-offs
  bool cutOff=isCutOff(point.x(),point.y());
  if(cutOff) {
    msg << MSG::VERBOSE << "cut off: x,y " << point.x() << "," << point.y()
        << endreq;    
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Is the point in the corner cut-off?
//==============================================================================
bool DeVeloRType::isCutOff(double x, double y) const
{
  if(m_cornerX1 > x) return true;
  y = fabs(y);
  if(m_cornerX1 <= x && m_cornerX2 >= x) {
    double yMax=m_corners[0]*x+m_corners[1];
    if(yMax > y) {
      return true;
    }
  }
  return false;
}
//==============================================================================
/// Get the nth nearest neighbour within a sector for a given channel
//==============================================================================
StatusCode DeVeloRType::neighbour(const LHCb::VeloChannelID& start, 
                                  const int& nOffset, 
                                  LHCb::VeloChannelID& channel) const
{
  unsigned int strip=0;
  strip = start.strip();
  unsigned int startZone;
  startZone = zoneOfStrip(strip);
  strip += nOffset;
  unsigned int endZone;
  endZone = zoneOfStrip(strip);
  // Check boundaries
  if(numberOfStrips() < strip) return StatusCode::FAILURE;
  if(startZone != endZone) {
    return StatusCode::FAILURE;
  }
  channel = start;
  channel.setStrip(strip);
  return StatusCode::SUCCESS;
}

//=============================================================================
/// Residual of 3-d point to a VeloChannelID
//=============================================================================
StatusCode DeVeloRType::residual(const Gaudi::XYZPoint& point, 
                                 const LHCb::VeloChannelID& channel,
                                 double &residual,
                                 double &chi2) const
{
  MsgStream msg(msgSvc(), "DeVeloRType");
  Gaudi::XYZPoint localPoint(0,0,0);
  StatusCode sc = DeVeloSensor::globalToLocal(point,localPoint);
  
  if(!sc.isSuccess()) return sc;
  
  // Check boundaries...
  sc = isInActiveArea(localPoint);
  if(!sc.isSuccess()) return sc;

  unsigned int strip=channel.strip();

  double rPoint = localPoint.Rho();
  double rStrip = rOfStrip(strip);
  residual = rStrip - rPoint;

  double sigma=m_resolution.first*rPitch(strip) - m_resolution.second;
  chi2 = pow(residual/sigma,2);

  msg << MSG::VERBOSE << "Residual; rPoint = " << rPoint << " strip " << strip 
      << " rStrip = " << rStrip << " residual " << residual
      << " sigma = " << sigma
      << " chi2 = " << chi2 << endreq;
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Residual [see DeVelo for explanation]
//==============================================================================
StatusCode DeVeloRType::residual(const Gaudi::XYZPoint& /*point*/,
                                   const LHCb::VeloChannelID& /*channel*/,
                                   const double /*localOffset*/,
                                   const double /*width*/,
                                   double &/*residual*/,
                                   double &/*chi2*/) const
{

  // Perpendicular distance to strip.....

  return StatusCode::SUCCESS;
}
//==============================================================================
/// Store the local radius and phi limits for each strip in the sensor
//==============================================================================
void DeVeloRType::calcStripLimits()
{
  MsgStream msg( msgSvc(), "DeVeloRType" );
  msg << MSG::VERBOSE << "calcStripLimits" << endreq;
  m_innerR = innerRadius() + m_innerPitch / 2;
  m_outerR = outerRadius() - m_outerPitch / 2;

  m_pitchSlope = (m_outerPitch - m_innerPitch) / 
    (m_outerR - m_innerR);

  /// Dead region from bias line
  m_phiGap = param<double>("RPhiGap") / 2;

  /// Corner cut offs
  cornerLimits();
  
  /// Angular limits for zones...
  phiZoneLimits();

  m_rStrips.clear();
  m_rPitch.clear();
  m_stripLimits.clear();
  double radius,pitch;
  for(unsigned int zone=0; zone<m_numberOfZones; zone++) {
    for(unsigned int istrip=0; istrip<m_stripsInZone; istrip++){
      radius = (exp(m_pitchSlope*istrip)*m_innerPitch - 
                (m_innerPitch-m_pitchSlope*m_innerR)) /
      m_pitchSlope;
      m_rStrips.push_back(radius);
      pitch = exp(m_pitchSlope*istrip)*m_innerPitch;
      m_rPitch.push_back(pitch);
      double phiMin=0;
      double phiMax=0;
      double phiLimit=0;
      double x=0,y=0;
      phiMin=phiMinZone(zone,radius);
      phiMax=phiMaxZone(zone,radius);
      if(0 == zone){
        phiLimit = phiMin;
        x = radius*cos(phiMin);
        y = radius*sin(phiMin);
      } else if(3 == zone){
        phiLimit = phiMax;
        x = radius*cos(phiMax);
        y = radius*sin(phiMax);
      }
      // Work out point where strip crosses cut off
      if(zone == 0 || zone == 3){
        if(isCutOff(x,y)){
          double a = pow(m_corners[0],2)+1;
          double b = 2*m_corners[0]*m_corners[1];
          double c = pow(m_corners[1],2) - pow(radius,2);
          double x1 = (-b + sqrt(b*b - (4*a*c))) / (2*a);
          double x2 = (-b - sqrt(b*b - (4*a*c))) / (2*a);
          if(m_cornerX1 <= x1 && m_cornerX2 >= x1) {
            phiLimit = -acos(x1/radius);
          } else if(m_cornerX1 <= x2 && m_cornerX2 >= x2) {
            phiLimit = -acos(x2/radius);
          }
          if(zone == 0){
            phiMin = phiLimit;
          } else if(zone == 3){
            if(phiMax != phiLimit) phiMax = -phiLimit;
          }
        }
      }
      m_stripLimits.push_back(std::pair<double,double>(phiMin,phiMax));
    }
  }
  for(unsigned int i=0; i < m_phiMin.size(); i++){
    msg << MSG::DEBUG << "Zone limits; zone " << i << " min " << m_phiMin[i]
        << " max " << m_phiMax[i] << " phiMin " 
        << phiMinZone(i,innerRadius()) 
        << " max " << phiMaxZone(i,innerRadius()) << endmsg;
  }
  msg << MSG::DEBUG << "Radius of first strip is " << m_rStrips[0] 
      << " last strip " << m_rStrips[m_rStrips.size()-1] << endmsg;
  msg << MSG::DEBUG << "Pitch; inner " << m_rPitch[0] << " outer " 
      << m_rPitch[m_rPitch.size()-1] << " Radius; inner " << m_innerR
      << " outer " << m_outerR 
      << " slope " << m_pitchSlope << endmsg;
}
//==============================================================================
/// Store line defining corner cut-offs
//==============================================================================
void DeVeloRType::cornerLimits()
{
  m_cornerX1 = param<double>("RCornerX1");
  m_cornerY1 = param<double>("RCornerY1");
  m_cornerX2 = param<double>("RCornerX2");
  m_cornerY2 = param<double>("RCornerY2");
  
  m_corners.clear();
  double gradient;
  gradient = (m_cornerY2 - m_cornerY1) /  (m_cornerX2 - m_cornerX1);
  m_corners.push_back(gradient);
  double intercept;
  intercept = m_cornerY2 - (gradient*m_cornerX2);
  m_corners.push_back(intercept);
}
//==============================================================================
/// Store the angular limits of zones sensor at +ve x
//==============================================================================
void DeVeloRType::phiZoneLimits()
{
  m_halfAngle     = 90.0 * degree;
  m_quarterAngle  = .5 * m_halfAngle;

  double phi;
  phi = acos(m_overlapInX/outerRadius());

  m_phiMin.clear();
  m_phiMax.clear();

  m_phiMin.push_back(-phi);
  m_phiMin.push_back(-m_quarterAngle);
  m_phiMin.push_back(0);
  m_phiMin.push_back(m_quarterAngle);
  
  m_phiMax.push_back(-m_quarterAngle);
  m_phiMax.push_back(0);
  m_phiMax.push_back(m_quarterAngle);
  m_phiMax.push_back(phi);
}
//==============================================================================
/// Build up routing line map
//==============================================================================
void DeVeloRType::BuildRoutingLineMap(){
  MsgStream msg( msgSvc(), "DeVeloRType" );
  /*  msg << MSG::DEBUG << "Building routing line map for sensor " 
      << (this->sensorNumber()) << endreq;*/
  for(unsigned int routLine=m_minRoutingLine;routLine<=m_maxRoutingLine/2;routLine++){
    unsigned int routArea=RoutingLineArea(routLine);
    unsigned int strip=RoutLineToStrip(routLine,routArea);
    // Sector 1
    m_mapStripToRoutingLine[strip]=routLine;
    m_mapRoutingLineToStrip[routLine]=strip;
    // sector 3
    m_mapStripToRoutingLine[strip+1024]=routLine+1024;
    m_mapRoutingLineToStrip[routLine+1024]=strip+1024;
    msg << MSG::VERBOSE << "Routing line " << routLine 
        << " area " << routArea
        << " strip " << m_mapRoutingLineToStrip[routLine]
        << " +1024 line " << routLine+1024
        << " +1024 strip " << m_mapRoutingLineToStrip[routLine+1024]
        << endreq;
    msg << MSG::VERBOSE << "Routing line " << routLine 
        << " strip " << RoutingLineToStrip(routLine)
        << " chip channel " << RoutingLineToChipChannel(routLine)
        << " and back " << ChipChannelToRoutingLine(RoutingLineToChipChannel(routLine))
        << " from strip " << endreq;
  }
}
//=============================================================================
// Select routing line area (pattern repeats for chips 7 to 0)
// (0) Chips 15 to 13 
// (1) Chip 12
// (2) Chip 11 
// (3) Chips 10 to 8
//=============================================================================
unsigned int DeVeloRType::RoutingLineArea(unsigned int routingLine){
  if(m_maxRoutingLine/2 < routingLine) routingLine /= 2;
  if(routingLine <= m_nChan0){
    return 0;
  } else if(routingLine <= m_nChan0+m_nChan1){
    return 1;
  } else if(routingLine <= m_nChan0+m_nChan1+m_nChan2){
    return 2;
  } else if (routingLine <= m_nChan0+m_nChan1+m_nChan2+m_nChan3){
    return 3;
  }
  return 999;
}
//=============================================================================
// Return strip from routing line area (first 1024 strips only)
//=============================================================================
unsigned int DeVeloRType::RoutLineToStrip(unsigned int routLine, unsigned int routArea){
  unsigned int strip;
  //  std::cout << "routLine " << routLine << " area " << routArea << std::endl;
  if(0 == routArea){
    strip = (m_nChan1+routLine-1);
  } else if(1 == routArea) {
    strip = (routLine-m_nChan0-1);
    //    strip = (m_maxRoutingLine-2*m_nChan0-2*m_nChan3-routLine); // NEW!
  } else if(2 == routArea){
    strip = (m_maxRoutingLine-m_nChan0-m_nChan1-m_nChan3-routLine);
    //    strip = routLine-1; // NEW!
  } else if(3 == routArea){
    strip = (m_maxRoutingLine-m_nChan3-routLine);
  } else strip=9999;
  //  std::cout << "strip " << strip << " scram " << ScrambleStrip(strip) 
  //            << " rl " << routLine << " scram " << ScrambleStrip(routLine)
  //        << std::endl;
  return ScrambleStrip(strip);
}
