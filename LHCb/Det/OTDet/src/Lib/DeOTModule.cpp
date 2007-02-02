// $Id: DeOTModule.cpp,v 1.25 2007-02-02 09:25:04 janos Exp $
/// Kernel
#include "GaudiKernel/Point3DTypes.h"
#include "Kernel/LineTraj.h"

/// LHCbMath
#include "LHCbMath/Line.h"
#include "LHCbMath/LineTypes.h"
#include "LHCbMath/GeomFun.h"

/// DetDesc
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"

/// GSL
#include "gsl/gsl_math.h"

/// local
#include "OTDet/DeOTModule.h"

/** @file DeOTModule.cpp
 *
 *  Implementation of class :  DeOTModule
 *
 *  @author Jeroen van Tilburg jtilburg@nikhef.nl
 */

using namespace LHCb;

DeOTModule::DeOTModule(const std::string& name) :
  DetectorElement(name),
  m_stationID(0u),
  m_layerID(0u),
  m_quarterID(0u),
  m_moduleID(0u),
  m_elementID(0u),
  m_uniqueModuleID(0u),
  m_nStraws(0u),
  m_stereoAngle(0.0),
  m_sinAngle(0.0),
  m_cosAngle(0.0),
  m_xPitch(0.0),
  m_zPitch(0.0),
  m_cellRadius(0.0),
  m_inefficientRegion(0.0),
  m_sensThickness(0.0),
  m_nModules(0),
  m_ySizeModule(0.0),
  m_xMinLocal(0.0),
  m_xMaxLocal(0.0),
  m_yMinLocal(0.0),
  m_yMaxLocal(0.0),
  m_xInverted(false),
  m_yInverted(false) {
  
  /// Constructor 
}

DeOTModule::~DeOTModule() { /// Destructor 
}

const CLID& DeOTModule::clID() const { 
  return classID();
}


StatusCode DeOTModule::initialize() {
  IDetectorElement* quarter = this->parentIDetectorElement();
  IDetectorElement* layer = quarter->parentIDetectorElement();
  IDetectorElement* station = layer->parentIDetectorElement();
  IDetectorElement* ot = station->parentIDetectorElement();

  // Get specific parameters from the module
  m_moduleID = (unsigned int)param<int>("moduleID");
  m_quarterID = (unsigned int)quarter->params()->param<int>("quarterID");
  m_layerID = (unsigned int)layer->params()->param<int>("layerID");
  m_stationID = (unsigned int)station->params()->param<int>("stationID");
  m_nStraws = (unsigned int)param<int>("nStraws");
  m_stereoAngle = layer->params()->param<double>("stereoAngle");
  m_sinAngle = sin(m_stereoAngle);
  m_cosAngle = cos(m_stereoAngle);  
  OTChannelID aChan = OTChannelID(m_stationID, m_layerID, m_quarterID, m_moduleID, 0u);
  setElementID(aChan);
  m_uniqueModuleID = aChan.uniqueModule();

  // Get some general parameters for the OT
  m_xPitch = ot->params()->param<double>("strawPitchX");
  m_zPitch = ot->params()->param<double>("strawPitchZ");
  m_cellRadius = ot->params()->param<double>("cellRadius");
  m_inefficientRegion = ot->params()->param<double>("inefficientRegion");
  m_sensThickness = ot->params()->param<double>("sensThickness");
  m_nModules = (unsigned int)ot->params()->param<int>("nModules");

  // Get the lenght of the module
  //const ILVolume* lv = (this->geometry())->lvolume();
  const ISolid* solid = this->geometry()->lvolume()->solid();
  const SolidBox* mainBox = dynamic_cast<const SolidBox*>(solid);
  m_ySizeModule = mainBox->ysize();
  m_xMaxLocal = (0.5*m_nStraws+0.25)*m_xPitch;
  m_xMinLocal = -m_xMaxLocal;
  m_yMaxLocal = mainBox->yHalfLength();
  m_yMinLocal = -m_yMaxLocal;
  
  /// cache trajectories/planes
  cacheInfo();

  return StatusCode::SUCCESS;
}

