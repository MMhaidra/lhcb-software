// $Id: DeVeloRType.cpp,v 1.2 2004-02-03 16:55:57 mtobin Exp $
//==============================================================================
#define VELODET_DEVELORTYPE_CPP 1
//==============================================================================
// Include files 
#include "VeloDet/DeVeloRType.h"

/** @file DeVeloRType.cpp
 *
 *  Implementation of class : DeVeloRType
 *
 *  @author Mark Tobin  Mark.Tobin@cern.ch
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
  MsgStream msg(msgSvc(), "DeVeloRType");
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
  
  sc = DeVeloSensor::initialize();
  if(!sc.isSuccess()) {
    msg << MSG::ERROR << "Failed to initialise DeVeloSensor" << endreq;
    return sc;
  }

  m_numberOfZones = 4;
  m_numberOfStrips = this->numberOfStrips();
  m_stripsInZone = m_numberOfStrips / m_numberOfZones;

  m_innerPitch = this->userParameterAsDouble("InnerPitch");
  m_outerPitch = this->userParameterAsDouble("OuterPitch");

  m_overlapInX = this->userParameterAsDouble("ROverlapInX");
  
  // the resolution of the sensor
  m_resolution.first = this->userParameterAsDouble("RResGrad");
  m_resolution.second = this->userParameterAsDouble("RResConst");
  
  /// Calculate the strip radii/phi limits 
  calcStripLimits();

  return StatusCode::SUCCESS;
}
//==============================================================================
/// Calculate the nearest channel to a 3-d point.
//==============================================================================
StatusCode DeVeloRType::pointToChannel(const HepPoint3D& point,
                                       VeloChannelID& channel,
                                       double& fraction,
                                       double& pitch)
{
  MsgStream msg(msgSvc(), "DeVeloRType");
  HepPoint3D localPoint(0,0,0);
  StatusCode sc = DeVeloSensor::globalToLocal(point,localPoint);

  if(!sc.isSuccess()) return sc;

  // Check boundaries...
  sc = isInside(localPoint);
  if(!sc.isSuccess()) return sc;

  // work out closet channel....
  unsigned int closestStrip=0;
  double logarithm, strip;
  double radius=localPoint.perp();
  logarithm = (m_pitchSlope*(radius - m_innerRadius)+m_innerPitch) / 
    m_innerPitch;
  strip = log(logarithm)/m_pitchSlope;
  closestStrip = VeloRound::round(strip);
  fraction = strip - closestStrip; 
 
  // Which zone is the strip in?
  double phi=localPoint.phi();
  unsigned int zone=zoneOfPhi(phi);
  
  // minimum strip in zone
  closestStrip += firstStrip(zone);

  unsigned int sensor=this->sensorNumber();
  // set VeloChannelID....
  channel.setSensor(sensor);
  channel.setStrip(closestStrip);
  if(m_isR) {
    channel.setType(VeloChannelID::RType);
  } else if(m_isPileUp) {
    channel.setType(VeloChannelID::PileUpType);
  }
  // calculate pitch....
  pitch = rPitch(channel.strip());
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Checks if local point is inside sensor
//==============================================================================
StatusCode DeVeloRType::isInside(const HepPoint3D& point)
{
  // check boundaries....  
  double radius=point.perp();
  if(m_innerActiveArea > radius || m_outerActiveArea < radius) {
    return StatusCode::FAILURE;
  }
  // Dead region from bias line
  double y=point.y();
  if (m_phiGap > y && -m_phiGap < y) {
    return StatusCode::FAILURE;
  }
  // corner cut-offs
  bool isCutOff=this->isCutOff(point.x(),point.y());
  if(isCutOff) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Is the point in the corner cut-off?
//==============================================================================
bool DeVeloRType::isCutOff(double x, double y)
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
StatusCode DeVeloRType::neighbour(const VeloChannelID& start, 
                                  const int& nOffset, 
                                  VeloChannelID& channel)
{
  unsigned int strip=0;
  strip = start.strip();
  unsigned int startZone;
  startZone = zoneOfStrip(strip);
  strip += nOffset;
  unsigned int endZone;
  endZone = zoneOfStrip(strip);
  // Check boundaries
  if(m_numberOfStrips < strip) return StatusCode::FAILURE;
  if(startZone != endZone) {
    return StatusCode::FAILURE;
  }
  channel = start;
  channel.setStrip(strip);
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Returns the number of channels between two channels
//==============================================================================
StatusCode DeVeloRType::channelDistance(const VeloChannelID& start,
                                        const VeloChannelID& end,
                                        int& nOffset)
{
  nOffset = 0;
  unsigned int startStrip=0;
  unsigned int endStrip=0;
  startStrip = start.strip();
  endStrip = end.strip();
  if(m_numberOfStrips<startStrip || m_numberOfStrips<endStrip) {
    return StatusCode::FAILURE;
  }

  unsigned int startZone;
  startZone = zoneOfStrip(startStrip);

  unsigned int endZone;
  endZone = zoneOfStrip(endStrip);
  // Check validity of zones
  if(startZone != endZone) {
    return StatusCode::FAILURE;
  }
  nOffset = endStrip-startStrip;
  return StatusCode::SUCCESS;
}
//==============================================================================
/// Residual of 3-d point to a VeloChannelID
//==============================================================================
StatusCode DeVeloRType::residual(const HepPoint3D& point, 
                                 const VeloChannelID& channel,
                                 double &residual,
                                 double &chi2)
{
  MsgStream msg(msgSvc(), "DeVeloRType");
  HepPoint3D localPoint(0,0,0);
  StatusCode sc = DeVeloSensor::globalToLocal(point,localPoint);
  
  if(!sc.isSuccess()) return sc;
  
  // Check boundaries...
  sc = isInside(localPoint);
  if(!sc.isSuccess()) return sc;

  unsigned int strip=channel.strip();

  double rPoint = localPoint.perp();
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
StatusCode DeVeloRType::residual(const HepPoint3D& /*point*/,
                                   const VeloChannelID& /*channel*/,
                                   const double /*localOffset*/,
                                   const double /*width*/,
                                   double &/*residual*/,
                                   double &/*chi2*/)
{

  // Perpendicular distance to strip.....

  return StatusCode::SUCCESS;
}
//==============================================================================
/// The number of zones in the detector
//==============================================================================
unsigned int DeVeloRType::numberOfZones()
{
  return m_numberOfZones;
}
//==============================================================================
/// The zone number for a given strip
//==============================================================================
unsigned int DeVeloRType::zoneOfStrip(const unsigned int strip)
{
  return static_cast<unsigned int>(strip/512);
}
//==============================================================================
/// Zone for a given local phi
//==============================================================================
unsigned int DeVeloRType::zoneOfPhi(double phi)
{
  unsigned int zone=0;
  if(m_phiMin[0] >= fabs(phi) && m_phiMax[0] < fabs(phi)) {
    zone = 0;
  } else if(m_phiMin[1] >= fabs(phi) && m_phiMax[1] < fabs(phi)) {
    zone = 1;
  }
  if(0 > phi) zone += 2;
  return zone;
}
//==============================================================================
/// Return the minumum strip for each zone
//==============================================================================
unsigned int DeVeloRType::firstStrip(unsigned int zone)
{
  return zone*512;
}

