"""
Line config dictionaries for the Production and Spectroscopy Working Group 

"""
NeuroBayesMuMu = {
    'BUILDERTYPE' : 'StrippingNeuroBayesMuMuConf',
    'CONFIG' : {'trackChi2'           :    5.0
              , 'MuMuMassMin'       :    0.0
              , 'MuMuMassMax'       :   12.0
              , 'MuMuMassHighMin'   :    8.5
              , 'MuMuMassHighMax'   :  130.0
              , 'JPsiMassMin'       :    3.04 # GeV
              , 'JPsiMassMax'       :    3.14 # GeV
              , 'vertexChi2'        :   25.0
              , 'NBCutJPsi'         :    0.7
              , 'NBCutMuMu'         :    0.95
              , 'NBCutMuMuHigh'     :   -0.7
              , 'ExpertiseJPsi'     : 'Muon/mumu_net_noip.nb'
              , 'ExpertiseMuMu'     : 'Muon/mumu_net_full.nb'
              , 'ExpertiseMuMuHigh' : 'Muon/mumuhighmass_full.nb'
              , 'NBVersionJPsi'     : "TuneSep2010"
              , 'NBVersionMuMu'     : "TuneSep2010"
              , 'NBVersionMuMuHigh' : "TuneSep2010"
              , 'PrescaleMuMu'      : 0.
              , 'PostscaleMuMu'     : 1.0
              , 'PrescaleMuMuHigh'  : 1.0
              , 'PostscaleMuMuHigh' : 1.0
              , 'PrescaleJPsi'      : 0.1
              , 'PostscaleJPsi'     : 1.0
              },
    'STREAMS' : [ 'Dimuon' ],
    'WGs'    : [ 'PSWG' ]
    }


MicroDSTNeuroBayesMuMu = {
    'BUILDERTYPE' : 'StrippingNeuroBayesMuMuConf',
    'CONFIG' : {'trackChi2'           :    5.0
              , 'MuMuMassMin'       :    0.0
              , 'MuMuMassMax'       :   12.0
              , 'MuMuMassHighMin'   :    8.5
              , 'MuMuMassHighMax'   :  130.0
              , 'JPsiMassMin'       :    3.04 # GeV
              , 'JPsiMassMax'       :    3.14 # GeV
              , 'vertexChi2'        :   25.0
              , 'NBCutJPsi'         :    0.7
              , 'NBCutMuMu'         :    0.95
              , 'NBCutMuMuHigh'     :   -0.7
              , 'ExpertiseJPsi'     : 'Muon/mumu_net_noip.nb'
              , 'ExpertiseMuMu'     : 'Muon/mumu_net_full.nb'
              , 'ExpertiseMuMuHigh' : 'Muon/mumuhighmass_full.nb'
              , 'NBVersionJPsi'     : "TuneSep2010"
              , 'NBVersionMuMu'     : "TuneSep2010"
              , 'NBVersionMuMuHigh' : "TuneSep2010"
              , 'PrescaleMuMu'      : 1.0
              , 'PostscaleMuMu'     : 1.0
              , 'PrescaleMuMuHigh'  : 0.
              , 'PostscaleMuMuHigh' : 1.0
              , 'PrescaleJPsi'      : 0.
              , 'PostscaleJPsi'     : 1.0
              },
    'STREAMS' : [ 'Leptonic' ],
    'WGs'    : [ 'PSWG' ]
    }



B2twobody = {
    'BUILDERTYPE' : 'B2nbodyConf',
    'CONFIG' : {  'nbody':              2,
                  'MinBMass':        4500.0,
                  'MaxBMass':        8000.0,
                  'MinBPt':          2000.0,
                  'MaxBVertChi2DOF':   10.0,
                  'MinBPVVDChi2':     225.0,
                  'MaxBPVIPChi2':      15.0,
                  'MinBPVDIRA':         0.0,
                  'MinNvc':             2,
                  'doPi':            True,
                  'doK':             True,
                  'dop':             True,
                  'doKs':            True,
                  'doLm':            True,
                  'doDz':            True,
                  'doDp':            True,
                  'doDs':            True,
                  'doLc':            True,
                  'doPh':            True,
                  'doKS':            True,
                  'doJp':            True,
                  'doDS':            True,
                  'MinPiPt':         1500.0,
                  'MinPiIPChi2DV':     25.0,
                  'MaxPiChi2':          5.0,
                  'MinKPt':          1500.0,
                  'MinKIPChi2DV':      25.0,
                  'MaxKChi2':           5.0,
                  'MinpPt':          1500.0,
                  'MinpIPChi2DV':      25.0,
                  'MaxpChi2':           5.0,
                  'MaxKsDeltaM':       40.0,
                  'MinKsPt':         1500.0,
                  'MaxKsVertChi2DOF':  10.0,
                  'MinKsPVVDChi2':    400.0,
                  'MinKsIPChi2':        0.0,
                  'MinKsDauPt':       200.0,
                  'MinKsDauIPChi2':    16.0,
                  'MaxKsDauTrkChi2':   10.0,
                  'MaxLmDeltaM':       15.0,
                  'MinLmPt':         1500.0,
                  'MaxLmVertChi2DOF':  10.0,
                  'MinLmPVVDChi2':    400.0,
                  'MinLmIPChi2':        0.0,
                  'MinLmPrtPt':       500.0,
                  'MinLmPiPt':        100.0,
                  'MinLmPrtIPChi2':     4.0,
                  'MinLmPiIPChi2':     16.0,
                  'MaxLmPrtTrkChi2':   10.0,
                  'MaxLmPiTrkChi2':    10.0,
                  'MaxDzDeltaM':      40.0,
                  'MinDzPt':         1500.0,
                  'MaxDzVertChi2DOF':  10.0,
                  'MinDzPVVDChi2':    225.0,
                  'MinDzIPChi2':        0.0,
                  'MinDzDauPt':       250.0,
                  'MinDzDauIPChi2':     9.0,
                  'MaxDzDauTrkChi2':    5.0,
                  'MinDzPIDK':         -5.0,
                  'MaxDpDeltaM':       40.0,
                  'MinDpPt':         1500.0,
                  'MaxDpVertChi2DOF':  10.0,
                  'MinDpPVVDChi2':    225.0,
                  'MinDpIPChi2':        0.0,
                  'MinDpDauPt':       250.0,
                  'MinDpDauIPChi2':     9.0,
                  'MaxDpDauTrkChi2':    5.0,
                  'MinDpPIDK':         -5.0,
                  'MaxDsDeltaM':       40.0,
                  'MinDsPt':         1500.0,
                  'MaxDsVertChi2DOF':  10.0,
                  'MinDsPVVDChi2':    225.0,
                  'MinDsIPChi2':        0.0,
                  'MinDsDauPt':       250.0,
                  'MinDsDauIPChi2':     9.0,
                  'MaxDsDauTrkChi2':    5.0,
                  'MinDsPIDK':         -5.0,
                  'MaxLcDeltaM':       40.0,
                  'MinLcPt':         1500.0,
                  'MaxLcVertChi2DOF':  10.0,
                  'MinLcPVVDChi2':    225.0,
                  'MinLcIPChi2':        0.0,
                  'MinLcDauPt':       250.0,
                  'MinLcDauIPChi2':     9.0,
                  'MaxLcDauTrkChi2':    5.0,
                  'MinLcPIDK':         -5.0,
                  'MinLcPIDp':         -5.0,
                  'MaxPhDeltaM':       30.0,
                  'MinPhPt':         1500.0,
                  'MaxPhVertChi2DOF':  10.0,
                  'MinPhPVVDChi2':    100.0,
                  'MinPhIPChi2':       16.0,
                  'MinPhDauPt':       500.0,
                  'MinPhDauIPChi2':     9.0,
                  'MaxPhDauTrkChi2':   10.0,
                  'MaxKSDeltaM':      150.0,
                  'MinKSPt':         2000.0,
                  'MaxKSVertChi2DOF':  10.0,
                  'MinKSPVVDChi2':    100.0,
                  'MinKSIPChi2':       16.0,
                  'MinKSDauPt':       500.0,
                  'MinKSDauIPChi2':    16.0,
                  'MaxKSDauTrkChi2':    5.0,
                  'MaxJpDeltaM':     3000.0,
                  'MinJpPt':         1000.0,
                  'MaxJpVertChi2DOF':  10.0,
                  'MinJpPVVDChi2':     16.0,
                  'MinJpIPChi2':        0.0,
                  'MinJpDauPt':       500.0,
                  'MinJpDauIPChi2':     0.0,
                  'MaxJpDauTrkChi2':   10.0,
                  'MaxDSDeltaM':      153.0,
                  'MinDSPt':         1500.0,
                  'MaxDSVertChi2DOF':  10.0,
                  'MinDSPVVDChi2':      0.0,
                  'MinDSIPChi2':        0.0,
                  'MinDSPiPt':        100.0,
                  'MinDSPiIPChi2':      0.0,
                  'MaxDSPiTrkChi2':     5.0,
                  'MaxDSD0DeltaM':     40.0,
                  'MinDSD0Pt':       1250.0,
                  'MaxDSD0VertChi2DOF':10.0,
                  'MinDSD0PVVDChi2':  225.0,
                  'MinDSD0IPChi2':      0.0,
                  'MinDSD0DauPt':     250.0,
                  'MinDSD0DauIPChi2':   9.0,
                  'MaxDSD0DauTrkChi2':  5.0
                },
    'STREAMS' : [ 'Bhadron' ] ,
    'WGs'    : [ 'PSWG' ]
    }


