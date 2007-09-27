// $Id: TsaSeedTrackCnv.cpp,v 1.3 2007-09-27 14:29:44 mneedham Exp $
//
// This File contains the implementation of the TsaEff
// C++ code for 'LHCb Tracking package(s)'
//

// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "TsaSeedTrackCnv.h"

using namespace Tf::Tsa;

DECLARE_ALGORITHM_FACTORY( SeedTrackCnv );

SeedTrackCnv::SeedTrackCnv(const std::string& name,
                           ISvcLocator* pSvcLocator):
  BaseAlg(name, pSvcLocator)
{
  // constructor
  /*
    declareProperty("eX2",m_EX2 = 40*Gaudi::Units::mm2);
    declareProperty("eY2",m_EY2 = 40*Gaudi::Units::mm2);
    declareProperty("eTX2",m_ETx2 = 1e-4);
    declareProperty("eTY2",m_ETy2 = 1.0e-4); // 2e-7;
    declareProperty("eQdivP2",m_EQdivP2 = 0.1);
  */

  declareProperty("eX2",m_EX2 = 31*Gaudi::Units::mm2);
  declareProperty("eY2",m_EY2 = 12*Gaudi::Units::mm2);
  declareProperty("eTX2",m_ETx2 = 2.0e-3);
  declareProperty("eTY2",m_ETy2 = 2.0e-3); // 2e-7;
  declareProperty("eQdivP2",m_EQdivP2 = 0.01);

  declareProperty("LikCut", m_likCut = -30.);
  declareProperty("curvatureFactor", m_curvFactor = 42.0/Gaudi::Units::MeV);
  declareProperty("pFromCurvature", m_pFromCurvature = false);
  declareProperty("increaseErrors", m_largeErrors = true);

  declareProperty("inputLocation", m_inputLocation = SeedTrackLocation::Default);
  declareProperty("outputLocation", m_outputLocation = LHCb::TrackLocation::Tsa );
}


StatusCode SeedTrackCnv::initialize()
{
  // Initialization

  const StatusCode sc = BaseAlg::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize");
  }

  m_ptKickTool = tool<ITrackPtKick>("TrackPtKick");

  return sc;
}

SeedTrackCnv::~SeedTrackCnv() { }

StatusCode SeedTrackCnv::execute()
{

  // get the tracks
  SeedTracks* trackCont = get<SeedTracks>(m_inputLocation);
  debug() << "Found " << trackCont->size() << " SeedTracks at " << m_inputLocation << endreq;

  // output container
  LHCb::Tracks* output = new LHCb::Tracks();
  output->reserve(trackCont->size());
  put(output,m_outputLocation);

  for (SeedTracks::const_iterator iterTrack = trackCont->begin();
       iterTrack != trackCont->end(); ++iterTrack)
  {
    verbose() << "SeedTrack " << (*iterTrack)->key() << " " << (*iterTrack)->select()
              << " " << (*iterTrack)->lik() << endreq;
    if ( ((*iterTrack)->select() != 0) && ((*iterTrack)->lik() > m_likCut) )
    {
      LHCb::Track* fitTrack = this->convert(*iterTrack);
      output->insert(fitTrack, (*iterTrack)->key());
    }
  }

  debug() << "Created " << output->size() << " converted SeedTracks at " << m_outputLocation << endreq;

  return StatusCode::SUCCESS;
}


LHCb::Track* SeedTrackCnv::convert(const SeedTrack* aTrack) const{

  // make a fit track...
  LHCb::Track* fitTrack = new LHCb::Track();


  std::vector<SeedPnt> pnts = aTrack->usedPnts();
  //std::cout << " starting a track " << pnts.size() << std::endl;
  for ( std::vector<SeedPnt>::const_iterator itP = pnts.begin(); pnts.end() != itP; ++itP ) {
    fitTrack->addToLhcbIDs((*itP).hit()->lhcbID());
    //std::cout << (*itP).hit()->ITChan() << std::endl;
  } //it

  addState(aTrack, fitTrack, pnts.back().z());

  // add history
  fitTrack->setHistory(LHCb::Track::TsaTrack);
  fitTrack->setType(LHCb::Track::Ttrack);
  fitTrack ->setPatRecStatus( LHCb::Track::PatRecIDs );
  fitTrack->addInfo(LHCb::Track::TsaLikelihood, aTrack->lik());

  return fitTrack;
}

void SeedTrackCnv::addState(const SeedTrack* aTrack, LHCb::Track* lTrack, const double z) const{

  // first  state info
  Gaudi::TrackVector stateVec = Gaudi::TrackVector();
  Gaudi::TrackSymMatrix stateCov = Gaudi::TrackSymMatrix();

  // initial errors - either large or from seed
  if (m_largeErrors == true){
    stateCov(0,0) = m_EX2;
    stateCov(1,1) = m_EY2;
    stateCov(2,2) = m_ETx2;
    stateCov(3,3) = m_ETy2;
  }
  else {
    // x
    stateCov(0,0) = aTrack->xErr(0);
    stateCov(2,2) = aTrack->xErr(3);
    stateCov(0,1) = aTrack->xErr(1);
    if (m_pFromCurvature == true){
      stateCov(0,4) = aTrack->xErr(2)*m_curvFactor;
      stateCov(2,4) = aTrack->xErr(4)*m_curvFactor;
    }
    // y
    stateCov(1,1) = aTrack->yErr(0);
    stateCov(3,3) = aTrack->yErr(2);
    stateCov(1,3) = aTrack->yErr(1);
  }


  LHCb::State aState = LHCb::State(stateVec,stateCov,z,LHCb::State::AtT);

  // state all parameters but p
  aState.setX(aTrack->x(z,0.));
  aState.setY(aTrack->y(z,0.));
  aState.setTx(aTrack->xSlope(z,0.));
  aState.setTy(aTrack->sy());

  // p estimate can come from the curvature or the pt kick
  if (m_pFromCurvature) {
    aState.setQOverP(estimateCurvature(aTrack->tx(), m_curvFactor));
    if (m_largeErrors) {
      aState.setErrQOverP2( m_EQdivP2*gsl_pow_2(stateVec(4)));
    }
    else {
      aState.setErrQOverP2(aTrack->xErr(5)*gsl_pow_2(m_curvFactor));
    }
  }
  else {
    StatusCode sc = m_ptKickTool->calculate(&aState);
    if( sc.isFailure() ) {
      Warning( "Pt Kick tool failed, but still adding State" );
    }
  }

  // add to states
  lTrack->addToStates(aState);

}


