// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "Event/WeightsVector.h"

// local
#include "PVOfflineRecalculate.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PVOfflineRecalculate
//
// 2010-10-05 : Mariusz Witek
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PVOfflineRecalculate );
 

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PVOfflineRecalculate::PVOfflineRecalculate( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<PVOfflineRecalculate>(this);
 
}
//=============================================================================
// Destructor
//=============================================================================
PVOfflineRecalculate::~PVOfflineRecalculate() {} 

StatusCode PVOfflineRecalculate::finalize()
{
  print_stats();
  return GaudiTool::finalize();  
}


StatusCode PVOfflineRecalculate::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if(!sc) return sc;
  // Linear track extrapolator
  m_linExtrapolator = tool<ITrackExtrapolator>("TrackLinearExtrapolator",this);
  if(!m_linExtrapolator) {
    err() << "Unable to retrieve the TrackLinearExtrapolator" << endmsg;
    return  StatusCode::FAILURE;
  }
  // Full track extrapolator
  m_fullExtrapolator = tool<ITrackExtrapolator>("TrackMasterExtrapolator",this);
  if(!m_fullExtrapolator) {
    err() << "Unable to get TrackMasterExtrapolator" << endmsg;
    return  StatusCode::FAILURE;
  }
  // set counters
  m_counter_name.push_back("Total calls to PV recalculate             "); // 0
  m_counter_name.push_back("Sizes of PV and Weight containers differ  "); // 1
  m_counter_name.push_back("Specified input vtx not in PV container   "); // 2
  m_counter_name.push_back("Number of PV tracks and Weights differ    "); // 3
  m_counter_name.push_back("None of tracks to remove comes from PV    "); // 4
  m_counter_name.push_back("Error inverting hessian matrix            "); // 5
  m_counter_name.push_back("Number of PV tracks after removal too low "); // 6
  m_counter_name.push_back("No tracks to be removed from PV           "); // 7
  m_counter_name.push_back("No of PV recalculations performed         "); // 8

  for ( unsigned int i=0; i< m_counter_name.size(); i++ ) {
    m_counter_count.push_back(0);    
  }

  return StatusCode::SUCCESS;
}