B2threebody = {
    'BUILDERTYPE' : 'B2nbodyConf',
    'CONFIG' : {  'nbody':              3,
                  'MinBMass':        4500.0,
                  'MaxBMass':        8000.0,
                  'MinBPt':          2000.0,
                  'MaxBVertChi2DOF':   10.0,
                  'MinBPVVDChi2':     225.0,
                  'MaxBPVIPChi2':      15.0,
                  'MinBPVDIRA':         0.0,
                  'MinNvc':             2,
                  'doPi':            True,
                  'doK':             True,
                  'dop':             True,
                  'doKs':            True,
                  'doLm':            True,
                  'doDz':            True,
                  'doDp':            True,
                  'doDs':            True,
                  'doLc':            True,
                  'doPh':            True,
                  'doKS':           False,
                  'doJp':            True,
                  'doDS':            True,
                  'MinPiPt':         1500.0,
                  'MinPiIPChi2DV':     25.0,
                  'MaxPiChi2':          5.0,
                  'MinKPt':          1500.0,
                  'MinKIPChi2DV':      25.0,
                  'MaxKChi2':           5.0,
                  'MinpPt':          1500.0,
                  'MinpIPChi2DV':      25.0,
                  'MaxpChi2':           5.0,
                  'MaxKsDeltaM':       40.0,
                  'MinKsPt':         1500.0,
                  'MaxKsVertChi2DOF':  10.0,
                  'MinKsPVVDChi2':    400.0,
                  'MinKsIPChi2':        0.0,
                  'MinKsDauPt':       200.0,
                  'MinKsDauIPChi2':    16.0,
                  'MaxKsDauTrkChi2':   10.0,
                  'MaxLmDeltaM':       15.0,
                  'MinLmPt':         1500.0,
                  'MaxLmVertChi2DOF':  10.0,
                  'MinLmPVVDChi2':    400.0,
                  'MinLmIPChi2':        0.0,
                  'MinLmPrtPt':       500.0,
                  'MinLmPiPt':        100.0,
                  'MinLmPrtIPChi2':     4.0,
                  'MinLmPiIPChi2':     16.0,
                  'MaxLmPrtTrkChi2':   10.0,
                  'MaxLmPiTrkChi2':    10.0,
                  'MaxDzDeltaM':      40.0,
                  'MinDzPt':         1500.0,
                  'MaxDzVertChi2DOF':  10.0,
                  'MinDzPVVDChi2':    225.0,
                  'MinDzIPChi2':        0.0,
                  'MinDzDauPt':       250.0,
                  'MinDzDauIPChi2':     9.0,
                  'MaxDzDauTrkChi2':    5.0,
                  'MinDzPIDK':         -5.0,
                  'MaxDpDeltaM':       40.0,
                  'MinDpPt':         1500.0,
                  'MaxDpVertChi2DOF':  10.0,
                  'MinDpPVVDChi2':    225.0,
                  'MinDpIPChi2':        0.0,
                  'MinDpDauPt':       250.0,
                  'MinDpDauIPChi2':     9.0,
                  'MaxDpDauTrkChi2':    5.0,
                  'MinDpPIDK':         -5.0,
                  'MaxDsDeltaM':       40.0,
                  'MinDsPt':         1500.0,
                  'MaxDsVertChi2DOF':  10.0,
                  'MinDsPVVDChi2':    225.0,
                  'MinDsIPChi2':        0.0,
                  'MinDsDauPt':       250.0,
                  'MinDsDauIPChi2':     9.0,
                  'MaxDsDauTrkChi2':    5.0,
                  'MinDsPIDK':         -5.0,
                  'MaxLcDeltaM':       40.0,
                  'MinLcPt':         1500.0,
                  'MaxLcVertChi2DOF':  10.0,
                  'MinLcPVVDChi2':    225.0,
                  'MinLcIPChi2':        0.0,
                  'MinLcDauPt':       250.0,
                  'MinLcDauIPChi2':     9.0,
                  'MaxLcDauTrkChi2':    5.0,
                  'MinLcPIDK':         -5.0,
                  'MinLcPIDp':         -5.0,
                  'MaxPhDeltaM':       30.0,
                  'MinPhPt':         1500.0,
                  'MaxPhVertChi2DOF':  10.0,
                  'MinPhPVVDChi2':    100.0,
                  'MinPhIPChi2':       16.0,
                  'MinPhDauPt':       500.0,
                  'MinPhDauIPChi2':     9.0,
                  'MaxPhDauTrkChi2':   10.0,
                  'MaxKSDeltaM':      150.0,
                  'MinKSPt':         2000.0,
                  'MaxKSVertChi2DOF':  10.0,
                  'MinKSPVVDChi2':    100.0,
                  'MinKSIPChi2':       16.0,
                  'MinKSDauPt':       500.0,
                  'MinKSDauIPChi2':    16.0,
                  'MaxKSDauTrkChi2':    5.0,
                  'MaxJpDeltaM':     3000.0,
                  'MinJpPt':         1000.0,
                  'MaxJpVertChi2DOF':  10.0,
                  'MinJpPVVDChi2':     16.0,
                  'MinJpIPChi2':        0.0,
                  'MinJpDauPt':       500.0,
                  'MinJpDauIPChi2':     0.0,
                  'MaxJpDauTrkChi2':   10.0,
                  'MaxDSDeltaM':      153.0,
                  'MinDSPt':         1500.0,
                  'MaxDSVertChi2DOF':  10.0,
                  'MinDSPVVDChi2':      0.0,
                  'MinDSIPChi2':        0.0,
                  'MinDSPiPt':        100.0,
                  'MinDSPiIPChi2':      0.0,
                  'MaxDSPiTrkChi2':     5.0,
                  'MaxDSD0DeltaM':     40.0,
                  'MinDSD0Pt':       1250.0,
                  'MaxDSD0VertChi2DOF':10.0,
                  'MinDSD0PVVDChi2':  225.0,
                  'MinDSD0IPChi2':      0.0,
                  'MinDSD0DauPt':     250.0,
                  'MinDSD0DauIPChi2':   9.0,
                  'MaxDSD0DauTrkChi2':  5.0
                  },
    'STREAMS' : [ 'Bhadron' ] ,
    'WGs'    : [ 'PSWG' ]
    }

