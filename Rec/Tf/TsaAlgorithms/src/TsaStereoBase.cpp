// $Id: TsaStereoBase.cpp,v 1.2 2007-09-16 10:01:51 mneedham Exp $

// GaudiKernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "TsaStereoBase.h"
#include "TsaKernel_Tf/SeedHit.h"
#include "TsaKernel_Tf/SeedTrack.h"
#include "TsaKernel_Tf/SeedFun.h"
#include "SeedLineFit.h"

#include "Event/State.h"

#include "TsaKernel_Tf/TsaConstants.h"

#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"

#include "TfKernel/RecoFuncs.h"

using namespace Tf::Tsa;

StereoBase::StereoBase(const std::string& type,
                       const std::string& name,
                       const IInterface* parent):
  GaudiTool(type, name, parent)
{
  declareProperty("sector", m_sector = -1);
}


StereoBase::~StereoBase()
{
  // destructer
  delete m_fitLine;
}

StatusCode StereoBase::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize",sc);
  }

  m_fitLine = new SeedLineFit(msg(),TsaConstants::z0, TsaConstants::sth);

  // sector must be set
  if (m_sector == -1){
    return Error("No sector set", StatusCode::FAILURE);
  }
  return StatusCode::SUCCESS;
}

StatusCode StereoBase::execute(LHCb::State& , std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[6] ){
  return execute(seeds, hits);
}

StatusCode StereoBase::execute(std::vector<SeedTrack*>& , std::vector<SeedHit*>* ){
  return StatusCode::SUCCESS;
}

void StereoBase::cleanup(std::vector<SeedHit*> hits[6])
{
  // clean up - copy hits to store
  SeedHits* hitsCont = getOrCreate<SeedHits,SeedHits>(evtSvc(),m_seedHitLocation);
  for ( int lay = 0; lay < 6; ++lay ) {
    for ( std::vector<SeedHit*>::iterator it = hits[lay].begin(); hits[lay].end() != it; ++it )
    {
      hitsCont->insert(*it);
      verbose() << "SeedHit inserted " << **it << endreq;
    }
  } // lay
}

unsigned int StereoBase::collectHits(SeedTrack* seed, std::vector<SeedHit*> hits[6],
                                     std::vector<SeedHit*> yHits[6] ){

  unsigned int nCollected = 0;

  for ( int lay = 0; lay < 6; ++lay ) {

    if (hits[lay].empty() == true) continue;

    bool uLay = false;
    if ( lay == 0 || lay == 2 || lay == 4 ) uLay = true;

    const double zFirst = hits[lay].front()->z();
    const double slope = seed->xSlope(zFirst,TsaConstants::z0);
    Gaudi::XYZVector vec(1., TsaConstants::tilt*slope, -slope);
    Gaudi::XYZPoint point(seed->x(zFirst,TsaConstants::z0), hits[lay].front()->yMid(), zFirst );
    Gaudi::Plane3D plane = Gaudi::Plane3D(vec,point);
    Gaudi::XYZPoint iPoint;
    const double tol = 0.1*hits[lay].front()->length();

    for ( std::vector<SeedHit*>::iterator it = SeedFun::startStereo(hits[lay],point.x(),tol);
          hits[lay].end() != it; ++it )
    {
      SeedHit* hit = (*it);
      //if ( !Tsa::ClusFun::intersection(hit->clus(),plane,iPoint) ) continue;
      if ( !Tf::intersection(hit->tfHit(),plane,iPoint) ) continue;

      if ( uLay ) {
        if ( iPoint.y() > hit->yMax() + m_yTol ) continue;
        if ( iPoint.y() < hit->yMin() - m_yTol ) break;
      } else {
        if ( iPoint.y() > hit->yMax() + m_yTol ) break;
        if ( iPoint.y() < hit->yMin() - m_yTol) continue;
      }
      verbose() << "  Intersection Point " << iPoint << endreq;
      hit->setY( iPoint.y() );
      hit->setZ( iPoint.z() );
      yHits[lay].push_back( hit );
      ++nCollected;
    }
  } //hits

  return nCollected;
}
