// $Id: BTaggingTool.h,v 1.1 2005-07-04 08:20:05 pkoppenb Exp $
#ifndef USER_BTAGGINGTOOL_H 
#define USER_BTAGGINGTOOL_H 1

// from STL
#include <fstream>
#include <string>
#include <math.h>
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event 
#include "Event/ProtoParticle.h"
#include "Kernel/IVertexFitter.h"
#include "Event/EventHeader.h"
#include "Event/FlavourTag.h"
#include "Event/TrgDecision.h"
#include "Event/HltScore.h"
#include "Event/TamperingResults.h"
// from RecoTools
#include "DaVinciTools/IGeomDispCalculator.h"
#include "Kernel/IPhysDesktop.h"
#include "FlavourTagging/ISecondaryVertexTool.h"
#include "FlavourTagging/INNetTool.h"
#include "FlavourTagging/IBTaggingTool.h" 
#include "FlavourTagging/ITagger.h" 

/** @class BTaggingTool BTaggingTool.h 
 *
 *  Tool to tag the B flavour
 *
 *  @author Marco Musy
 *  @date   05/06/2005
 */

class IGeomDispCalculator;

class BTaggingTool : public GaudiTool,
        virtual public IBTaggingTool {

public: 

  /// Standard constructor
  BTaggingTool( const std::string& type,
		const std::string& name,
		const IInterface* parent );
  virtual ~BTaggingTool( ); ///< Destructor
  StatusCode initialize();    ///<  initialization
  StatusCode finalize  ();    ///<  finalization

  //-------------------------------------------------------------
  virtual FlavourTag* tag( Particle* );
  virtual FlavourTag* tag( Particle*, Vertex* );
  virtual FlavourTag* tag( Particle*, Vertex*, 
			   std::vector<Particle*> );
  //-------------------------------------------------------------

private:
  bool isinTree( Particle*, ParticleVector& );
  StatusCode calcIP( Particle*, Vertex*, double&, double& );
  StatusCode calcIP( Particle*, 
		     VertexVector, double&, double& ) ;
  ParticleVector toStdVector( SmartRefVector<Particle>& );
  ParticleVector FindDaughters( Particle* );
  long trackType( Particle* );
  double pol2(double, double, double );
  double pol3(double, double, double, double );
  double pol4(double, double, double, double, double );

  std::string m_SecondaryVertexToolName, m_CombinationTechnique;

  ISecondaryVertexTool* m_svtool;
  INNetTool* m_nnet;
  IDataProviderSvc* m_eventSvc;
  IGeomDispCalculator *m_Geom;

  IPhysDesktop *m_physd;
  ITagger *m_taggerMu,*m_taggerEle,*m_taggerKaon;
  ITagger *m_taggerKaonS,*m_taggerPionS, *m_taggerVtx ;

  //properties ----------------
  double m_thetaMin;
  double m_IPPU_cut;

  double m_ProbMin;
  double m_VchOmega;

  bool m_RequireL0, m_RequireL1, m_RequireHLT;
  bool m_RequireTrigger, m_RequireL1Tamp;
  bool m_EnableMuonTagger,m_EnableElectronTagger,m_EnableKaonOSTagger;
  bool m_EnableKaonSSTagger,m_EnablePionTagger,m_EnableVertexChargeTagger;
};

//=======================================================================//
#endif // USER_BTAGGINGTOOL_H
