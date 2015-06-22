from GaudiKernel.SystemOfUnits import GeV, MeV, picosecond, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser

class CharmSpectroscopyLines :
    def localcuts(self) :
        return {
##  D0,pi is considered separately from the others because the inclusive D0 rate is
##  expected to be very high due to the large number of real D0 signal events
                'Spec_D0Pi' : {
                                        'DeltaM_AM_MIN'    : 135 * MeV,
                                        'DeltaM_AM_MAX'    : 1025 * MeV,
                                        'DeltaM_MIN'       : 135 * MeV,
                                        'DeltaM_MAX'       : 995 * MeV,
                                        'TagVCHI2PDOF_MAX' : 10.0,
                                        'TisTosSpec'       : "Hlt1.*MVA.*Decision%TOS",
                                       }
##  Dpm,pi is considered separately from the others because the inclusive Dpm rate is
##  expected to be very high due to the large number of real D0 signal events
               ,'Spec_DpmPi' : {
                                        'DeltaM_AM_MIN'    : 135 * MeV,
                                        'DeltaM_AM_MAX'    : 1025 * MeV,
                                        'DeltaM_MIN'       : 135 * MeV,
                                        'DeltaM_MAX'       : 995 * MeV,
                                        'TagVCHI2PDOF_MAX' : 10.0,
                                        'TisTosSpec'       : "Hlt1.*MVA.*Decision%TOS",
                                       }
               ,'Spec_DPi' : {
                                        'DeltaM_AM_MIN'    : 135 * MeV,
                                        'DeltaM_AM_MAX'    : 1025 * MeV,
                                        'DeltaM_MIN'       : 135 * MeV,
                                        'DeltaM_MAX'       : 995 * MeV,
                                        'TagVCHI2PDOF_MAX' : 10.0,
                                        'TisTosSpec'       : "Hlt1.*MVA.*Decision%TOS",
                                       }
               ,'Spec_DK' : {
                                        'DeltaM_AM_MIN'    : 490 * MeV,
                                        'DeltaM_AM_MAX'    : 1380 * MeV,
                                        'DeltaM_MIN'       : 490 * MeV,
                                        'DeltaM_MAX'       : 1350 * MeV,
                                        'TagVCHI2PDOF_MAX' : 10.0,
                                        'TisTosSpec'       : "Hlt1.*MVA.*Decision%TOS",
                                       }
               ,'Spec_DPr' : {
                                        'DeltaM_AM_MIN'    : 930 * MeV,
                                        'DeltaM_AM_MAX'    : 1820 * MeV,
                                        'DeltaM_MIN'       : 930 * MeV,
                                        'DeltaM_MAX'       : 1790 * MeV,
                                        'TagVCHI2PDOF_MAX' : 10.0,
                                        'TisTosSpec'       : "Hlt1.*MVA.*Decision%TOS",
                                       }
               ,'Spec_DLambda' : {
                                        'DeltaM_AM_MIN'    : 1110 * MeV,
                                        'DeltaM_AM_MAX'    : 2000 * MeV,
                                        'DeltaM_MIN'       : 1110 * MeV,
                                        'DeltaM_MAX'       : 1970 * MeV,
                                        'TagVCHI2PDOF_MAX' : 10.0,
                                        'TisTosSpec'       : "Hlt1.*MVA.*Decision%TOS",
                                       }
               ,'Spec_DEta' : {
                                        'DeltaM_AM_MIN'    :  540 * MeV,
                                        'DeltaM_AM_MAX'    : 1440 * MeV,
                                        'DeltaM_MIN'       :  540 * MeV,
                                        'DeltaM_MAX'       : 1410 * MeV,
                                        'TagVCHI2PDOF_MAX' : 10.0,
                                        'TisTosSpec'       : "Hlt1.*MVA.*Decision%TOS",
                                       }
                 ,'ChargedSpectroscopyBachelors' : {
                                        'IPCHI2_MAX'       :  15,
                                       }

                 # these are cuts for the PromptSpectroscopyFilter
                 # For the Lc lines, despite the name, the mass window is
                 # wide enough to catch the Xi_c as well.
                 #  the default values, 2211 - 2543 correspond to the values in D2HHHLines.py
                 ,'LcSpec' : {
                                'IPCHI2_MAX'       :  15,
                                'D_BPVLTIME_MIN'   :  0.3 * picosecond,
                                'DMASS_MIN'        :  2211 * MeV,
                                'DMASS_MAX'        :  2543 * MeV,
                               }

                 , 'D02KPiPiPiSpec' : {
                                'IPCHI2_MAX'       :  15,
                                'D_BPVLTIME_MIN'   :  0.3 * picosecond,
                                'DMASS_MIN'        :  1790 * MeV,
                                'DMASS_MAX'        :  1940 * MeV,
                   }
        }

    def locallines(self):