TwoBodyPrompt = {
    'BUILDERTYPE'  :  'TwoBody_promptConf',
    'CONFIG' :  { 'nbody':              2,
                  'MinBMass':        3690.0,
                  'MaxBMass':        100000.0,
                  'MinBPt':          2000.0,
                  'MaxBVertChi2DOF':   10.0,
                  'MaxBPVVDCHI2Comb' : 10.,
                  'MinBPVDIRA' : 0.999,
                  'MinNvc':             3,
                  'MinFly':1,
                  'doLm':            True,
                  'doDz':            True,
                  'doDp':            True,
                  'doDs':            True,
                  'doLc':            True,
                  'doPh':            True,
                  'doJp':            True,
                  'doDS':            True,
                  'MinSingleTrackPtPi' : 6000.0,
                  'MinSingleTrackPtK' : 4000.0,
                  'MinSingleTrackPtp' : 3500.0,  
                  'MaxSingleTrackIPChi2DV' : 5.0,   # changed 
                  'MinDPIDpSingle' : 10.0,
                  'MinDPIDKSingle' : 5.0,
                  'MinLongLivedDauPt' : 200.0,
                  'MinLongLivedDauIPChi2' : 4.0,      
                  'MinLongLivedPt' : 3000.0,
                  'MinKsPt':2500.,
                  'MaxLongLivedIPChi2' : 10.0,
                  'MaxLongLivedVertChi2DOF' : 10.0,
                  'MinLongLivedPVVDChi2' : 200.0,   # halved
                  'MaxKsDeltaM' : 25.0, 
                  'MaxLmDeltaM' : 15.0, 
                  
                  'MinDPIDK' : 5.0,
                  'MinDPIDp' : 5.0,
                  
                  'MinDPIDpK' : 5.0,
                  'MinDPIDKforD0Dplus' : 0.0,
                  'MinDDauPt' : 250.0,
                  'MinDDauIPChi2' : 7.0,      
                  'MinDPt' : 1200.0,       
                  'MaxDIPChi2' : 10.0,   # changed
                  'MaxDVertChi2DOF' : 10.0,
                  'MinDPVVDChi2' : 50.0,  # halved/2 
                  'MaxDDeltaM' : 30.0,
                  
                  'MinDDauIPChi2Lc': 2. ,   
                  'MinDPVVDChi2Lc': 5., 
                  
                  
                  'MinPhDauPt' : 1000.0,      
                  'MaxPhDauIPChi2' : 5.0,  # changed      
                  'MinPhPt' : 2500.0,      
                  'MaxPhVertChi2DOF' : 10.0,
                  'MaxPhDeltaM' : 20.0, 
                  
                  'MinJpDauPt' : 500.0,      
                  'MaxJpDauIPChi2' : 10.0,   # changed
                  'MinJpPt' : 1000.0,      
                  'MaxJpVertChi2DOF' : 10.0,
                  
                  'MaxDSD0VertChi2DOF' : 10.0,
                  'MinDSD0PVVDChi2' : 60.0,   # halved/2
                  'MaxDSD0DeltaM' : 30.0,
                  'MinDSPt' : 1000.0,      
                  'MaxDSDeltaM':3.0,
                  'MaxDSSlowPionIPChi2':7.,  # added   
                  'MaxDSD0IPChi2':7.,  # added
                  'MinDSD0DauPt' : 250.0, # added
                  'MinDSD0DauIPChi2' : 9.0,
                  'MinDSSlowPionDauPt' : 150.0 # added
                  },
    'STREAMS' : [ 'Bhadron' ],
    'WGs'    : [ 'PSWG' ]
    }

JpsiMuMuforD0MuMu = {
    'BUILDERTYPE'	: 'JpsiMuMuforD0MuMuConf',
    'CONFIG'	: { 'Jpsi2MuMuForD0mumu_Prescale'              :     1.   ,
                    'Jpsi2MuMuForD0mumu_Postscale'             :     1.   ,
                    'Jpsi2MuMuForD0mumu_MuonPT'                :  750.    ,
                    'Jpsi2MuMuForD0mumu_MuonP'                 : 5000.    ,
                    'Jpsi2MuMuForD0mumu_MuonTRCHI2DOF'         :   5.     ,
                    'Jpsi2MuMuForD0mumu_MuonBPVIPCHI2'         :   3.     ,
                    'Jpsi2MuMuForD0mumu_MinMass'               : 3000     , 
                    'Jpsi2MuMuForD0mumu_MaxMass'               : 3180     ,
                    'Jpsi2MuMuForD0mumu_VCHI2PDOF'             :  10.     ,
                    'Jpsi2MuMuForD0mumu_PT'                    : 1800.    , 
                    'Jpsi2MuMuForD0mumu_BPVVDCHI2'             :   20.    ,
                    'Jpsi2MuMuForD0mumu_BPVDIRA'               : 0.9997   ,
                    'Jpsi2MuMuForD0mumu_BPVIPCHI2'             : 15.      ,
                    'Jpsi2MuMuForD0mumu_MuonMaxBPVIPCHI2'          : 8.       ,
                    'Jpsi2MuMuForD0mumu_MuonMaxPT'                 : 1100.    
                    },
    'STREAMS' : [ 'Dimuon' ],
    'WGs'    : [ 'PSWG' ]
    }


