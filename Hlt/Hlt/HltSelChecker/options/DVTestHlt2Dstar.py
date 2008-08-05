### @file DVTestHlt2Dstar.opts
 #
 #  Test file for HLT D*->D0(hh)pi selection
 #
 #  @author P. Koppenburg
 #  @date 2007-07-20
###
#--------------------------------------------------------------
from Gaudi.Configuration import *
from Configurables import HltCorrelations, FilterTrueTracks, MCDecayFinder, GaudiSequencer, PhysDesktop, DecayTreeTuple
#
# Preselection
#
importOptions( "$B2DSTARXROOT/options/DVPreselDstarNoPID.opts")
HltCorrelations("Hlt2SelectionsCorrs").Algorithms += [ "DVPreselDstarNoPID" ]
#
# Hlt test
#
importOptions( "$HLTSELECTIONSROOT/options/DVTestHlt2.py")
#
# Plots
#
importOptions( "$HLTSELECTIONSROOT/options/Hlt2MonitorPlots.py")
#
# True filter criterion 
#
importOptions( "$HLTSELCHECKERROOT/options/FilterTrueTracks.py")
FilterTrueTracks().addTool(MCDecayFinder())
FilterTrueTracks().MCDecayFinder.Decay =  "{[D*(2010)+ -> (D0 -> ^K- ^pi+ {,gamma}{,gamma}{,gamma}) ^pi+  {,gamma}{,gamma}]cc, [D*(2010)+ -> (D0 -> ^K+ ^pi- {,gamma}{,gamma}{,gamma}) ^pi+  {,gamma}{,gamma}]cc, [D*(2010)+ -> (D0 -> ^pi+ ^pi- {,gamma}{,gamma}{,gamma}) ^pi+  {,gamma}{,gamma}]cc, [D*(2010)+ -> (D0 -> ^K+ ^K- {,gamma}{,gamma}{,gamma}) ^pi+  {,gamma}{,gamma}]cc}"
#
# Set the following to false if you want only events with a signal
# fully reconstructed in the HLT
#
GaudiSequencer("SeqHlt2TruthFilter").IgnoreFilterPassed = TRUE  
#
# Overwrite input - uncomment to run HLT on TRUE signal only
#
# importOptions( "$HLTSELCHECKERROOT/options/OverwriteWithTruth.py")
#
# Tuple tool
#
importOptions( "$HLTSELCHECKERROOT/options/Hlt2DecayTreeTuple.py")
DecayTreeTuple("Hlt2DecayTreeTuple").addTool(PhysDesktop())
DecayTreeTuple("Hlt2DecayTreeTuple").PhysDesktop.InputLocations = ["Phys/Hlt2SelDstar"]
DecayTreeTuple("Hlt2DecayTreeTuple").Decay = "[D*(2010)+ -> (^D0 -> ^K+ ^pi-) ^pi+]cc" # only Cabibbo-faoured ? 

EventSelector().Input   = [
  "DATAFILE='PFN:/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/Dst-PiD-HH-lum2.dst' TYP='POOL_ROOTTREE' OPT='READ'" ]

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

ApplicationMgr().ExtSvc +=  [ "NTupleSvc" ]                             
NTupleSvc().Output =  [ "FILE1 DATAFILE='HLT-Dstar.root' TYP='ROOT' OPT='NEW'" ] 
HistogramPersistencySvc().OutputFile = "DVHlt2-Dstar.root"

ApplicationMgr().EvtMax = 100 
EventSelector().FirstEvent = 1 
EventSelector().PrintFreq = 1 
