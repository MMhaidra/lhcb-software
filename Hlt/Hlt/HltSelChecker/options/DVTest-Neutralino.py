### @file DVTest-.py
 #
 #  Test file for HLT on selected  events
 #
 #  @author P. Koppenburg
 #  @date 2009-10-22
 #/
from Gaudi.Configuration import *
from Configurables import DaVinci
from HltSelChecker.CheckerSeq import *
#--------------------------------------------------------------
CS = CheckerSeq( DV=DaVinci()
               , Signal=""
               , Decay = ""
               # 
               , Input = [  ])
CS.configure()

