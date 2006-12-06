// $Id: TsaITXSearch.cpp,v 1.1 2006-12-06 14:35:01 mneedham Exp $

// GaudiKernel
#include "GaudiKernel/ToolFactory.h"

#include "TsaITXSearch.h"
#include "SeedParabolaFit.h"
#include "TsaKernel/SeedTrack.h"

#include "TsaKernel/TsaConstants.h"
#include "TsaKernel/IITDataSvc.h"
#include "TsaKernel/STCluster.h"

#include <utility>
#include <algorithm>

// BOOST !
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>
using namespace boost::assign;
using namespace boost;

// macro for member function calls
#define CALL_MEMBER_FN(object,ptrToMember)  (object.*(ptrToMember))

DECLARE_TOOL_FACTORY( TsaITXSearch );

TsaITXSearch::TsaITXSearch(const std::string& type,
                               const std::string& name,
                               const IInterface* parent):
  TsaXSearchBase(type, name, parent),
  m_parabolaFit(0)
{
  declareProperty("itDataSvcType", m_dataSvcType = "ITDataSvc");
  declareProperty("itDataSvcName", m_dataSvcName = "ITDataSvc");
 
  declareProperty("sxCut", m_sxCut = 0.3);
  declareProperty("xsCut", m_xsCut = 0.3);
  declareProperty("x0Cut", m_x0Cut = 1500.);
  declareProperty("sx2Cut",m_sx2Cut = 0.12);
  declareProperty("dthCut", m_dthCut = 0.4);
  declareProperty("tdCut",m_tdCut = 0.2);
  declareProperty("win", m_win = 0.6);
  declareProperty("win2",m_win2 = 5.0);
  declareProperty("nWin",m_nWin = 2);
  declareProperty("nWin2",m_nWin2 = 4);
  declareProperty("nxCut",m_nxCut = 4);
  declareProperty("nT2",m_nT2Conf = 0); 
  declareProperty("deltaX", m_deltaX = 0.3*Gaudi::Units::mm); 
  declareProperty("collectPolicy", m_collectPolicy = "Linear"); 

 
  m_parabolaFit = new SeedParabolaFit(TsaConstants::z0);

};

TsaITXSearch::~TsaITXSearch(){
  // destructer
  delete m_parabolaFit;
}


StatusCode TsaITXSearch::initialize(){

  StatusCode sc = TsaXSearchBase::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize",sc);
  }

  // data service tool !
  m_dataSvc = tool<IITDataSvc>(m_dataSvcType,m_dataSvcName);

  return StatusCode::SUCCESS;
}

