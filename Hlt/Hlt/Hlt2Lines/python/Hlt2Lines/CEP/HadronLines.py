from GaudiKernel.SystemOfUnits import GeV, MeV, mm
from Hlt2Lines.Utilities.Hlt2LinesConfigurableUser import Hlt2LinesConfigurableUser
class CEPHadronLines() :
    def localcuts(self) :
        localStages={'L2pPi'    :   {'APTmin'         :     0.0 * MeV, 
                                     'APTmax'         :     100000.0 * GeV,
                                     'APmin'          :     10000.0 * MeV,
                                     'ADOCAmax'       :     0.5 * mm,
                                     'VtxChi2DoFmax'  :     15.0,
                                     'nVeloTracksmax' :     12,
                                     'nBackTracksmax' :     1,
                                     'AMmin'          :     1036.0 * MeV,
                                     'AMmax'          :     1196.0 * MeV,
                                     'P_PIDpmin'      :     0.0,
                                     'Pi_PIDKmax'     :     10000}
                  , 'D2KPi'     :   {'APTmin'         :     0.0 * MeV, 
                                     'APTmax'         :     100000.0 * GeV,
                                     'APmin'          :     10000.0 * MeV,
                                     'ADOCAmax'       :     0.5 * mm,
                                     'VtxChi2DoFmax'  :     15.0,
                                     'nVeloTracksmax' :     12,
                                     'nBackTracksmax' :     1,
                                     'AMmin'          :     1785.0 * MeV,
                                     'AMmax'          :     1945.0 * MeV,
                                     'K_PIDKmin'      :     0.0,     
                                     'Pi_PIDKmax'     :     10000}
                  , 'D2KPiPi'   :   {'APTmin'         :     0.0 * MeV, 
                                     'APTmax'         :     100000.0 * GeV,
                                     'APmin'          :     10000.0 * MeV,
                                     'ADOCAmax'       :     0.5 * mm,
                                     'VtxChi2DoFmax'  :     15.0,
                                     'nVeloTracksmax' :     12,
                                     'nBackTracksmax' :     1,
                                     'AMmin'          :     1785.0 * MeV,
                                     'AMmax'          :     1945.0 * MeV,
                                     'K_PIDKmin'      :     0.0,     
                                     'Pi_PIDKmax'     :     10000}
                  , 'D2K3Pi'    :   {'APTmin'         :     0.0 * MeV, 
                                     'APTmax'         :     100000.0 * GeV,
                                     'APmin'          :     10000.0 * MeV,
                                     'ADOCAmax'       :     0.7 * mm,
                                     'VtxChi2DoFmax'  :     15.0,
                                     'nVeloTracksmax' :     12,
                                     'nBackTracksmax' :     1,
                                     'AMmin'          :     1785.0 * MeV,
                                     'AMmax'          :     1945.0 * MeV,
                                     'K_PIDKmin'      :     0.0,     
                                     'Pi_PIDKmax'     :     10000}
                  , 'ChiC2HH'   :   {'APTmin'         :     0.0 * MeV,
                                     'APTmax'         :     5000.0 * MeV,
                                     'APmin'          :     10000.0 * MeV,
                                     'ADOCAmax'       :     0.5 * mm,
                                     'VtxChi2DoFmax'  :     15.0,
                                     'nVeloTracksmax' :     8,
                                     'nBackTracksmax' :     1,
                                     'AMmin'          :     2850.0 * MeV,
                                     'AMmax'          :     3600.0 * MeV,
                                     'K_PIDKmin'      :     0.0,     
                                     'Pi_PIDKmax'     :     10000}
                  , 'ChiC2HHHH' :   {'APTmin'         :     0.0 * MeV,
                                     'APTmax'         :     5000.0 * MeV,
                                     'APmin'          :     10000.0 * MeV,
                                     'ADOCAmax'       :     0.7 * mm,
                                     'VtxChi2DoFmax'  :     15.0,
                                     'nVeloTracksmax' :     8,
                                     'nBackTracksmax' :     1,
                                     'AMmin'          :     2850.0 * MeV,
                                     'AMmax'          :     4500.0 * MeV,
                                     'K_PIDKmin'      :     0.0,     
                                     'Pi_PIDKmax'     :     10000}
                  , 'ChiC2PP'   :   {'APTmin'         :     0.0 * MeV,
                                     'APTmax'         :     5000.0 * MeV,
                                     'APmin'          :     10000.0 * MeV,
                                     'ADOCAmax'       :     0.5 * mm,
                                     'VtxChi2DoFmax'  :     15.0,
                                     'nVeloTracksmax' :     8,
                                     'nBackTracksmax' :     1,
                                     'AMmin'          :     2850.0 * MeV,
                                     'AMmax'          :     3600.0 * MeV,
                                     'P_PIDpmin'      :     0.0}
                  , 'LMR2HH'    :   {'APTmin'         :     0.0 * MeV,
                                     'APTmax'         :     10000.0 * GeV,
                                     'APmin'          :     0.0 * MeV,
                                     'ADOCAmax'       :     0.3 * mm,
                                     'VtxChi2DoFmax'  :     4.0,
                                     'nVeloTracksmax' :     8,
                                     'nBackTracksmax' :     1,
                                     'AMmin'          :     0.240 * GeV,
                                     'AMmax'          :     15000.0 * GeV,
                                     'K_PIDKmin'      :     0.0,     
                                     'Pi_PIDKmax'     :     10000,
                                     'P_PIDpmin'      :     0.0}
                  , 'LMR2HHHH'  :   {'APTmin'         :     0.0 * MeV,
                                     'APTmax'         :     5000.0 * MeV,
                                     'APmin'          :     0.0 * MeV,
                                     'ADOCAmax'       :     0.3 * mm,
                                     'VtxChi2DoFmax'  :     4.0,
                                     'nVeloTracksmax' :     8,
                                     'nBackTracksmax' :     1,
                                     'AMmin'          :     400.0 * MeV,
                                     'AMmax'          :     10000.0 * GeV,
                                     'K_PIDKmin'      :     0.0,     
                                     'Pi_PIDKmax'     :     10000}
                  , 'LMR2HH_mediumPS':   {'APTmin'         :     0.0 * MeV,
                                     'APTmax'         :     5000.0 * MeV,
                                     'APmin'          :     15000.0 * MeV,
                                     'ADOCAmax'       :     0.2 * mm,
                                     'VtxChi2DoFmax'  :     4.0,
                                     'nVeloTracksmax' :     8,
                                     'nBackTracksmax' :     1,
                                     'AMmin'          :     0.0 * MeV,
                                     'AMmax'          :     15000.0 * GeV,
                                     'K_PIDKmin'      :     0.0,     
                                     'Pi_PIDKmax'     :     10000,
                                     'P_PIDpmin'      :     0.0}
                  , 'LMR2HHHH_mediumPS'  :   {'APTmin'         :     0.0 * MeV,
                                     'APTmax'         :     5000.0 * MeV,
                                     'APmin'          :     15000.0 * MeV,
                                     'ADOCAmax'       :     0.2 * mm,
                                     'VtxChi2DoFmax'  :     4.0,
                                     'nVeloTracksmax' :     8,
                                     'nBackTracksmax' :     1,
                                     'AMmin'          :     400.0 * MeV,
                                     'AMmax'          :     5000.0 * MeV,
                                     'K_PIDKmin'      :     0.0,     
                                     'Pi_PIDKmax'     :     10000}
                  , 'LMR2HH_heavyPS'    :   {'APTmin'         :     0.0 * MeV,
                                     'APTmax'         :     5000.0 * MeV,
                                     'APmin'          :     15000.0 * MeV,
                                     'ADOCAmax'       :     0.2 * mm,
                                     'VtxChi2DoFmax'  :     4.0,
                                     'nVeloTracksmax' :     8,
                                     'nBackTracksmax' :     1,
                                     'AMmin'          :     0.0 * MeV,
                                     'AMmax'          :     15000.0 * GeV,
                                     'K_PIDKmin'      :     0.0,     
                                     'Pi_PIDKmax'     :     10000,
                                     'P_PIDpmin'      :     0.0}
                  , 'LMR2HHHH_heavyPS'  :   {'APTmin'         :     0.0 * MeV,
                                     'APTmax'         :     5000.0 * MeV,
                                     'APmin'          :     15000.0 * MeV,
                                     'ADOCAmax'       :     0.2 * mm,
                                     'VtxChi2DoFmax'  :     4.0,
                                     'nVeloTracksmax' :     8,
                                     'nBackTracksmax' :     1,
                                     'AMmin'          :     400.0 * MeV,
                                     'AMmax'          :     5000.0 * MeV,
                                     'K_PIDKmin'      :     0.0,     
                                     'Pi_PIDKmax'     :     10000}
                  , 'Hadron_noTrFilt':{'PTmin'        :     0.5 * GeV}
                  } 
        return localStages

    def locallines(self):
      from Stages import (LowMultL2pPiFilter, LowMultD2KPiFilter, LowMultD2KPiPiFilter, LowMultD2K3PiFilter, LowMultD2KKPiFilter,
                          LowMultL2pPiWSFilter, LowMultD2KPiWSFilter, LowMultD2KPiPiWSFilter, LowMultD2K3PiWSFilter, LowMultD2KKPiWSFilter,
                          LowMultLMR2HHFilter, LowMultLMR2HHHHFilter,LowMultLMR2HHWSFilter, LowMultLMR2HHHHWSFilter,
                          LowMultChiC2HHFilter, LowMultChiC2HHHHFilter, LowMultChiC2PPFilter,
                          LowMultChiC2HHWSFilter, LowMultChiC2HHHHWSFilter,LowMultChiC2PPWSFilter,
                          LowMultHadron_noTrFiltFilter)
      stages = { # First the Lambda0 -> p pi lines
                 'LowMultL2pPi'       : [LowMultL2pPiFilter('L2pPi')],
                 'LowMultL2pPiWS'     : [LowMultL2pPiWSFilter('L2pPi')],
                 # First the D0 -> K pi lines
                 'LowMultD2KPi'       : [LowMultD2KPiFilter('D2KPi')],
                 'LowMultD2KPiWS'     : [LowMultD2KPiWSFilter('D2KPi')],
                 # Then the D+ -> K pi pi and K K pi lines
                 'LowMultD2KPiPi'     : [LowMultD2KPiPiFilter('D2KPiPi')],
                 'LowMultD2KPiPiWS'   : [LowMultD2KPiPiWSFilter('D2KPiPi')],
                 'LowMultD2KKPi'      : [LowMultD2KKPiFilter('D2KPiPi')],
                 'LowMultD2KKPiWS'    : [LowMultD2KKPiWSFilter('D2KPiPi')],
                 # Then  D -> K pi pi pi
                 'LowMultD2K3Pi'      : [LowMultD2K3PiFilter('D2K3Pi')],
                 'LowMultD2K3PiWS'    : [LowMultD2K3PiWSFilter('D2K3Pi')],
                 # Then chic lines
                 'LowMultChiC2HH'     : [LowMultChiC2HHFilter('ChiC2HH')],
                 'LowMultChiC2HHWS'   : [LowMultChiC2HHWSFilter('ChiC2HH')],
                 'LowMultChiC2HHHH'   : [LowMultChiC2HHHHFilter('ChiC2HHHH')],
                 'LowMultChiC2HHHHWS' : [LowMultChiC2HHHHWSFilter('ChiC2HHHH')], 
                 'LowMultChiC2PP'     : [LowMultChiC2PPFilter('ChiC2PP')],
                 'LowMultChiC2PPWS'   : [LowMultChiC2PPWSFilter('ChiC2PP')],
                 # Then low mass lines
                 'LowMultLMR2HH'      : [LowMultLMR2HHFilter('LMR2HH')],
                 'LowMultLMR2HHWS'    : [LowMultLMR2HHWSFilter('LMR2HH')],
                 'LowMultLMR2HHHH'    : [LowMultLMR2HHHHFilter('LMR2HHHH')],
                 'LowMultLMR2HHHHWS'  : [LowMultLMR2HHHHWSFilter('LMR2HHHH')],
                 'LowMultLMR2HH_mediumPS'      : [LowMultLMR2HHFilter('LMR2HH_mediumPS')],
                 'LowMultLMR2HHWS_mediumPS'    : [LowMultLMR2HHWSFilter('LMR2HH_mediumPS')],
                 'LowMultLMR2HHHH_mediumPS'    : [LowMultLMR2HHHHFilter('LMR2HHHH_mediumPS')],
                 'LowMultLMR2HHHHWS_mediumPS'  : [LowMultLMR2HHHHWSFilter('LMR2HHHH_mediumPS')],
                 'LowMultLMR2HH_heavyPS'      : [LowMultLMR2HHFilter('LMR2HH_heavyPS')],
                 'LowMultLMR2HHWS_heavyPS'    : [LowMultLMR2HHWSFilter('LMR2HH_heavyPS')],
                 'LowMultLMR2HHHH_heavyPS'    : [LowMultLMR2HHHHFilter('LMR2HHHH_heavyPS')],
                 'LowMultLMR2HHHHWS_heavyPS'  : [LowMultLMR2HHHHWSFilter('LMR2HHHH_heavyPS')],
                 # Unbiased hadron line
                 'LowMultHadron_noTrFilt':  [LowMultHadron_noTrFiltFilter('Hadron_noTrFilt')] 
               }
      return stages