FullDSTDiMuon = {
    'BUILDERTYPE'	:	'DiMuonConf',
    'CONFIG'	: {
            'MicroDST'                                 :  False   ,
                
        # DiMuon line
        'DiMuon_Prescale'                          :     1.   ,
        'DiMuon_Postscale'                         :     1.   ,
        'DiMuon_checkPV'                           : False    ,  
        
        'DiMuon_MuonPT'                            :   650.   ,  # MeV
        'DiMuon_MuonP'                             : -8000.   ,  # MeV, no cut now 
        'DiMuon_MuonTRCHI2DOF'                     :     5.   , 
        'DiMuon_MinMass'                           :  2900.   ,  # MeV
        'DiMuon_VCHI2PDOF'                         :    20.   , 
        'DiMuon_PT'                                :  3000.   ,  # MeV, no cut now 

        # DiMuon Same Sign line
        'DiMuonSameSign_Prescale'                  :     0.05  ,
        'DiMuonSameSign_Postscale'                 :     1.   ,
        'DiMuonSameSign_checkPV'                   : False    ,

        # DiMuonPrescaled line
        'DiMuonPrescaled_Prescale'                 :     0.1  ,
        'DiMuonPrescaled_Postscale'                :     1.   ,
        'DiMuonPrescaled_checkPV'                  : False    ,
        
        'DiMuonPrescaled_MuonPT'                   :   650.   ,  # MeV
        'DiMuonPrescaled_MuonP'                    : -8000.   ,  # MeV, no cut now 
        'DiMuonPrescaled_MuonTRCHI2DOF'            :     5.   , 
        'DiMuonPrescaled_MinMass'                  :  2900.   ,  # MeV
        'DiMuonPrescaled_VCHI2PDOF'                :    20.   , 
        'DiMuonPrescaled_PT'                       : -1000.   ,  # MeV, no cut now 
        
        # DiMuonExclusive line
        'DiMuonExclusive_Prescale'                 :     0.   ,
        'DiMuonExclusive_Postscale'                :     1.   ,
        'DiMuonExclusive_checkPV'                  :  True    ,
        
        'DiMuonExclusive_MuonPT'                   :   650.   ,  # MeV
        'DiMuonExclusive_MuonP'                    : -8000.   ,  # MeV, no cut now 
        'DiMuonExclusive_MuonTRCHI2DOF'            :     5.   , 
        'DiMuonExclusive_MinMass'                  :  2900.   ,
        'DiMuonExclusive_VCHI2PDOF'                :    20.   ,
        'DiMuonExclusive_PT'                       : -1000.   ,  # MeV, no cut now 
        'DiMuonExclusive_DZ'                       :    -1.   ,  # mm, upstream of any PV

        # DiMuonNoPV line
        'DiMuonNoPV_Prescale'                      :     1.   ,
        'DiMuonNoPV_Postscale'                     :     1.   ,
        
        # DiMuon High Mass line
        'DiMuonHighMass_Prescale'                  :     1.   ,
        'DiMuonHighMass_Postscale'                 :     1.   ,
        'DiMuonHighMass_checkPV'                   : False    ,
        
        'DiMuonHighMass_MuonPT'                    :   650.   , # MeV
        'DiMuonHighMass_MuonP'                     : -8000.   , 
        'DiMuonHighMass_MuonTRCHI2DOF'             :     5.   , 
        'DiMuonHighMass_MinMass'                   :  8500.   , # MeV
        'DiMuonHighMass_VCHI2PDOF'                 :    20.   ,
        'DiMuonHighMass_PT'                        : -1000.   , # MeV, no cut now 

        # DiMuon High Mass Same Sign line
        'DiMuonHighMassSameSign_Prescale'          :     0.5  ,
        'DiMuonHighMassSameSign_Postscale'         :     1.   ,
        'DiMuonHighMassSameSign_checkPV'           : False    ,

        # DiMuon Low Mass line
        'DiMuonLowMass_Prescale'                   :     1.   ,
        'DiMuonLowMass_Postscale'                  :     1.   ,
        'DiMuonLowMass_checkPV'                    : False    , 
        
        'DiMuonLowMass_MuonPT'                     :   650.   ,  # MeV
        'DiMuonLowMass_MuonP'                      : -8000.   ,  # MeV, no cut now
        'DiMuonLowMass_MuonTRCHI2DOF'              :     5.   , 
        'DiMuonLowMass_MinMass'                    :   500.   ,  # MeV
        'DiMuonLowMass_VCHI2PDOF'                  :    20.   ,       
        'DiMuonLowMass_PT'                         : -1000.   ,  # MeV, no cut now

        # Jpsi2MuMu line
        'Jpsi2MuMu_Prescale'                       :     1.   ,
        'Jpsi2MuMu_Postscale'                      :     1.   ,
        'Jpsi2MuMu_checkPV'                        : False    ,
        
        'Jpsi2MuMu_MuonPT'                         :   650.   ,  # MeV
        'Jpsi2MuMu_MuonP'                          :  8000.   ,  # MeV
        'Jpsi2MuMu_MuonPIDmu'                      :     0.   , 
        'Jpsi2MuMu_MuonTRCHI2DOF'                  :     5.   , 
        'Jpsi2MuMu_MinMass'                        :  3010.   ,  # MeV
        'Jpsi2MuMu_MaxMass'                        :  3170.   ,  # MeV
        'Jpsi2MuMu_VCHI2PDOF'                      :    20.   ,
        'Jpsi2MuMu_PT'                             :  3000.   ,  # MeV

        # Psi2MuMu line
        'Psi2MuMu_Prescale'                        :     1.   ,
        'Psi2MuMu_Postscale'                       :     1.   ,
        'Psi2MuMu_checkPV'                         : False    ,

        'Psi2MuMu_ParticleName'                    : "'psi(2S)'", # Particle Name, like "'psi(2S)'"   
        'Psi2MuMu_MuonPT'                          :  1000.   ,  # MeV
        'Psi2MuMu_MuonP'                           :  8000.   ,  # MeV
        'Psi2MuMu_MuonPIDmu'                       :     0.   , 
        'Psi2MuMu_MuonTRCHI2DOF'                   :     5.   , 
        'Psi2MuMu_MassWindow'                      :   120.   ,  # MeV
        'Psi2MuMu_VCHI2PDOF'                       :    20.   ,
        'Psi2MuMu_PT'                              :  3000.   ,   # MeV
        
        # DiMuonDetached line
        'DiMuonDetached_Prescale'                 :     0.   ,
        'DiMuonDetached_Postscale'                :     1.   ,
        
        'DiMuonDetached_MuonPT'                   :   500.   ,  # MeV
        'DiMuonDetached_MuonP'                    : -8000.   ,  # MeV, no cut now
        'DiMuonDetached_MuonPIDmu'                :    -5.   ,
        'DiMuonDetached_MuonTRCHI2DOF'            :     5.   , 
        'DiMuonDetached_MinMass'                  :  2950.   ,
        'DiMuonDetached_VCHI2PDOF'                :    20.   ,
        'DiMuonDetached_PT'                       : -1000.   ,  # MeV, no cut now 
        'DiMuonDetached_DLS'                      :     5.   ,  # mm, upstream of any PV
        
        # Jpsi2MuMuDetached line
        'Jpsi2MuMuDetached_Prescale'                       :     1.   ,
        'Jpsi2MuMuDetached_Postscale'                      :     1.   ,
        
        'Jpsi2MuMuDetached_MuonPT'                         :   500.   ,  # MeV
        'Jpsi2MuMuDetached_MuonP'                          : -8000.   ,  # MeV, no cut now
        'Jpsi2MuMuDetached_MuonPIDmu'                      :    -5.   , 
        'Jpsi2MuMuDetached_MuonTRCHI2DOF'                  :     5.   , 
        'Jpsi2MuMuDetached_MinMass'                        :  2996.916,  # MeV
        'Jpsi2MuMuDetached_MaxMass'                        :  3196.916,  # MeV
        'Jpsi2MuMuDetached_VCHI2PDOF'                      :    20.   ,
        'Jpsi2MuMuDetached_PT'                             : -1000.   ,  # MeV
        'Jpsi2MuMuDetached_DLS'                            :     3.   ,

        # Psi2MuMuDetachedDetached line
        'Psi2MuMuDetached_Prescale'                        :     1.   ,
        'Psi2MuMuDetached_Postscale'                       :     1.   ,

        'Psi2MuMuDetached_ParticleName'                    : "'psi(2S)'", # Particle Name, like "'psi(2S)'"   
        'Psi2MuMuDetached_MuonPT'                          :   500.   ,  # MeV
        'Psi2MuMuDetached_MuonP'                           : -8000.   ,  # MeV, no cut now
        'Psi2MuMuDetached_MuonPIDmu'                       :    -5.   , 
        'Psi2MuMuDetached_MuonTRCHI2DOF'                   :     5.   , 
        'Psi2MuMuDetached_MassWindow'                      :   120.   ,  # MeV
        'Psi2MuMuDetached_VCHI2PDOF'                       :    20.   ,
        'Psi2MuMuDetached_PT'                              : -1000.   ,   # MeV, no cut now
        'Psi2MuMuDetached_DLS'                             :     5.   
        },
    'STREAMS' : [ 'Dimuon' ],
    'WGs'    : [ 'PSWG' ]
    }