##  these are  combiners instantiated in Stages.py
##  as indicated below, there is a different grammar to access
##  candidates passing trigger selections specified in  other files
        from Stages import D02HH_D0ToKmPip
        from Stages import TagDecay
        from Stages import SharedPromptChild_pi, SharedPromptChild_K, SharedPromptChild_p
        from Stages import CharmHadSharedKsLL, CharmHadSharedKsDD
        from Stages import CharmHadSharedSecondaryLambdaLL, CharmHadSharedSecondaryLambdaDD
        from Stages import D02KsKK_LL, D02KsKK_DD, D02KsPiPi_LL, D02KsPiPi_DD
        from Stages import D02KsKPi_LL, D02KsKPi_DD
        from Stages import D2HHH_DspToKmKpPip, D2HHH_DpToKmPipPip
        from Stages import D0_TAG_CPV_Dstp2D0Pip_D02KmPip
        from Stages import DstToD02HHHH_D02CFKPiPiPi
        from Stages import D02HHHHMass_D02KPiPiPi
        from Stages import SharedNeutralLowPtChild_pi0, SharedNeutralLowPtChild_eta
        from Stages import PromptBachelorFilter, PromptSpectroscopyFilter
        from Stages import SharedTighterPromptChild_p
##

 
## these come from D02HHHHLines.py
        D02KPiPiPiForSpectroscopy = PromptSpectroscopyFilter('D02KPiPiPiSpec'
                , inputs=[D02HHHHMass_D02KPiPiPi]
                , shared = True)

        from Stages import Lc2HHH_LcpToKmPpPip




        from Hlt2Lines.Utilities.Hlt2Stage import Hlt2ExternalStage
        from Hlt2Lines.DPS.Stages import MergeCharm

        d0 = MergeCharm('MergedD0', inputs = [D02HH_D0ToKmPip, D02KsKK_LL, D02KsKK_DD, 
                D02KsPiPi_LL, D02KsPiPi_DD,
                D02KsKPi_LL, D02KsKPi_DD,
                D02KPiPiPiForSpectroscopy
                ])

        dstar = MergeCharm('MergedDstar', inputs = [D0_TAG_CPV_Dstp2D0Pip_D02KmPip, DstToD02HHHH_D02CFKPiPiPi])
##        dstar = MergeCharm('MergedDstar', inputs = [D0_TAG_CPV_Dstp2D0Pip_D02KmPip])


##  
##  
##  start by filtering the standard "prompt" track lists to add IPChi2 cuts
##
        ## shared instances should usually be defined in Stages.py.
        ## Consider moving them there.
        BachelorPi = PromptBachelorFilter('CharmHadBachelorPi',
                                       inputs = [SharedPromptChild_pi],
                                       nickname = 'ChargedSpectroscopyBachelors',
                                       shared = True)

        BachelorK = PromptBachelorFilter('CharmHadBachelorK',
                                       inputs = [SharedPromptChild_K],
                                       nickname = 'ChargedSpectroscopyBachelors',
                                       shared = True)

        BachelorPr = PromptBachelorFilter('CharmHadBachelorPr',
                                       inputs = [SharedTighterPromptChild_p],
                                       nickname = 'ChargedSpectroscopyBachelors',
                                       shared = True)


