#ifndef CALO2TRACK_H
#define CALO2TRACK_H 1

#include <string>

//Forward dec'
template <class FROM,class TO,class WEIGHT> class IRelationWeighted;
template <class FROM,class TO,class WEIGHT> class IRelationWeighted2D;
template <class FROM,class TO> class IRelation;

namespace LHCb{
  class CaloHypo;
  class CaloCluster;
  class Track;
}

/** @class Calo2Track Calo2Track.h Event/Calo2Track.h
 *
 *
 *  @author Olivier Deschamps
 *  @date   2006-02-21
 */
namespace LHCb {
  namespace CaloIdLocation {
    // Calo-Track matching tables   : IRelationWeighted< CaloCluster/Hypo , Track , float >
    const std::string ClusterMatch  = "Rec/Calo/ClusterMatch";
    const std::string PhotonMatch   = "Rec/Calo/PhotonMatch";
    const std::string BremMatch     = "Rec/Calo/BremMatch";
    const std::string ElectronMatch = "Rec/Calo/ElectronMatch";
    // Acceptance tables       : IRelation< Track , bool  >
    const std::string InSpd         =  "Rec/Calo/InAccSpd";
    const std::string InPrs         =  "Rec/Calo/InAccPrs";
    const std::string InBrem        =  "Rec/Calo/InAccBrem";
    const std::string InEcal        =  "Rec/Calo/InAccEcal";
    const std::string InHcal        =  "Rec/Calo/InAccHcal";
    // Intermediate estimators : IRelation< Track , float >
    const std::string SpdE          = "Rec/Calo/SpdE";
    const std::string PrsE          = "Rec/Calo/PrsE";
    const std::string EcalE         = "Rec/Calo/EcalE";
    const std::string HcalE         = "Rec/Calo/HcalE";
    const std::string EcalChi2      = "Rec/Calo/EcalChi2";
    const std::string BremChi2      = "Rec/Calo/BremChi2";
    const std::string ClusChi2      = "Rec/Calo/ClusChi2";
    // CaloPID DLLs            : IRelation< Track , float >
    const std::string PrsPIDe       = "Rec/Calo/PrsPIDe";
    const std::string EcalPIDe      = "Rec/Calo/EcalPIDe";
    const std::string BremPIDe      = "Rec/Calo/BremPIDe";
    const std::string HcalPIDe      = "Rec/Calo/HcalPIDe";
    const std::string EcalPIDmu     = "Rec/Calo/EcalPIDmu";
    const std::string HcalPIDmu     = "Rec/Calo/HcalPIDmu";
    // Neutral ID DLLs
    const std::string PhotonID      = "Rec/Calo/PhotonID" ;
    const std::string MergedID      = "Rec/Calo/MergedID" ;
    const std::string PhotonFromMergedID      = "Rec/Calo/PhotonFromMergedID" ;



    // =================== Hlt locations ============== DEPRECATED : use CaloAlgUtils::pathFromContext instead !!

    // Calo-Track matching tables   : IRelationWeighted< CaloCluster/Hypo , Track , float >
    const std::string ClusterMatchHlt  = "Hlt/Calo/ClusterMatch";
    const std::string PhotonMatchHlt   = "Hlt/Calo/PhotonMatch";
    const std::string BremMatchHlt     = "Hlt/Calo/BremMatch";
    const std::string ElectronMatchHlt = "Hlt/Calo/ElectronMatch";
    // Acceptance tables       : IRelation< Track , bool  >
    const std::string InSpdHlt        =  "Hlt/Calo/InAccSpd";
    const std::string InPrsHlt         =  "Hlt/Calo/InAccPrs";
    const std::string InBremHlt        =  "Hlt/Calo/InAccBrem";
    const std::string InEcalHlt        =  "Hlt/Calo/InAccEcal";
    const std::string InHcalHlt        =  "Hlt/Calo/InAccHcal";
    // Intermediate estimators : IRelation< Track , float >
    const std::string SpdEHlt          = "Hlt/Calo/SpdE";
    const std::string PrsEHlt         = "Hlt/Calo/PrsE";
    const std::string EcalEHlt         = "Hlt/Calo/EcalE";
    const std::string HcalEHlt         = "Hlt/Calo/HcalE";
    const std::string EcalChi2Hlt      = "Hlt/Calo/EcalChi2";
    const std::string BremChi2Hlt      = "Hlt/Calo/BremChi2";
    const std::string ClusChi2Hlt      = "Hlt/Calo/ClusChi2";
    // CaloPID DLLs            : IRelation< Track , float >
    const std::string PrsPIDeHlt       = "Hlt/Calo/PrsPIDe";
    const std::string EcalPIDeHlt      = "Hlt/Calo/EcalPIDe";
    const std::string BremPIDeHlt      = "Hlt/Calo/BremPIDe";
    const std::string HcalPIDeHlt      = "Hlt/Calo/HcalPIDe";
    const std::string EcalPIDmuHlt     = "Hlt/Calo/EcalPIDmu";
    const std::string HcalPIDmuHlt     = "Hlt/Calo/HcalPIDmu";
    // PhotonID DLL
    const std::string HltPhotonID         = "Hlt/Calo/PhotonID" ;
    const std::string HltMergedID         = "Hlt/Calo/MergedID" ;
    const std::string HltPhotonFromMergedID      = "Hlt/Calo/PhotonFromMergedID" ;
  }
  namespace Calo2Track {
    typedef IRelationWeighted< LHCb::CaloCluster , LHCb::Track , float >             IClusTrTable;
    typedef IRelationWeighted< LHCb::Track , LHCb::CaloCluster , float >             ITrClusTable;
    typedef IRelationWeighted< LHCb::CaloHypo , LHCb::Track , float >                IHypoTrTable;
    typedef IRelationWeighted< LHCb::Track , LHCb::CaloHypo , float >                ITrHypoTable;

    typedef IRelationWeighted2D< LHCb::CaloCluster , LHCb::Track , float >           IClusTrTable2D;
    typedef IRelationWeighted2D< LHCb::Track , LHCb::CaloCluster , float >           ITrClusTable2D;
    typedef IRelationWeighted2D< LHCb::CaloHypo , LHCb::Track , float >              IHypoTrTable2D;
    typedef IRelationWeighted2D< LHCb::Track , LHCb::CaloHypo , float >              ITrHypoTable2D;
    typedef IRelation< LHCb::CaloHypo , float >                                      IHypoEvalTable;

    typedef IRelation< LHCb::Track , float >                                         ITrEvalTable;
    typedef IRelation< LHCb::Track , bool  >                                         ITrAccTable;
  }
}

#endif // CALO2TRACK_H