MicroDSTDiMuon = {
    'BUILDERTYPE' : 'DiMuonConf',
    'CONFIG' : {
        'MicroDST'                                 :   True   ,
    
        # DiMuon line
        'DiMuon_Prescale'                          :     1.   ,
        'DiMuon_Postscale'                         :     1.   ,
        'DiMuon_checkPV'                           :  False   ,  
        
        'DiMuon_MuonPT'                            :   650.   ,  # MeV
        'DiMuon_MuonP'                             : -8000.   ,  # MeV, no cut now 
        'DiMuon_MuonTRCHI2DOF'                     :     5.   , 
        'DiMuon_MinMass'                           :  2900.   ,  # MeV
        'DiMuon_VCHI2PDOF'                         :    20.   , 
        'DiMuon_PT'                                :  3000.   ,  # MeV

        # DiMuon Same Sign line
        'DiMuonSameSign_Prescale'                  :     0.05 ,
        'DiMuonSameSign_Postscale'                 :     1.   ,
        'DiMuonSameSign_checkPV'                   :  False   ,

        # DiMuonPrescaled line
        'DiMuonPrescaled_Prescale'                 :     0.1  ,
        'DiMuonPrescaled_Postscale'                :     1.   ,
        'DiMuonPrescaled_checkPV'                  : False    ,
        
        'DiMuonPrescaled_MuonPT'                   :   650.   ,  # MeV
        'DiMuonPrescaled_MuonP'                    : -8000.   ,  # MeV, no cut now 
        'DiMuonPrescaled_MuonTRCHI2DOF'            :     5.   , 
        'DiMuonPrescaled_MinMass'                  :  2900.   ,  # MeV
        'DiMuonPrescaled_VCHI2PDOF'                :    20.   , 
        'DiMuonPrescaled_PT'                       : -1000.   ,  # MeV, no cut now 

        # DiMuonExclusive line
        'DiMuonExclusive_Prescale'                 :     1.   ,
        'DiMuonExclusive_Postscale'                :     1.   ,
        'DiMuonExclusive_checkPV'                  :  True    ,
        
        'DiMuonExclusive_MuonPT'                   :   650.   ,  # MeV
        'DiMuonExclusive_MuonP'                    : -8000.   ,  # MeV, no cut now 
        'DiMuonExclusive_MuonTRCHI2DOF'            :     5.   , 
        'DiMuonExclusive_MinMass'                  :  2900.   ,
        'DiMuonExclusive_VCHI2PDOF'                :    20.   ,
        'DiMuonExclusive_PT'                       : -1000.   ,  # MeV, no cut now 
        'DiMuonExclusive_DZ'                       :    -1.   ,  # mm, upstream of any PV

        # DiMuonNoPV line
        'DiMuonNoPV_Prescale'                      :     1.   ,
        'DiMuonNoPV_Postscale'                     :     1.   ,
               
        # DiMuon High Mass line
        'DiMuonHighMass_Prescale'                  :     1.   ,
        'DiMuonHighMass_Postscale'                 :     1.   ,
        'DiMuonHighMass_checkPV'                   :  True    ,
        
        'DiMuonHighMass_MuonPT'                    :   650.   , # MeV
        'DiMuonHighMass_MuonP'                     : -8000.   , 
        'DiMuonHighMass_MuonTRCHI2DOF'             :     5.   , 
        'DiMuonHighMass_MinMass'                   :  8000.   , # MeV
        'DiMuonHighMass_VCHI2PDOF'                 :    20.   ,
        'DiMuonHighMass_PT'                        : -1000.   , # MeV, no cut now 

        # DiMuon High Mass Same Sign line
        'DiMuonHighMassSameSign_Prescale'          :     1.   ,
        'DiMuonHighMassSameSign_Postscale'         :     1.   ,
        'DiMuonHighMassSameSign_checkPV'           :  True    ,

        # DiMuon Low Mass line
        'DiMuonLowMass_Prescale'                   :     1.   ,
        'DiMuonLowMass_Postscale'                  :     1.   ,
        'DiMuonLowMass_checkPV'                    :  True    , 
        
        'DiMuonLowMass_MuonPT'                     :   650.   ,  # MeV
        'DiMuonLowMass_MuonP'                      : -8000.   ,  # MeV, no cut now
        'DiMuonLowMass_MuonTRCHI2DOF'              :     5.   , 
        'DiMuonLowMass_MinMass'                    :   500.   ,  # MeV
        'DiMuonLowMass_VCHI2PDOF'                  :    20.   ,       
        'DiMuonLowMass_PT'                         : -1000.   ,  # MeV, no cut now

        # Jpsi2MuMu line
        'Jpsi2MuMu_Prescale'                       :     1.   ,
        'Jpsi2MuMu_Postscale'                      :     1.   ,
        'Jpsi2MuMu_checkPV'                        :  True    ,
        
        'Jpsi2MuMu_MuonPT'                         :   650.   ,  # MeV
        'Jpsi2MuMu_MuonP'                          : -8000.   ,  # MeV, no cut now
        'Jpsi2MuMu_MuonPIDmu'                      :     0.   ,  
        'Jpsi2MuMu_MuonTRCHI2DOF'                  :     5.   , 
        'Jpsi2MuMu_MinMass'                        :  3010.   ,  # MeV
        'Jpsi2MuMu_MaxMass'                        :  3170.   ,  # MeV
        'Jpsi2MuMu_VCHI2PDOF'                      :    20.   ,
        'Jpsi2MuMu_PT'                             :  3000.0  ,  # MeV

        # Psi2MuMu line
        'Psi2MuMu_Prescale'                        :     1.   ,
        'Psi2MuMu_Postscale'                       :     1.   ,
        'Psi2MuMu_checkPV'                         :  True    ,

        'Psi2MuMu_ParticleName'                    : "'psi(2S)'", # Particle Name, like "'psi(2S)'"   
        'Psi2MuMu_MuonPT'                          :  1000.   ,  # MeV
        'Psi2MuMu_MuonP'                           :  8000.   ,  # MeV
        'Psi2MuMu_MuonPIDmu'                           :     0.   , 
        'Psi2MuMu_MuonTRCHI2DOF'                   :     5.   , 
        'Psi2MuMu_MassWindow'                      :   120.   ,  # MeV
        'Psi2MuMu_VCHI2PDOF'                       :    20.   ,
        'Psi2MuMu_PT'                              :  2000.   ,  # MeV

        # DiMuonDetached line
        'DiMuonDetached_Prescale'                 :     1.   ,
        'DiMuonDetached_Postscale'                :     1.   ,
        
        'DiMuonDetached_MuonPT'                   :   500.   ,  # MeV
        'DiMuonDetached_MuonP'                    : -8000.   ,  # MeV, no cut now
        'DiMuonDetached_MuonPIDmu'                :    -5.   ,
        'DiMuonDetached_MuonTRCHI2DOF'            :     5.   , 
        'DiMuonDetached_MinMass'                  :  2950.   ,
        'DiMuonDetached_VCHI2PDOF'                :    20.   ,
        'DiMuonDetached_PT'                       : -1000.   ,  # MeV, no cut now 
        'DiMuonDetached_DLS'                      :     5.   ,  # mm, upstream of any PV
        
        # Jpsi2MuMuDetached line
        'Jpsi2MuMuDetached_Prescale'                       :     1.   ,
        'Jpsi2MuMuDetached_Postscale'                      :     1.   ,
        
        'Jpsi2MuMuDetached_MuonPT'                         :   500.   ,  # MeV
        'Jpsi2MuMuDetached_MuonP'                          : -8000.   ,  # MeV, no cut now
        'Jpsi2MuMuDetached_MuonPIDmu'                      :    -5.   , 
        'Jpsi2MuMuDetached_MuonTRCHI2DOF'                  :     5.   , 
        'Jpsi2MuMuDetached_MinMass'                        :  2976.916,  # MeV
        'Jpsi2MuMuDetached_MaxMass'                        :  3216.916,  # MeV
        'Jpsi2MuMuDetached_VCHI2PDOF'                      :    20.   ,
        'Jpsi2MuMuDetached_PT'                             : -1000.   ,  # MeV
        'Jpsi2MuMuDetached_DLS'                            :     3.   ,

        # Psi2MuMuDetachedDetached line
        'Psi2MuMuDetached_Prescale'                        :     1.   ,
        'Psi2MuMuDetached_Postscale'                       :     1.   ,

        'Psi2MuMuDetached_ParticleName'                    : "'psi(2S)'", # Particle Name, like "'psi(2S)'"   
        'Psi2MuMuDetached_MuonPT'                          :   500.   ,  # MeV
        'Psi2MuMuDetached_MuonP'                           : -8000.   ,  # MeV, no cut now
        'Psi2MuMuDetached_MuonPIDmu'                       :    -5.   , 
        'Psi2MuMuDetached_MuonTRCHI2DOF'                   :     5.   , 
        'Psi2MuMuDetached_MassWindow'                      :   120.   ,  # MeV
        'Psi2MuMuDetached_VCHI2PDOF'                       :    20.   ,
        'Psi2MuMuDetached_PT'                              : -1000.   ,   # MeV, no cut now
        'Psi2MuMuDetached_DLS'                             :     5.
        }, 
    'STREAMS' : [ 'Leptonic' ] ,
    'WGs'    : [ 'PSWG' ]
    }


