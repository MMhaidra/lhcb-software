#ifndef MINTDALITZ_PHASE_DIFF_BINNING_HH
#define MINTDALITZ_PHASE_DIFF_BINNING_HH


#include "Mint/ScpBoxSet.h"
#include "Mint/ScpBox.h"
#include "Mint/ScpBinning.h"
#include "Mint/PhaseBinning.h"



#include "Mint/IFastAmplitudeIntegrable.h"
#include "Mint/FitAmpSum.h"

#include "Mint/IDalitzEventList.h"
#include "Mint/IDalitzEvent.h"
#include "Mint/IGetRealEvent.h"
#include "Mint/IDalitzPdf.h"

#include "Mint/DalitzHistoStackSet.h"
#include "TH1D.h"
#include "TMath.h"

#include <vector>
#include <iostream>
#include <algorithm>

class PhaseDiffBinning : public PhaseBinning {

  FitAmpSum* _fas;

 public:
  PhaseDiffBinning(FitAmpSum* fas)
  {
	  _fas = fas;
  }
  
  int createBinning(IDalitzEventList* events
 		    , int minPerBin = 10
 		    , int maxPerBin = 100
 		    );


  void fillData(IDalitzEventList* data);
  void fillDataCC(IDalitzEventList* data);
// 
//  int createBinning(IDalitzEventList* events
//		    , int minPerBin = 10
//		    , int maxPerBin = 100
//		    );

  double setEvents(IDalitzEventList* data
			 , IDalitzEventList* mc
			 );

};


#endif
//