void DeOTModule::findStraws(const Gaudi::XYZPoint& entryPoint, 
                            const Gaudi::XYZPoint& exitPoint,
                            Straws& straws) const {
  /// This is in local cooridinates of a module
  const double xOffset = m_xMinLocal; //-(0.5*m_nStraws + 0.25)*m_xPitch;
  double lo = (entryPoint.x()-xOffset)/m_xPitch; 
  double hi = (exitPoint.x()-xOffset)/m_xPitch;
  
  if (lo > hi) std::swap(lo , hi);
  
  const int exStraw = 1; ///< Add extra straws to the the left and right
  unsigned int strawLo = GSL_MAX_INT(0, int(std::floor(lo)) - exStraw);
  unsigned int strawHi = GSL_MIN_INT(int(m_nStraws)-1, int(std::ceil(hi)) + exStraw);

  /// Now let's fill the vector. Remember straw numbering starts at 1, i.e. i+1
  straws.clear();
  straws.reserve(strawHi-strawLo + 1u);
  for (unsigned int i = strawLo; i <= strawHi; ++i) straws.push_back(i+1);
}

void DeOTModule::calculateHits(const Gaudi::XYZPoint& entryPoint,
                               const Gaudi::XYZPoint& exitPoint,
                               std::vector<std::pair<OTChannelID, double> >& chanAndDist) const {
  /// Make sure channels and driftdistances vectors are empty 
  chanAndDist.clear(); ///< This should erase all elements, if any.
  
  /// Go from global to local.
  Gaudi::XYZPoint enP = toLocal(entryPoint);
  Gaudi::XYZPoint exP = toLocal(exitPoint);
     
  /// Need this to check that enZ and exZ aren't sort of in the same plane,
  /// i.e. not a curly track. These are typically low momentum (50 MeV) 
  /// electrons.  
  bool samePlane = std::abs(enP.z()-exP.z()) < m_cellRadius;
  if (!samePlane) { // Track in cell
    /// Track
    Gaudi::XYZLine track(enP, (exP-enP).unit());
    /// Now let's get a list of possible hit straws
    Straws straws; 
    findStraws(enP, exP, straws);
    /// Wire
    const double z = 0.5*m_zPitch;
    Gaudi::XYZPoint wB(0.0, m_yMinLocal, -z);
    Gaudi::XYZPoint wT(0.0, m_yMaxLocal, -z);
    Gaudi::XYZLine wire;
    /// Are the wire and track parallel
    bool notParallel = true;
    Gaudi::XYZPoint mu;
    Gaudi::XYZPoint lambda;
    /// is in efficient region of (F-modules)
    bool efficientY = true;
    unsigned int straw = 0u;
    double x = 0.0;
    double dist = 0.0; /// lambda - mu
    OTChannelID aChannel; /// channelID
    /// loop over straws
    /// First monolayer
    Straws::const_iterator iS; 
    for (iS= straws.begin(); iS != straws.end(); ++iS) {
      straw = (*iS);
      x = localUOfStraw(straw);
      wB.SetX(x);
      wT.SetX(x); 
      wire = Gaudi::XYZLine(wB, (wT-wB).unit());
      notParallel = Gaudi::Math::closestPoints(wire, track, mu, lambda);
      if (notParallel) {
        dist = driftDistance(lambda-mu);
        efficientY = isEfficientA(mu.y());
        if (efficientY && std::abs(dist) < m_cellRadius) {
          aChannel = OTChannelID(m_stationID, m_layerID, m_quarterID, m_moduleID, straw);
          chanAndDist.push_back(std::make_pair(aChannel, dist));
        }
      }
    }
    /// Second monolayer
    wB.SetZ(z);
    wT.SetZ(z);
    for (iS= straws.begin(); iS != straws.end(); ++iS) {
      straw = (*iS) + m_nStraws;
      x = localUOfStraw(straw);
      wB.SetX(x);
      wT.SetX(x);
      wire = Gaudi::XYZLine(wB, (wT-wB).Unit());
      notParallel = Gaudi::Math::closestPoints(wire, track, mu, lambda);
      if (notParallel) {
        dist = driftDistance(lambda-mu);
        efficientY = isEfficientB(mu.y());
        if (efficientY && std::abs(dist) < m_cellRadius) {
          aChannel = OTChannelID(m_stationID, m_layerID, m_quarterID, m_moduleID, straw);
          chanAndDist.push_back(std::make_pair(aChannel, dist));
        }
      }
    }
  } else {/// Need this to estimate occupancies
    const double x1 = enP.x();
    const double z1 = enP.z();
    const double x2 = exP.x();
    const double z2 = exP.z();

    double uLow = x1;
    double uHigh = x2;      
    if ( uLow > uHigh ) std::swap(uLow, uHigh);
        
    // zfrac is between 0 and 1. 2.7839542167 means nothing.
    // This seems to acts as a random number generator.
    // if distance xy entry-exit is small generate z3 close
    // to the z1 and z2 ( z1 is close to z2)
    /// Circle in global
    const double u1 = (entryPoint.z() > exitPoint.z()) ? exitPoint.x() : entryPoint.x();
    const double v1 = (entryPoint.z() > exitPoint.z()) ? exitPoint.y() : entryPoint.y();
    double zint;
    const double zfrac = std::modf((std::abs(u1)+std::abs(v1))/2.7839542167, &zint);
    const double distXY = std::sqrt(( exP - enP ).perp2());
    double z3Circ = ((distXY > 2.0*m_xPitch) ? 2.0 * (zfrac-0.5) :(z1<0?-zfrac:zfrac))*m_zPitch;
    double zCirc, uCirc, rCirc;
    sCircle(z1, x1, z2, x2, z3Circ, zCirc, uCirc, rCirc);
    
    
    double uStep = uLow;
    double distCirc = 0.0;
    int amb = 0;
    double dist = 0.0;
    OTChannelID aChannel;
    
    // monolayer A
    unsigned int strawA = hitStrawA(uLow);
    const double zStrawA = -0.5*m_zPitch;//localZOfStraw(strawA);
    while ( (uStep < uHigh) && strawA != 0 ) {
      uStep = localUOfStraw(strawA);
      distCirc = gsl_hypot((zCirc-zStrawA), (uCirc-uStep));
      amb = ((-(uStep-(x1+x2)/2.0)*(distCirc-rCirc)) < 0.0) ? -1 : 1;
      dist = amb*std::abs(distCirc-rCirc);
      const unsigned int straw = strawA;
      if ( std::abs(dist) < m_cellRadius ) {
        aChannel = OTChannelID(m_stationID, m_layerID, m_quarterID, m_moduleID, straw);
        chanAndDist.push_back(std::make_pair(aChannel, dist));
      }
      strawA = nextRightStraw(straw);
    }
    
    // monolayer B
    unsigned int strawB = hitStrawB(uLow);
    const double zStrawB = 0.5*m_zPitch;//localZOfStraw(strawB);
    uStep = uLow;
    while ( (uStep < uHigh) && strawB != 0 ) {
      uStep = localUOfStraw(strawB);
      distCirc = gsl_hypot((zCirc-zStrawB), (uCirc-uStep));
      amb = ((-(uStep-(x1+x2)/2.0)*(distCirc-rCirc))< 0.0) ?  -1 : 1;
      dist = amb*std::abs(distCirc-rCirc);
      const unsigned int straw = strawB;
      if ( std::abs(dist) < m_cellRadius ) {
        aChannel = OTChannelID(m_stationID, m_layerID, m_quarterID, m_moduleID, straw);
        chanAndDist.push_back(std::make_pair(aChannel, dist));
      }
      strawB = nextRightStraw(straw);
    }
  } //curling tracks
}

