################################################################################
##                          S T R I P P I N G  21r0p1                         ##
##                                                                            ##
##  Configuration for SL WG                                                   ##
##  Contact person: Greg Ciezarek (gregory.max.ciezarek@cern.ch)              ##
################################################################################

from GaudiKernel.SystemOfUnits import *
B23MuNu = {
    "BUILDERTYPE": "B23MuNuConf", 
    "CONFIG": {
        "BPT": 2000.0, 
        "CORRM_MAX": 10000.0, 
        "CORRM_MIN": 2500.0, 
        "DIRA": 0.999, 
        "FlightChi2": 50.0, 
        "LOWERMASS": 0.0, 
        "MisIDPrescale": 0.01, 
        "Muon_MinIPCHI2": 9.0, 
        "Muon_PIDmu": 0.0, 
        "Muon_PIDmuK": 0.0, 
        "Muon_PT": 0, 
        "SpdMult": 600, 
        "Track_CHI2nDOF": 3.0, 
        "Track_GhostProb": 0.35, 
        "UPPERMASS": 7500.0, 
        "VertexCHI2": 4.0
    }, 
    "STREAMS": {
        "Semileptonic": [ 
            "StrippingB23MuNu_TriFakeMuLine" 
        ]
    },  
    "WGs": [ "Semileptonic" ]
}

B2DMuForTauMu = {
    "BUILDERTYPE": "B2DMuForTauMuconf", 
    "CONFIG": {
        "BDIRA": 0.999, 
        "BVCHI2DOF": 6.0, 
        "DSumPT": 2500.0, 
        "DsAMassWin": 100.0, 
        "DsDIRA": 0.999, 
        "DsFDCHI2": 25.0, 
        "DsMassWin": 80.0, 
        "DsVCHI2DOF": 4.0, 
        "GhostProb": 0.5, 
        "Hlt2Line": "HLT_PASS_RE('Hlt2CharmHadLambdaC2PiPKDecision')", 
        "Hlt2LineFake": "HLT_PASS_RE('Hlt2CharmHadLambdaC2PiPKDecision')", 
        "KPiPT": 300.0, 
        "KaonPIDK": 4.0, 
        "MINIPCHI2": 9.0, 
        "MuonIPCHI2": 16.0, 
        "PIDmu": -200.0, 
        "PionPIDKTight": 2.0, 
        "ProtonPIDp": 0.0, 
        "SPDmax": 600,
        "FakePrescale": 0.1
    }, 
    "STREAMS": {
        "Semileptonic": [ 
            "Strippingb2LcMuXB2DMuForTauMuLine",
            "Strippingb2LcMuXFakeB2DMuForTauMuLine" 
        ]
    }, 
    "WGs": [ "Semileptonic" ]
}

