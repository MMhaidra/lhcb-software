// $Id: CombineTaggersTDR.cpp,v 1.1 2005-07-06 00:36:02 musy Exp $
#include "CombineTaggersTDR.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CombineTaggersTDR v1.3
//
// 2005-07-05 : Marco Musy 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<CombineTaggersTDR>          s_factory ;
const        IToolFactory& CombineTaggersTDRFactory = s_factory ; 

//=============================================================================
CombineTaggersTDR::CombineTaggersTDR( const std::string& type,
				      const std::string& name,
				      const IInterface* parent ) :
  GaudiTool ( type, name, parent ) { 
  declareInterface<ICombineTaggersTool>(this);
}
CombineTaggersTDR::~CombineTaggersTDR(){}
StatusCode CombineTaggersTDR::initialize() { return StatusCode::SUCCESS; }
StatusCode CombineTaggersTDR::finalize() { return StatusCode::SUCCESS; }

//=============================================================================
int CombineTaggersTDR::combineTaggers(FlavourTag& theTag, 
				      std::vector<Tagger*>& vtg ){
  int catt=0;
  double tagdecision=0;
  std::vector<int> itag;
  //itag is now the individual B-flavour guess of each separate tagger:
  for( int j=0; j!=6; j++ ) itag.push_back(0);
  for( int j=1; j!=6; j++ ) itag.at(j) = (vtg.at(j-1))->decision();
 
  int ic=0;
  if( itag.at(1) ) ic+=1000;
  if( itag.at(2) ) ic+= 100;
  if( itag.at(3) ) ic+=  10;
  if( itag.at(4) ) ic+=   1;

  //if pion same side fill only category nr7
  //(pion SS is only considered when no other tagger is present)
  if( (vtg.at(4-1))->type() == Tagger::SS_Pion
      && itag.at(4) && ic>1 ) ic--;

  if(ic==1000 || ic== 1100) {    // only muon
    catt=1; tagdecision = itag.at(1);
  }
  else if(ic== 100) {    // only electron 
    catt=2; tagdecision = itag.at(2);
  }
  else if(ic==  10) {    // only kaon
    catt=3; tagdecision = itag.at(3);
  }
  else if(ic==1010) {   // mu-k (muon and kaon are both present)
    catt=4; tagdecision = itag.at(1)+itag.at(3);
  }
  else if(ic== 110) {   // e-k 
    catt=5; tagdecision = itag.at(2)+itag.at(3);
  }
  else if(ic==   0 && itag.at(5) ) { // vertex charge
    catt=6; tagdecision = itag.at(5);
  }
  else if(ic==   1) {   // Same Side pion or kaon only
    catt=7; tagdecision = itag.at(4);
  }
  else if(ic==1001) {   // mu-kS 
    catt=8; tagdecision = itag.at(1)+itag.at(4);
  }
  else if(ic== 101) {   // e-kS 
    catt=9; tagdecision = itag.at(2)+itag.at(4);
  }
  else if(ic==  11) {   // k-kS 
    catt=10; tagdecision= itag.at(3)+itag.at(4);
  }
  else if(ic==1011) {   // mu-k-kS 
    catt=11; tagdecision= itag.at(1)+itag.at(3)+itag.at(4);
  }
  else if(ic== 111) {   // e-k-kS 
    catt=12; tagdecision= itag.at(2)+itag.at(3)+itag.at(4);
  }
  if(tagdecision) tagdecision = tagdecision>0 ? 1 : -1;
  if(!tagdecision) catt = 0;

  ///fill FlavourTag object
  if(      tagdecision ==  1 ) theTag.setDecision( FlavourTag::bbar );
  else if( tagdecision == -1 ) theTag.setDecision( FlavourTag::b );
  else theTag.setDecision( FlavourTag::none );
  theTag.setCategory( catt );
  theTag.setOmega( 0.5 );
  //fill in taggers info into FlavourTag object
  for( int j=0; j!=5; j++ ) if(itag.at(j)) theTag.addTagger(*(vtg.at(j))); 

  return catt;
}
//=============================================================================