void DeOTModule::sCircle(const double z1, const double u1, const double z2, 
                         const double u2, const double z3c,
                         double& zc, double& uc, double& rc) const {
  const double zw=(z1+z2)/2.0;
  double uw=(u2+u1)/2.0;
  
  zc=0.5*(z3c*z3c-zw*zw-(u1-uw)*(u1-uw))/(z3c-zw);
  uc=uw;
  rc=std::abs(zc-z3c);
}

double DeOTModule::distanceToWire(const unsigned int aStraw, 
                                  const Gaudi::XYZPoint& aPoint, 
                                  const double tx, const double ty) const {
  // go to the local coordinate system
  Gaudi::XYZVector vec(tx, ty, 1.);
  Gaudi::XYZPoint localPoint = toLocal(aPoint);
  Gaudi::XYZVector localVec = toLocal(aPoint+vec) - localPoint;

  // calculate distance to the straw
  double u = localPoint.x()+localVec.x()*(localZOfStraw(aStraw)-localPoint.z());
  double cosU = 1.0/gsl_hypot(1.0, (localVec.x()/localVec.z()));
  
  // return distance to straw
  return (u-localUOfStraw(aStraw))*cosU;
}

void DeOTModule::clear() {
  m_midTraj[0].reset();
  m_midTraj[1].reset();
}

