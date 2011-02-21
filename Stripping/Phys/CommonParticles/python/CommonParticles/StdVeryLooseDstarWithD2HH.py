#!/usr/bin/env python
# =============================================================================
# $Id: StdVeryLooseDstarWithD2HH.py,v 1.5 2010-09-05 20:36:44 gcowan Exp $ 
# =============================================================================
## @file  CommonParticles/StdVeryLooseDstarWithD02HH.py
#  configuration file for 'Standard VeryLoose Dstar WithD02HH' 
#  similar to  StdLooseDstarWithD2HH.py but using Very Loose D0
#  and also StdNoPIDsUpPions to make the D* slow pion. Add two cuts on slow pion. 
# =============================================================================
"""
Configuration file for 'Standard VeryLoose Dstar WithD02HH'
"""
__author__  = "Marta Calvi"
__date__ = '10/12/2009'
__version__ = "version $Revision: 1.5 $"

# =============================================================================
__all__ = (
    'StdVeryLooseDstarWithD02KPi' ,
    'StdVeryLooseDstarWithD02KPiDCS' ,
    'StdVeryLooseDstarWithD02PiPi' ,
    'StdVeryLooseDstarWithD02KK' ,
    'locations'
    )
# =============================================================================
from Gaudi.Configuration import *
from Configurables       import CombineParticles
from CommonParticles.Utils import *

## ============================================================================
## create the algorithm 
StdVeryLooseDstarWithD02KPi = CombineParticles ( 'StdVeryLooseDstarWithD02KPi' )

StdVeryLooseDstarWithD02KPi.InputLocations = [ "Phys/StdNoPIDsPions/Particles",
                                               "Phys/StdVeryLooseD02KPi/Particles" ]
StdVeryLooseDstarWithD02KPi.DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc" 

StdVeryLooseDstarWithD02KPi.CombinationCut = "(ADAMASS('D*(2010)+')<80*MeV) & (APT>1250*MeV)"
StdVeryLooseDstarWithD02KPi.MotherCut = "(VFASPF(VCHI2/VDOF)<25) & (M-MAXTREE('D0'==ABSID,M)<165.5)"
StdVeryLooseDstarWithD02KPi.DaughtersCuts = {"pi+" : "(PT>110*MeV) & (MIPDV(PRIMARY)>0.04*mm)"}

## configure Data-On-Demand service 
locations = updateDoD ( StdVeryLooseDstarWithD02KPi )

################################################################################
# D* with Cabibbo-suppressed D0->K+Pi- is a clone of D* with D0->KPi
#
StdVeryLooseDstarWithD02KPiDCS = StdVeryLooseDstarWithD02KPi.clone("StdVeryLooseDstarWithD02KPiDCS")
GaudiSequencer("SeqStdVeryLooseDstarWithD02KPiDCS").Members += [ StdVeryLooseDstarWithD02KPiDCS ] 
StdVeryLooseDstarWithD02KPiDCS.DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc"
StdVeryLooseDstarWithD02KPiDCS.InputLocations = [ "Phys/StdNoPIDsPions/Particles",
                                                  "Phys/StdVeryLooseD02KPiDCS/Particles" ]
## configure Data-On-Demand service 
locations = updateDoD ( StdVeryLooseDstarWithD02KPiDCS )

################################################################################
# D* with D0->KK is a clone of D* with D0->Kpi
#
StdVeryLooseDstarWithD02KK = StdVeryLooseDstarWithD02KPi.clone("StdVeryLooseDstarWithD02KK")
GaudiSequencer("SeqStdVeryLooseDstarWithD02KK").Members += [ StdVeryLooseDstarWithD02KK ] 
StdVeryLooseDstarWithD02KK.DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc"
StdVeryLooseDstarWithD02KK.InputLocations = [  "Phys/StdNoPIDsPions/Particles",
                                               "Phys/StdVeryLooseD02KK/Particles" ]
## configure Data-On-Demand service 
locations = updateDoD ( StdVeryLooseDstarWithD02KK )

################################################################################
# D* with D0->PiPi is a clone of D* with D0->KK
#
StdVeryLooseDstarWithD02PiPi = StdVeryLooseDstarWithD02KK.clone("StdVeryLooseDstarWithD02PiPi")
GaudiSequencer("SeqStdVeryLooseDstarWithD02PiPi").Members += [ StdVeryLooseDstarWithD02PiPi ] 
StdVeryLooseDstarWithD02PiPi.DecayDescriptor = "[D*(2010)+ -> D0 pi+]cc"
StdVeryLooseDstarWithD02PiPi.InputLocations = [  "Phys/StdNoPIDsPions/Particles",
                                                 "Phys/StdVeryLooseD02PiPi/Particles" ]
## configure Data-On-Demand service 
locations = updateDoD ( StdVeryLooseDstarWithD02PiPi )

## ============================================================================
if '__main__' == __name__ :

    print __doc__
    print __author__
    print __version__
    print locationsDoD ( locations ) 

# =============================================================================
# The END 
# =============================================================================
