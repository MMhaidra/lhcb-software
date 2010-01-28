// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "RecVertices2Particles.h"

using namespace Gaudi::Units ;
using namespace LHCb ;
using namespace std ;

//-----------------------------------------------------------------------------
// Implementation file for class : RecVertices2Particles
//
// 2009-april-14 : Neal Gauvin
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( RecVertices2Particles );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RecVertices2Particles::RecVertices2Particles( const std::string& name,
                ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
    , m_vFit(0)
    , m_nEvents(0)
    , pi(3.1415926)
{
  declareProperty("Prey", m_Prey = "~chi_10" );
  //Bc+ Mass 6.286GeV
  //declareProperty("PreyMinMass", m_PreyMinMass = 6.3*Gaudi::Units::GeV );
  declareProperty("PreyMinMass", m_PreyMinMass = 0.0 );
  declareProperty("PreyMaxMass", m_PreyMaxMass = 14.*Gaudi::Units::TeV );
  declareProperty("RMin", m_RMin = 0.3*Gaudi::Units::mm );
  declareProperty("RMax", m_RMax = 50.*Gaudi::Units::m );
  declareProperty("NbTracks", m_nTracks = 1 );//~ nb B meson max # of tracks
  declareProperty("RecVerticesLocation", m_RVLocation );
  //"BlindVertexFitter", "OfflineVertexFitter"
  declareProperty("VertexFitter", m_Fitter = "none"  );
  declareProperty("KeepLowestZ", m_KeepLowestZ = false  );
  declareProperty("IsolationDistance", m_Dist = 0.0*mm  );
  declareProperty("RemVtxFromDet", m_RemVtxFromDet = 0  );
  //"", "FromUpstreamPV", "FromBeamLine"
  declareProperty("RCutMethod", m_RCut = "FromUpstreamPV" );
  declareProperty("BeamLineLocation", 
		  m_BLLoc = "HLT/Hlt2LineDisplVertices/BeamLine");
}
//=============================================================================
// Destructor
//=============================================================================
RecVertices2Particles::~RecVertices2Particles() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode RecVertices2Particles::initialize() {

  StatusCode sc = DVAlgorithm::initialize(); 
  if ( sc.isFailure() ) return sc;

  debug() << "==> Initialize the RecVertices2Particles algorithm" << endmsg;

  //RecVerticesLocation
  if( m_RVLocation.empty() ) m_RVLocation.push_back("Rec/Vertices/RV");

  //Prey IDs
  const ParticleProperty* Prey = ppSvc()->find( m_Prey );
  if ( !Prey ) { //
    err() << "Cannot find particle property for " << m_Prey << endmsg ;
    return StatusCode::FAILURE;
  }
  m_PreyID = Prey->particleID( ); 
  m_PreyPID = m_PreyID.pid ();

  info() << "--------------------------------------------------------"<<endmsg;
  info() <<"RecVertices2Particles takes every RecVertex from "
	 << m_RVLocation <<" and turns them into Particles if they satisfies "
	 <<"the following criterias :"<< endmsg ;
  info() <<"RecVertex has no backward tracks"<< endmsg ;
  if(!m_KeepLowestZ) 
    info() <<"RecVertex has not the lowest z position (PV)"<< endmsg ;
  if( m_RemVtxFromDet < 0 )
    info()<<"Remove RecVertex if in detector material"<< endmsg;
  if( m_RemVtxFromDet > 0 )
    info()<<"Remove RecVertex if closer than " << m_RemVtxFromDet 
          <<"mm from detector material"<< endmsg;
  info() <<"Reconstructed Mass of the RecVertex"<< endmsg ;
  info() <<"Min Mass : " << m_PreyMinMass/GeV <<" GeV"<< endmsg ;
  info() <<"Max Mass : " << m_PreyMaxMass/GeV <<" GeV"<< endmsg ;
  info() <<"Minimum number of tracks at the RecVertex : "
	 << m_nTracks <<" tracks."<< endmsg ;
  info()<< "The radial displacement is ";
  if( m_RCut == "FromUpstreamPV" ){
    info() << "computed with respect to the upstream PV of PV3D." << endmsg;
  } else if( m_RCut == "FromBeamLine" ){
    info() << "computed with respect to the beam line given at " 
	   << m_BLLoc << endmsg;
  } else {
    info() << "computed with respect to (0,0,z) in the global LHCb frame" 
	    << endmsg;
    info()<< "THIS OPTION SHOULD NOT BE USED ON REAL DATA !!" 
	   << endmsg;
  }
  info() <<"Min R    : " << m_RMin/mm <<" mm"<< endmsg ;
  info() <<"Max R    : " << m_RMax/mm <<" mm"<< endmsg ;
  info() <<"Particles will be made with "<< m_Prey <<" id" << endmsg ;
  info() <<"Vertex fitter used : "<< m_Fitter << endmsg ;
  info() << "--------------------------------------------------------"<<endmsg;

  //Initialize Vertex Fitter
  if( m_Fitter != "none" ){
    m_vFit = vertexFitter(m_Fitter);
  } 

  //To recreate track measurements
  //   m_measProvider = tool<IMeasurementProvider>
  //     ( "MeasurementProvider","MeasProvider", this );

  if( m_RemVtxFromDet != 0 ){
    //Get detector elements
    IDetectorElement* lhcb = getDet<IDetectorElement>
      ( "/dd/Structure/LHCb/BeforeMagnetRegion/Velo" );
    m_lhcbGeo = lhcb->geometry();

    // Get Transport Service
    if( m_RemVtxFromDet > 0 ) 
      m_transSvc = svc<ITransportSvc>( "TransportSvc", true  );
  }

  //Initialize the beam line
  if( m_RCut !="FromBeamLine" ) m_BeamLine = new Particle();

  //Set beam line to z axis
  if( m_RCut =="" ){
    m_BeamLine->setReferencePoint( Gaudi::XYZPoint( 0., 0., 0. ) );
    m_BeamLine->setMomentum( Gaudi::LorentzVector( 0., 0., 1., 0. ) );
  } 

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
 StatusCode RecVertices2Particles::execute() {

  setFilterPassed(false);   // Mandatory. Set to true if event is accepted.
  ++m_nEvents;
  if( msgLevel(MSG::DEBUG) )
    debug() << "==> Execute the RecVertices2Particles algorithm, event "
	    << m_nEvents << endmsg;

  //Check track and Particle content
  //PrintTrackandParticles();
  //return StatusCode::SUCCESS;

  //Set the beam line
  if( m_RCut=="FromBeamLine" ){
    if( exist<Particles>( m_BLLoc ) ){
      Particles* BL = get<Particles>( m_BLLoc );      
      m_BeamLine = *(BL->begin());
      if( msgLevel(MSG::DEBUG) )
	debug()<<"Beam line position "<< m_BeamLine->referencePoint()
	       <<" direction " << m_BeamLine->momentum() << endmsg;
    } else {
      warning()<<"No Beam line found at "<< m_BLLoc << endmsg;
      return StatusCode::SUCCESS;
    }
  } else if( m_RCut=="FromUpstreamPV" ){
    const RecVertex * UpPV = GetUpstreamPV();
    if(UpPV == NULL) return StatusCode::SUCCESS;
    m_BeamLine->setReferencePoint( UpPV->position() );
    m_BeamLine->setMomentum( Gaudi::LorentzVector( 0., 0., 1., 0. ) );
    if(msgLevel(MSG::DEBUG))
      debug() <<"Upstream PV position "<< UpPV->position() << endmsg;
  }


  //Retrieve data Particles from Desktop.
  Particle::ConstVector Particles = desktop()->particles();
  int size = Particles.size() ;
  if( msgLevel(MSG::DEBUG) )
    debug()<< "Number of Particles in TES " << size << endmsg;
  plot( size,"NbofPartsTES", 0,500 );


  //Retrieve Reconstructed Vertices
  RecVertex::ConstVector RV;
  GetRecVertices( RV );
  if( RV.size() < 1 ) return StatusCode::SUCCESS;


  //The PV is assumed to be the rec vertex with lowest z
  //sort rec vertex according to z position
  sort( RV.begin(), RV.end(), RecVertices2Particles::CondRVz() );


  //Print out displaced vertices and change them into Particles
  Particle::ConstVector RecParts;
  RecVertex::ConstVector::const_iterator i = RV.begin();
  RecVertex::ConstVector::const_iterator iend = RV.end();
  for( ; i != iend; ++i) {
    const RecVertex* rv = *i;
    const SmartRefVector< Track > & Tracks = rv->tracks();
    size = Tracks.size();
    double r = RFromBL( rv->position() ); //R to beam line
    if( msgLevel(MSG::DEBUG) ){
      debug() << "------------- Reconstructed Vertex -----------" << endmsg;
      debug() << "Position " << rv->position() <<" R "<< r
	      << " Number of associated tracks " << size 
	      << " Chi2/NDoF " << rv->chi2PerDoF() << endmsg;
    }

    //Do not change the PV into a particle ! (if any !)
    if( !m_KeepLowestZ && i == RV.begin() ) continue;

    //PVs have no backward tracks
    if( HasBackwardTracks(rv) ){ 
      if( msgLevel(MSG::DEBUG) )
        debug() <<"RV has a backward track --> disguarded !"<< endmsg;
      continue;
    }

    if( !IsIsolated( rv, RV ) ){
      if( msgLevel(MSG::DEBUG) )
        debug() <<"RV is not isolated  --> disguarded !"<< endmsg;
      continue;
    }

    //Cuts !!
    if( size < m_nTracks || r < m_RMin || r > m_RMax ){
      if( msgLevel(MSG::DEBUG) )
        debug() <<"RV did not passed the cuts  --> disguarded !"<< endmsg;
      continue;
    }

    //Turn it into a Particle !
    //Eventually don't keep it if close to/in detector material
    if( !RecVertex2Particle( rv, Particles, RecParts ) ) continue;

    //Study Prey Composition
    // ***Don't forget to turn on the IMeasurementProvider tool***
    if( false && r > m_RMin && r < m_RMax ) 
      StudyPreyComposition( RecParts.back() );

  }
  size = RecParts.size();
  if( msgLevel(MSG::DEBUG) )
    debug()<<"# of Preys " << size << endmsg;
  plot( size, "NbofPreys", 0, 20 );
  
  //Save Preys from Desktop to the TES.
  desktop()->saveDesktop() ;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RecVertices2Particles::finalize() {

  debug() << "==> Finalize" << endmsg;
  return DVAlgorithm::finalize(); 
}

//=============================================================================
// Get the reconstructed vertices
//=============================================================================
void RecVertices2Particles::GetRecVertices( RecVertex::ConstVector & RV )
{
  
  vector<string>::iterator itName;
  for( itName = m_RVLocation.begin(); itName != m_RVLocation.end();
       ++itName ) {
    RecVertices* tRV = get<RecVertices>( *itName );
    if(!tRV) {
      warning() << "No reconstructed vertices found at location: " 
                << *itName << endmsg;
      continue;
    }
    RecVertices::const_iterator i = tRV->begin();
    for( ; i != tRV->end(); ++i ) RV.push_back( *i );
  }
  unsigned int size = RV.size();
  if( msgLevel(MSG::DEBUG) )
    debug()<< "Number of Reconstructed Vertices " << size << endmsg;
  plot( size,"NbofRecVtx", 0,50 );
  
}

//=============================================================================
// Get the upstream PV
//=============================================================================
const RecVertex * RecVertices2Particles::GetUpstreamPV(){

  const RecVertex::Container * PVs = desktop()->primaryVertices();
  const RecVertex * upPV = NULL;
  double tmp = 1000;
  for ( RecVertex::Container::const_iterator i = PVs->begin(); 
	i != PVs->end() ; ++i ){
    double z = (*i)->position().z();
    if( z < tmp ){
      tmp = z;
      upPV = (*i);
    } 
  }
  return upPV;
}


//=============================================================================
// Turn RecVertices into Particles (from Parts) saved in RecParts
//=============================================================================
bool RecVertices2Particles::RecVertex2Particle( const RecVertex* rv, 
					Particle::ConstVector & Parts, 
					Particle::ConstVector & RecParts ){
  

  Gaudi::LorentzVector mom;
  SmartRefVector< Track >::const_iterator iVtx = rv->tracks().begin();
  int endkey = rv->tracks().back()->key();
  
  if( m_Fitter == "none" ){

    //Create an decay vertex
    const Gaudi::XYZPoint point = rv->position();
    Vertex tmpVtx = Vertex( point );
    tmpVtx.setNDoF( rv->nDoF());
    tmpVtx.setChi2( rv->chi2());
    tmpVtx.setCovMatrix( rv->covMatrix() );
    
    //Create a particle
    Particle tmpPart = Particle( m_PreyID );
    //Fix end vertex
    tmpPart.setEndVertex( &tmpVtx );
    tmpPart.setReferencePoint( point );
    
    //Find all particles that have tracks in RecVertex
    Particle::ConstVector::const_iterator jend = Parts.end();
    for ( Particle::ConstVector::const_iterator j = Parts.begin();
          j != jend;++j) {
      if( (*j)->proto()->track() == NULL ) continue;
      const Track * tk = (*j)->proto()->track();
      while( ((*iVtx)->key() < tk->key()) && (*iVtx)->key() != endkey ){
        ++iVtx;
      }
      if( (*iVtx)->key() == tk->key() ){ 
        //debug()<<"Track should be the same "<< (*iVtx)->momentum() <<" "
        //     << tk->momentum() << endmsg;
        //debug() <<"Track type "<< tk->type() << endmsg;
        if( (*iVtx)->key() != endkey ) ++iVtx; 
        tmpVtx.addToOutgoingParticles ( *j );
        tmpPart.addToDaughters( *j );
        mom += (*j)->momentum();
        continue;
      }
    }
    //Should be 100%
    //if the efficiency is less than 100%, you can work with pion with 
    //default pt of 400MeV. See Hlt2SelHidValley for implementation.
    //double eff = 100.*(double)tmpPart.daughters().size()/
    //(double)rv->tracks().size();
    //plot( eff, "BestTrk2PartEff", 0., 101. );
    //debug()<<"Found "<< Daughters.size() <<" related particles."<< endmsg;
    //Do I really care about the number of tracks found ?
    //if( Daughters.size() < m_nTracks ) return;
    

    //Fill momentum and mass estimate
    tmpPart.setMomentum( mom );
    tmpPart.setMeasuredMass( mom.M() );
    tmpPart.setMeasuredMassErr( 0 );
    if( !TestMass( tmpPart ) ){
      if( msgLevel(MSG::DEBUG) )
        debug() <<"Particle did not passed the mass cut --> disguarded !"
                << endmsg;
      return false; 
    }
    
    Gaudi::SymMatrix4x4 MomCovMatrix = 
      Gaudi::SymMatrix4x4( ROOT::Math::SMatrixIdentity() );
    tmpPart.setMomCovMatrix( MomCovMatrix );
    Gaudi::SymMatrix3x3 PosCovMatrix = 
      Gaudi::SymMatrix3x3( ROOT::Math::SMatrixIdentity() );
    tmpPart.setPosCovMatrix( PosCovMatrix );
    Gaudi::Matrix4x3 PosMomCovMatrix;
    tmpPart.setPosMomCovMatrix( PosMomCovMatrix );

    //Remove if found to be in detector material
    if( m_RemVtxFromDet && RemVtxFromDet( tmpPart ) )
      return false;
    
 
    //Save Rec Particle in the Desktop
    RecParts.push_back( desktop()->keep( &tmpPart ) );
    
  } else {

    Particle::ConstVector Daughters;
    Particle::ConstVector::const_iterator jend = Parts.end();
    for ( Particle::ConstVector::const_iterator j = Parts.begin();
	  j != jend;++j) {
      if( (*j)->proto()->track() == NULL ) continue;
      const Track * tk = (*j)->proto()->track();
      while( ((*iVtx)->key() < tk->key()) && (*iVtx)->key() != endkey ){
        ++iVtx;
      }
      if( (*iVtx)->key() == tk->key() ){ 
        //debug()<<"Track should be the same "<< (*iVtx)->momentum() <<" "
        //     << tk->momentum() << endmsg;
        //debug() <<"Track type "<< tk->type() << endmsg;
        if( (*iVtx)->key() != endkey ) ++iVtx; 
        Daughters.push_back( *j );
        continue;
      }
    }
    
    //Make particle !
    Vertex tmpVtx; 
    Particle tmpPart;
    if( !( m_vFit->fit( Daughters, tmpVtx, tmpPart ) ) ){
      Warning("Fit error."+ m_Fitter +" not able to create Particle !"); 
      return false;
    }

    if( !TestMass( tmpPart ) ){
      if( msgLevel(MSG::DEBUG) )
        debug() <<"Particle did not passed the mass cut --> disguarded !"
                << endmsg;
      return false;   
    }

    //Remove if found to be in detector material
    if( m_RemVtxFromDet && RemVtxFromDet( tmpPart ) )
      return false;
    
    //Save Rec Particle in the Desktop
    RecParts.push_back( desktop()->keep( &tmpPart ) );
     
  }
  setFilterPassed(true);

  if ( true ){
    if( msgLevel(MSG::DEBUG) ){
      debug() << "------------- Reconstructed Particle -----------" << endmsg;
      debug() << "Mass " << RecParts.back()->measuredMass()/GeV << "+/-" 
              << RecParts.back()->measuredMassErr() <<" GeV" << endmsg;
      debug() << "Chi2 " << RecParts.back()->endVertex()->chi2() 
	      << " Chi2/NDoF " << RecParts.back()->endVertex()->chi2PerDoF()
	      <<endmsg;
      Gaudi::XYZPoint pos = RecParts.back()->endVertex()->position();
      debug() << "Pos of end vertex " << pos << " R "<< pos.rho() << endmsg;
      debug() << "Number of associated tracks " 
	      << RecParts.back()->daughtersVector().size() << endmsg;
      debug() << "----------------------------" << endmsg;
    }
  }
  return true;
}

//=============================================================================
//  Loop on the daughter track to see if there is a backward track
//=============================================================================

bool RecVertices2Particles::HasBackwardTracks( const RecVertex* RV ){
  SmartRefVector< Track >::const_iterator i = RV->tracks().begin();
  SmartRefVector< Track >::const_iterator iend = RV->tracks().end();
  for( ; i != iend; ++i ){
    if ( (*i)->checkFlag( Track::Backward ) ) return true;
  }
  return false;
}

//=============================================================================
//  Check if RV is isolated from other RVs
//=============================================================================

bool RecVertices2Particles::IsIsolated( const RecVertex* rv, RecVertex::ConstVector & RV ){

  if( m_Dist < 0.01 ) return true;

  //Retrieve Displaced Vertices
  RecVertex::ConstVector::const_iterator i = RV.begin();
  RecVertex::ConstVector::const_iterator iend = RV.end();
  for( ; i != iend; ++i) {
    if( (*i) == rv ) continue;
    if( VertDistance(rv->position(),(*i)->position()) < m_Dist ) return false;
  }
  return true;
}

//=============================================================================
//  Remove RV if found to be in detector material
//
// if m_RemVtxFromDet = 0  : disabled
// if m_RemVtxFromDet < 0  : remove reco vtx if in detector material
// if m_RemVtxFromDet > 0  : remove reco vtx if rad length along z 
//                           from (decay pos - m_RemVtxFromDet) to 
//                           (decay pos + m_RemVtxFromDet)  is > threshold
//
//=============================================================================

bool RecVertices2Particles::RemVtxFromDet( Particle & P ){

  double threshold = 1e-10;
  const Vertex * RV = P.endVertex();

  if( m_RemVtxFromDet < 0 ){

    IGeometryInfo* start = 0;
    ILVolume::PVolumePath path ;
    
    //Get the physical volume, if any.
    StatusCode sc = m_lhcbGeo->fullGeoInfoForPoint
      ( RV->position(), 1000, start, path ) ;
    if ( sc.isFailure() ) { 
      warning()<<"Impossible to get any physical volume related to point "
               << RV->position() <<endmsg;
      return false; 
    } 
    int size = path.size();
    plot( size, "NbofDetV", 0, 5 );
    if( msgLevel(MSG::DEBUG) )
      debug()<<"Found "<< size <<" physical volumes related to point "
             << RV->position() <<endmsg;
    
    const IPVolume* pvlast = 0;
    if ( !path.empty() ) { pvlast = path.back(); }
    
    const ILVolume * lvlast = 0;
    if ( 0 != pvlast ) { lvlast = pvlast->lvolume(); }
    
    const Material* matlast = 0 ;
    if ( 0 != lvlast ) { matlast = lvlast->material(); }
    
    if ( 0 != matlast )  { 
      if( msgLevel(MSG::DEBUG) ){
        debug()<<"Physical volume related to point "<< RV->position() <<endmsg;
        debug()<< matlast << endl;
      }
      //if( matlast->name() == "Vacuum" ) return false;
      return true;
    } 
  } //end of <0 condition
  else if( m_RemVtxFromDet > 0 ){

    const Gaudi::XYZPoint pos = RV->position();
    const Gaudi::XYZPoint nvec = Normed( P.momentum(), m_RemVtxFromDet );
    const Gaudi::XYZPoint start = Minus( pos, nvec );
    const Gaudi::XYZPoint end = Plus( pos, nvec );

    //Compute the radiation length
    if( m_lhcbGeo == 0 ){ 
      warning()<<"IGeometryInfo* m_lhcbGeo is broken"<< endmsg; return true; }

    IGeometryInfo* dum = 0;
    double radlength = m_transSvc->distanceInRadUnits
      ( start, end, 1e-35, dum, m_lhcbGeo );

    plot( radlength, "RVRadLength", 0, 0.01);
    if( msgLevel(MSG::DEBUG) )
      debug()<<"Radiation length from "<< start <<" to "
	     << end <<" : "<< radlength 
	     <<" [mm]" << endmsg;


    if( radlength > threshold ){
      if( msgLevel(MSG::DEBUG) )
        debug()<<"RV is too closed to a detector material --> disguarded !"
               << endmsg;
      return true;
    }
    

  } //end of >0 condition

  return false;
}

//=============================================================================
// Apply some cuts on the Particle or Vertex
//=============================================================================
bool RecVertices2Particles::TestMass( Particle & part ){
  double mass = part.measuredMass();
  if( mass > m_PreyMinMass && mass < m_PreyMaxMass ) return true;
  return false;
}
bool RecVertices2Particles::TestMass( const Particle * part ){
  double mass = part->measuredMass();
  if( mass > m_PreyMinMass && mass < m_PreyMaxMass ) return true;
  return false;
}

//=============================================================================
// Get radial distance to Beam Line
//=============================================================================
double RecVertices2Particles::RFromBL( const Gaudi::XYZPoint& p ){
  
  //intersection of the beam line with the XY plane, 
  //find the lambda parameter of the line.
  double lambda = (p.z() - m_BeamLine->referencePoint().z()) /
    m_BeamLine->momentum().z();

  //find x and y of intersection point
  double x = m_BeamLine->referencePoint().x() 
    + lambda * m_BeamLine->momentum().x();
  double y = m_BeamLine->referencePoint().y() 
    + lambda * m_BeamLine->momentum().y();
  
  //return distance to the intersection point 
  x -= p.x(); y -= p.y();
  return sqrt( x*x + y*y );
}

//=============================================================================
// Distance between two vertices
//=============================================================================
double RecVertices2Particles::VertDistance( const Gaudi::XYZPoint & v1, 
					    const Gaudi::XYZPoint & v2){
  return sqrt(pow(v1.x()-v2.x(),2)+pow(v1.y()-v2.y(),2)+pow(v1.z()-v2.z(),2));
}


//============================================================================
// Angle between two tracks   :   tracks are supposed to be straight lines
//============================================================================
double RecVertices2Particles::Angle( const Gaudi::LorentzVector & a, 
			     const Gaudi::LorentzVector & b ){
  return Angle( a.Vect(), b.Vect() );  
};

double RecVertices2Particles::Angle( const Gaudi::XYZVector & a, 
			     const Gaudi::XYZVector & b ){
  return acos( a.Dot(b) / sqrt( a.Mag2() * b.Mag2() ) );
}


//============================================================================
// Basic operations between two Gaudi::XYZPoint
//============================================================================
Gaudi::XYZPoint RecVertices2Particles::Plus( const Gaudi::XYZPoint & p1, 
				  const Gaudi::XYZPoint & p2 ){
  return Gaudi::XYZPoint( p1.x()+p2.x(), p1.y()+p2.y(), p1.z()+p2.z() );
}

Gaudi::XYZPoint RecVertices2Particles::Minus( const Gaudi::XYZPoint& p1, 
				   const Gaudi::XYZPoint & p2 ){
  return Gaudi::XYZPoint( p1.x()-p2.x(), p1.y()-p2.y(), p1.z()-p2.z() );
}

double RecVertices2Particles::Mult( const Gaudi::XYZPoint & p1, 
			 const Gaudi::XYZPoint & p2 ){

  return p1.x()*p2.x() + p1.y()*p2.y() + p1.z()*p2.z();
}

double RecVertices2Particles::Mult(  const Gaudi::LorentzVector & p1, 
			  const Gaudi::LorentzVector & p2 ){

  return p1.x()*p2.x() + p1.y()*p2.y() + p1.z()*p2.z();
}

Gaudi::XYZPoint RecVertices2Particles::Normed( const Gaudi::LorentzVector & P,
                                       double range ){
  double norm = range/Norm( P );
  return Gaudi::XYZPoint( P.x()*norm, P.y()*norm, P.z()*norm );
}

double RecVertices2Particles::Norm( const Gaudi::LorentzVector & P ){
  return sqrt( pow(P.x(),2) + pow(P.y(),2) + pow(P.z(),2) );
}

double RecVertices2Particles::Norm( const Gaudi::XYZPoint & P ){
  return sqrt( pow(P.x(),2) + pow(P.y(),2) + pow(P.z(),2) );
}

double RecVertices2Particles::Norm( const Gaudi::XYZVector & P ){
  return sqrt( pow(P.x(),2) + pow(P.y(),2) + pow(P.z(),2) );
}


//=============================================================================
// Print out Track and Particle content
//=============================================================================
void RecVertices2Particles::PrintTrackandParticles(){

  //TrackLocation::Default = Rec/Track/Best 
  //(Upstream,Long,Ttrack,Downstream, Velo)
  Tracks* BestTrks = get<Tracks>( TrackLocation::Default );
  debug()<<"Dumping "<< TrackLocation::Default <<" Track content, size "
	 << BestTrks->size() <<endmsg;
  for(Track::Container::const_iterator itr = BestTrks->begin(); 
      BestTrks->end() != itr; ++itr) {
    const Track* trk = *itr;
    string s = "False !";
    if ( trk->checkFlag( Track::Backward ) ) s = "True !";
    debug()<<"Track key "<< trk->key() //<<" slope "<< trk->slopes() 
	   <<" type "<<trk->type()<<" Is backward ? "<< s 
	   <<" momentum "<< trk->momentum() <<" pt "<< trk->pt() <<endmsg;
  }

  debug()<<"---------------------------------------------------------"<<endmsg;
  const Particle::ConstVector& inputParts = desktop()->particles();
  debug()<<"Dumping Particle content, size "
	 << inputParts.size() <<endmsg;
  for ( Particle::ConstVector::const_iterator j = inputParts.begin();
	j != inputParts.end();++j) {
    if( (*j)->proto()->track() == NULL ) continue;
    const Track * trk = (*j)->proto()->track();
    debug()<<"Particle id "<< (*j)->particleID().pid() <<" key "
	   << (*j)->key() <<" Mass "<< (*j)->measuredMass()/1000. << " GeV" 
      //<<" slope "<< (*j)->slopes()
      //<<" Mass "<< (*j)->momentum().M()
	   <<" type "<< trk->type() 
	   <<" momentum "<< (*j)->momentum() <<" pt "<< (*j)->pt() << endmsg;
  }
}


//=============================================================================
// Study in details the composition of the Prey's daughters (id..)
//=============================================================================
void RecVertices2Particles::StudyPreyComposition( const Particle * p ){

  if( m_measProvider == 0){
    warning()<<"Turn on the IMeasurementProvider tool"<< endmsg;
    return;
  } 

  Gaudi::XYZPoint pos = p->endVertex()->position();
  if( msgLevel(MSG::DEBUG) ){
    debug()<<"----------------------------------------" << endmsg;
    debug() <<"Particle Mass " << p->measuredMass() << ", Pos of end vertex " 
	    << pos << ", R "<< pos.rho() << ", Number of associated tracks " 
	    << p->daughtersVector().size() << endmsg;
    debug()<<"--------------- Composition -----------" << endmsg;
  }

  int nbe = 0; //count number of electrons
  int nbz = 0; //nb of tracks with first z measurement after RV position
  int nbm = 0; //nb of 2 by 2 null rec masses 
  int nbpairs = 0;      //Nb of daughter pairs
  double meanangle = 0; //mean angle between pair of particles
  double maxangle = 0;  //max angle between pair of particles

  //Let's loop on the daughters
  SmartRefVector< Particle >::const_iterator iend = p->daughters().end();
  SmartRefVector< Particle >::const_iterator i = p->daughters().begin();
  for( ; i != iend; ++i ){
    const Particle * d = i->target() ;
    int pid = d->particleID().pid();
    if( abs(pid) == 11 ) ++nbe;
    
    //Let's find the position of the first hit
    if( d->proto()->track() == NULL ) continue;
    const Track * tk = d->proto()->track();
    StatusCode sc = m_measProvider -> load( const_cast<Track &>( *tk ) );
    //if(!sc) debug()<<"Unable to recreate track measurements"<< endmsg;
    double z = tk->measurements().front()->z();
    debug()<<"ID "<< pid <<", z first meas. "
	   << z << endmsg;
    if( z < pos.z() ) ++nbz;
    
    //let's take the tracks 2 by 2 and look if there is some null rec mass
    SmartRefVector< Particle >::const_iterator j = i; ++j;
    while( j != iend ){
      //Compute angle between the 2 tracks
      ++nbpairs;
      double angle = Angle( j->target()->momentum(), d->momentum() );
      if( angle > maxangle  ) maxangle = angle;
      meanangle += angle;
      const Gaudi::LorentzVector l = d->momentum() + j->target()->momentum();
      plot( l.M()/1000., "PreyDgts2b2Mass", 0, 6 );
      plot( l.M()/1000., "PreyDgts2b2Mass2", 0, 0.5 );
      if( l.M() < 100 ) ++nbm;

      //debug()<<"Rec mass "<< l.M() << endmsg;
      ++j;
    }
    
  }
  meanangle = double( meanangle/nbpairs );
  //if( maxangle > pi ) maxangle -= pi;

  plot( nbe, "PreyDaughtersNbEl", 0, 10 );
  plot( nbz, "PreyZDgtsBefRV", 0, 10 );
  plot( nbm, "PreyDgts2b2MassNb", 0, 10);
  plot( maxangle, "PreyDgtsMaxAngle", 0., pi/2);
  plot( meanangle, "PreyDgtsMeanAngle", 0., pi/4 );
  //Study Boost
  plot( p->momentum().Gamma(), "PreyGamma", 0, 30. );
  plot( p->momentum().BoostToCM().z(), "PreyGammaZ", -1., -0.93 );

  debug() <<"Mean Prey Gamma "<< p->momentum().Gamma() <<", z boost "
	  << p->momentum().BoostToCM().z() << endmsg;

}