StatusCode TsaITXSearch::execute(std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[6]){

//-------------------------------------------------------------------------
//  Search for hits to form seed tracks in X stations
//-------------------------------------------------------------------------

//  std::vector<SeedHit*> hits[6];   //  Hits per layer in X 
  for (int i = 0; i < 6; ++i){
    hits[i].reserve(200);
  }
  loadData(hits);

  std::vector<SeedHit*> select[3];
  for (unsigned int ii = 0; ii < 3; ++ii) select[ii].reserve(8);
  
  for ( int lay1 = 0; lay1 < 2; ++lay1 ) {
    for ( int lay2 = 5; lay2 > 3; --lay2 ) {
 
      std::vector<SeedHit*>::iterator start = hits[lay2].begin();
      for ( std::vector<SeedHit*>::iterator it1 = hits[lay1].begin(); hits[lay1].end() != it1; ++it1 ) {
        SeedHit* hit1 = (*it1);
        double x1 = hit1->x();
        double z1 = hit1->z();

        bool first = true;
        for ( std::vector<SeedHit*>::iterator it2 = start; hits[lay2].end() != it2; ++it2 ) {
          SeedHit* hit2 = (*it2);
          double x2 = hit2->x();
          double z2 = hit2->z();
          double sx = (x2-x1) / (z2-z1);
          if ( sx < m_sxMin ) continue;
          if ( first ) {
            first = false;
            start = it2;
          }
          if ( sx > m_sxMax ) break;

          std::vector<SeedHit*>& skip = hit1->skip();
          std::vector<SeedHit*>::iterator it = std::find(skip.begin(),skip.end(),hit2);
          if (it != skip.end()) continue;

          if ( fabs(x1*m_xsParam - sx) > m_xsCut) continue;
          double dth = atan( sx ) - atan( (x1 - (z1-TsaConstants::zMagnet)*sx)*TsaConstants::oneOverZMagnet );

          if ( fabs( dth ) > m_dthCut ) continue;
   
          for (unsigned int ii = 0; ii < 3; ++ii) select[ii].clear();
 
	  // collect hits in station 3
          for ( int lay = 2; lay < 4; ++lay ) {
            CALL_MEMBER_FN(*this,m_collectFun)(hits[lay],x1,z1,sx, m_win2,select[1]);
	  }
	    
          if (select[1].size() <= (unsigned int) m_nT2Conf) continue;

	  if (lay1 == 0){
            CALL_MEMBER_FN(*this,m_collectFun) (hits[lay1+1],x1,z1,sx, m_win,select[0]);
            std::for_each(select[0].begin(), select[0].end(), std::bind2nd(std::mem_fun(&SeedHit::setUse),false) );
	  }
       
          if (lay2 == 5){
            CALL_MEMBER_FN(*this,m_collectFun)(hits[lay2-1],x1,z1,sx,m_win,select[2]);
            std::for_each(select[2].begin(), select[2].end(), std::bind2nd(std::mem_fun(&SeedHit::setUse),false) );
	  }
        	 
	  collectXHits(hits[lay1], it1, x1, z1, m_win, sx, select[0]);
	  collectXHits(hits[lay2], it2, x2, z1, m_win, sx ,select[2]);

          if (select[0].size()+select[1].size()+select[2].size() < (unsigned int)m_nWin ) continue;
 
	  // flag t3 hits as not used
	  std::for_each(select[1].begin(), select[1].end(), std::bind2nd(std::mem_fun(&SeedHit::setUse),false) );
	                      
          const double dz = z2-z1;
          double csth = sqrt(1.0 + sx*sx);	
	 
          int nhSel = 0;
          SeedHit* hSel = 0;
          SeedHit* iSel[100];
          for ( std::vector<SeedHit*>::iterator it3 = select[1].begin(); select[1].end() != it3; ++it3 ) {
            SeedHit* hit3 = (*it3);
            if ( hit3->use1() && hit3->use2() ) continue;
            double x3 = hit3->x();
            double z3 = hit3->z();
            double ax = ( sx-(x3-x1)/(z3-z1) )/(z2-z3);
            double bx = sx - ax*dz;
            int nhMax[3] = {0, 0, 0};
            SeedHit* ih[3][100];
            for ( int stn = 0; stn < 3; ++stn ) {
      
              int nh = 0;
              for ( std::vector<SeedHit*>::iterator it = select[stn].begin(); select[stn].end() != it; ++it ) {
                SeedHit* hit = (*it);
                if ( hit == hit3 ) continue;
                const double dz = hit->z()-z1;
                double xSeed = dz*(ax*dz + bx) + x1;
                if ( fabs(hit->x() - xSeed)  < m_deltaX ) {
                  ih[stn][nh] = hit;
                  if ( nh < 99 ) ++nh;
                  hit->setUse1( true );
                  hit->setUse2( true );
                }
              } // for 
              nhMax[stn] = nh;         
            }  // loop station

            if ( nhMax[0]+nhMax[1]+nhMax[2] > nhSel ) {
              nhSel = nhMax[0]+nhMax[1]+nhMax[2];
              hSel = hit3;
              int nh = 0;
              for ( int stn = 0; stn < 3; ++stn ) {
                 for ( int i = 0; i < nhMax[stn]; ++i ) {
                    iSel[nh] = ih[stn][i];
                    ++nh;
	         }  //i
	      } // stn
	    }
          }   //it3
          if ( nhSel < 1 ) continue;

          std::vector<SeedPnt> xPnts; xPnts.reserve(12);
          xPnts +=  SeedPnt(hit1),  SeedPnt( hit2), SeedPnt( hSel);

          for ( int i = 0; i < nhSel; ++i ) {
            SeedPnt pnt( iSel[i] );
            xPnts.push_back( pnt );
          }
      
          if ( xPnts.size() < (unsigned int)m_nWin2 ) continue;

          SeedTrack* seed = new SeedTrack(xPnts,sector(), dth);
  
          int rc = m_parabolaFit->fit( seed, csth );
	  if (rc < 1 || goodTrack(seed) == false){
             delete seed;
            //continue;
          }
          else {
            seeds.push_back( seed );
            tagUsedHits(hit1, hit2, select);   
	  }
        } //hits
      } //hits
    } // lay
  } // lay

  // cleanup(hits);
  return StatusCode::SUCCESS;
}

void TsaITXSearch::loadData(std::vector<SeedHit*> hits[6]) const{

  //  Load hits with data from Tsa clusters
  for ( int station = 1; station < 4; ++station ) {
    for ( int layer = 0; layer < 4; layer+=3 ) {
      int lay = 2*(station-1);
      if ( layer > 0 ) ++lay;
        for ( int ib = 0; ib < 2; ++ib ) {
          if ( sector() > 0 && ib > 0 ) continue;
          int box = 2 + sector() - ib;
          Tsa::STRange iRange =  m_dataSvc->partition(station,layer+1,box);
          for (Tsa::STClusters::const_iterator itIter = iRange.begin(); itIter != iRange.end(); ++itIter){
            if ((*itIter)->isHot() == false) {
              SeedHit* hit = new SeedHit(*itIter); 
              hits[lay].push_back( hit );
	    }
          } //clusters
        } // box

      std::sort( hits[lay].begin(), hits[lay].end(), SeedFunctor::increasingX<const SeedHit*>() );
    } // layer
  } // station

}

#undef CALL_MEMBER_FN