//==============================================================================
/// The number of strips in a zone
//==============================================================================
unsigned int DeVeloRType::stripsInZone(const unsigned int /*zone*/)
{
  return m_stripsInZone;
}
//==============================================================================
/// Store the local radius and phi limits for each strip in the sensor
//==============================================================================
void DeVeloRType::calcStripLimits()
{
  MsgStream msg( msgSvc(), "DeVeloRType" );
  msg << MSG::VERBOSE << "calcStripLimits" << endreq;
  m_innerActiveArea = this->innerRadius();
  m_outerActiveArea = this->outerRadius();
  m_innerRadius = m_innerActiveArea + m_innerPitch / 2;
  m_outerRadius = m_outerActiveArea - m_outerPitch / 2;

  m_pitchSlope = (m_outerPitch - m_innerPitch) / 
    (m_outerRadius - m_innerRadius);

  /// Dead region from bias line
  m_phiGap = this->userParameterAsDouble("RPhiGap") / 2;

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
                (m_innerPitch-m_pitchSlope*m_innerRadius)) /
      m_pitchSlope;
      m_rStrips.push_back(radius);
      pitch = exp(m_pitchSlope*istrip)*m_innerPitch;
      m_rPitch.push_back(pitch);
      double phiMin=0;
      double phiMax=0;
      double phiLimit=0;
      double x=0,y=0;
      if(0 == zone){
        phiLimit=phiMin = acos(m_overlapInX/radius);
        phiMax = phiMaxZone(zone);
        x = radius*cos(phiMin);
        y = radius*sin(phiMin);
      } else if(1 == zone){
        phiMin = phiMinZone(zone);
        phiMax = asin(m_phiGap/radius);
      } else if(2 == zone){
        phiMin = asin(-m_phiGap/radius);
        phiMax = phiMaxZone(zone);
      } else if(3 == zone){
        phiMin = phiMinZone(zone);
        phiLimit=phiMax = -acos(m_overlapInX/radius);
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
            phiLimit = acos(x1/radius);
          } else if(m_cornerX1 <= x2 && m_cornerX2 >= x2) {
            phiLimit = acos(x2/radius);
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
  msg << MSG::DEBUG << "Radius of first strip is " << m_rStrips[0] 
      << " last strip " << m_rStrips[m_rStrips.size()-1] << endmsg;
  msg << MSG::DEBUG << "Pitch; inner " << m_rPitch[0] << " outer " 
      << m_rPitch[m_rPitch.size()-1] << " Radius; inner " << m_innerRadius 
      << " outer " << m_outerRadius 
      << " slope " << m_pitchSlope << endmsg;
}
//==============================================================================
/// Store line defining corner cut-offs
//==============================================================================
void DeVeloRType::cornerLimits()
{
  m_cornerX1 = this->userParameterAsDouble("RCornerX1");
  m_cornerY1 = this->userParameterAsDouble("RCornerY1");
  m_cornerX2 = this->userParameterAsDouble("RCornerX2");
  m_cornerY2 = this->userParameterAsDouble("RCornerY2");
  
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
  phi = acos(m_overlapInX/m_outerRadius);

  m_phiMin.clear();
  m_phiMin.push_back(phi);
  m_phiMin.push_back(m_quarterAngle);
  m_phiMin.push_back(0);
  m_phiMin.push_back(-m_quarterAngle);
  
  m_phiMax.clear();
  m_phiMax.push_back(m_quarterAngle);
  m_phiMax.push_back(0);
  m_phiMax.push_back(-m_quarterAngle);
  m_phiMax.push_back(-phi);

}
//==============================================================================
/// Return the radius of the strip
//==============================================================================
double DeVeloRType::rOfStrip(const unsigned int strip)
{
  return m_rStrips[strip];
}
//==============================================================================
/// Return the radius of the strip+fractional distance to strip.
//==============================================================================
double DeVeloRType::rOfStrip(unsigned int strip, double fraction)
{
  /*  unsigned int next=strip+1;
  if(next%512) return m_rStrips[strip]*(1+fraction);
  return  m_rStrips[strip]*(1-fraction) + fraction*m_rStrips[next];*/
  return m_rStrips[strip]+fraction*rPitch(strip);
}
//==============================================================================
/// Return the local pitch of the sensor for a given channel...
//==============================================================================
double DeVeloRType::rPitch(unsigned int strip)
{
  return m_rPitch[strip];
  
}
//==============================================================================
/// Return the local pitch of the sensor for a given channel...
//==============================================================================
double DeVeloRType::rPitch(unsigned int strip, double fraction)
{
  return exp(fraction)*m_rPitch[strip];
  
}
//==============================================================================
/// Return the local pitch of the sensonr
//==============================================================================
double DeVeloRType::rPitch(double radius)
{
  return m_innerPitch + m_pitchSlope*(radius - m_innerRadius);
}
//==============================================================================
/// The minimum phi for a zone
//==============================================================================
double DeVeloRType::phiMinZone(unsigned int zone)
{
  return m_phiMin[zone];
}
//==============================================================================
/// Returns the minimum phi in a zone at given radius
//==============================================================================
double DeVeloRType::phiMinZone(unsigned int zone, double radius)
{
  double phiMin;
  if(0 == zone){
    phiMin = acos(m_overlapInX/radius);
  } else if(2 == zone){
    phiMin = asin(-m_phiGap/radius);
  } else {
    phiMin = this->phiMinZone(zone);
  }
  return phiMin;
}
//==============================================================================
/// The maximum phi for a zone
//==============================================================================
double DeVeloRType::phiMaxZone(unsigned int zone)
{
  return m_phiMax[zone];
}
//==============================================================================
/// Returns the maximum phi in a zone at given radius
//==============================================================================
double DeVeloRType::phiMaxZone(unsigned int zone, double radius)
{
  double phiMax;
  if(1 == zone){
    phiMax = asin(m_phiGap/radius);
  } else if(3 == zone){
    phiMax = -acos(m_overlapInX/radius);
  } else {
    phiMax = this->phiMaxZone(zone);
  }
  return phiMax;
}
//==============================================================================
/// Return the capacitance of the strip
//==============================================================================
double DeVeloRType::stripCapacitance(unsigned int /*strip*/)
{
  double C=0.0;
  return C;
}
//==============================================================================
/// Return the strip geometry for panoramix
//==============================================================================
StatusCode DeVeloRType::stripLimits(unsigned int strip, double &radius,
                                    double &phiMin, double &phiMax)
{
  radius = rOfStrip(strip);
  phiMin = phiMinStrip(strip);
  phiMax = phiMaxStrip(strip);
  return StatusCode::SUCCESS;
}
double DeVeloRType::phiMinStrip(unsigned int strip)
{
  return m_stripLimits[strip].first;
}
double DeVeloRType::phiMaxStrip(unsigned int strip)
{
  return m_stripLimits[strip].second;
}