Bc2JpsiMu = {
    'BUILDERTYPE' : 'Bc2JpsiMuXConf',
    'CONFIG' : {
    'LinePrescale'        :    1.   ,
    'LinePostscale'       :    1.   ,
    
    'MuonTRCHI2DOF'       :    5.   ,  # adimentional 
    'MuonPT'              : 1400.   ,  # MeV
    'MuonP'               :   -5.   ,  # MeV, not applied now
    'MuMuParticleName'    : "'J/psi(1S)'", # Particle Name, like "'psi(2S)'"
    'MuMuMassWindow'      :  200.   ,  # MeV, 10 sigma, may decreased to 150
    'MuMuVtxCHI2'         :    9.   ,  # adimentional
    'MuMuPT'              :  -10.   ,  # MeV, not applied 
    
    'MuonBcTRCHI2DOF'     :    5.   ,  # adimentional     
    'MuonBcPT'            : 2500.   ,  # MeV
    'MuonBcP'             :   -5.   ,  # MeV, not applied now
    'BcUpperMass'         : 6400.   ,  # MeV, Upper limit for partial rec. 
    'BcLowerMass'         : 3200.   ,  # MeV, Lower limit for partial rec.
    'BcVtxCHI2'           :    9.   ,  # adimentional
    'BcPT'                : 6000.      # MeV, May incrase up to 5000 MeV if needed       
    },
    'STREAMS' : [ 'Dimuon' ] ,
    'WGs'    : [ 'PSWG' ]
  }

Bc2JpsiH = {
    'BUILDERTYPE'	: 'Bc2JpsiHConf',
    'CONFIG'	: {
    'LinePrescale'        :    1.   ,
    'LinePostscale'       :    1.   ,
    
    'MuonTRCHI2DOF'       :    5.   ,  # adimentional 
    'MuonPT'              : 1200.   ,  # MeV
    'MuonP'               :   -5.   ,  # MeV, not applied now
    'MuMuParticleName'    : "'J/psi(1S)'", # Particle Name, like "'psi(2S)'"
    'MuMuMassWindow'      :   80.   ,  # MeV, 10 sigma, may decreased to 150
    'MuMuVtxCHI2'         :    9.   ,  # adimentional
    'MuMuPT'              : 2000.   ,  # MeV
    
    'PionTRCHI2DOF'       :    5.   ,  # adimentional     
    'PionPT'              : 2000.   ,  # MeV
    'PionP'               :   -5.   ,  # MeV, not applied now
    'BcMassWindow'        :  400.   ,  # MeV, mass window
    'BcVtxCHI2'           :    9.   ,  # adimentional
    'BcPT'                : 6000.   ,  # MeV, May incrase up to 5000 MeV if needed       

    'PionIPCHI2'          :   -1.   ,
    'MuonIPCHI2'          :   -1.   ,
    'MuMuIPCHI2'          :   -1.   ,
    'BcIPCHI2'            :    5.0e+9,
    'BcLTIME'             :   -1.0e+9  
    },
    'STREAMS' : [ 'Dimuon' ],
    'WGs'    : [ 'PSWG' ]
    }

Bc2JpsiHDetached = {
    'BUILDERTYPE'   : 'Bc2JpsiHConf',
    'CONFIG'	: {
    'LinePrescale'        :    1.   ,
    'LinePostscale'       :    1.   ,
    
    'MuonTRCHI2DOF'       :    5.   ,  # adimentional 
    'MuonPT'              :  900.   ,  # MeV
    'MuonP'               :   -5.   ,  # MeV, not applied now
    'MuMuParticleName'    : "'J/psi(1S)'", # Particle Name, like "'psi(2S)'"
    'MuMuMassWindow'      :  100.   ,  # MeV, 10 sigma, may decreased to 150
    'MuMuVtxCHI2'         :    9.   ,  # adimentional
    'MuMuPT'              :  -10.   ,  # MeV, not applied 
    
    'PionTRCHI2DOF'       :    5.   ,  # adimentional     
    'PionPT'              : 1400.   ,  # MeV
    'PionP'               :   -5.   ,  # MeV, not applied now
    'BcMassWindow'        :  400.   ,  # MeV, mass window
    'BcVtxCHI2'           :    9.   ,  # adimentional
    'BcPT'                : 4000.   ,  # MeV, May incrase up to 5000 MeV if needed       

    'PionIPCHI2'          :   -1.   ,
    'MuonIPCHI2'          :   -1.   ,
    'MuMuIPCHI2'          :   -1.   ,
    'BcIPCHI2'            :    5.0e+9,
    'BcLTIME'             :    0.1  
    },
    'STREAMS'   : [ 'Dimuon' ],
    'WGs'    : [ 'PSWG' ]
}