B2DMuNuX = {
    "BUILDERTYPE": "B2DMuNuXAllLinesConf", 
    "CONFIG": {
        "B_DIRA": 0.999, 
        "B_D_DZ": -0.05, 
        "B_DocaChi2Max": 10, 
        "B_MassMax": 8000.0, 
        "B_MassMin": 2200.0, 
        "B_VCHI2DOF": 9.0, 
        "D_AMassWin": 90.0, 
        "D_BPVDIRA": 0.99, 
        "D_DocaChi2Max": 20, 
        "D_FDCHI2": 25.0, 
        "D_MassWin": {
            "Omegac": 60.0, 
            "Xic0": 60.0, 
            "default": 80.0
        }, 
        "D_VCHI2DOF": 6.0, 
        "ElectronPIDe": 0.0, 
        "ElectronPT": 1200.0, 
        "GEC_nLongTrk": 250, 
        "HLT1": "HLT_PASS_RE('Hlt1.*Decision')", 
        "HLT2": "HLT_PASS_RE('Hlt2(SingleMuon|Topo).*Decision')", 
        "HadronIPCHI2": 4.0, 
        "HadronP": 2000.0, 
        "HadronPT": 250.0, 
        "KaonP": 2000.0, 
        "KaonPIDK": -2.0, 
        "Monitor": False, 
        "MuonIPCHI2": 9.0, 
        "MuonP": 6000.0, 
        "MuonPIDmu": 0.0, 
        "MuonPT": 1000.0, 
        "PionPIDK": 20.0, 
        "ProtonP": 8000.0, 
        "ProtonPIDp": 0.0, 
        "ProtonPIDpK": 0.0, 
        "TRCHI2": 3.0, 
        "TRGHOSTPROB": 0.35, 
        "TTSpecs": {
            "Hlt1.*Track.*Decision%TOS": 0, 
            "Hlt2Global%TIS": 0, 
            "Hlt2SingleMuon.*Decision%TOS": 0, 
            "Hlt2Topo.*Decision%TOS": 0
        }, 
        "UseNoPIDsInputs": False, 
        "prescaleFakes": 0.02, 
        "prescales": {
            "StrippingB2DMuNuX_D0_Electron": 0.1
        }
    }, 
    "STREAMS": {
        "Semileptonic": [ 
            "StrippingB2DMuNuX_D0",
            "StrippingB2DMuNuX_D0_FakeMuon",
            "StrippingB2DMuNuX_Dp",
            "StrippingB2DMuNuX_Dp_FakeMuon",
            "StrippingB2DMuNuX_Ds",
            "StrippingB2DMuNuX_Ds_FakeMuon",
            "StrippingB2DMuNuX_Lc",
            "StrippingB2DMuNuX_Lc_FakeMuon",
            "StrippingB2DMuNuX_Omegac", 
            "StrippingB2DMuNuX_Xic", 
            "StrippingB2DMuNuX_Xic0",
            "StrippingB2DMuNuX_Omegac_FakeMuon", 
            "StrippingB2DMuNuX_Xic_FakeMuon", 
            "StrippingB2DMuNuX_Xic0_FakeMuon"
        ]
    }, 
    "WGs": [ "Semileptonic" ]
}



