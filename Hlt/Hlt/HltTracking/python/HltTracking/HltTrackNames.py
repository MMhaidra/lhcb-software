# A module to hold the hardcoded names of tracks/protoparticles in the Hlt
# and rules for deriving containers from them  
__author__  = "V. Gligorov vladimir.gligorov@cern.ch"
########################################################################
# Globals
########################################################################
from HltTracking.Hlt1TrackNames import HltGlobalEventPrefix
########################################################################
# These "globals" define that Tracks go into .../Track/... and
# protoparticles into .../ProtoP/...
from HltTracking.Hlt1TrackNames import HltGlobalTrackLocation
HltGlobalProtoPLocation			= "ProtoP"
#
########################################################################
# The rules for generating track and proto particle containers
# These rules apply to HLT2 TODO: add rule for HLT1 usable from Hlt1Units
########################################################################
# For tracks, the format is e.g. Hlt2/Track/Unfitted/Forward 
#
# First of all we have the "base" track and protoparticle
# location; this just defines that tracks and protoparticles go into
# some_prefix_you_choose/Track/... and some_refix_you_choose/ProtoP/... 
#
from HltTracking.Hlt1TrackNames import _baseTrackLocation
#
def _baseProtoPLocation(prefix,protos) :
    return prefix + "/" + HltGlobalProtoPLocation + "/" + protos

########################################################################
# Tracks
########################################################################
# prefixes where to put the tracks (these go into the prefix field of 
# _trackLocation and _protosLocation)
#
HltSharedTracksPrefix 		= HltGlobalEventPrefix + "Hlt"
Hlt2TracksPrefix 			= HltGlobalEventPrefix + "Hlt2"

Hlt2TrackRoot               = Hlt2TracksPrefix + "/" + HltGlobalTrackLocation
HltSharedTrackRoot          = HltSharedTracksPrefix + "/" + HltGlobalTrackLocation

#
# names of track types (these go into the tracks field of _trackLocation 
# and _protosLocation)
#
HltSharedRZVeloTracksName               = "RZVelo"

from HltTracking.Hlt1TrackNames import TrackName
# The prefix says where this track has been produced
Hlt2TrackLoc = { name : _baseTrackLocation(Hlt2TracksPrefix,loc) for name,loc in TrackName.iteritems() }
HltSharedTrackLoc = { name : _baseTrackLocation(HltSharedTracksPrefix,loc) for name,loc in TrackName.iteritems() }

# check if the Decoders are writing to the correct locations
from DAQSys.Decoders import DecoderDB
DecoderLocations = DecoderDB["HltTrackReportsDecoder"].listOutputs()
from HltTracking.Hlt1TrackNames import Hlt1TrackLoc
for loc in DecoderLocations :
    if not loc in Hlt1TrackLoc.values() + HltSharedTrackLoc.values()  : 
        print "TrackReports location: " + loc + " not found in track locations. Go synchronize HltTrackNames.py and HltDAQ."
        raise Exception("TrackReports location not found in TrackNames") 
    #else : print "Checked TrackReports location "+loc
    #endif#endloop    

#HltSharedVeloTracksName               = "Velo"
#HltSharedVeloTTTracksName               = "VeloTT"
#HltSharedForwardTracksName               = "Forward"
#
#HltSharedVeloLocation = _baseTrackLocation(HltSharedTracksPrefix,HltSharedVeloTracksName)
#HltSharedVeloTTLocation = _baseTrackLocation(HltSharedTracksPrefix,HltSharedVeloTTTracksName)
#HltSharedForwardLocation = _baseTrackLocation(HltSharedTracksPrefix,HltSharedForwardTracksName)

#
#Hlt1SeedingTracksName                   = "Seeding"

#Hlt1ForwardPestimateTracksName          = "PestiForward"

#
#Hlt2VeloTracksName 			= "Velo"
#Hlt2ForwardTracksName 			= "Forward"
#Hlt2ForwardSecondLoopTracksName = "ForwardSecondLoop"
#Hlt2MatchTracksName 			= "Match"
#Hlt2LongTracksName 			= "Long"
#Hlt2SeedingTracksName 			= Hlt1SeedingTracksName
#Hlt2DownstreamTracksName 		= "SeedTT"
# The next two are "trackified" PID objects
HltMuonTracksName			= "MuonSegments"
HltAllMuonTracksName			= "AllMuonSegments"
#
# types of track fit (including no fit!) (these go into the fastFitType 
# field of _trackLocation and _protosLocation)
#
HltUnfittedTracksSuffix			= "Unfitted"
HltBiDirectionalKalmanFitSuffix 	= "BiKalmanFitted"
HltUniDirectionalKalmanFitSuffix 	= "UniKalmanFitted"  
#
# The recognised track types for the Hlt2 Tracking
#
Hlt2TrackingRecognizedTrackTypes 	= [	"Forward",
                                        "Long", 
                                        "Downstream" ]