void DeOTModule::cacheInfo() {
  clear();
  
  double xUpper = m_xMaxLocal;
  double xLower = m_xMinLocal;
  double yUpper = m_yMaxLocal;
  double yLower = m_yMinLocal;

  /// Direction; points to readout 
  if (bottomModule()) std::swap(yUpper, yLower);
  Gaudi::XYZPoint g1 = globalPoint(0.0, yLower, 0.0);
  Gaudi::XYZPoint g2 = globalPoint(0.0, yUpper, 0.0);
  m_dir = g2 - g1;
  m_dir = m_dir.Unit();  

  /// trajs of middle of monolayers
  Gaudi::XYZPoint g3[2];
  /// 0 -> first monolayer
  g3[0] = globalPoint(xLower, 0.0, -0.5*m_zPitch);
  /// 1 -> second monolayer
  g3[1] = globalPoint(xLower, 0.0, +0.5*m_zPitch);
  Gaudi::XYZPoint g4[2]; 
  /// first monolayer
  g4[0] = globalPoint(xUpper, 0.0, -0.5*m_zPitch);
  /// second monolayer
  g4[1] = globalPoint(xUpper, 0.0, +0.5*m_zPitch);
  /// first monolayer
  m_midTraj[0].reset(new LineTraj(g3[0], g4[0]));
  /// second monolayer
  m_midTraj[1].reset(new LineTraj(g3[1], g4[1]));
 
  /// range -> wire length
  /// wire lenght = module length
  /// first mono layer
  m_range[0] = std::make_pair(m_yMinLocal, m_yMaxLocal);
  /// second mono layer
  m_range[1] = std::make_pair(m_yMinLocal, m_yMaxLocal);
  // correct for inefficient regions in long modules
  if (longModule() && topModule()) m_range[0].first=m_yMinLocal+m_inefficientRegion;
  if (longModule() && bottomModule()) m_range[1].first=m_yMinLocal+m_inefficientRegion;
  
  /// plane
  m_plane = Gaudi::Plane3D(g1, g2, g4[0] + 0.5*(g4[1]-g4[0]));
  
  m_entryPlane = Gaudi::Plane3D(m_plane.Normal(), globalPoint(0.,0.,-0.5*m_sensThickness));
  m_exitPlane = Gaudi::Plane3D(m_plane.Normal(), globalPoint(0.,0., 0.5*m_sensThickness));
  m_centerModule = globalPoint(0.,0.,0.);

}

std::auto_ptr<LHCb::Trajectory> DeOTModule::trajectoryFirstWire(int monolayer) const {
  /// Default is 0 -> straw 1
  double lUwire = (monolayer==1?localUOfStraw(m_nStraws+1):localUOfStraw(1));
  Gaudi::XYZPoint firstWire = m_midTraj[monolayer]->position(lUwire);
  return std::auto_ptr<LHCb::Trajectory>(new LineTraj(firstWire, m_dir, m_range[monolayer], true));
}

std::auto_ptr<LHCb::Trajectory> DeOTModule::trajectoryLastWire(int monolayer) const {
  /// Default is 1 -> straw 64(s3)/128
  double lUwire = (monolayer==0?localUOfStraw(m_nStraws):localUOfStraw(2*m_nStraws));
  Gaudi::XYZPoint lastWire = m_midTraj[monolayer]->position(lUwire);
  return std::auto_ptr<LHCb::Trajectory>(new LineTraj(lastWire, m_dir, m_range[monolayer], true));
}

/// Returns a Trajectory representing the wire identified by the LHCbID
/// The offset is zero for all OT Trajectories
std::auto_ptr<LHCb::Trajectory> DeOTModule::trajectory(const OTChannelID& aChan,
                                                       const double /*offset*/) const {
  if (!contains(aChan)) {
    throw GaudiException("Failed to make trajectory!", "DeOTModule.cpp",
			 StatusCode::FAILURE);
  }
  
  unsigned int aStraw = aChan.straw();
  
  unsigned int mono = (monoLayerA(aStraw)?0u:1u);

  Gaudi::XYZPoint posWire = m_midTraj[mono]->position(localUOfStraw(aStraw));
  
  return std::auto_ptr<Trajectory>(new LineTraj(posWire, m_dir, m_range[mono],true));
}