## LcpForSpectroscopy is meant to be used as input for spectroscopy studies. It has
## a tighter set of selections on  decay time, on IPChi2 (nonexistent for 
## default CPV lines), and (potentially) Lambda_c mass to create candidates for spectroscopy studies
## It should be sent to *neither* Full or Turbo.
        ## shared instances should usually be defined in Stages.py.
        ## Consider moving them there.
        LcpForSpectroscopy = PromptSpectroscopyFilter('LcForSpec', nickname = 'LcSpec',
                                            inputs=[Lc2HHH_LcpToKmPpPip], shared = True)



        stages = {}
##  recall that the "nicknake" in TagDecay is the "name" used in the dictionary of cuts.
##  
##
        stages.update ( {
                  'Spec_D0PiTurbo' : [TagDecay('Spec_D0Pi',
                     decay = ["[D*(2010)+ -> D0 pi+]cc", "[D*(2010)- -> D0 pi-]cc"],
                     nickname = 'Spec_D0Pi',
                     inputs = [d0,BachelorPi])],
                  'Spec_D0KsTurbo' : [TagDecay('Spec_D0Ks',
                     decay = ["[D*(2007)0 -> D0 KS0]cc"],
                     nickname = 'Spec_DK',
                     inputs = [d0,CharmHadSharedKsLL,CharmHadSharedKsDD])],
                  'Spec_D0KpmTurbo' : [TagDecay('Spec_D0K',
                     decay = ["[D*(2010)+ -> D0 K+]cc", "[D*(2010)- -> D0 K-]cc"],
                     nickname = 'Spec_DK',
                     inputs = [d0,BachelorK])],
                  'Spec_D0PrTurbo' : [TagDecay('Spec_D0Pr',
                     decay = ["[D*(2010)+ -> D0 p+]cc", "[D*(2010)- -> D0 p~-]cc"],
                     nickname = 'Spec_DPr',
                     inputs = [d0,BachelorPr])],
                  'Spec_D0LambdaTurbo' : [TagDecay('Spec_D0Lambda',
                     decay = ["[D*(2007)0 -> D0 Lambda0]cc", "[D*(2007)0 -> D~0 Lambda0]cc"],
                     nickname = 'Spec_DLambda',
                     inputs = [d0,CharmHadSharedSecondaryLambdaLL,CharmHadSharedSecondaryLambdaDD])],
                  'Spec_D0Pi0Turbo' : [TagDecay('Spec_D0Pi0',
                     decay = ["[D*(2007)0 -> D0 pi0]cc"],
                     nickname = 'Spec_DPi',
                     inputs = [d0,SharedNeutralLowPtChild_pi0])],
                  'Spec_D0EtaTurbo' : [TagDecay('Spec_D0Eta',
                     decay = ["[D*(2007)0 -> D0 eta]cc"],
                     nickname = 'Spec_DEta',
                     inputs = [d0,SharedNeutralLowPtChild_eta])],

                  'Spec_DsPiTurbo' : [TagDecay('Spec_D0Pi',
                     decay = ["[Delta(1905)++ -> D_s+ pi+]cc", "[D*(2007)0 -> D_s+ pi-]cc"],
                     nickname = 'Spec_DPi',
                     inputs = [D2HHH_DspToKmKpPip,BachelorPi])],
                  'Spec_DsKsTurbo' : [TagDecay('Spec_D0K',
                     decay = ["[D*(2010)+ -> D_s+ KS0]cc"],
                     nickname = 'Spec_DK',
                     inputs = [D2HHH_DspToKmKpPip,CharmHadSharedKsLL,CharmHadSharedKsDD])],
                  'Spec_DsKpmTurbo' : [TagDecay('Spec_D0K',
                     decay = ["[Delta(1905)++ -> D_s+ K+]cc", "[D*(2007)0 -> D_s+ K-]cc"],
                     nickname = 'Spec_DK',
                     inputs = [D2HHH_DspToKmKpPip,BachelorK])],
                  'Spec_DsPrTurbo' : [TagDecay('Spec_D0Pr',
                     decay = ["[Delta(1905)++ -> D_s+ p+]cc", "[D*(2007)0 -> D_s+ p~-]cc"],
                     nickname = 'Spec_DPr',
                     inputs = [D2HHH_DspToKmKpPip,BachelorPr])],
                  'Spec_DsLambdaTurbo' : [TagDecay('Spec_D0Lambda',
                     decay = ["[D*(2010)+ -> D_s+ Lambda0]cc", "[D*(2010)- -> D_s- Lambda0]cc"],
                     nickname = 'Spec_DLambda',
                     inputs = [D2HHH_DspToKmKpPip,CharmHadSharedSecondaryLambdaLL,CharmHadSharedSecondaryLambdaDD])],
                  'Spec_DsPi0Turbo' : [TagDecay('Spec_DsPi0',
                     decay = ["[D*(2010)+ -> D_s+ pi0]cc"],
                     nickname = 'Spec_DPi',
                     inputs = [D2HHH_DspToKmKpPip,SharedNeutralLowPtChild_pi0])],
                  'Spec_DsEtaTurbo' : [TagDecay('Spec_DsEta',
                     decay = ["[D*(2010)+ -> D_s+ eta]cc"],
                     nickname = 'Spec_DEta',
                     inputs = [D2HHH_DspToKmKpPip,SharedNeutralLowPtChild_eta])],


                  'Spec_DpPiTurbo' : [TagDecay('Spec_DpPi',
                     decay = ["[Delta(1905)++ -> D+ pi+]cc", "[D*(2007)0 -> D+ pi-]cc"],
                     nickname = 'Spec_DpmPi',
                     inputs = [D2HHH_DpToKmPipPip,BachelorPi])],
                  'Spec_DpKsTurbo' : [TagDecay('Spec_DpK',
                     decay = ["[D*(2010)+ -> D+ KS0]cc"],
                     nickname = 'Spec_DK',
                     inputs = [D2HHH_DpToKmPipPip,CharmHadSharedKsLL,CharmHadSharedKsDD])],
                  'Spec_DpKpmTurbo' : [TagDecay('Spec_DpK',
                     decay = ["[Delta(1905)++ -> D+ K+]cc", "[D*(2007)0 -> D+ K-]cc"],
                     nickname = 'Spec_DK',
                     inputs = [D2HHH_DpToKmPipPip,BachelorK])],
                  'Spec_DpPrTurbo' : [TagDecay('Spec_DpPr',
                     decay = ["[Delta(1905)++ -> D+ p+]cc", "[D*(2007)0 -> D+ p~-]cc"],
                     nickname = 'Spec_DPr',
                     inputs = [D2HHH_DpToKmPipPip,BachelorPr])],
                  'Spec_DpLambdaTurbo' : [TagDecay('Spec_DpLambda',
                     decay = ["[D*(2010)+ -> D+ Lambda0]cc", "[D*(2010)- -> D_s- Lambda0]cc"],
                     nickname = 'Spec_DLambda',
                     inputs = [D2HHH_DpToKmPipPip,CharmHadSharedSecondaryLambdaLL,CharmHadSharedSecondaryLambdaDD])],
                  'Spec_DpPi0Turbo' : [TagDecay('Spec_DpPi0',
                     decay = ["[D*(2010)+ -> D+ pi0]cc"],
                     nickname = 'Spec_DPi',
                     inputs = [D2HHH_DpToKmPipPip,SharedNeutralLowPtChild_pi0])],
                  'Spec_DpEtaTurbo' : [TagDecay('Spec_DpEta',
                     decay = ["[D*(2010)+ -> D+ eta]cc"],
                     nickname = 'Spec_DEta',
                     inputs = [D2HHH_DpToKmPipPip,SharedNeutralLowPtChild_eta])],

                  'Spec_LcPiTurbo' : [TagDecay('Spec_LcPi',
                     decay = ["[Delta(1905)++ -> Lambda_c+ pi+]cc", "[D*(2007)0 -> Lambda_c+ pi-]cc"],
                     nickname = 'Spec_DPi',
                     inputs = [LcpForSpectroscopy,BachelorPi])],
                  'Spec_LcKsTurbo' : [TagDecay('Spec_LcK',
                     decay = ["[D*(2010)+ -> Lambda_c+ KS0]cc"],
                     nickname = 'Spec_DK',
                     inputs = [LcpForSpectroscopy,CharmHadSharedKsLL,CharmHadSharedKsDD])],
                  'Spec_LcKpmTurbo' : [TagDecay('Spec_LcK',
                     decay = ["[Delta(1905)++ -> Lambda_c+ K+]cc", "[D*(2007)0 -> Lambda_c+ K-]cc"],
                     nickname = 'Spec_DK',
                     inputs = [LcpForSpectroscopy,BachelorK])],
                  'Spec_LcPrTurbo' : [TagDecay('Spec_LcPr',
                     decay = ["[Delta(1905)++ -> Lambda_c+ p+]cc", "[D*(2007)0 -> Lambda_c+ p~-]cc"],
                     nickname = 'Spec_DPr',
                     inputs = [LcpForSpectroscopy,BachelorPr])],
                  'Spec_LcLambdaTurbo' : [TagDecay('Spec_LcLambda',
                     decay = ["[D*(2010)+ -> Lambda_c+ Lambda0]cc", "[D*(2010)- -> Lambda_c~- Lambda0]cc"],
                     nickname = 'Spec_DLambda',
                     inputs = [LcpForSpectroscopy,CharmHadSharedSecondaryLambdaLL,
                               CharmHadSharedSecondaryLambdaDD])],
                  'Spec_LcPi0Turbo' : [TagDecay('Spec_LcPi0',
                     decay = ["[D*(2010)+ -> Lambda_c+ pi0]cc"],
                     nickname = 'Spec_DPi',
                     inputs = [LcpForSpectroscopy,SharedNeutralLowPtChild_pi0])],
                  'Spec_LcEtaTurbo' : [TagDecay('Spec_LcEta',
                     decay = ["[D*(2010)+ -> Lambda_c+ eta]cc"],
                     nickname = 'Spec_DEta',
                     inputs = [LcpForSpectroscopy,SharedNeutralLowPtChild_eta])],

                  'Spec_DstPiTurbo' : [TagDecay('Spec_D0Pi',
                     decay = ["[Delta(1905)++ -> D*(2010)+ pi+]cc", "[D*(2007)0 -> D*(2010)+ pi-]cc"],
                     nickname = 'Spec_DPi',
                     inputs = [dstar, BachelorPi])],
                  'Spec_DstKsTurbo' : [TagDecay('Spec_D0K',
                     decay = ["[Delta(1905)~+ -> D*(2010)+ KS0]cc"],
                     nickname = 'Spec_DK',
                     inputs = [dstar, CharmHadSharedKsLL,CharmHadSharedKsDD])],
                  'Spec_DstKpmTurbo' : [TagDecay('Spec_D0K',
                     decay = ["[Delta(1905)++ -> D*(2010)+ K+]cc", "[D*(2007)0 -> D*(2010)+ K-]cc"],
                     nickname = 'Spec_DK',
                     inputs = [dstar, BachelorK])],
                  'Spec_DstPrTurbo' : [TagDecay('Spec_D0Pr',
                     decay = ["[Delta(1905)++ -> D*(2010)+ p+]cc", "[D*(2007)0 -> D*(2010)+ p~-]cc"],
                     nickname = 'Spec_DPr',
                     inputs = [dstar, BachelorPr])],
                  'Spec_DstLambdaTurbo' : [TagDecay('Spec_D0Lambda',
                     decay = ["[Delta(1905)~+ -> D*(2010)+ Lambda0]cc", "[D*(2010)- -> D_s- Lambda0]cc"],
                     nickname = 'Spec_DLambda',
                     inputs = [dstar, CharmHadSharedSecondaryLambdaLL,CharmHadSharedSecondaryLambdaDD])],
                  'Spec_DstPi0Turbo' : [TagDecay('Spec_DstPi0',
                     decay = ["[Delta(1905)~+ -> D_s+ pi0]cc"],
                     nickname = 'Spec_DPi',
                     inputs = [dstar,SharedNeutralLowPtChild_pi0])],
                  'Spec_DstEtaTurbo' : [TagDecay('Spec_DstEta',
                     decay = ["[Delta(1905)~+ -> D_s+ eta]cc"],
                     nickname = 'Spec_DEta',
                     inputs = [dstar,SharedNeutralLowPtChild_eta])],


                 } )
        

        return stages