BuToKX3872 = {
    'BUILDERTYPE'	: 'StrippingBu2KX3872Conf',
    'CONFIG' : {
        'Prescale_BuToKX3872'    : 1.0,
    'Postscale_BuToKX3872'   : 1.0,

    'Prescale_BuToKX3872Loose'    : 1.0,
    'Postscale_BuToKX3872Loose'   : 1.0,

    'Prescale_BuToKPsi2S'    : 1.0,
    'Postscale_BuToKPsi2S'   : 1.0,

    'Prescale_BuToKPsi2SLoose'    : 1.0,
    'Postscale_BuToKPsi2SLoose'   : 1.0,

    # B cuts
    'Bu_Comb_MassWindow'         : 450.0,
    'Bu_Comb_MassWindowLoose'    : 500.0,
    'Bu_MassWindow'              : 400.0,
    'Bu_MassWindowLoose'         : 400.0,
    'Bu_VertexCHI2'              :   5.0,
    'Bu_VertexCHI2Loose'         :   7.0,
    'Bu_IPCHI2'                  :  15.0,
    'Bu_IPCHI2Loose'             :  20.0,
    'Bu_FlightCHI2'              :  30.0,
    'Bu_FlightCHI2Loose'         :  20.0,
    'Bu_DIRA'                    :   0.9995,
    'Bu_DIRALoose'               :   0.9995,
   
    # X3872 / Psi(2S) cuts
    'X3872_Comb_MassWindow'      : 180.0,
    'X3872_Comb_MassWindowLoose' : 220.0,
    'X3872_MassWindow'           : 150.0,
    'X3872_MassWindowLoose'      : 190.0,
    'X3872_VertexCHI2'           :   8.0,
    'X3872_VertexCHI2Loose'      :  10.0,

    # Track cuts
    'Track_CHI2nDOF'       : 4.0,

    # Kaon cuts
    'Kaon_MinIPCHI2'       : 4.5,
    'Kaon_MinIPCHI2Loose'  : 4.0,

    # Pion cuts
    'Pion_MinIPCHI2'       : 4.5,
    'Pion_MinIPCHI2Loose'  : 4.0,

    # JPsi cuts
    'JPsi_MassWindow'       :  70.0,
    'JPsi_MassWindowLoose'  :  70.0,
    'JPsi_VertexCHI2'       :  10.0,
    'JPsi_VertexCHI2Loose'  :  10.0,

    # Muon cuts
    'Muon_MinIPCHI2'        :   1.5,
    'Muon_MinIPCHI2Loose'   :   1.5,
    'Muon_PT'               : 500.0,
    'Muon_IsMuon'           : True
        },
    'STREAMS' : [ 'Dimuon' ],
    'WGs'    : [ 'PSWG' ]
    }

FullDSTDiElectron = {
    'BUILDERTYPE'	: 'DiElectronConf',
    'CONFIG'	: {
            'MicroDST'                                      :  False  ,
        
        # DiElectron line
        'DiElectron_Prescale'                           :     1.  ,
        'DiElectron_Postscale'                          :     1.  ,
        'DiElectron_checkPV'                            : False   , 
        
        'DiElectron_ElectronPT'                         :  1200.  ,  # MeV
        'DiElectron_ElectronP'                          : -8000.  ,  # MeV, no cut now
        'DiElectron_ElectronPIDe'                       :     5.  ,  
        'DiElectron_ElectronTRCHI2DOF'                  :     5.  , 
        'DiElectron_MinMass'                            :  2000.  ,  # MeV
        'DiElectron_VCHI2PDOF'                          :    16.  , 
        'DiElectron_PT'                                 :  2000.  ,  # MeV, no cut now 

        # DiElectronLowMass line
        'DiElectronLowMass_Prescale'                    :     0.04,
        'DiElectronLowMass_Postscale'                   :     1.  ,
        'DiElectronLowMass_checkPV'                     : False   ,   
        
        'DiElectronLowMass_ElectronPT'                  :  1200.  ,  # MeV
        'DiElectronLowMass_ElectronP'                   : -8000.  ,  # MeV, no cut now
        'DiElectronLowMass_ElectronPIDe'                :     5.  , 
        'DiElectronLowMass_ElectronTRCHI2DOF'           :     5.  , 
        'DiElectronLowMass_MinMass'                     :    30.  ,  # MeV
        'DiElectronLowMass_VCHI2PDOF'                   :    16.  ,       
        'DiElectronLowMass_PT'                          : -1000.  ,  # MeV, no cut now
        
        # BiasedDiElectron line
        'BiasedDiElectron_Prescale'                     :     1.  ,
        'BiasedDiElectron_Postscale'                    :     1.  ,
        
        'BiasedDiElectron_ElectronPT'                   :   500.  ,  # MeV
        'BiasedDiElectron_ElectronP'                    : -8000.  ,  # MeV, no cut now
        'BiasedDiElectron_ElectronPIDe'                 :     5.  ,
        'BiasedDiElectron_ElectronMIPCHI2'              :    16.  , 
        'BiasedDiElectron_ElectronTRCHI2DOF'            :     5.  , 
        'BiasedDiElectron_MinMass'                      :  2000.  ,  # MeV
        'BiasedDiElectron_VCHI2PDOF'                    :    16.  ,
        'BiasedDiElectron_PT'                           : -1000.  ,  # MeV, no cut now 

        # BiasedDiElectronLowMass line
        'BiasedDiElectronLowMass_Prescale'              :     0.01 ,
        'BiasedDiElectronLowMass_Postscale'             :     1.  ,
        
        'BiasedDiElectronLowMass_ElectronPT'            :   500.  ,  # MeV
        'BiasedDiElectronLowMass_ElectronP'             : -8000.  ,  # MeV
        'BiasedDiElectronLowMass_ElectronPIDe'          :     0.  ,
        'BiasedDiElectronLowMass_ElectronMIPCHI2'       :    16.  ,  
        'BiasedDiElectronLowMass_ElectronTRCHI2DOF'     :     9.  , 
        'BiasedDiElectronLowMass_MinMass'               :    30.  ,  # MeV
        'BiasedDiElectronLowMass_VCHI2PDOF'             :    16.  ,
        'BiasedDiElectronLowMass_PT'                    : -1000.  ,  # MeV, no cut now
        
        # Jpsi2ee line
        'Jpsi2ee_Prescale'                              :     1.  ,
        'Jpsi2ee_Postscale'                             :     1.  ,
        'Jpsi2ee_checkPV'                               : False   ,   
        
        'Jpsi2ee_ElectronPT'                            :  1200.  ,  # MeV
        'Jpsi2ee_ElectronP'                             : -8000.  ,  # MeV, no cut now
        'Jpsi2ee_ElectronPIDe'                          :     5.  ,
        'Jpsi2ee_ElectronTRCHI2DOF'                     :     5.  , 
        'Jpsi2ee_MinMass'                               :  2000.  ,  # MeV
        'Jpsi2ee_MaxMass'                               :  3800.  ,  # MeV
        'Jpsi2ee_VCHI2PDOF'                             :    16.  ,
        'Jpsi2ee_PT'                                    :  2000.     # MeV
        },
    'STREAMS' : [ 'Dielectron' ],
    'WGs'    : [ 'PSWG' ]
    }

