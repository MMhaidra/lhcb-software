from Configurables import Swimming
from Gaudi.Configuration import *

hhes = ['PP','KK']
lldd = ['LL','DD']
selconj = [ '', 'SelConjugate']
dstarloc = '/Event/CharmToBeSwum/Phys/DstarD2KShh{hh}{ll}Line'
Swimming().Debug = False
Swimming().DataType = '2012'
Swimming().EvtMax = -1
Swimming().Simulation = False
Swimming().Persistency = 'ROOT'
Swimming().InputType = 'DST'
Swimming().SwimStripping = False
Swimming().Hlt1Triggers = ["Hlt1TrackAllL0Decision","Hlt1TrackPhotonDecision"]
Swimming().Hlt2Triggers = ["Hlt2CharmHadD02HHXDst_hhX{wm}Decision".format(wm=wm) for wm in ['','WideMass']]
Swimming().OffCands = dict(("/Event/CharmToBeSwum/Phys/{selconj}D2KS{hh}{ll}ForDstarD2KShh".format(ll=ll,hh=hh,selconj=sc), Swimming().getProp('SwimmingPrefix')) for ll in lldd for sc in selconj for hh in hhes)
Swimming().MuDSTCands = [ dstarloc.format(ll=ll,hh=hh) for ll in lldd for hh in hhes]
Swimming().SkipEventIfNoMuDSTCandFound = True
Swimming().SkipEventIfNoMuDSTCandsAnywhere = False
Swimming().TransformName = '2012_WithBeamSpotFilter_NoRecoLines_ForInclusiveDstar_tight'
Swimming().SelectMethod = 'all'
Swimming().OutputType = 'DST'
Swimming().UseFileStager = False
Swimming().OverrideStageName = 'Trigger_Inclusive'
Swimming().LifetimeFitter = ['DecayTreeFitter', 'LifetimeFitter']
Swimming().DecayTreeFitterConstraints = {
    'DTFD0KS0'  : { 'D0' : -1.0, 'KS0' : -1.0 },
    'DTFKS0'    : { 'KS0' : -1.0 }  
    }
Swimming().StoreExtraTPs = True

# The custom event loop
from Gaudi.Configuration import setCustomMainLoop
from Swimming.EventLoop import SwimmingEventLoop
setCustomMainLoop(SwimmingEventLoop)