B2XTauNuAllLines = {
    "BUILDERTYPE": "B2XTauNuAllLinesConf", 
    "CONFIG": {
        "B_BPVDIRA": 0.995, 
        "B_DOCAMAX": 0.15, 
        "B_DeltaM_high": 300.0, 
        "B_DeltaM_low": -2579.0, 
        "B_upperDeltaM_high": 1721.0, 
        "B_upperDeltaM_low": 720.0, 
        "D0_BPVVDCHI2": 36.0, 
        "D0_DIRA": 0.995, 
        "D0_DOCAMAX": 0.5, 
        "D0_MassW": 40.0, 
        "D0_PT": 1200.0, 
        "D0_VCHI2": 10.0, 
        "D1_DeltaM_high": 450.0, 
        "D1_DeltaM_low": 350.0, 
        "D1_MassW": 300.0, 
        "D1_PT": 500.0, 
        "D1_Pi_IPCHI2": 4.0, 
        "D1_Pi_PIDK": 8.0, 
        "D1_Pi_TRCHI2": 3.0, 
        "D1_VCHI2": 25.0, 
        "DInvVertD": 1.0, 
        "D_BPVVDCHI2": 50.0, 
        "D_DIRA": 0.995, 
        "D_K_IPCHI2": 10.0, 
        "D_K_PIDK": -3, 
        "D_K_PT": 150.0, 
        "D_K_TRCHI2DOF": 30.0, 
        "D_MIPCHI2": 10.0, 
        "D_MassW": 40.0, 
        "D_PT": 1600.0, 
        "D_Pi_IPCHI2": 10.0, 
        "D_Pi_PIDK": 50.0, 
        "D_Pi_PT": 150.0, 
        "D_Pi_TRCHI2": 3.0, 
        "D_VCHI2": 10.0, 
        "Dplus_K_PIDK": 3, 
        "Dplus_K_PT": 1500.0, 
        "Dplus_K_TRPCHI2": 1e-08, 
        "Dplus_Pi_TRPCHI2": 1e-08, 
        "Ds_BPVVDCHI2": 36.0, 
        "Ds_K_PT": 1500.0, 
        "Dstar_DeltaM_high": 160.0, 
        "Dstar_DeltaM_low": 135.0, 
        "Dstar_MassW": 50.0, 
        "Dstar_PT": 1250.0, 
        "Dstar_VCHI2": 25.0, 
        "Jpsi_MassW": 80.0, 
        "Jpsi_PT": 2000.0, 
        "Jpsi_VCHI2": 9.0, 
        "LbInvVertD": 1.0, 
        "Lc_BPVVDCHI2": 50.0, 
        "Lc_DIRA": 0.995, 
        "Lc_K_IPCHI2": 10.0, 
        "Lc_K_PIDK": 3.0, 
        "Lc_K_PT": 150.0, 
        "Lc_K_TRCHI2DOF": 3.0, 
        "Lc_K_TRPCHI2": 1e-08, 
        "Lc_MIPCHI2": 10.0, 
        "Lc_MassW": 30.0, 
        "Lc_PT": 1200.0, 
        "Lc_Pi_IPCHI2": 10.0, 
        "Lc_Pi_PIDK": 50.0, 
        "Lc_Pi_PT": 150.0, 
        "Lc_Pi_TRCHI2": 3.0, 
        "Lc_Pi_TRPCHI2": 1e-08, 
        "Lc_VCHI2": 10.0, 
        "Lc_p_IPCHI2": 10.0, 
        "Lc_p_PIDp": 5.0, 
        "Lc_p_PT": 150.0, 
        "Lc_p_TRCHI2DOF": 3.0, 
        "Lc_p_TRPCHI2": 1e-08, 
        "Muon_PT": 1000.0, 
        "Muon_TRCHI2DOF": 3.0, 
        "Postscale": 1.0, 
        "Prescale_B0d2DTauNu": 0, 
        "Prescale_B0d2DdoubleStarTauNu": 0, 
        "Prescale_B0d2DstarTauNu": 1.0, 
        "Prescale_B0s2DsTauNu": 0, 
        "Prescale_Bc2JpsiTauNu": 0, 
        "Prescale_Bu2D0TauNu": 0, 
        "Prescale_Lb2LcTauNu": 0, 
        "Prescale_Lb2pTauNu": 0.1666, 
        "Prescale_NonPhys": 0.1, 
        "TRGHP": 0.4, 
        "TRGHP_slowPi": 0.6, 
        "TisTosSpecs": {
            "Hlt1.*Decision%TOS": 0
        }, 
        "p_IPCHI2": 25.0, 
        "p_PE": 15000, 
        "p_PIDp": 10.0, 
        "p_PT": 1200.0, 
        "p_TRCHI2DOF": 3.0, 
        "slowPi_PT": 50.0, 
        "slowPi_TRCHI2DOF": 30.0
    },
    "STREAMS": {
        "BhadronCompleteEvent": [ 
            "StrippingLb2pTauNuForB2XTauNuAllLines", 
            "StrippingLb2pTauNuWSForB2XTauNuAllLines", 
            "StrippingB0d2DstarTauNuForB2XTauNuAllLines",
            "StrippingB0d2DstarTauNuWSForB2XTauNuAllLines", 
            "StrippingB0d2DstarTauNuNonPhysTauForB2XTauNuAllLines"
        ]
    }, 
    "WGs": [ "Semileptonic" ]
}