MicroDSTDiElectron = {
    'BUILDERTYPE'	:	'DiElectronConf',
    'CONFIG'	: {
        'MicroDST'                                      :  True   ,
        
        # DiElectron line
        'DiElectron_Prescale'                           :     1.  ,
        'DiElectron_Postscale'                          :     1.  ,        
        'DiElectron_checkPV'                            :  True   ,
        
        'DiElectron_ElectronPT'                         :  1100.  ,  # MeV
        'DiElectron_ElectronP'                          : -8000.  ,  # MeV, no cut now
        'DiElectron_ElectronPIDe'                       :     2.  ,  
        'DiElectron_ElectronTRCHI2DOF'                  :     5.  , 
        'DiElectron_MinMass'                            :  2000.  ,  # MeV
        'DiElectron_VCHI2PDOF'                          :    16.  , 
        'DiElectron_PT'                                 : -1000.  ,  # MeV, no cut now 

        # DiElectronLowMass line
        'DiElectronLowMass_Prescale'                    :     1.  ,
        'DiElectronLowMass_Postscale'                   :     1.  ,
        'DiElectronLowMass_checkPV'                     :  True   , 
        
        'DiElectronLowMass_ElectronPT'                  :   750.  ,  # MeV
        'DiElectronLowMass_ElectronP'                   : -8000.  ,  # MeV, no cut now
        'DiElectronLowMass_ElectronPIDe'                :     2.  , 
        'DiElectronLowMass_ElectronTRCHI2DOF'           :     5.  , 
        'DiElectronLowMass_MinMass'                     :    30.  ,  # MeV
        'DiElectronLowMass_VCHI2PDOF'                   :    16.  ,       
        'DiElectronLowMass_PT'                          : -1000.  ,  # MeV, no cut now
        
        # BiasedDiElectron line
        'BiasedDiElectron_Prescale'                     :     1.  ,
        'BiasedDiElectron_Postscale'                    :     1.  ,
        
        'BiasedDiElectron_ElectronPT'                   :   500.  ,  # MeV
        'BiasedDiElectron_ElectronP'                    : -8000.  ,  # MeV, no cut now
        'BiasedDiElectron_ElectronPIDe'                 :     2.  ,
        'BiasedDiElectron_ElectronMIPCHI2'              :     4.  , 
        'BiasedDiElectron_ElectronTRCHI2DOF'            :     5.  , 
        'BiasedDiElectron_MinMass'                      :  2000.  ,  # MeV
        'BiasedDiElectron_VCHI2PDOF'                    :    16.  ,
        'BiasedDiElectron_PT'                           : -1000.  ,  # MeV, no cut now 

        # BiasedDiElectronLowMass line
        'BiasedDiElectronLowMass_Prescale'              :     0.1  ,
        'BiasedDiElectronLowMass_Postscale'             :     1.  ,

        'BiasedDiElectronLowMass_ElectronPT'            :   500.  ,  # MeV
        'BiasedDiElectronLowMass_ElectronP'             : -8000.  ,  # MeV
        'BiasedDiElectronLowMass_ElectronPIDe'          :     0.  ,
        'BiasedDiElectronLowMass_ElectronMIPCHI2'       :     4.  ,  
        'BiasedDiElectronLowMass_ElectronTRCHI2DOF'     :     5.  , 
        'BiasedDiElectronLowMass_MinMass'               :    30.  ,  # MeV
        'BiasedDiElectronLowMass_VCHI2PDOF'             :    16.  ,
        'BiasedDiElectronLowMass_PT'                    :  -1000. ,  # MeV, no cut now
        
        # Jpsi2ee line
        'Jpsi2ee_Prescale'                              :     1.  ,
        'Jpsi2ee_Postscale'                             :     1.  ,
        'Jpsi2ee_checkPV'                               :  True   , 
        
        'Jpsi2ee_ElectronPT'                            :  1200.  ,  # MeV
        'Jpsi2ee_ElectronP'                             : -8000.  ,  # MeV, no cut now
        'Jpsi2ee_ElectronPIDe'                          :     4.  ,
        'Jpsi2ee_ElectronTRCHI2DOF'                     :     5.  , 
        'Jpsi2ee_MinMass'                               :  2000.  ,  # MeV
        'Jpsi2ee_MaxMass'                               :  3800.  ,  # MeV
        'Jpsi2ee_VCHI2PDOF'                             :    16.  ,
        'Jpsi2ee_PT'                                    : -1500.0    # MeV
        },
    'STREAMS' : [ 'Leptonic' ] ,
    'WGs'    : [ 'PSWG' ]
}


Ccbar2Ppbar = {
    'BUILDERTYPE'	: 'Ccbar2PpbarConf',
    'CONFIG'	: { 'LinePrescale'     :    1.   ,
                    'LinePostscale'    :    1.   ,
                    
                    'SpdMult'          :   300.  , # dimensionless, Spd Multiplicy cut 
                    'ProtonPT'         :  1900.  , # MeV
                    'ProtonP'          :    -2.  , # MeV
                    'ProtonTRCHI2DOF' :      5.  ,
                    'ProtonPIDppi'     :    20.  , # CombDLL(p-pi)
                    'ProtonPIDpK'      :    10.  , # CombDLL(p-K)
                    'CombMaxMass'      :  4100.  , # MeV, before Vtx fit
                    'CombMinMass'      :  2750.  , # MeV, before Vtx fit
                    'MaxMass'          :  4000.  , # MeV, after Vtx fit
                    'MinMass'          :  2800.  , # MeV, after Vtx fit
                    'VtxCHI2'          :     9.  , # dimensionless
                    'CCPT'             :  6000.    
                   },
    'STREAMS' : [ 'CharmCompleteEvent' ] ,
    'WGs'    : [ 'PSWG' ]
}

Ccbar2PpbarExclusive = {
    'BUILDERTYPE'	: 'Ccbar2PpbarConf',
    'CONFIG'	: {  'LinePrescale'     :     1.  ,
                     'LinePostscale'    :     1.  ,
                     
                     'SpdMult'          :    20.  , # dimensionless, Spd Multiplicy cut 
                     'ProtonPT'         :   500.  , # MeV
                     'ProtonP'          :    -2.  , # MeV
                     'ProtonTRCHI2DOF' :      5.  ,
                     'ProtonPIDppi'     :    10.  , # CombDLL(p-pi)
                     'ProtonPIDpK'      :     5.  , # CombDLL(p-K)
                     'CombMaxMass'      :  1.0e+6 , # MeV, before Vtx fit
                     'CombMinMass'      :     0.  , # MeV, before Vtx fit
                     'MaxMass'          :  1.0e+6 , # MeV, after Vtx fit
                     'MinMass'          :     0.  , # MeV, after Vtx fit
                     'VtxCHI2'          :     9.  , # dimensionless
                     'CCPT'             :    -2.    
                     },
    'STREAMS' : [ 'CharmCompleteEvent' ] ,
    'WGs'    : [ 'PSWG' ]
    }

Ccbar2Baryons = {
    'BUILDERTYPE'  : 'Ccbar2BaryonsConf',
    'CONFIG'       : {
    'TRCHI2DOF'        :    5.   ,
    'CombMaxMass'      :  4100.  , # MeV, before Vtx fit
    'CombMinMass'      :  2750.  , # MeV, before Vtx fit
    'MaxMass'          :  4000.  , # MeV, after Vtx fit
    'MinMass'          :  2800.    # MeV, after Vtx fit
    },
    'STREAMS' : [ 'CharmCompleteEvent' ],
    'WGs'    : ['PSWG']
    }

## CharmAssociative = {
##     'BUILDERTYPE'  :'StrippingCharmAssociativeConf',
##     'CONFIG'  : { 'DiCharmPrescale'        : 1.0 ,
##                   'DiMuonAndCharmPrescale' : 1.0 ,
##                   'DiMuonAndGammaPrescale' : 1.0 ,
##                   'DoubleDiMuonPrescale'   : 1.0 ,
##                   ##  allow to move up to 4 GeV or even 5 GeV
##                   'PhotonCuts'  : " PT > 3 * GeV "
##                   },
##     'STREAMS' : [ 'Dimuon' ] ,
##     'WGs'     : [ 'PSWG'    ]
##     }

HeavyBaryon = {
    'BUILDERTYPE'  : 'HeavyBaryonsConf',
    'CONFIG'       : {    'TRCHI2DOF'             :       4.,
                          'PionPIDK'              :       5.,
                          'KaonPIDK'              :      -5.,
                          'DLSForLongLived'       :       5.,
                          'XiMassWindow'          :      50.,
                          'XistarMassWindow'      :      150.,
                          'OmegaMassWindow'       :      500.,
                          "XibminusMassWindow"    :      300.,
                          "XibzeroMassWindow"     :      500.,
                          "OmegabminusMassWindow" :      500.    
                          },
    'STREAMS' : [ 'Dimuon' ],
    'WGs'    : ['PSWG']
    }