//=============================================================================
void PVOfflineRecalculate::RecalculateVertex(const LHCb::RecVertex* pvin,
                                             std::vector<const LHCb::Track*>& tracks2remove,
                                             LHCb::RecVertex& vtx) 
{ 
   m_counter_count[0] += 1;    

  // Copy input vtx to output vtx. In case of failure original PV is returned
  vtx = *pvin;

  // PV and Weights are in separate containers. Retrive info and check consistency.
  LHCb::RecVertices*    recoVertices     = get<LHCb::RecVertices>(LHCb::RecVertexLocation::Primary);
  LHCb::WeightsVectors* weightsContainer = get<LHCb::WeightsVectors>(LHCb::WeightsVectorLocation::Default);

  if ( weightsContainer->size() != recoVertices->size() ) {
    // Sizes of PV and Weight containers differ
    m_counter_count[1] += 1;    
    return;
  }
  std::vector<LHCb::RecVertex*>::const_iterator itpv;  
  itpv = std::find(recoVertices->begin(), recoVertices->end(), pvin);
  if ( itpv == recoVertices->end() ) {
    // Specified input vtx not in PV container 
    m_counter_count[2] += 1;    
    return;
  }
  // get weights vector for input vertex
  int ipv = itpv - recoVertices->begin();
  const std::vector<std::pair<int,float> >& the_weights = (*weightsContainer)(ipv)->weights();

  int ntr = (*itpv)->tracks().size();
  int nw = the_weights.size();
  
  if ( ntr != nw ) {
    // # tracks in PV does not match # weights
    m_counter_count[3] += 1;    
    return;
  }

  // Collect tracks to remove together with their weights. If no weight found track is skipped.
  std::vector<const LHCb::Track*> trin;
  std::vector<double> wgin;
  for (std::vector<const LHCb::Track*>::iterator itr = tracks2remove.begin(); itr != tracks2remove.end(); itr++) {
    for ( unsigned int iw = 0; iw < the_weights.size() ; iw++ ) {
      if ( (*itr)->key() == the_weights[iw].first ) {
        trin.push_back(*itr);
        wgin.push_back(the_weights[iw].second);
      }      
    }
  }
  if ( trin.size() < 1 ) {
    // nothing to do if no tracks to removed
    m_counter_count[4] += 1; 
    return; // success
  }

  // remove track
  Gaudi::SymMatrix3x3 hess;
  hess = pvin->covMatrix();
  hess = hess * 0.5;
  int fail;  
  hess.Inverse(fail);
  if (0 != fail) {
    debug() << "Error inverting hessian matrix" << endmsg;
    m_counter_count[5] += 1; 
    return;
  } else {
    hess.Invert();
  }

  ROOT::Math::SVector<double,3> d0vec;
  for ( int i = 0; i<3; i++ ) d0vec[i]=0;
  double dchi2 = 0.;
  int ntr_removed = 0;  
  std::vector<const LHCb::Track*> removed_tracks;
  for ( unsigned int it = 0; it <  trin.size(); it++) {

    bool removed = remove_track(pvin, hess, d0vec, dchi2, trin[it],  wgin[it] );
    if ( removed ) { 
      removed_tracks.push_back(trin[it]);    
      ntr_removed++;
    }
    
    // check if enough tracks left in PV i.e. at least 5 tracks (ndof=2*ntrack-3).
    if ( (pvin->nDoF() - 2 *ntr_removed ) < 7 ) {
      // Number of PV tracks after removal too low
      m_counter_count[6] += 1;
      return;
    }    

  }

  // nothing to do if no tracks removed
  if (  ntr_removed < 1 ) {
    // No tracks to be removed from PV
    m_counter_count[7] += 1;
    return; // success
  }
  
  // calculate new vertex
  hess.Inverse(fail);
  if (0 != fail) {
    debug() << "Error inverting hessian matrix" << endmsg;
    m_counter_count[5] += 1;
    return;
  } else {
    hess.Invert();
  }
  ROOT::Math::SVector<double,3> delta;
  for(int i = 0; i < 3; i++) {
    delta[i] = 0.0;
  }
  delta = hess * d0vec;
  if(msgLevel(MSG::DEBUG)) {
    debug() << "PV recalculate - displacement found: " 
            << delta[0] << " , " 
            << delta[1] << " , " 
            << delta[2] << " , " << endmsg;
  }
  Gaudi::XYZPoint newVtx(pvin->position().x() + delta[0], 
                         pvin->position().y() + delta[1], 
                         pvin->position().z() + delta[2]);
  vtx.setPosition(newVtx);
  if(msgLevel(MSG::DEBUG)) {
    debug() << " PV recalculate - new vertex position: "
            << vtx.position().x() << " , "
            << vtx.position().y() << " , " 
            << vtx.position().z() << endmsg;
  }

  // Set covariance matrix
  hess = hess * 2.0;
  vtx.setCovMatrix(hess);
  // Set tracks
  vtx.clearTracks();
  SmartRefVector< LHCb::Track >  vtx_tracks = pvin->tracks();
  for (unsigned int it = 0; it < vtx_tracks.size(); it++ ) {
    const LHCb::Track* ptr =  vtx_tracks[it];
    if ( std::find(removed_tracks.begin(), removed_tracks.end(), ptr) == removed_tracks.end() ) {
      vtx.addToTracks(ptr);
    }
  }

  vtx.setChi2(pvin->chi2()-dchi2);
  vtx.setNDoF(pvin->nDoF()-2*ntr_removed);

  m_counter_count[8] += 1;
  
  return; // success

}