B2XuMuNu = {
    "BUILDERTYPE": "B2XuMuNuBuilder", 
    "CONFIG": {
        "BCorrMHigh": 7000.0, 
        "BCorrMLow": 2500.0, 
        "BDIRA": 0.99, 
        "BDIRAMed": 0.994, 
        "BDIRATight": 0.999, 
        "BFDCHI2ForPhi": 50.0, 
        "BFDCHI2HIGH": 100.0, 
        "BFDCHI2Tight": 120.0, 
        "BVCHI2DOF": 4.0, 
        "BVCHI2DOFTight": 2.0, 
        "Enu": 1850.0, 
        "EnuK": 2000.0, 
        "GEC_nLongTrk": 250.0, 
        "KMuMassLowTight": 1500.0, 
        "KS0DaugMIPChi2": 50.0, 
        "KS0DaugP": 2000.0, 
        "KS0DaugPT": 250.0, 
        "KS0DaugTrackChi2": 4.0, 
        "KS0MIPChi2": 8.0, 
        "KS0PT": 700.0, 
        "KS0VertexChi2": 10.0, 
        "KSLLCutFDChi2": 100.0, 
        "KSLLMassHigh": 536.0, 
        "KSLLMassLow": 456.0, 
        "KSMajoranaCutFDChi2": 100.0, 
        "KaonMINIPCHI2": 16, 
        "KaonP": 3000.0, 
        "KaonPIDK": 5.0, 
        "KaonPIDK_phi": 0.0, 
        "KaonPIDmu": 5.0, 
        "KaonPIDmu_phi": -2.0, 
        "KaonPIDp": 5.0, 
        "KaonPIDp_phi": -2.0, 
        "KaonPT": 500.0, 
        "KaonPTight": 10000.0, 
        "KaonTRCHI2": 6.0, 
        "KsLLCutFD": 20.0, 
        "KsLLMaxDz": 650.0, 
        "KshMuMassLowTight": 3000.0, 
        "KshZ": 0.0, 
        "KstarChPionMINIPCHI2": 9.0, 
        "KstarChPionP": 2000.0, 
        "KstarChPionPIDK": -10.0, 
        "KstarChPionPT": 100.0, 
        "KstarChPionTRCHI2": 10.0, 
        "KstarMuMassLowTight": 2500.0, 
        "MuonGHOSTPROB": 0.35, 
        "MuonMINIPCHI2": 12, 
        "MuonP": 3000.0, 
        "MuonPIDK": 0.0, 
        "MuonPIDmu": 3.0, 
        "MuonPIDp": 0.0, 
        "MuonPT": 1000.0, 
        "MuonPTTight": 1500.0, 
        "MuonPTight": 6000.0, 
        "MuonTRCHI2": 4.0, 
        "PhiDIRA": 0.9, 
        "PhiMINIPCHI2": 9, 
        "PhiMu_MCORR": 2500.0, 
        "PhiPT": 500.0, 
        "PhiUpperMass": 1500.0, 
        "PhiVCHI2DOF": 6, 
        "PiMuNu_prescale": 0.1, 
        "RhoChPionMINIPCHI2": 9.0, 
        "RhoChPionPIDK": -10.0, 
        "RhoChPionPT": 300.0, 
        "RhoChPionTRCHI2": 10.0, 
        "RhoDIRA": 0.9, 
        "RhoLeadingPionPT": 800.0, 
        "RhoMINIPCHI2": 4, 
        "RhoMassWindow": 150.0, 
        "RhoMassWindowMax1SB": 620.0, 
        "RhoMassWindowMax2SB": 1200.0, 
        "RhoMassWindowMin1SB": 300.0, 
        "RhoMassWindowMin2SB": 920.0, 
        "RhoMuMassLowTight": 2000.0, 
        "RhoPT": 500.0, 
        "RhoVCHI2DOF": 6, 
        "TRGHOSTPROB": 0.5, 
        "XMuMassUpper": 5500.0, 
        "XMuMassUpperHigh": 6500.0
    },
    "STREAMS": {
        "Semileptonic": [ 
            "StrippingB2XuMuNuB2PhiLine", 
            "StrippingB2XuMuNuBs2KstarLine", 
            "StrippingB2XuMuNuBs2KstarSSLine", 
            "StrippingB2XuMuNuBs2KLine", 
            "StrippingB2XuMuNuBs2KSSLine", 
            "StrippingB2XuMuNuBs2K_FakeMuLine", 
            "StrippingB2XuMuNuBs2KSS_FakeMuLine", 
            "StrippingB2XuMuNuBs2K_FakeKLine",
            "StrippingB2XuMuNuBs2KSS_FakeKLine",
            "StrippingB2XuMuNuBs2K_FakeKMuLine", 
            "StrippingB2XuMuNuBs2KSS_FakeKMuLine"
        ]
    },  
    "WGs": [ "Semileptonic" ]
}

