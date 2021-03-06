##
## B-> hh, pp 
##
Hb2Charged2Body = {
    'WGs'         : ['Charmless'],
    'BUILDERTYPE' : 'Hb2Charged2BodyLines',
    'CONFIG'      : {
           'PrescaleB2Charged2Body'   : 1,      'PrescaleB2PPbar' : 0, 'PrescaleBs2KK_NoIPCutOnB' : 1,  
           'MinPTB2Charged2Body'      : 1100,    'MinPTB2PPbar' : 900,
           'MinIPB2Charged2Body'      : 0.12,
           'MinIPChi2B2Charged2Body'  : 100,     'MinIPChi2B2PPbar' : 9,
           'TrChi2'                   : 5,
           'PIDppi'                   : 0,
           'PIDpk'                    : -2, 
           'MaxPTB2Charged2Body'      : 2500,   'MaxPTB2PPbar' : 2100,
           'MaxIPB2Charged2Body'      : 0.25,
           'MaxIPChi2B2Charged2Body'  : 200,    'MaxIPChi2B2PPbar' : 25, 
           'CombMassLow'              : 4800,
           'CombMassHigh'             : 5800,   'CombMassWindow' : 200,
           'DOCA'                     : 0.08,   'VertexChi2B2PPbar' : 16, 
           'BPT'                      : 1200,   'BPTB2PPbar' : 1000,
           'BIP'                      : 0.1,
           'BIPChi2B2Charged2Body'    : 12,     'BIPChi2B2PPbar' : 36, 
           'BDIRA'                    : 0.9995,
           'BTAU'                     : 0.0006,
           'MassLow'                  : 4800,
           'MassHigh'                 : 5800,
           'PIDKBs2KK_NoIPCutOnB'     : 0.
           },
    'STREAMS'     : [ 'Bhadron' ]
    }


# Quasi two-body (Fred Blanc)
BetaSQ2B = {
    'WGs'               : ['Charmless'],
    'BUILDERTYPE'       : 'B2Quasi2BodyConf',
    'CONFIG' : { 'Q2BPrescale'     : 1.,
                 'Q2BResMinPT'     : 600.,
                 'Q2BResMinP'      : 10.,
                 'Q2BResMaxMass'   : 1000.,
                 'Q2BResVtxChiDOF' : 9.,
                 'Q2BBMinM4pi'     : 3800.,
                 'Q2BBMinM3pi'     : 4400.,
                 'Q2BBMaxM'        : 5700.,
                 'Q2BBMinCorrM4pi' : 0.,
                 'Q2BBMinCorrM3pi' : 0.,
                 'Q2BBMaxCorrM'    : 6000.,
                 'Q2BBVtxChi2DOF'  : 9.
                 },
    'STREAMS' : [ 'Bhadron' ]
    }

# New line since Stripping17b
B2KPhiPhi = {
    'WGs'               : ['Chamless'],
    'BUILDERTYPE'       : 'B2KPhiPhiConf',
    'CONFIG' : {
    'Prescale'                : 1.0 ,
    'Postscale'               : 1.0 ,

    #K parameters
    'K_PT'                    : 300.,         
    'K_IPCHI2'                : 6.,
    'K_TRCHI2'                : 5.0,       
    
    #phi parameters         
    'phi_VDZ'                 : 0.,
    'phi_PT'                  : 300.,             
    'phi_IPCHI2'              : 6.,
    'phi_VCHI2_VDOF'          : 25.,
    'phi_MM_max'              : 1070., 
    'phi_MM_min'              : 980.,
 
    #B parameters
    'B_ADAMASS'              : 500.,
    'B_DIRA'                 : 0.999,
    'B_IPCHI2'               : 12.25,   
    'B_VCHI2_VDOF'           : 25.,
    'B_LTCHI2'               : 64.,
    'B_VDZ'                  : 0.
    },
    'STREAMS' : [ 'Bhadron' ]
    }

##
## B->Kshh
##
B2KShh = {
    'BUILDERTYPE' : 'B2KShhConf',
    'CONFIG' : {  'Trk_Chi2'                : 4.0,
                  'KS_DD_MassWindow'        : 30.0,
                  'KS_DD_VtxChi2'           : 12.0,
                  'KS_DD_FDChi2'            : 50.0,
                  'KS_DD_Pmin'              : 6000.0,
                  'KS_LL_MassWindow'        : 20.0,
                  'KS_LL_VtxChi2'           : 12.0,
                  'KS_LL_FDChi2'            : 80.0,
                  'B_Mlow'                  : 500.0,
                  'B_Mhigh'                 : 587.0,
                  'B_APTmin'                : 1000.0,
                  'B_PTmin'                 : 1500.0,
                  'BDaug_MedPT_PT'          : 800.0,
                  'BDaug_MaxPT_IP'          : 0.05,
                  'BDaug_DD_maxDocaChi2'    : 5.0,
                  'BDaug_LL_maxDocaChi2'    : 5.0,
                  'BDaug_DD_PTsum'          : 3000.0,
                  'BDaug_LL_PTsum'          : 3000.0,
                  'B_VtxChi2'               : 12.0,
                  'B_Dira'                  : 0.9999,
                  'B_DD_IPCHI2wrtPV'        : 8.0,
                  'B_LL_IPCHI2wrtPV'        : 8.0,
                  'B_FDwrtPV'               : 1.0,
                  'B_DD_FDChi2'             : 50.0,
                  'B_LL_FDChi2'             : 50.0,
                  'GEC_MaxTracks'           : 250,
                  'Prescale'                : 1.0,
                  'Postscale'               : 1.0
                 },
    'STREAMS' : [ 'Bhadron' ],
    'WGs' : ['Chamless']
    }