bool PVOfflineRecalculate::remove_track(const LHCb::RecVertex* pvin, 
                                        Gaudi::SymMatrix3x3& hess, 
                                        ROOT::Math::SVector<double,3>& d0vec,
                                        double& dchi2wg,
                                        const LHCb::Track* vtrack,
                                        double wg) 
{

  // Check if tracks has Velo
  if ( !(vtrack->hasVelo()) ) return false;

  // extrapolate track to z of vtx
  LHCb::State mstate;
  mstate =  vtrack->firstState();
  if ( mstate.checkLocation(LHCb::State::ClosestToBeam) != true ) {
    // extrapolate
    if(fabs(mstate.qOverP()) > 0) {
      StatusCode sc = m_fullExtrapolator->propagate(mstate,pvin->position().z());
      if(sc.isFailure()) return false;
    } else {
       StatusCode sc = m_linExtrapolator->propagate(mstate,pvin->position().z());
       if(sc.isFailure()) return false;
    }  
  } else {
    StatusCode sc = m_linExtrapolator->propagate(mstate,pvin->position().z());
    if(sc.isFailure()) return false;  
  }

  // calculate IP and its error
  Gaudi::XYZVector unitVect = mstate.slopes().Unit();  
  Gaudi::XYZPoint trkPoint(mstate.x(),mstate.y(),mstate.z());
  
  Gaudi::XYZVector diffVect = trkPoint - pvin->position();
  // Compute IP
  Gaudi::XYZVector vd0 = unitVect.Cross(diffVect.Cross(unitVect));
  // Compute impact parameter d0
  double d02 = vd0.Mag2();
  // Compute IP err
  Gaudi::XYZVector vd0Unit = vd0.unit();
  ROOT::Math::SVector<double,2> xyVec;
  xyVec[0] = vd0Unit.x();
  xyVec[1] = vd0Unit.y();
  Gaudi::SymMatrix2x2 covMatrix = mstate.covariance().Sub<Gaudi::SymMatrix2x2>(0,0);
  ROOT::Math::SVector<double,2> covMatrix_xyVec_product;
  covMatrix_xyVec_product = covMatrix * xyVec;
  double err2d0 = xyVec[0] * covMatrix_xyVec_product[0] + xyVec[1] * covMatrix_xyVec_product[1];
  double chi2 = 0.;  
  if(err2d0 > 0.0) chi2 = d02/err2d0;
  // Return if track outside 3 sigma
  if ( chi2 > 9. ) return false;  

  // remove track from hessian
  double invs = -1.0;
  invs *= wg*2.0/err2d0;

  double unitVectStd[3];
  unitVectStd[0] = unitVect.x();
  unitVectStd[1] = unitVect.y();
  unitVectStd[2] = unitVect.z(); 
  
  double vd0Std[3];
  vd0Std[0] = vd0.x();
  vd0Std[1] = vd0.y();
  vd0Std[2] = vd0.z();
  for(int k = 0; k < 3; k++) {
    double vdu = 0.;
    for (int l = 0; l < 3; l++) {
      double delta_kl = (k == l) ? 1.0 : 0.0;
      double val = delta_kl - unitVectStd[k] * unitVectStd[l];
      vdu += vd0Std[l] * val * invs;
      if(l <= k) {
        hess(k,l) += val * invs;
      }
    }
    d0vec[k] += vdu;
  }
  dchi2wg += wg*d02/err2d0;
  return true;
  
}


void PVOfflineRecalculate::print_stats() 
{
  info() << "Statistics for PV recalculation (in case of any failure original PV is returned)" << endreq;
  info() << "=================================================================================" << endreq;

  if (  m_counter_count.size() < m_counter_name.size() ) return;

  for ( unsigned int i = 0; i < m_counter_count.size(); i++ ) {
    printRat(m_counter_name[i], m_counter_count[i], m_counter_count[0]);
  }
  info() << "--------------------------------------------------------------------------------" << endreq;
  info() << endreq;  
}

void PVOfflineRecalculate::printRat(std::string& mes, int a, int b) {

  double rat = 0.;
  if(b>0) rat = 1.0*a/b;

  // reformat message
  unsigned int len = 50;
  std::string pmes = mes;
  while(pmes.length() < len) {
    pmes+=" ";
  }
  pmes+= " : ";

  info() << pmes << format(" %8.5f ( %7d / %8d )", rat,a,b) << endreq; 

}