D2HMuNu = {
    "BUILDERTYPE": "D2HLepNuBuilder", 
    "CONFIG": {
        "BDIRA": 0.99, 
        "BFDCHI2HIGH": 100.0, 
        "BVCHI2DOF": 20, 
        "DELTA_MASS_MAX": 400, 
        "ElectronPIDe": 0.0, 
        "ElectronPT": 500, 
        "GEC_nLongTrk": 160.0, 
        "KLepMassHigh": 2000, 
        "KLepMassLow": 500, 
        "KaonPIDK": 5.0, 
        "KaonPIDmu": 5.0, 
        "KaonPIDp": 5.0, 
        "KaonPT": 500.0, 
        "MuonGHOSTPROB": 0.35, 
        "MuonPIDK": 0.0, 
        "MuonPIDmu": 3.0, 
        "MuonPIDp": 0.0, 
        "MuonPT": 500.0, 
        "Slowpion_PIDe": 5, 
        "Slowpion_PT": 200, 
        "Slowpion_TRGHOSTPROB": 0.35, 
        "TOSFilter": {
            "Hlt2CharmHad.*HHX.*Decision%TOS": 0
        }, 
        "TRGHOSTPROB": 0.35, 
        "useTOS": True
    }, 
    "STREAMS": [ "Semileptonic" ], 
    "WGs": [
        "Semileptonic", 
        "Charm"
    ]
}

bhad2PMuX = {
    "BUILDERTYPE": "bhad2PMuXBuilder", 
    "CONFIG": {
        "BDIRA": 0.9994, 
        "BFDCHI2HIGH": 150.0, 
        "BVCHI2DOF": 20.0, 
        "DECAYS": [
            "[Lambda_b0 -> J/psi(1S) mu-]cc", 
            "[Lambda_b0 -> N(1440)+ mu-]cc"
        ], 
        "GEC_nLongTrk": 250.0, 
        "MuonGHOSTPROB": 0.35, 
        "MuonMINIPCHI2": 16.0, 
        "MuonP": 3000.0, 
        "MuonPT": 1500.0, 
        "MuonTRCHI2": 4.0, 
        "NstarMass": 3000.0, 
        "NstarVCHI2DOF": 2.0, 
        "PPbarDIRA": 0.994, 
        "PPbarFDCHI2HIGH": 150.0, 
        "PPbarPT": 1500.0, 
        "PPbarVCHI2DOF": 4.0, 
        "ProtonMINIPCHI2": 16.0, 
        "ProtonP": 15000.0, 
        "ProtonPIDK": 5.0, 
        "ProtonPIDp": 10.0, 
        "ProtonPT": 1000.0, 
        "ProtonTRCHI2": 6.0, 
        "SSDECAYS": [
            "[Lambda_b0 -> N(1440)+ mu+]cc"
        ], 
        "TRGHOSTPROB": 0.35, 
        "pMuMassLower": 1000.0, 
        "pMuPT": 1500.0
    }, 
    "STREAMS": [ "Semileptonic" ], 
    "WGs": [ "Semileptonic" ]
}