#
# The recognised fit types for the Hlt2 Tracking
# 
Hlt2TrackingRecognizedFitTypes		= [	HltUnfittedTracksSuffix,
						HltBiDirectionalKalmanFitSuffix,
						HltUniDirectionalKalmanFitSuffix
					  ]
#
Hlt2TrackingRecognizedFitTypesForRichID = [	HltBiDirectionalKalmanFitSuffix
					  ]	
#




########################################################################
# ProtoParticles
########################################################################
# The suffix (this goes into the type field of _protosLocation)
#
Hlt2ChargedProtoParticleSuffix 		= "Charged"
Hlt2NeutralProtoParticleSuffix 		= "Neutrals" 
#
########################################################################
# PID
########################################################################
# We want to generate PID containers for different track types the
# same way in which we generate tracks, meaning using the same rules,
# because we need different PID for different tracks; they should not 
# overwrite each other!
# 
HltSharedPIDPrefix			= "PID"
HltNoPIDSuffix				= "NoPID"
HltMuonIDSuffix				= "Muon"
HltRICHIDSuffix				= "RICH"
HltCALOIDSuffix				= "CALO"
#
# The default RICH options
#


from Configurables import HltRecoConf
MaxChi2 = HltRecoConf().getProp("MaxTrCHI2PDOF")
HltRichDefaultHypos			= ["pion","kaon"]
HltRichDefaultRadiators			= ["Rich1Gas","Rich2Gas"] 
HltRichDefaultTrackCuts     = {"Forward" :{ "Chi2Cut" : [0.,MaxChi2], "PCut" : [2,100], "PtCut" : [0.8,100]},
                               "Match" :{ "Chi2Cut" : [0.,MaxChi2], "PCut" : [2,100], "PtCut" : [0.8,100]}}

OfflineRichDefaultHypos	       = ["pion","kaon","proton","belowThreshold"]
OfflineRichDefaultRadiators    = ["Rich1Gas","Rich2Gas"]
OfflineRichDefaultTrackCuts    = { tt : {'Chi2Cut' : [0,3.0], 'PCut' : [0,1e9], 'PtCut' : [0,1e9] } for tt in ['Forward','Match'] }


# And the subdirectories. These are necessary so that different algorithms
# using e.g. different options for the RICH reco (radiators, hypotheses) 
# don't clash with each other 
#
HltCaloProtosSuffix			= "WithCaloID"
HltMuonProtosSuffix			= "WithMuonID"
HltRichProtosSuffix			= "WithRichID"

from HltTracking.Hlt1TrackNames import Hlt1TracksPrefix, Hlt1TrackRoot

#
__all__ = (	
		#
		# The strings
		#
		HltGlobalTrackLocation,
		HltGlobalProtoPLocation,
		# 
		HltSharedTracksPrefix, 
           	Hlt1TracksPrefix, 
           	Hlt2TracksPrefix, 
#        HltSharedVeloLocation,
#        HltSharedVeloTTLocation,
#        HltSharedForwardLocation,
		#
        TrackName,
        Hlt1TrackRoot,
        Hlt2TrackRoot,
        HltSharedTrackRoot,
        Hlt1TrackLoc,
        Hlt2TrackLoc,
        HltSharedTrackLoc,
        #
		HltSharedRZVeloTracksName,
#Hlt2ForwardTracksName, 
#		Hlt2ForwardSecondLoopTracksName,
#        Hlt2MatchTracksName, 
#		Hlt2LongTracksName, 
#		Hlt2SeedingTracksName, 
#		Hlt2DownstreamTracksName,
		HltMuonTracksName,
		HltAllMuonTracksName,
		#
		HltUnfittedTracksSuffix, 
		HltBiDirectionalKalmanFitSuffix, 
		HltUniDirectionalKalmanFitSuffix,
		#
		Hlt2TrackingRecognizedTrackTypes,
		Hlt2TrackingRecognizedFitTypes, 
		Hlt2TrackingRecognizedFitTypesForRichID,
		#
		Hlt2ChargedProtoParticleSuffix, 
		Hlt2NeutralProtoParticleSuffix,
		#	
		HltSharedPIDPrefix,
		HltNoPIDSuffix, 
		HltMuonIDSuffix,   
		HltRICHIDSuffix,   
		HltCALOIDSuffix,
		#
		HltRichDefaultHypos,
		HltRichDefaultRadiators,
		#
		HltCaloProtosSuffix,               
		HltMuonProtosSuffix,                     
		HltRichProtosSuffix,                   
		#
		# The functions
		#
		_baseTrackLocation,
		_baseProtoPLocation

	  ) 
