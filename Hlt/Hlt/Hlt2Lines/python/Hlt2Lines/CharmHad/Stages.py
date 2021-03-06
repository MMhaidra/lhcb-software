## Each stage must specify its own inputs
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2VoidFilter
from Hlt2Lines.Utilities.Hlt2Combiner import Hlt2Combiner
from Hlt2Lines.Utilities.Hlt2Filter import Hlt2ParticleFilter
from Configurables import DaVinci__N4BodyDecays
from Configurables import DaVinci__N5BodyDecays

## ========================================================================= ##
## Global event cuts
## ========================================================================= ##
class TrackGEC(Hlt2VoidFilter):
    '''
    Cut on the number of reconstructed long tracks.
    Historically useful, may be obsolete.
    '''
    def __init__(self, name):
        from Configurables import LoKi__Hybrid__CoreFactory as Factory
        modules =  Factory().Modules
        for i in [ 'LoKiTrigger.decorators' ] :
            if i not in modules : modules.append(i)

        from HltTracking.Hlt2TrackingConfigurations import Hlt2BiKalmanFittedForwardTracking as Hlt2LongTracking
        tracks = Hlt2LongTracking().hlt2PrepareTracks()
        code = ("CONTAINS('%s')" % tracks.outputSelection()) + " < %(NTRACK_MAX)s"
        Hlt2VoidFilter.__init__(self, 'CharmHad' + name, code, [tracks], shared = True, nickname = 'TrackGEC')


## ========================================================================= ##
## Filters for Input particles
## ========================================================================= ##

## ------------------------------------------------------------------------- ##
class InParticleFilter(Hlt2ParticleFilter) : # {
    '''
    Filter charged particles on TRCHI2DOF, PT, P, and (optionally) a PID
    variable.  Does NOT filter on IP or IPCHI2.

    Always creates a shared instance of the filter.

    Configuration dictionaries must contain the following keys:
        'Trk_ALL_TRCHI2DOF_MAX' :  upper limit on TRCHI2DOF
        'Trk_ALL_PT_MIN'        :  lower limit on PT
        'Trk_ALL_P_MIN'         :  lower limit on P

    If filtering on a PID variable, the pidVar parameter must be set to the
    name of the PID functor on which to cut, and the configuration dictionary
    must contain the key 'PID_LIM' defining the one-sided limit for that
    variable.  By default the cut on the PID variable is a lower limit.
    It may be changed to an upper limit by setting the parameter
    pidULim = True.
    '''
    def __init__(self, name, inputs, pidVar = None, pidULim = False): # {
        cut = ("  (TRCHI2DOF < %(Trk_ALL_TRCHI2DOF_MAX)s )" +
               "& (PT > %(Trk_ALL_PT_MIN)s)" +
               "& (P > %(Trk_ALL_P_MIN)s)" )
        if pidVar : # {
            pidCut = "& (%s %s %%(PID_LIM)s)" % (pidVar, ('<' if pidULim else '>'))
            cut = cut + pidCut
        # }

        Hlt2ParticleFilter.__init__(self, name, cut, inputs, shared = True)
    # }
# }


## Shared instances of InParticleFilter
## Names of shared particles  begin with CharmHad to avoid name conflicts
##   with other subdirectories.
## ------------------------------------------------------------------------- ##
from Inputs import Hlt2LoosePions, Hlt2LooseKaons, Hlt2LooseMuons, Hlt2LooseProtons
from Inputs import Hlt2NoPIDsPions, Hlt2NoPIDsKaons, Hlt2NoPIDsProtons
SharedPromptChild_pi = InParticleFilter( 'CharmHadSharedPromptChild_pi',
                                         [Hlt2LoosePions], 'PIDK', True )
SharedPromptChild_K = InParticleFilter( 'CharmHadSharedPromptChild_K',
                                        [Hlt2LooseKaons], 'PIDK' )
SharedPromptChild_p = InParticleFilter( 'CharmHadSharedPromptChild_p',
                                        [Hlt2LooseProtons], 'PIDp' )
SharedSoftTagChild_pi = InParticleFilter( 'CharmHadSharedSoftTagChild_pi',
                                          [Hlt2NoPIDsPions] )
SharedSoftTagChild_mu = InParticleFilter( 'CharmHadSharedSoftTagChild_mu',
                                          [Hlt2LooseMuons] )

## ------------------------------------------------------------------------- ##
class TighterProtonFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs, nickname = None , shared = True  ):
        pidp_cut = "(PIDp > %(PIDp_MIN)s  )"
        deltaPID_cut = "( (PIDp-PIDK) > %(DeltaPID_MIN)s )"
        momentum_cut = "( P > %(P_MIN)s )"
        cut = pidp_cut + " & " + deltaPID_cut + " & " + momentum_cut
        nickname = name if nickname == None else nickname
        name     = name if not shared       else 'CharmHad%sTighterProton' % name
        Hlt2ParticleFilter.__init__(self, name, cut, inputs,
                                    nickname = nickname , shared = shared )

##  --------------

SharedTighterPromptChild_p = TighterProtonFilter('SharedPromptChild',
                               inputs = [SharedPromptChild_p],
                               nickname = 'TighterProtons',
                               shared = True)

## ------------------------------------------------------------------------- ##
class DetachedInParticleFilter(Hlt2ParticleFilter) : # {
    '''
    Filter charged particles on TRCHI2DOF, PT, P, MIPCHI2DV(PRIMARY),
    and (optionally) a PID variable.

    Always creates a shared instance of the filter.

    Configuration dictionaries must contain the following keys:
        'Trk_ALL_TRCHI2DOF_MAX' :  upper limit on TRCHI2DOF
        'Trk_ALL_PT_MIN'        :  lower limit on PT
        'Trk_ALL_P_MIN'         :  lower limit on P
        'Trk_ALL_MIPCHI2DV_MIN' :  lower limit on MIPCHI2DV(PRIMARY)

    If filtering on a PID variable, the pidVar parameter must be set to the
    name of the PID functor on which to cut, and the configuration dictionary
    must contain the key 'PID_LIM' defining the one-sided limit for that
    variable.  By default the cut on the PID variable is a lower limit.
    It may be changed to an upper limit by setting the parameter
    pidULim = True.
    '''
    def __init__(self, name, inputs, pidVar = None, pidULim = False): # {
        cut = ("  (TRCHI2DOF < %(Trk_ALL_TRCHI2DOF_MAX)s )" +
               "& (PT > %(Trk_ALL_PT_MIN)s)" +
               "& (P > %(Trk_ALL_P_MIN)s)" +
               "& (MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)")

        if pidVar : # {
            pidCut = "& (%s %s %%(PID_LIM)s)" % (pidVar, ('<' if pidULim else '>'))
            cut = cut + pidCut
        # }

        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, shared = True,
                                    dependencies = [PV3D('Hlt2')],
                                    UseP2PVRelations = False)

    # }
# }


## Shared instances of DetachedInParticleFilter
## Names of shared particles  begin with CharmHad to avoid name conflicts
##   with other subdirectories.
## These are all associated with specific combiners and should perhaps be
##   defined closer to that context.
## ------------------------------------------------------------------------- ##
SharedDetachedDpmChild_pi = DetachedInParticleFilter( 'CharmHadSharedDetachedDpmChild_pi',
                                                      [Hlt2LoosePions], 'PIDK', True )
SharedDetachedDpmChild_Tightpi = DetachedInParticleFilter( 'CharmHadSharedDetachedDpmChild_Tightpi',
                                                           [Hlt2LoosePions], 'PIDK', True )
SharedDetachedDpmChild_K = DetachedInParticleFilter( 'CharmHadSharedDetachedDpmChild_K',
                                                     [Hlt2LooseKaons], 'PIDK' )
SharedDetachedDpmChild_TightK = DetachedInParticleFilter( 'CharmHadSharedDetachedDpmChild_TightK',
                                                     [Hlt2LooseKaons], 'PIDK' )
SharedDetachedLcChild_pi = DetachedInParticleFilter('CharmHadSharedDetachedLcChild_pi',
                                                    [Hlt2LoosePions], 'PIDK', True )
SharedDetachedLcChild_K = DetachedInParticleFilter( 'CharmHadSharedDetachedLcChild_K',
                                                    [Hlt2LooseKaons], 'PIDK' )
SharedDetachedLcChild_p = DetachedInParticleFilter('CharmHadSharedDetachedLcChild_p',
                                                   [Hlt2LooseProtons], 'PIDp' )
SharedTighterDetachedLcChild_p = TighterProtonFilter('SharedDetachedLcChild',
                               inputs = [SharedDetachedLcChild_p],
                               nickname = 'TighterProtons',
                               shared = True)
SharedDetachedD0ToHHHHChild_pi = DetachedInParticleFilter( 'CharmHadSharedDetachedD0ToHHHHChild_pi', [Hlt2LoosePions], 'PIDK', True )
SharedDetachedD0ToHHHHChild_K = DetachedInParticleFilter( 'CharmHadSharedDetachedD0ToHHHHChild_K', [Hlt2LooseKaons], 'PIDK' )

SharedNoPIDDetachedChild_pi = DetachedInParticleFilter( 'CharmHadSharedNoPIDDetachedChild_pi', [Hlt2NoPIDsPions] )
SharedNoPIDDetachedChild_K = DetachedInParticleFilter( 'CharmHadSharedNoPIDDetachedChild_K', [Hlt2NoPIDsKaons] )
SharedNoPIDDetachedChild_p = DetachedInParticleFilter( 'CharmHadSharedNoPIDDetachedChild_p', [Hlt2NoPIDsProtons] )

## For detached D0 -> hh lines.  PID cuts not yet implemented.
CharmHadSharedDetachedD0ToHHChildPi = DetachedInParticleFilter( 'CharmHadSharedDetachedD0ToHHChildPi', [Hlt2NoPIDsPions], 'PIDK', True )
CharmHadSharedDetachedD0ToHHChildK  = DetachedInParticleFilter( 'CharmHadSharedDetachedD0ToHHChildK', [Hlt2LooseKaons], 'PIDK' )



## ------------------------------------------------------------------------- ##
class KsFilterForHHKs(Hlt2ParticleFilter) : # {
    """
    Filter KS0 candidates on BPVLTIME and VFASPF(VZ).  Applicable to both
    LL and DD KS0 candidates.

    Always creates a shared instance of the filter.

    Configuration dictionaries must contain the following keys:
        'DecayTime_MIN' : upper limit on candiate BPVLTIME()
        'VZ_MIN'
        'VZ_MAX'        : lower and upper limits on decay vertex VZ
    """
    def __init__(self, name, inputs): # {
        cut = (" (BPVLTIME() > %(DecayTime_MIN)s) " +
               "& (in_range( %(VZ_MIN)s, VFASPF(VZ), %(VZ_MAX)s ))")
        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__( self, name, cut, inputs
                        , dependencies = [PV3D('Hlt2')]
                        , shared = True )
    # }
# }

## Shared instances of KsFilterForHHKs
## Names of shared particles  begin with CharmHad to avoid name conflicts
##   with other subdirectories.
## ------------------------------------------------------------------------- ##
from Inputs import KS0_LL, KS0_DD, Lambda_LL, Lambda_DD
CharmHadSharedKsLL =  KsFilterForHHKs('CharmHadSharedKsLL',[KS0_LL])
CharmHadSharedKsDD =  KsFilterForHHKs('CharmHadSharedKsDD',[KS0_DD])


## ------------------------------------------------------------------------- ##
class NeutralInParticleFilter(Hlt2ParticleFilter) : # {
    '''
    Filter neutral particles on PT and (optionally) an ADMASS window centered
    on the database mass of a particle type identified in a constructor
    argument.
    class NeutralInParticleFilter(Hlt2ParticleFilter):
    def __init__(self, name):
        cut = ("  (PT > %(Trk_ALL_PT_MIN)s)" )
    Configuration dictionaries must contain the following keys:
        'Neut_ALL_PT_MIN' :  lower limit on PT

    If filtering on a ADMASS window, the massPart parameter must be set to
    the name of the particle type (e.g. 'pi0'), and the configuration
    dictionary must contain the key 'Neut_ALL_ADMASS_MAX' defining the window
    half-width, which is the upper limit for ADMASS.
    '''
    def __init__(self, name, inputs, massPart = None) : # {
        cut = "(PT > %(Neut_ALL_PT_MIN)s)"

        if massPart : # {
            masscut = "(ADMASS('%s') < %%(Neut_ALL_ADMASS_MAX)s) & " % (massPart)
            cut = masscut + cut
        # }

        Hlt2ParticleFilter.__init__(self, name, cut, inputs, shared = True)
    # }
# }


## Shared instances of NeutralInParticleFilter
## Names of shared particles  begin with CharmHad to avoid name conflicts
##   with other subdirectories.
## ------------------------------------------------------------------------- ##
from Inputs import Hlt2ResolvedPi0, Hlt2MergedPi0, Hlt2NoPIDsPhotons, Hlt2AllPi0, Hlt2ResolvedEta
SharedNeutralChild_pi0R  = NeutralInParticleFilter("CharmHadSharedNeutralChild_pi0R",
                                                   [Hlt2ResolvedPi0], 'pi0' )
SharedNeutralChild_pi0M  = NeutralInParticleFilter("CharmHadSharedNeutralChild_pi0M",
                                                   [Hlt2MergedPi0], 'pi0' )
SharedNeutralChild_pi0   = NeutralInParticleFilter("CharmHadSharedNeutralChild_pi0",
                                                   [Hlt2AllPi0], 'pi0' )
SharedNeutralChild_gamma = NeutralInParticleFilter("CharmHadSharedNeutralChild_gamma",
                                                   [Hlt2NoPIDsPhotons] )

SharedNeutralLowPtChild_pi0 = NeutralInParticleFilter("CharmHadSharedNeutralLowPtChild_pi0",
                                                       [Hlt2AllPi0], 'pi0' )
SharedNeutralLowPtChild_pi0R = NeutralInParticleFilter("CharmHadSharedNeutralLowPtChild_pi0R",
                                                       [Hlt2ResolvedPi0], 'pi0' )
SharedNeutralLowPtChild_pi0M = NeutralInParticleFilter("CharmHadSharedNeutralLowPtChild_pi0M",
                                                       [Hlt2MergedPi0], 'pi0' )
SharedNeutralLowPtChild_gamma = NeutralInParticleFilter("CharmHadSharedNeutralLowPtChild_gamma",
                                                        [Hlt2NoPIDsPhotons] )
SharedNeutralLowPtChild_eta = NeutralInParticleFilter("CharmHadSharedNeutralLowPtChild_eta",
                                                      [Hlt2ResolvedEta] , 'eta')



##  -----------------  define a shared Lambda class, and its instantiations,
##  -----------------  here so the instantiations are available anywhere below
##  -----------------  in this files

class SecondaryLambdaFilter(Hlt2ParticleFilter) : # {
    '''
    Filter Lambda candidates for building hyperons on

           decay time wrt PV                    BPVLTIME
           vertex position                      VFASPF

    Always creates a shared instance of the filter.

    Configuration dictionaries must contain the following keys:
        'DecayTime_MIN' :  lower limit on decay time wrt PV
        'VZ_MIN'        :  lower limit vertex z position
        'VZ_MAX'        :  upper limit vertex z position

    '''
    def __init__(self, name, inputs): # {
        cut = ("  (BPVLTIME() > %(DecayTime_MIN)s) " +
               "& (VFASPF(VZ) > %(VZ_MIN)s)" +
               "& (VFASPF(VZ) < %(VZ_MAX)s)")
        # }

        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__(self, name, cut, inputs, shared = True,
                                    dependencies = [PV3D('Hlt2')],
                                    UseP2PVRelations = False)

    # }
# }

## and instantiate  combiners for LL and DD
CharmHadSharedSecondaryLambdaLL = SecondaryLambdaFilter("CharmHadSharedSecondaryLambdaLL",[Lambda_LL])
CharmHadSharedSecondaryLambdaDD = SecondaryLambdaFilter("CharmHadSharedSecondaryLambdaDD",[Lambda_DD])


## ========================================================================= ##
## Filters for composite particles
## ========================================================================= ##

# Mass filter
## ------------------------------------------------------------------------- ##
## Ugh.  I do not think that i approve of this hack.  (P.S.)
def refit_pvs_kwargs(reFitPVs) :
    if not reFitPVs :
        return {}
    return {'ReFitPVs' : True,
            'CloneFilteredParticles' : True}

class MassFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs, nickname = None, shared = False, reFitPVs=False ):
        cut = "in_range( %(Mass_M_MIN)s , M , %(Mass_M_MAX)s )"
        nickname = name if nickname == None else nickname
        name     = name if not shared       else 'CharmHad%sMass' % name
        Hlt2ParticleFilter.__init__(self, name, cut, inputs,
                                    nickname = nickname , shared = shared,
                                    **refit_pvs_kwargs(reFitPVs))


## ========================================================================= ##
## 2-body Combiners
## ========================================================================= ##

## ------------------------------------------------------------------------- ##
class TagDecay(Hlt2Combiner) : # {
    """
    Generic 2-body combiner for adding a soft particle to another particle
    candidate, e.g., adding a pion to a D0 to created a D*+ -> D0 pi+ candidate.
    It cuts on the Q-value (difference between the combination mass and
    the sum of the masses of the individal decay products) and on chi^2/Ndof
    of the 2-body vertex.

    Previous versions of this combiner cut on the difference between the mass
    of the combination and the mass of the first individual decay product.
    This is the usual Delta m cut for D* decays if the D is the first particle
    in the list of descendents.  This restriction meant that decay descriptors
    for TagDecay had to be written carefuilly.  It was a fragile system.
    The cut on the Q-value is symmetric with respect to the decay products
    and removes this restriction and its associated danger.

    Configuration dictionaries must contain the following keys:
        'Q_AM_MIN'
        'Q_AM_MAX'         : lower and upper limits of the mass difference
                             window at the CombinationCut level, AM - AM1 - AM2.
        'TagVCHI2PDOF_MAX' : upper limit on VFASPF(VCHI2PDOF) in MotherCut
        'Q_M_MIN'
        'Q_M_MAX'          : lower and upper limits of the mass difference
                             window at the MotherCut level, M - M1 - M2.
    """
    def __init__(self, name, decay, inputs, DaughtersCuts = { }, shared = False, nickname = None, **kwargs):
        '''**kwargs can be anything accepted by the Hlt2Combiner constructor, eg, to enable PV refitting use
        ReFitPVs = True.'''

        cc =    ('in_range( %(Q_AM_MIN)s, (AM - AM1 - AM2), %(Q_AM_MAX)s )')
        mc =    ("(VFASPF(VCHI2PDOF) < %(TagVCHI2PDOF_MAX)s)" +
                 "& in_range( %(Q_M_MIN)s, (M - M1 - M2), %(Q_M_MAX)s )")

        ## Since this class allows freedom to externally specify DaughtersCuts,
        ##   we should add a dependence on the PV3D
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              DaughtersCuts = DaughtersCuts,
                              CombinationCut = cc,
                              shared = shared,
                              nickname = nickname,
                              MotherCut = mc, Preambulo = [],
                              **kwargs)
# }

class TagDecayWithNeutral(Hlt2Combiner) : # {
    """
    Generic 2-body combiner for adding a soft neutral particle to another particle
    candidate, e.g., adding a pi0 to a D0 to created a D*0 -> D0 pi0 candidate.
    It cuts on the Q-value (difference between the combination mass and
    the sum of the masses of the individal decay products) of the 2-body
    combination.

    Previous versions of this combiner cut on the difference between the mass
    of the combination and the mass of the first individual decay product.
    This is the usual Delta m cut for D* decays if the D is the first particle
    in the list of descendents.  This restriction meant that decay descriptors
    for TagDecay had to be written carefuilly.  It was a fragile system.
    The cut on the Q-value is symmetric with respect to the decay products
    and removes this restriction and its associated danger.

    Configuration dictionaries must contain the following keys:
        'Q_AM_MIN'
        'Q_AM_MAX'         : lower and upper limits of the mass difference
                             window at the CombinationCut level, AM - AM1 - AM2.
        'Q_M_MIN'
        'Q_M_MAX'          : lower and upper limits of the mass difference
                             window at the MotherCut level, M - M1 - M2.
    """
    def __init__(self, name, decay, inputs, DaughtersCuts = { }, shared = False, nickname = None):
        cc =    ('in_range( %(Q_AM_MIN)s, (AM - AM1 - AM2), %(Q_AM_MAX)s )')
        mc =    ("in_range( %(Q_M_MIN)s, (M - M1 - M2), %(Q_M_MAX)s )")

        ## Since this class allows freedom to externally specify DaughtersCuts,
        ##   we should add a dependence on the PV3D
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              DaughtersCuts = DaughtersCuts,
                              CombinationCut = cc,
                              shared = shared,
                              nickname = nickname,
                              MotherCut = mc,
                              ParticleCombiners={'':'ParticleAdder'},
                              Preambulo = [])
# }


## Combiner class for D0 -> h h' decays with detachement cuts
## ------------------------------------------------------------------------- ##
class DetachedD02HHCombiner(Hlt2Combiner) : # {
    """
    Combiner for 3 basic track-based particles.  The 'Detached' in the class
    name indicates that it applies cuts to lifetime-biasing non-lifetime
    values like the MIPCHI2DV(PRIMARY) of the decay products and the
    BPVVDCHI2 of the fitted vertex.

    Configuration dictionaries must contain the following keys:
        ## P.S. -- If filtered input particles are used, i would prefer that
        ##         all filtering that applies to every charged input be done
        ##         in those filters to prevent inadvertent configuration
        ##         mismatches.
        'Trk_ALL_PT_MIN'        :  lower limit on PT for all inputs
        'Trk_ALL_P_MIN'         :  lower limit on P for all inputs
        'Trk_ALL_MIPCHI2DV_MIN' :  lower limit on MIPCHI2DV(PRIMARY)

        'Comb_AM_MIN'
        'Comb_AM_MAX'       : lower and upper limits on AM in CombinationCut
        'Trk_Max_APT_MIN'   : lower limit on largest product PT (APT1 or APT2)
        'D0_PT_MIN'         : lower limit on APT in CombinationCut
        'Pair_AMINDOCA_MAX' : upper limit on AMINDOCA('') in CombinationCut
        'D0_VCHI2PDOF_MAX'  : upper limit on VFASPF(VCHI2PDOF) in MotherCut
        'D0_BPVVDCHI2_MIN'  : lower limit on BPVVDCHI2 in MotherCut
        'D0_acosBPVDIRA_MAX': upper limit on acos(BPVDIRA) in MotherCut (rad)
        'TisTosSpec'        : configuration string of the Hlt1 TISTOS filter.
    """
    def __init__(self, name, decay, inputs, nickname = None, shared = False, **kwargs) : # {
        '''**kwargs can be anything accepted by the Hlt2Combiner constructor, eg, to enable PV refitting use
        ReFitPVs = True.'''
        dc = { }
        for child in ['pi+','K+','p+'] :
            dc[child] = "(PT > %(Trk_ALL_PT_MIN)s)" \
                        "& (P > %(Trk_ALL_P_MIN)s)" \
                        "& (MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)"

        ## Assume that the wide mass range is wider than the narrow range.
        combcuts = "in_range(%(Comb_AM_MIN)s,  AM, %(Comb_AM_MAX)s)" \
                   "& ((APT1 > %(Trk_Max_APT_MIN)s) " \
                       "| (APT2 > %(Trk_Max_APT_MIN)s))" \
                   "& (APT > %(D0_PT_MIN)s)" \
                   "& (AMINDOCA('') " \
                       "< %(Pair_AMINDOCA_MAX)s )"

        parentcuts = "(VFASPF(VCHI2PDOF) < %(D0_VCHI2PDOF_MAX)s)" \
                     "& (BPVVDCHI2> %(D0_BPVVDCHI2_MIN)s )" \
                     "& (BPVDIRA > lcldira )"

        pream = [
                "import math"
                , "lcldira = math.cos( %(D0_acosBPVDIRA_MAX)s )"
        ]

        from HltTracking.HltPVs import PV3D
        ## The name passed to the base class constructor determines the
        ##   configuration dictionary that is picked up.
        name = name if not shared else 'CharmHad' + name
        Hlt2Combiner.__init__( self, name, decay, inputs,
                               dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                               tistos = 'TisTosSpec',
                               nickname = nickname,
                               shared = shared,
                               DaughtersCuts = dc,
                               CombinationCut = combcuts,
                               MotherCut = parentcuts,
                               Preambulo = pream,
                               **kwargs)
    # }
# }


## Shared instances of DetachedD02HHCombiner
## ------------------------------------------------------------------------- ##

D02HH_D0ToKmPip  = DetachedD02HHCombiner( 'D02HH_D0ToKmPip'
        , decay = "[D0 -> K- pi+]cc"
        , inputs = [ CharmHadSharedDetachedD0ToHHChildPi, CharmHadSharedDetachedD0ToHHChildK ]
        , nickname = 'D02HH'            ## def in D02HHLines.py
        , shared = True
        , ReFitPVs = True)
D02HH_D0ToKmKp   = DetachedD02HHCombiner( 'D02HH_D0ToKmKp'
        , decay = "D0 -> K- K+"         ## Only D0s to prevent duplication
        , inputs = [ CharmHadSharedDetachedD0ToHHChildK ]
        , nickname = 'D02HH'            ## def in D02HHLines.py
        , shared = True
        , ReFitPVs = True)
D02HH_D0ToPimPip = DetachedD02HHCombiner( 'D02HH_D0ToPimPip'
        , decay = "D0 -> pi- pi+"       ## Only D0s to prevent duplication
        , inputs = [ CharmHadSharedDetachedD0ToHHChildPi ]
        , nickname = 'D02HH'            ## def in D02HHLines.py
        , shared = True
        , ReFitPVs = True)

## Shared instance of TagDecay that depends on DetachedD02HHCombiner
## Names of shared particles  begin with CharmHad to avoid name conflicts
##   with other subdirectories.
## ------------------------------------------------------------------------- ##
D0_TAG_CPV_Dstp2D0Pip_D02KmPip = TagDecay('CharmHadD0_TAG_CPV_Dstp2D0Pip_D02KmPip'
        , decay = ["[D*(2010)+ -> D0 pi+]cc"]
        , inputs = [ D02HH_D0ToKmPip, SharedSoftTagChild_pi ]
        , nickname = 'D0_TAG_CPV'       ## def in D02HHLines.py
        , shared = True
        , ReFitPVs = True)




## ========================================================================= ##
## 3-body Combiners
## ========================================================================= ##

# ------------------------------------------------------------------------- ##
class DetachedHHHCombiner(Hlt2Combiner) : # {
    """
    Combiner for 3 basic track-based particles.  The 'Detached' in the class
    name indicates that it applies cuts to lifetime-biasing non-lifetime
    values like the MIPCHI2DV(PRIMARY) of the decay products and the
    BPVVDCHI2 of the fitted vertex.

    Always creates a shared instance of the filter.

    Configuration dictionaries must contain the following keys:
        'Trk_1OF3_PT_MIN'
        'Trk_2OF3_PT_MIN'
        'Trk_ALL_PT_MIN'        : tiered lower limits on product PT.
                                  All 3 must pass the ALL threshold.
                                  At least 2 must pass the 2OF3 threshold.
                                  At least 1 must pass the 1OF3 threshold.
        'Trk_1OF3_MIPCHI2DV_MIN'
        'Trk_2OF3_MIPCHI2DV_MIN'
        'Trk_ALL_MIPCHI2DV_MIN' : tiered lower limits on product
                                  MIPCHI2DV(PRIMARY)
        'AM_MIN'
        'AM_MAX'                : lower and upper limits on AM in CombinationCut
        'ASUMPT_MIN'            : lower limit on APT1+APT2+APT3 in CombinationCut
        'VCHI2PDOF_MAX'         : upper limit on VFASPF(VCHI2PDOF) in MotherCut
        'acosBPVDIRA_MAX'       : upper limit on acos(BPVDIRA) in MotherCut (rad)
        'BPVVDCHI2_MIN'         : lower limit on BPVVDCHI2 in MotherCut
        'BPVLTIME_MIN'          : lower limit on BPVLTIME() in MotherCut
        'TisTosSpec'            : configuration string of the Hlt1 TISTOS filter.
    """
    def __init__(self, name, decay, inputs, nickname = None) : # {
        dc =    {}
        for child in ['pi+','K+','p+','mu+'] :
            dc[child] = "(PT > %(Trk_ALL_PT_MIN)s) & (MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)"
        cc =    ("(in_range( %(AM_MIN)s, AM, %(AM_MAX)s ))" +
                 " & ((APT1+APT2+APT3) > %(ASUMPT_MIN)s )" +
                 " & (AHASCHILD(PT > %(Trk_1OF3_PT_MIN)s))"+
                 " & (ANUM(PT > %(Trk_2OF3_PT_MIN)s) >= 2)"+
                 " & (AHASCHILD((MIPCHI2DV(PRIMARY)) > %(Trk_1OF3_MIPCHI2DV_MIN)s))"+
                 " & (ANUM(MIPCHI2DV(PRIMARY) > %(Trk_2OF3_MIPCHI2DV_MIN)s) >= 2)")
        ## P.S. -- Are cuts on both BPVVDCHI2 and BPVLTIME useful?
        mc =    ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > lcldira )" +
                 " & (BPVLTIME() > %(BPVLTIME_MIN)s )")

        pream = [
                "import math"
                , "lcldira = math.cos( %(acosBPVDIRA_MAX)s )"
        ]

        nickname = name if nickname == None else nickname
        from HltTracking.HltPVs import PV3D
        ## P.S. -- I want to remove the dependence on the GEC.
        ## This kind of automatic name-mangling breaks under cloning.
        Hlt2Combiner.__init__(self, "CharmHad" + name, decay, inputs,
                              nickname = nickname, dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              shared = True, tistos = 'TisTosSpec', DaughtersCuts = dc,
                              CombinationCut = cc, MotherCut = mc, Preambulo = pream)
    # }
# }


## Shared instances of DetachedHHHCombiner
## ------------------------------------------------------------------------- ##

## Main line D+ -> 3h combiners
D2HHH_DpToKmPipPip = DetachedHHHCombiner( 'D2HHH_DpToKmPipPip'
        , decay = "[D+ -> K- pi+ pi+]cc"
        , inputs = [ SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi ]
        , nickname = 'Dpm2HHH' )  ## 'D2HHH' defined in D2HHHLines.py

D2HHH_DpToKpPimPip = DetachedHHHCombiner( 'D2HHH_DpToKpPimPip'
        , decay = "[D+ -> K+ pi- pi+]cc"
        , inputs = [ SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi ]
        , nickname = 'Dpm2HHH' )  ## 'D2HHH' defined in D2HHHLines.py

D2HHH_DpToKmKpPip = DetachedHHHCombiner( 'D2HHH_DpToKmKpPip'
        , decay = "[D+ -> K- K+ pi+]cc"
        , inputs = [ SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi ]
        , nickname = 'Dpm2HHH' )  ## 'D2HHH' defined in D2HHHLines.py

D2HHH_DpToKpKpPim = DetachedHHHCombiner( 'D2HHH_DpToKpKpPim'
        , decay = "[D+ -> pi- K+ K+]cc"
        , inputs = [ SharedDetachedDpmChild_TightK, SharedDetachedDpmChild_pi ]
        , nickname = 'Dpm2PKK' )  ## 'D2HHH' defined in D2HHHLines.py


D2HHH_DpToPimPipPip = DetachedHHHCombiner( 'D2HHH_DpToPimPipPip'
        , decay = "[D+ -> pi- pi+ pi+]cc"
        , inputs = [ SharedDetachedDpmChild_pi ]
        , nickname = 'Dpm2PPP' )  ## 'Dpm2PPP' defined in D2HHHLines.py

D2HHH_DpToKmKpKp = DetachedHHHCombiner( 'D2HHH_DpToKmKpKp'
        , decay = "[D+ -> K- K+ K+]cc"
        , inputs = [ SharedDetachedDpmChild_K ]
        , nickname = 'Dpm2KKK' )  ## 'Dpm2KKK' defined in D2HHHLines.py
## Main line D_s+ -> 3h combiners
D2HHH_DspToKpPimPip = DetachedHHHCombiner( 'D2HHH_DspToKpPimPip'
        , decay = "[D_s+ -> K+ pi- pi+]cc"
        , inputs = [ SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi ]
        , nickname = 'Ds2HHH' )  ## 'D2HHH' defined in D2HHHLines.py

D2HHH_DspToKpKpPim = DetachedHHHCombiner( 'D2HHH_DspToKpKpPim'
        , decay = "[D_s+ -> K+ K+ pi-]cc"
        , inputs = [ SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi ]
        , nickname = 'Ds2HHH' )  ## 'D2HHH' defined in D2HHHLines.py

D2HHH_DspToKmKpPip = DetachedHHHCombiner( 'D2HHH_DspToKmKpPip'
        , decay = "[D_s+ -> K- K+ pi+]cc"
        , inputs = [ SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi ]
        , nickname = 'Ds2HHH' )  ## 'D2HHH' defined in D2HHHLines.py

D2HHH_DspToKmPipPip = DetachedHHHCombiner( 'D2HHH_DspToKmPipPip'
        , decay = "[D_s+ -> K- pi+ pi+]cc"
        , inputs = [ SharedDetachedDpmChild_TightK, SharedDetachedDpmChild_pi ]
        , nickname = 'Ds2KPP' )  ## 'Ds2KPP' defined in D2HHHLines.py



D2HHH_DspToPimPipPip = DetachedHHHCombiner( 'D2HHH_DspToPimPipPip'
        , decay = "[D_s+ -> pi- pi+ pi+]cc"
        , inputs = [ SharedDetachedDpmChild_pi ]
        , nickname = 'Ds2PPP' )  ## 'Ds2PPP' defined in D2HHHLines.py

D2HHH_DspToKmKpKp = DetachedHHHCombiner( 'D2HHH_DspToKmKpKp'
        , decay = "[D_s+ -> K- K+ K+]cc"
        , inputs = [ SharedDetachedDpmChild_K ]
        , nickname = 'Ds2KKK' )  ## 'D2KKK' defined in D2KKKLines.py

## Main line Lambda_c+ -> 3h combiners
LcXic2HHH_LcpToKmPpPip = DetachedHHHCombiner( 'LcXic2HHH_LcpToKmPpPip'
        , decay = "[Lambda_c+ -> K- p+ pi+]cc"
        , inputs = [ SharedTighterDetachedLcChild_p, SharedDetachedLcChild_K,
                     SharedDetachedLcChild_pi ]
        , nickname = 'LcXic2HHH' ) ## 'LcXic2HHH' defined in D2HHHLines.py

LcXic2HHH_LcpToKmPpKp = DetachedHHHCombiner( 'LcXic2HHH_LcpToKmPpKp'
        , decay = "[Lambda_c+ -> K- p+ K+]cc"
        , inputs = [ SharedTighterDetachedLcChild_p, SharedDetachedLcChild_K ]
        , nickname = 'LcXic2HHH' ) ## 'LcXic2HHH' defined in D2HHHLines.py

LcXic2HHH_LcpToPimPpPip = DetachedHHHCombiner( 'LcXic2HHH_LcpToPimPpPip'
        , decay = "[Lambda_c+ -> pi- p+ pi+]cc"
        , inputs = [ SharedTighterDetachedLcChild_p, SharedDetachedLcChild_pi ]
        , nickname = 'LcXic2HHH' ) ## 'LcXic2HHH' defined in D2HHHLines.py

LcXic2HHH_LcpToPimPpKp = DetachedHHHCombiner( 'LcXic2HHH_LcpToPimPpKp'
        , decay = "[Lambda_c+ -> pi- p+ K+]cc"
        , inputs = [ SharedTighterDetachedLcChild_p, SharedDetachedLcChild_K,
                     SharedDetachedLcChild_pi ]
        , nickname = 'LcXic2HHH' ) ## 'LcXic2HHH' defined in D2HHHLines.py

## Mass filter to select the Lambda_c+ mass window for Lambda_c+ -> p K- pi+.
## Defined here becuase it is used in multiple CharmHad modules.
Lc2HHH_LcpToKmPpPip = MassFilter('Lc2HHH_LcpToKmPpPip'
                                , inputs=[LcXic2HHH_LcpToKmPpPip]
                                , nickname = 'Lc2HHH'   ## def in D2HHHLines.py
                                , shared = True, reFitPVs = True)
Xic2HHH_XicpToKmPpPip = MassFilter('Xic2HHH_XicpToKmPpPip'
                                , inputs=[LcXic2HHH_LcpToKmPpPip]
                                , nickname = 'Xic2HHH'   ## def in D2HHHLines.py
                                , shared = True, reFitPVs = True)


## Combiners for cross-section measurements
XSec_DpToKmPipPip = DetachedHHHCombiner( 'XSec_DpToKmPipPip'
        , decay = "[D+ -> K- pi+ pi+]cc"
        , inputs = [ SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi ]
        , nickname = 'Dpm2HHH_XSec' )   ## 'Dpm2HHH_XSec' def in XSecLines.py

XSec_DpToKmKpPim = DetachedHHHCombiner( 'XSec_DpToKmKpPim'
        , decay = "[D+ -> K- K+ pi+]cc"
        , inputs = [ SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi ]
        , nickname = 'Dpm2HHH_XSec' )   ## 'Dpm2HHH_XSec' def in XSecLines.py

XSec_DspToKmKpPim = DetachedHHHCombiner( 'XSec_DspToKmKpPim'
        , decay = "[D_s+ -> K- K+ pi+]cc"
        , inputs = [ SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi ]
        , nickname = 'Ds2HHH_XSec' )    ## 'Ds2HHH_XSec' def in XSecLines.py

XSec_DspToPimPipPip = DetachedHHHCombiner( 'XSec_DspToPimPipPip'
        , decay = "[D_s+ -> pi- pi+ pi+]cc"
        , inputs = [ SharedDetachedDpmChild_pi ]
        , nickname = 'Ds2HHH_XSec' )   ## 'Ds2HHH_XSec' def in XSecLines.py

XSec_LcpToKmPpPip = DetachedHHHCombiner( 'XSec_LcpToKmPpPip'
        , decay = "[Lambda_c+ -> K- p+ pi+]cc"
        , inputs = [ SharedTighterDetachedLcChild_p, SharedDetachedLcChild_K,
                     SharedDetachedLcChild_pi ]
        , nickname = 'Lc2HHH_XSec' ) ## 'Lc2HHH_XSec' defined in XSecLines.py

XSec_LcpToPimPpPip = DetachedHHHCombiner( 'XSec_LcpToPimPpPip'
        , decay = "[Lambda_c+ -> pi- p+ pi+]cc"
        , inputs = [ SharedTighterDetachedLcChild_p, SharedDetachedLcChild_pi ]
        , nickname = 'Lc2HHH_XSec' ) ## 'Lc2HHH_XSec' defined in XSecLines.py

XSec_LcpToKmPpKp = DetachedHHHCombiner( 'XSec_LcpToKmPpKp'
        , decay = "[Lambda_c+ -> K- p+ K+]cc"
        , inputs = [ SharedTighterDetachedLcChild_p, SharedDetachedLcChild_K]
        , nickname = 'Lc2HHH_XSec' ) ## 'Lc2HHH_XSec' defined in XSecLines.py

XSec_D02KPi = DetachedD02HHCombiner('XSec_D02KPi'
        , decay = "[D0 -> K- pi+]cc"
        , inputs = [ SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi ]
        , nickname = 'D02HH_XSec',shared=True ) ## 'D02HH_XSec' defined in XSecLines.py

## Combiner for KPi detection asymmetry studies
DetAsym_DpToKmPipPip = DetachedHHHCombiner( 'DetAsym_DpToKmPipPip'
        , decay = "[D+ -> K- pi+ pi+]cc"
        , inputs = [ SharedNoPIDDetachedChild_pi, SharedNoPIDDetachedChild_K ]
        , nickname = 'Dpm2KPiPi_ForKPiAsym' ) ## 'Dpm2KPiPi_ForKPiAsym' def in D2HHHLines.py



## ========================================================================= ##
## Yet-to-be sorted things.
## ========================================================================= ##


## ------------------------------------------------------------------------- ##
from Configurables import DaVinci__N3BodyDecays as N3BodyDecays
class HHKshCombiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs):

## HHKs mother cuts
        mc =    (
                 "   (PT > %(D_PT_MIN)s )" +
                 " & (CHI2VXNDOF < %(D_VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > lcldira )" +
                 " & (BPVVDCHI2 > %(D_BPVVDCHI2_MIN)s )" +
                 " & (BPVLTIME() > %(D_BPVLTIME_MIN)s )"
                )

        c12Cut = ("(AM <  %(HHMass_MAX)s)" +
                  " & (ADOCAMAX('') < %(HHMaxDOCA)s)")

        cc =    ("(in_range( %(AM_MIN)s, AM, %(AM_MAX)s ))" +
                 " & ((APT1+APT2+APT3) > %(ASUMPT_MIN)s )")

        pream = [
                "import math"
                , "lcldira = math.cos( %(D_acosBPVDIRA_MAX)s )"
        ]

        from HltTracking.HltPVs import PV3D

##  The argument "nickname = 'D02HHKsh'"  points at the common
##  dictionary which allows the individual instantiations of
##  this "shared = True" combiner to have unique names but only one
##  dictionary, reducing the number of slots defined in Lines.py
##  By making this a "shared = True" class, the individual lines
##  can be used as input to multiple combiners (TagDecay being the
##  prime example) without creating redundant copies.
        Hlt2Combiner.__init__(self, name, decay, inputs,
                     dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                     shared = True,
                     tistos = 'TisTosSpec', combiner = N3BodyDecays,
                     nickname = 'D02HHKsh',
                     Combination12Cut =  c12Cut,
                     CombinationCut = cc,
                     MotherCut = mc, Preambulo = pream )


## ------------------------------------------------------------------------- ##
##  TheLTUNB_HHKshCombiner is very similar to the standard HHKshCombiner
##  except that it has no BPVVDCHI2 cut and it is intended to be used
##  with TisTosSpec which is TIS rather than TOS to avoid bias
class LTUNB_HHKshCombiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs):

## HHKs mother cuts
        mc =    ("(VFASPF(VCHI2PDOF) < %(D_VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > lcldira )" +
                 " & (PT > %(D_PT_MIN)s )" +
                 " & (BPVLTIME() > %(D_BPVLTIME_MIN)s )")

        c12Cut = ("(AM <  %(HHMass_MAX)s)" +
                  " & (ADOCAMAX('') < %(HHMaxDOCA)s)")

        cc =    ("(in_range( %(AM_MIN)s, AM, %(AM_MAX)s ))" +
                 " & ((APT1+APT2+APT3) > %(ASUMPT_MIN)s )")

        pream = [
                "import math"
                , "lcldira = math.cos( %(D_acosBPVDIRA_MAX)s )"
        ]

        from HltTracking.HltPVs import PV3D

##  The argument "nickname = 'LTUNB_D02HHKsh'"  points at the common
##  dictionary which allows the individual instantiations of
##  this "shared = True" combiner to have unique names but only one
##  dictionary, reducing the number of slots defined in Lines.py
##  By making this a "shared = True" class, the individual lines
##  can be used as input to multiple combiners (TagDecay being the
##  prime example) without creating redundant copies.
        Hlt2Combiner.__init__(self, name, decay, inputs,
                     dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                     shared = True,
                     tistos = 'TisTosSpec', combiner = N3BodyDecays,
                     nickname = 'LTUNB_D02HHKsh',
                     Combination12Cut =  c12Cut,
                     CombinationCut = cc,
                     MotherCut = mc, Preambulo = pream )

## ------------------------------------------------------------------------- ##
class HKsKsCombiner(Hlt2Combiner):
    """
    Combiner for exclusive X->hKsKs lines.

    Configuration dictionaries must contain the following keys:
        'KS0LL_ALL_PT_MIN'        : Minimum KS transverse momentum
        'KS0LL_ALL_MIPCHI2DV_MIN' : Minimum KS IPChi2
        'KS0DD_ALL_PT_MIN'        : Minimum KS transverse momentum
        'KS0DD_ALL_MIPCHI2DV_MIN' : Minimum KS IPChi2
        'Trk_ALL_PT_MIN'          : Minimum bachelor transverse momentum
        'Trk_ALL_MIPCHI2DV_MIN'   : Minimum bachelor IPChi2
        'AM_MIN'                  : Minimum mass of the 3-body combination
        'AM_MAX'                  : Maximum mass of the 3-body combination
        'AM_3'                    : Mass of the last particle
        'PT12_MIN'                : Minimum PT of the two Ks combination
        'ASUMPT_MIN'              : Minimum value of the sum of the PT of the daughters
        'VCHI2PDOF_MAX'           : Upper limit on VFASPF(VCHI2PDOF) in MotherCut
        'acosBPVDIRA_MAX'         : Upper limit on acosDIRA wrt bestPV in MotherCut
        'BPVLTIME_MIN'            : Lower limit on Lifetime wrt bestPV in MotherCut
        'DPT_MIN'                 : Minimum PT of the vertex candidate
        'DMOM_MIN'                : mother minimum momentum
        'TisTosSpec'              : Configuration string of the Hlt1 TISTOS filter.
    """
    def __init__(self, name, decay, inputs, shared = False, nickname = None):
        dc = { 'KS0' : "( (INTREE((ABSID=='pi+') & (ISLONG))) "+
                        "&(PT > %(KS0LL_ALL_PT_MIN)s) & (MIPCHI2DV(PRIMARY) > %(KS0LL_ALL_MIPCHI2DV_MIN)s) )"+
                    " | ( (INTREE((ABSID=='pi+') & (ISDOWN))) "+
                        "&(PT > %(KS0DD_ALL_PT_MIN)s) & (MIPCHI2DV(PRIMARY) > %(KS0DD_ALL_MIPCHI2DV_MIN)s) )" }
        for child in ['pi+','K+','p+'] :
            dc[child] = "(PT > %(Trk_ALL_PT_MIN)s) & (MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)"
        c12 = (" ( AM < (%(AM_MAX)s - %(AM_3)s) ) " +
               "&( APT > %(PT12_MIN)s ) " )
        cc =  (" (in_range( %(AM_MIN)s, AM, %(AM_MAX)s ))" +
               "&((APT1+APT2+APT3) > %(ASUMPT_MIN)s )")
        mc =  (" (VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +
               "&(PT > %(DPT_MIN)s )" +
               "&(P > %(DMOM_MIN)s )" +
               "&(BPVDIRA > lcldira )" +
               "&(BPVLTIME() > %(BPVLTIME_MIN)s )")
        #if lldd == True :
        #  mc = "(INTREE((ABSID=='pi+') & (ISLONG)) & INTREE((ABSID=='pi+') & (ISDOWN))) & " + mc

        pream = [
                "import math"
                , "lcldira = math.cos( %(acosBPVDIRA_MAX)s )"
        ]

        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs,
             dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
             shared = shared,
             tistos = 'TisTosSpec', combiner = N3BodyDecays,
             nickname = nickname,
             Combination12Cut =  c12,
             CombinationCut = cc,
             MotherCut = mc, Preambulo = pream)
## ---END OF HKsKsCombiner-------------------------------------------------- ##


## ------------------------------------------------------------------------- ##
class HHHCombiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs, shared = False, nickname = None) :
        dc =    {}
        cc =    ("(in_range( %(AM_MIN)s, AM, %(AM_MAX)s ))" +
                 " & ((APT1+APT2+APT3) > %(ASUMPT_MIN)s )" )
        mc =    ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > lcldira )" +
                 " & (BPVLTIME() > %(BPVLTIME_MIN)s )")

        pream = [
                "import math"
                , "lcldira = math.cos( %(acosBPVDIRA_MAX)s )"
        ]

        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs, shared = shared,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec', DaughtersCuts = dc, CombinationCut = cc,
                              MotherCut = mc, Preambulo = pream, nickname = nickname)




class DV4BCombiner(Hlt2Combiner):
    """
    Combiner for exclusive D0->hhhh lines.

    Configuration dictionaries must contain the following keys:
        'AM_MAX'        : Maximum mass of the n-body combination
        'AM_34'         : Mass of the remaining 2 particles
        'AM_4'          : Mass of the last particle
        'ACHI2DOCA_MAX' : Maximum Doca-chi2 of each two-body combination (should be >= VCHI2NDOF)
        'ASUMPT_MIN'    : Lower limit on the sum of the PTs of the daughters
        'VCHI2PDOF_MAX' : Upper limit on VFASPF(VCHI2PDOF) in MotherCut
        'acosBPVDIRA_MAX' : Upper limit on acos(DIRA) wrt bestPV in MotherCut
        'BPVLTIME_MIN'  : Lower limit on Lifetime wrt bestPV in MotherCut
        'TisTosSpec'    : Configuration string of the Hlt1 TISTOS filter.
        'AMOM_MIN'      : combination minimum  momentum
        'DPT_MIN'       : mother minimum pT
        'DMOM_MIN'      : mother minimum momentum
    """
    def __init__(self, name, decay,inputs, shared = False, nickname = None):
        dc =  {}
        for child in ['pi+','K+','p+'] :
            dc[child] = "(PT > %(Trk_ALL_PT_MIN)s) & (MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)"
        c12 = (" ( AM < (%(AM_MAX)s - %(AM_34)s) ) " +
               "&( ADOCA(1,2) < %(ADOCA_MAX)s ) " +
               "&( ACHI2DOCA(1,2) < %(ACHI2DOCA_MAX)s ) " )
        c123 =(" ( AM < (%(AM_MAX)s - %(AM_4)s) ) " +
               "&( ADOCA(1,3) < %(ADOCA_MAX)s ) " +
               "&( ADOCA(2,3) < %(ADOCA_MAX)s ) " +
               "&( ACHI2DOCA(1,3) < %(ACHI2DOCA_MAX)s ) " +
               "&( ACHI2DOCA(2,3) < %(ACHI2DOCA_MAX)s ) " )
        cc =  (" (in_range( %(AM_MIN)s, AM, %(AM_MAX)s )) " +
               "&( (APT1+APT2+APT3+APT4) > %(ASUMPT_MIN)s )" +
               "&( AP > %(AMOM_MIN)s )" +
               "&( ADOCA(1,4) < %(ADOCA_MAX)s ) " +
               "&( ADOCA(2,4) < %(ADOCA_MAX)s ) " +
               "&( ADOCA(3,4) < %(ADOCA_MAX)s ) " +
               "&( ACHI2DOCA(1,4) < %(ACHI2DOCA_MAX)s ) " +
               "&( ACHI2DOCA(2,4) < %(ACHI2DOCA_MAX)s ) " +
               "&( ACHI2DOCA(3,4) < %(ACHI2DOCA_MAX)s ) " )
        mc =    ("(CHI2VXNDOF < %(VCHI2PDOF_MAX)s)" +
                 " & (PT > %(DPT_MIN)s )" +
                 " & (P  > %(DMOM_MIN)s )"+
                 " & (BPVDIRA > lcldira )" +
                 " & (BPVLTIME() > %(BPVLTIME_MIN)s )" +
                 " & (BPVVDCHI2 > %(BPVVDCHI2_MIN)s )"  )
        pream = [
                "import math"
                , "lcldira = math.cos( %(acosBPVDIRA_MAX)s )"
        ]

        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs, shared = shared,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec', combiner = DaVinci__N4BodyDecays, DaughtersCuts = dc,
                              Combination12Cut = c12, Combination123Cut = c123, CombinationCut = cc,
                              MotherCut = mc, Preambulo = pream, nickname = nickname)

## Shared instances of DV4BCombiner
## ------------------------------------------------------------------------- ##
XSec_D02K3Pi = DV4BCombiner('XSec_D02K3Pi'
        , decay = "[D0 -> K- pi+ pi+ pi-]cc"
        , inputs = [ SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi ]
        , nickname = 'D02HHHH_XSec',shared=True ) ## 'D02HHHH_XSec' defined in XSecLines.py

LcpToLambda0KmKpPip = DV4BCombiner('CharmHadLcpToLamKmKpPip'
        , decay = "[Lambda_c+ -> Lambda0 K- K+ pi+]cc"
        , inputs =[CharmHadSharedSecondaryLambdaLL,CharmHadSharedSecondaryLambdaDD,
                   SharedDetachedLcChild_K, SharedDetachedLcChild_pi]
        , nickname = 'LcpToLamKmKpPip'  ## def in D2HHHKsLines.py
        , shared = True)

LcpToLambda0KmPipPip = DV4BCombiner('CharmHadLcpToLamKmPipPip'
        , decay = "[Lambda_c+ -> Lambda0 K- pi+ pi+]cc"
        , inputs =[CharmHadSharedSecondaryLambdaLL,CharmHadSharedSecondaryLambdaDD,
                   SharedDetachedLcChild_K, SharedDetachedLcChild_pi]
        , nickname = 'LcpToLamKmPipPip'  ## def in D2HHHKsLines.py
        , shared = True )

D02HHHH_D02PiPiPiPi = DV4BCombiner('CharmHadD02HHHH_D02PiPiPiPi'
                , inputs=[SharedDetachedD0ToHHHHChild_pi]
                , decay=['D0 -> pi+ pi+ pi- pi-']
                , nickname = 'D02HHHH'  ## def in D02HHHHLines.py
                , shared=True)

D02HHHH_D02KPiPiPi = DV4BCombiner('CharmHadD02HHHH_D02KPiPiPi'
                , inputs=[ SharedDetachedD0ToHHHHChild_pi
                           , SharedDetachedD0ToHHHHChild_K]
                , decay=['[D0 -> K- pi+ pi+ pi-]cc']
                , nickname = 'D02HHHH'  ## def in D02HHHHLines.py
                , shared=True)

D02HHHH_D02KKPiPi = DV4BCombiner('CharmHadD02HHHH_D02KKPiPi'
                , inputs=[ SharedDetachedD0ToHHHHChild_pi
                           , SharedDetachedD0ToHHHHChild_K]
                , decay=['D0 -> K+ K- pi+ pi-']
                , nickname = 'D02HHHH'  ## def in D02HHHHLines.py
                , shared=True)

D02HHHH_D02KKKPi = DV4BCombiner('CharmHadD02HHHH_D02KKKPi'
                , inputs = [ SharedDetachedD0ToHHHHChild_pi
                           , SharedDetachedD0ToHHHHChild_K]
                , decay = ['[D0 -> K- K- K+ pi+]cc']
                , nickname = 'D02HHHH'  ## def in D02HHHHLines.py
                , shared = True)

## Nominal mass window filters for the D0 -> 4h combiners.  Shared for use
## in untagged lines, tagged lines, and other spectroscopy lines.
D02HHHHMass_D02PiPiPiPi = MassFilter('CharmHadD02HHHH_D02PiPiPiPiFilt'
                , inputs = [D02HHHH_D02PiPiPiPi]
                , nickname = 'D02HHHHMass'      ## def in D02HHHHLines.py
                , shared = True)
D02HHHHMass_D02KPiPiPi  = MassFilter('CharmHadD02HHHH_D02KPiPiPiFilt'
                , inputs = [D02HHHH_D02KPiPiPi]
                , nickname = 'D02HHHHMass'      ## def in D02HHHHLines.py
                , shared = True)
D02HHHHMass_D02KKPiPi   = MassFilter('CharmHadD02HHHH_D02KKPiPiFilt'
                , inputs = [D02HHHH_D02KKPiPi]
                , nickname = 'D02HHHHMass'      ## def in D02HHHHLines.py
                , shared = True)
D02HHHHMass_D02KKKPi    = MassFilter('CharmHadD02HHHH_D02KKKPiFilt'
                , inputs = [D02HHHH_D02KKKPi]
                , nickname = 'D02HHHHMass'      ## def in D02HHHHLines.py
                , shared = True)

## The D*+ reconstructed from D02HHHHMass_D02KPiPiPi is also used in a line in
## CharmSpectroscopyLines.py.  It should be shared and is defined here.
DstToD02HHHH_D02CFKPiPiPi = TagDecay('CharmHadDstToD02HHHH_D02CFKPiPiPi'
                , decay = ["[D*(2010)+ -> D0 pi+]cc"]
                , inputs = [D02HHHHMass_D02KPiPiPi, SharedSoftTagChild_pi]
                , nickname = 'DstToD02HHHH'      ## def in D02HHHHLines.py
                , shared = True)



class HHHHCombiner(Hlt2Combiner):
    def __init__(self, name, decay,inputs):
        dc =    {}
        cc =    ("(in_range( %(AM_MIN)s, AM, %(AM_MAX)s ))" +
                 " & ((APT1+APT2+APT3+APT4) > %(ASUMPT_MIN)s )" )
        mc =    ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > lcldira )" +
                 " & (BPVLTIME() > %(BPVLTIME_MIN)s )")
        pream = [
                "import math"
                , "lcldira = math.cos( %(acosBPVDIRA_MAX)s )"
        ]

        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec', DaughtersCuts = dc, CombinationCut = cc,
                              MotherCut = mc, Preambulo = pream)



class DetachedHHHHCombiner(Hlt2Combiner) :
    def __init__(self, name, decay, inputs):
        dc =    {}
        for child in ['pi+','K+','p+'] :
            dc[child] = "(PT > %(Trk_ALL_PT_MIN)s) & (MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)"
        cc =    ("(in_range( %(AM_MIN)s, AM, %(AM_MAX)s ))" +
                 " & ((APT1+APT2+APT3+APT4) > %(ASUMPT_MIN)s )" +
                 " & (AHASCHILD(PT > %(Trk_1OF4_PT_MIN)s))"+
                 " & (ANUM(PT > %(Trk_2OF4_PT_MIN)s) >= 2)"+
                 " & (AHASCHILD((MIPCHI2DV(PRIMARY)) > %(Trk_1OF4_MIPCHI2DV_MIN)s))"+
                 " & (ANUM(MIPCHI2DV(PRIMARY) > %(Trk_2OF4_MIPCHI2DV_MIN)s) >= 2)")
        mc =    ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > lcldira )" +
                 " & (BPVVDCHI2 > %(BPVVDCHI2_MIN)s )" +
                 " & (BPVLTIME() > %(BPVLTIME_MIN)s )")
        pream = [
                "import math"
                , "lcldira = math.cos( %(acosBPVDIRA_MAX)s )"
        ]


        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, "CharmHad" + name + "_" + type(self).__name__, decay, inputs,
                              nickname = name, dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              shared = True, tistos = 'TisTosSpec', DaughtersCuts = dc,
                              CombinationCut = cc, MotherCut = mc, Preambulo = pream)

## Shared instances of DetachedHHHHCombiner
## ------------------------------------------------------------------------- ##
Xic02PKKPi = DetachedHHHHCombiner( 'Xic0ToPpKmKmPipTurbo'
        , decay = "[Xi_c0 -> p+ K- K- pi+]cc"
        , inputs = [ SharedPromptChild_K, SharedPromptChild_pi,
                     SharedTighterPromptChild_p ] )


##  a combiner intended for P_c --> phi,p,pi  (a charmed pentaquark, as suggested by Lipkin;
##  see http://link.springer.com/article/10.1007%2Fs002180050168)
class PentaCombiner(Hlt2Combiner):
    """
    Combiner for exclusive D0->hhhh lines, designed for the case where the first two
    tracks have a low invariant mass

    Configuration dictionaries must contain the following keys:
        'AM_MAX'        : Maximum mass of the n-body combination
        'AM12_MAX'      : Maximum mass of the first two tracks (good for phi --> KK cands)
        'AM_4'          : Mass of the last particle
        'ACHI2DOCA_MAX' : Maximum Doca-chi2 of each two-body combination (should be >= VCHI2NDOF)
        'ASUMPT_MIN'    : Lower limit on the sum of the PTs of the daughters
        'VCHI2PDOF_MAX' : Upper limit on VFASPF(VCHI2PDOF) in MotherCut
        'acosBPVDIRA_MAX' : Upper limit on acos(DIRA) wrt bestPV in MotherCut
        'BPVLTIME_MIN'  : Lower limit on Lifetime wrt bestPV in MotherCut'
        'PT_MIN'        : lower limit on Mother PT
        'IPCHI2_MAX'    : max Mother IPCHI2; can restrict to only "prompt" candidates
        'TisTosSpec'    : Configuration string of the Hlt1 TISTOS filter.
    """
    def __init__(self, name, decay, inputs, shared = False, nickname = None):
        dc =  {}
        for child in ['pi+','K+','p+'] :
            dc[child] = "(PT > %(Trk_ALL_PT_MIN)s) & (MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)"
        c12 = (" ( AM < (%(AM12_MAX)s) ) " +
               "&( ACHI2DOCA(1,2) < %(ACHI2DOCA_MAX)s ) " )
        c123 =(" ( AM < (%(AM_MAX)s - %(AM_4)s) ) " +
               "&( ACHI2DOCA(1,3) < %(ACHI2DOCA_MAX)s ) " +
               "&( ACHI2DOCA(2,3) < %(ACHI2DOCA_MAX)s ) " )
        cc =  (" (in_range( %(AM_MIN)s, AM, %(AM_MAX)s )) " +
               "&( (APT1+APT2+APT3+APT4) > %(ASUMPT_MIN)s )" +
               "&( ACHI2DOCA(1,4) < %(ACHI2DOCA_MAX)s ) " +
               "&( ACHI2DOCA(2,4) < %(ACHI2DOCA_MAX)s ) " +
               "&( ACHI2DOCA(3,4) < %(ACHI2DOCA_MAX)s ) " )
##  the mother cuts do not explicitly include a specific mass range
##  this will be specified in D2HHHHLines.py where a MassFilter is applied
        mc =    ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > lcldira )" +
                 " & (BPVLTIME() > %(BPVLTIME_MIN)s )" +
                 " & (PT> %(PT_MIN)s)" +
                 " & (BPVIPCHI2() < %(IPCHI2_MAX)s )" )
        pream = [
                "import math"
                , "lcldira = math.cos( %(acosBPVDIRA_MAX)s )"
        ]

        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs, shared = shared,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec', combiner = DaVinci__N4BodyDecays, DaughtersCuts = dc,
                              Combination12Cut = c12, Combination123Cut = c123, CombinationCut = cc,
                              MotherCut = mc, Preambulo = pream, nickname = nickname)

PentaPhiPimPp = PentaCombiner('CharmHadPentaPhiPimPpComb'
                , decay = "[Xi_c0 -> K- K+  p+ pi-]cc"
                , inputs = [ SharedDetachedLcChild_K
                             , SharedTighterDetachedLcChild_p
                             , SharedDetachedLcChild_pi]
                , nickname = 'PentaPhiPimPp'    ## def in Hc2HHHHLines.py
                , shared = True )


##  a combiner intended for D --> 5 hadrons
class D2HHHHHCombiner(Hlt2Combiner):
    """
    Combiner for exclusive prompt D0->hhhhh lines,

    Configuration dictionaries must contain the following keys:
        'AM_MIN'        : Minimum mass of the n-body combination
        'AM_MAX'        : Maximum mass of the n-body combination
        'AM_3'          : Mass of the 3rd daughter
        'AM_4'          : Mass of the 4th daughter
        'AM_5'          : Mass of the 5th daughter
        'ACHI2DOCA_MAX' : Maximum Doca-chi2 of each two-body combination (should be >= VCHI2NDOF)
        'ASUMPT_MIN'    : Lower limit on the sum of the PTs of the daughters
        'VCHI2PDOF_MAX' : Upper limit on VFASPF(VCHI2PDOF) in MotherCut
        'acosBPVDIRA_MAX' : Upper limit on acos(DIRA) wrt bestPV in MotherCut
        'BPVLTIME_MIN'  : Lower limit on Lifetime wrt bestPV in MotherCut'
        'PT_MIN'        : lower limit on Mother PT
        'IPCHI2_MAX'    : max Mother IPCHI2; can restrict to only "prompt" candidates
        'TisTosSpec'    : Configuration string of the Hlt1 TISTOS filter.
    """
    def __init__(self, name, decay,inputs, shared = False):
        dc =  {}
        for child in ['pi+','K+','p+'] :
            dc[child] = "(PT > %(Trk_ALL_PT_MIN)s) & (MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)"
        c12 = (" ( AM < (%(AM_MAX)s - %(AM_3)s - %(AM_4)s - %(AM_5)s) ) " +
               "&( ACHI2DOCA(1,2) < %(ACHI2DOCA_MAX)s ) " )
        c123 =(" ( AM < (%(AM_MAX)s - %(AM_4)s - %(AM_5)s) ) " +
               "&( ACHI2DOCA(1,3) < %(ACHI2DOCA_MAX)s ) " +
               "&( ACHI2DOCA(2,3) < %(ACHI2DOCA_MAX)s ) " )
        c1234 = (" ( AM < (%(AM_MAX)s - %(AM_5)s) ) " +
               "&( ACHI2DOCA(1,4) < %(ACHI2DOCA_MAX)s ) " +
               "&( ACHI2DOCA(2,4) < %(ACHI2DOCA_MAX)s ) " +
               "&( ACHI2DOCA(3,4) < %(ACHI2DOCA_MAX)s ) " )
        cc =  (" (in_range( %(AM_MIN)s, AM, %(AM_MAX)s )) " +
               "&( APT  > %(ASUMPT_MIN)s )" +
               "&( ACHI2DOCA(1,5) < %(ACHI2DOCA_MAX)s ) " +
               "&( ACHI2DOCA(2,5) < %(ACHI2DOCA_MAX)s ) " +
               "&( ACHI2DOCA(3,5) < %(ACHI2DOCA_MAX)s ) " +
               "&( ACHI2DOCA(4,5) < %(ACHI2DOCA_MAX)s ) " )
##  the mother cuts do not explicitly include a specific mass range
##  this will be specified in D2HHHHLines.py where a MassFilter is applied
        mc =    ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > lcldira )" +
                 " & (BPVLTIME() > %(BPVLTIME_MIN)s )" +
                 " & (PT> %(PT_MIN)s)" +
                 " & (BPVIPCHI2() < %(IPCHI2_MAX)s )" )
        pream = [
                "import math"
                , "lcldira = math.cos( %(acosBPVDIRA_MAX)s )"
        ]

        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs, shared = shared,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec', combiner = DaVinci__N5BodyDecays, DaughtersCuts = dc,
                              Combination12Cut = c12, Combination123Cut = c123,
                              Combination1234Cut = c1234, CombinationCut = cc,
                              MotherCut = mc, Preambulo = pream )
##  The DchXXXX lines are intended to cover both the D+ and D_s mass ranges.
##  Separate mass, decay time, PT cuts can be applied elsewhere (D2HHHHHLines.py)
##  to create disjoint or overlapping subsamples. The comments for each line
##  refer to the D+ decay as the original use case is find D+ --> K,K,K,pi,pi
##  for a high-precision mass measurement and the other two are there for
##  comparison and completeness.

## The CF line with 4 pions can probably be pre-scaled, perhaps a factor of 10
Dch2KmPimPipPipPip = D2HHHHHCombiner('Dch2KmPimPipPipPip', decay = "[D+ -> K- pi- pi+ pi+ pi+]cc",
                inputs=[SharedDetachedDpmChild_K,SharedDetachedDpmChild_pi],
                shared = True)
##  We might want to keep all the SCS candidates for a BF measurement, and possibly
##  an amplitude analysis
Dch2KmKpPimPipPip = D2HHHHHCombiner('Dch2KmKpPimPipPip', decay = "[D+ -> K- K+ pi- pi+ pi+]cc",
                inputs=[SharedDetachedDpmChild_K,SharedDetachedDpmChild_pi],
                shared = True)
## The CF decay with 3 kaons has a very small Q-value, so is interesting for
## for a mass measurement relatively robust against momentum scale systematics
## We looked for this channel in BaBar data, and did not see it
## With three kaons in the final state, and limited phase-space, the trigger rate
## should be small.
Dch2KmKmKpPipPip = D2HHHHHCombiner('Dch2KmKmKpPipPip', decay = "[D+ -> K- K- K+ pi+ pi+]cc",
                inputs=[SharedDetachedDpmChild_K,SharedDetachedDpmChild_pi],
                shared = True)

LcpToPpKmKpPimPip = D2HHHHHCombiner('LcpToPpKmKpPimPip',
                decay = "[Lambda_c+ -> p+ K- K+ pi- pi+]cc",
                inputs=[SharedTighterDetachedLcChild_p,
                 SharedDetachedLcChild_K,SharedDetachedLcChild_pi],
                 shared = True)
##  The following five lines added 150825 mds
LcpToPpKmPimPipPip = D2HHHHHCombiner('LcpToPpKmPimPipPip',
                decay = "[Lambda_c+ -> p+ K- pi- pi+ pi+]cc",
                inputs=[SharedTighterDetachedLcChild_p,
                 SharedDetachedLcChild_K,SharedDetachedLcChild_pi],
                 shared = True)
LcpToPpPimPimPipPip = D2HHHHHCombiner('LcpToPpPimPimPipPip',
                decay = "[Lambda_c+ -> p+ pi- pi- pi+ pi+]cc",
                inputs=[SharedTighterDetachedLcChild_p,
                 SharedDetachedLcChild_pi],
                 shared = True)
LcpToPpKpPimPimPip = D2HHHHHCombiner('LcpToPpKpPimPimPip',
                decay = "[Lambda_c+ -> p+ K+ pi- pi- pi+]cc",
                inputs=[SharedTighterDetachedLcChild_p,
                 SharedDetachedLcChild_K,SharedDetachedLcChild_pi],
                 shared = True)
LcpToPpKpKpPimPim = D2HHHHHCombiner('LcpToPpKpKpPimPim',
                decay = "[Lambda_c+ -> p+ K+ K+ pi- pi-]cc",
                inputs=[SharedTighterDetachedLcChild_p,
                 SharedDetachedLcChild_K,SharedDetachedLcChild_pi],
                 shared = True)
LcpToPpKmKmPipPip = D2HHHHHCombiner('LcpToPpKmKmPipPip',
                decay = "[Lambda_c+ -> p+ K- K- pi+ pi+]cc",
                inputs=[SharedTighterDetachedLcChild_p,
                 SharedDetachedLcChild_K,SharedDetachedLcChild_pi],
                 shared = True)

class DetachedV0HCombiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs, shared = False, nickname = None) :
        dc =    {'KS0'    : "ALL",
                 'Lambda0' : "ALL",
                 'pi+'    : "(MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)",
                 'K+'     : "(MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)"}
        cc =    ("(in_range( %(AM_MIN)s, AM, %(AM_MAX)s ))" +
                 " & ((APT1+APT2) > %(ASUMPT_MIN)s )" )
        mc =    ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > lcldira )" +
                 " & (BPVVDCHI2 > %(BPVVDCHI2_MIN)s )" +
                 " & (BPVLTIME() > %(BPVLTIME_MIN)s )")
        pream = [
                "import math"
                , "lcldira = math.cos( %(acosBPVDIRA_MAX)s )"
        ]

        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs, shared = shared,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec', DaughtersCuts = dc, CombinationCut = cc,
                              MotherCut = mc, Preambulo = pream, nickname = nickname)

class ChargedHyperonLambdaHCombiner(Hlt2Combiner):
    def __init__(self, name, decay,inputs):
        dc =    {'Lambda0' : "ALL",
                 'pi+'    : "(MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)",
                 'K+'     : "(MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)"}
        cc =    ("(in_range( %(AM_MIN)s, AM, %(AM_MAX)s ))" +
                 " & ((APT1+APT2+APT3) > %(ASUMPT_MIN)s )" )
        mc =    ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > lcldira )" +
                 " & (BPVVDCHI2 > %(BPVVDCHI2_MIN)s )" +
                 " & (BPVLTIME() > %(BPVLTIME_MIN)s )")
        pream = [
                "import math"
                , "lcldira = math.cos( %(acosBPVDIRA_MAX)s )"
        ]

        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = [], DaughtersCuts = dc, CombinationCut = cc,
                              MotherCut = mc, Preambulo = pream)

## instantiate some charged hyperon combiners for specific final states

Xi2LambdaPi_LLL =   ChargedHyperonLambdaHCombiner('Ximinus2LambdaPi_LLL',
                     decay="[Xi- -> Lambda0 pi-]cc",
                     inputs=[CharmHadSharedSecondaryLambdaLL,SharedDetachedDpmChild_pi])
Xi2LambdaPi_DDL =   ChargedHyperonLambdaHCombiner('Ximinus2LambdaPi_DDL',
                     decay="[Xi- -> Lambda0 pi-]cc",
                     inputs=[CharmHadSharedSecondaryLambdaDD,SharedDetachedDpmChild_pi])
Omega2LambdaK_LLL = ChargedHyperonLambdaHCombiner('Omegaminus2LambdaK_LLL',
                     decay="[Omega- -> Lambda0 K-]cc",
                     inputs=[CharmHadSharedSecondaryLambdaLL,SharedDetachedDpmChild_K])
Omega2LambdaK_DDL = ChargedHyperonLambdaHCombiner('Omegaminus2LambdaK_DDL',
                     decay="[Omega- -> Lambda0 K-]cc",
                     inputs=[CharmHadSharedSecondaryLambdaDD,SharedDetachedDpmChild_K])
##  added 150715  mds
from Inputs import Hlt2DownPions, Hlt2DownKaons
Xi2LambdaPi_DDD =   ChargedHyperonLambdaHCombiner('Ximinus2LambdaPi_DDD',
                     decay="[Xi- -> Lambda0 pi-]cc",
                     inputs=[CharmHadSharedSecondaryLambdaDD,Hlt2DownPions])
Omega2LambdaK_DDD = ChargedHyperonLambdaHCombiner('Omegaminus2LambdaK_DDD',
                     decay="[Omega- -> Lambda0 K-]cc",
                     inputs=[CharmHadSharedSecondaryLambdaDD,Hlt2DownKaons])
##   ------------  end of code for Charged Hyperons
class DetachedV0V0Combiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs, lldd = False, shared = False, nickname = None):
        dc =    {'KS0'    : ( "(PT > %(KS0_ALL_PT_MIN)s) &"+
                              "(MIPCHI2DV(PRIMARY) > %(KS0_ALL_MIPCHI2DV_MIN)s)" )
                }
        cc =    ("(in_range( %(AM_MIN)s, AM, %(AM_MAX)s ))" +
                 " & ((APT1+APT2) > %(ASUMPT_MIN)s )" )
        mc =    ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > lcldira )" +
                 " & (BPVVDCHI2 > %(BPVVDCHI2_MIN)s )" +
                 " & (BPVLTIME() > %(BPVLTIME_MIN)s )")
        if lldd == True :
            mc = "(INTREE((ABSID=='pi+') & (ISLONG)) & INTREE((ABSID=='pi+') & (ISDOWN))) & " + mc
        pream = [
                "import math"
                , "lcldira = math.cos( %(acosBPVDIRA_MAX)s )"
        ]

        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs, shared = shared,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec', DaughtersCuts = dc, CombinationCut = cc,
                              MotherCut = mc, Preambulo = pream, nickname = nickname)

## add a variation on the DetachedV0V0Combiner meant for prompt H-Dibaryon --> Lambda,Lambda
## no minimum mass cut is required, so don't waste time with this comparison
## also, no need/benefit for TOS, so remove tistos specification
##
##  more generally, a H above Lambda,Lambda threshold should decay "promptly",
##  so the Lambdas should point to the PV, not away, and the "decay vertex"
##  should be consistent with the PV. Change many _MIN cuts to _MAX cuts
##

class H2LambdaLambdaCombiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs, lldd = False, shared = False, nickname = None):
        dc =    {'Lambda0'    : ( "(PT > %(Lam_ALL_PT_MIN)s) &"+
                              "(MIPCHI2DV(PRIMARY) < %(Lam_ALL_MIPCHI2DV_MAX)s)" )
                }
        cc =    ("( AM < %(AM_MAX)s )" +
                 " & ((APT1+APT2) > %(ASUMPT_MIN)s )" )
        mc =    ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +
                 " & (BPVVDCHI2 < %(BPVVDCHI2_MAX)s )" +
                 " & (BPVLTIME() > %(BPVLTIME_MIN)s )" +
                 " & (BPVLTIME() < %(BPVLTIME_MAX)s )"
                )
        if lldd == True :
            mc = "(INTREE((ABSID=='pi+') & (ISLONG)) & INTREE((ABSID=='pi+') & (ISDOWN))) & " + mc
        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs, shared = shared,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              DaughtersCuts = dc, CombinationCut = cc,
                              MotherCut = mc, Preambulo = [], nickname = nickname)


## Lifetime unbiased combiner class for D0 -> h h' decays
class D02HHCombiner(Hlt2Combiner) : # {
    def __init__(self, name, decay, inputs, nickname = None, shared = False, **kwargs) : # {
        '''**kwargs can be anything accepted by the Hlt2Combiner constructor, eg, to enable PV refitting use
        ReFitPVs = True.'''

        incuts = "(TRCHI2DOF< %(Trk_ALL_TRCHI2DOF_MAX)s )" \
                  "& (PT> %(Trk_ALL_PT_MIN)s)" \
                  "& (P> %(Trk_ALL_P_MIN)s)"

        dc = {   'pi+' : incuts
               , 'K+' : incuts
             }

        ## Assume that the wide mass range is wider than the narrow range.
        combcuts = "in_range(%(AM_MIN)s,  AM, %(AM_MAX)s)" \
                   "& ((APT1 > %(Trk_Max_APT_MIN)s) " \
                       "| (APT2 > %(Trk_Max_APT_MIN)s))" \
                   "& (APT > %(D0_PT_MIN)s)" \
                   "& (AMINDOCA('') " \
                       "< %(Pair_AMINDOCA_MAX)s )"

        parentcuts = "(VFASPF(VCHI2PDOF) < %(D0_VCHI2PDOF_MAX)s)" \
                     "& (BPVDIRA > lcldira )" \
                     "& (BPVLTIME() > %(D0_BPVLTIME_MIN)s )"

        pream = [
                "import math"
                , "lcldira = math.cos( %(D0_acosBPVDIRA_MAX)s )"
        ]

        from HltTracking.HltPVs import PV3D
        ## The name passed to the base class constructor determines the
        ##   configuration dictionary that is picked up.
        name = name if not shared else 'CharmHad' + name
        Hlt2Combiner.__init__( self, name, decay, inputs,
                               dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                               tistos = 'TisTosSpec',
                               nickname = nickname,
                               shared = shared,
                               DaughtersCuts = dc,
                               CombinationCut = combcuts,
                               MotherCut = parentcuts,
                               Preambulo = pream,
                               **kwargs)

## Shared instances of D02HHCombiner for LTUnb lines
## ------------------------------------------------------------------------- ##


D02HH_D0ToKmPip_LTUNB  = D02HHCombiner( 'D02HH_D0ToKmPip_LTUNB'
        , decay = "[D0 -> K- pi+]cc"
        , inputs = [ SharedPromptChild_K,SharedPromptChild_pi]
        , nickname = 'D02HH_LTUNB'            ## def in D02HHLines.py
        , shared = True
        , ReFitPVs = True )
D02HH_D0ToKmKp_LTUNB   = D02HHCombiner( 'D02HH_D0ToKmKp_LTUNB'
        , decay = "D0 -> K- K+"         ## Only D0s to prevent duplication
        , inputs = [ SharedPromptChild_K]
        , nickname = 'D02HH_LTUNB'            ## def in D02HHLines.py
        , shared = True
        , ReFitPVs = True )
D02HH_D0ToPimPip_LTUNB = D02HHCombiner( 'D02HH_D0ToPimPip_LTUNB'
        , decay = "D0 -> pi- pi+"       ## Only D0s to prevent duplication
        , inputs = [ SharedPromptChild_pi]
        , nickname = 'D02HH_LTUNB'            ## def in D02HHLines.py
        , shared = True
        , ReFitPVs = True )

class DetachedHHChildCombiner(Hlt2Combiner):
    # combiner for 2-body displaced tracks to be used in multi-body D decays
    def __init__(self, name, decay, inputs, daucuts = {}, shared = False, nickname = None):
        dc = daucuts
        cc = (" ( AM < %(AM_MAX)s ) " +
              "&( (APT1+APT2) > %(ASUMPT_MIN)s )" +
              "&( ACUTDOCA( %(ADOCA_MAX)s,'LoKi::TrgDistanceCalculator' ) )" +
              "&( AALLSAMEBPV )" )
        mc = (" ( VFASPF(VCHI2)<%(VCHI2_MAX)s ) " +
              "&( BPVVD > %(BPVVD_MIN)s )" +
              "&( BPVCORRM < %(BPVCORRM_MAX)s )" +
              "&( BPVVDCHI2 > %(BPVVDCHI2_MIN)s )")

        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs, shared = shared,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              #tistos = 'TisTosSpec',
                              DaughtersCuts = dc, CombinationCut = cc,
                              nickname = nickname,
                              MotherCut = mc, Preambulo = [])

D2HHPi0_PiPi = DetachedHHChildCombiner('CharmHadD2HHPi0_PiPi'
                        , decay=["K*(892)0 -> pi+ pi-"]
                        , inputs = [SharedNoPIDDetachedChild_pi]
                        , nickname = 'D02HHPi0_HH' ## def in D2HHPi0Lines.py
                        , shared = True)

D2HHPi0_KPi = DetachedHHChildCombiner('CharmHadD2HHPi0_KPi'
                        , decay=["[K*(892)0 -> pi+ K-]cc"]
                        , inputs = [SharedNoPIDDetachedChild_pi,
                                    SharedNoPIDDetachedChild_K]
                        , nickname = 'D02HHPi0_HH' ## def in D2HHPi0Lines.py
                        , shared = True)

D2HHPi0_KK = DetachedHHChildCombiner('CharmHadD2HHPi0_KK'
                        , decay=["K*(892)0 -> K+ K-"]
                        , inputs = [SharedNoPIDDetachedChild_K]
                        , nickname = 'D02HHPi0_HH' ## def in D2HHPi0Lines.py
                        , shared = True)


class DetachedHHHChildCombiner(Hlt2Combiner):
    """
    Combiner for 3-body displaced candidates to be used in multi-body D decays.

    Configuration dictionaries must contain the following keys:
        'AM_MIN'                 : Minimum mass of the 3-body combination
        'AM_MAX'                 : Maximum mass of the 3-body combination
        'ASUMPT_MIN'             : Lower limit on the sum of the PTs of the daughters
        'Trk_1OF3_MIPCHI2DV_MIN' : Minimum IPChi2 of at least one daughters
        'Trk_2OF3_MIPCHI2DV_MIN' : Minimum IPChi2 of at least two daughters
        'Trk_ALL_MIPCHI2DV_MIN'  : Minimum IPChi2 of the daughters (for PIDCalib line)
        'VCHI2PDOF_MAX'          : Upper limit on VFASPF(VCHI2PDOF) in MotherCut
        'acosBPVDIRA_MAX'        : Upper limit on acos(DIRA) wrt bestPV in MotherCut
        'BPVVDCHI2_MIN'          : Lower limit on vertex distance chi2 wrt bestPV in MotherCut
        'BPVLTIME_MIN'           : Lower limit on Lifetime wrt bestPV in MotherCut
        'BPVCORRM_MAX'           : Upper limit on the corrected mass wrt bestPV in MotherCut
        'TisTosSpec'             : Configuration string of the Hlt1 TISTOS filter.
    """
    def __init__(self, name, decay, inputs, nickname = None) :
        if name.find('PIDCALIB')>-1 :
            dc =    {'pi+' : "(MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)",
                     'K+'  : "(MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)",
                     'p+'  : "(MIPCHI2DV(PRIMARY) > %(Trk_ALL_MIPCHI2DV_MIN)s)"}
        else :
            dc =    {}
        cc =    ("(in_range( %(AM_MIN)s, AM, %(AM_MAX)s ))" +
                 " & ((APT1+APT2+APT3) > %(ASUMPT_MIN)s )" +
                 " & (AHASCHILD((MIPCHI2DV(PRIMARY)) > %(Trk_1OF3_MIPCHI2DV_MIN)s))"+
                 " & (ANUM(MIPCHI2DV(PRIMARY) > %(Trk_2OF3_MIPCHI2DV_MIN)s) >= 2)")
        mc =    ("(VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > lcldira )" +
                 " & (BPVVDCHI2 > %(BPVVDCHI2_MIN)s )" +
                 " & (BPVLTIME() > %(BPVLTIME_MIN)s )"+
                 " & (BPVCORRM < %(BPVCORRM_MAX)s)")
        pream = [
                "import math"
                , "lcldira = math.cos( %(acosBPVDIRA_MAX)s )"
        ]

        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec', DaughtersCuts = dc, CombinationCut = cc,
                              MotherCut = mc, Preambulo = pream, nickname = nickname)


# 2-body combiner to attach a neutral particle (child no. 2) to another candidate (child no. 1)
class AttachParticle(Hlt2Combiner):
    def __init__(self, name, decay,inputs,nickname = None):
        dc =    {}

        cc =    ("  ( in_range( %(AM_MIN)s, AM, %(AM_MAX)s ) " +
                 "& ( APT > %(APT_MIN)s ) " +
                 "& ( (ACHILD(PT,1) > %(ADAU1PT_MIN)s) & (ACHILD(BPVIPCHI2(),1) > %(ADAU1BPVIPCHI2_MIN)s) ) " +
                 "& ( (ACHILD(PT,2) > %(ADAU2PT_MIN)s) ) )"
                 )
        mc =    ("   ( in_range( %(DMASS_MIN)s, M, %(DMASS_MAX)s ) " +
                 " & ( BPVIPCHI2()< %(BPVIPCHI2_MAX)s )" +
                 " & (BPVDIRA > lcldira )" +
                 " & ( VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +
                 " & ( BPVLTIME() > %(BPVLTIME_MIN)s ) )")
        pream = [
                "import math"
                , "lcldira = math.cos( %(acosBPVDIRA_MAX)s )"
        ]

        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name,
                              decay, inputs,
                              nickname = nickname ,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec', DaughtersCuts = dc, CombinationCut = cc,
                              MotherCut = mc, Preambulo = pream)


class D2HHHKsCombiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs):
        dc = {}
        cc = (" (in_range( %(AM_MIN)s, AM, %(AM_MAX)s )) " )
        mc = (" (VFASPF(VCHI2PDOF)< %(VCHI2PDOF_MAX)s )" +
              "&(PT> %(PT_MIN)s)" +
              "&(BPVLTIME() > %(BPVLTIME_MIN)s)" +
              "&(BPVIPCHI2() < %(BPVIPCHI2_MAX)s )"+
              "& (BPVDIRA > lcldira )" )

        pream = [
                "import math"
                , "lcldira = math.cos( %(acosBPVDIRA_MAX)s )"
        ]

        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec', DaughtersCuts = dc, CombinationCut = cc,
                              MotherCut = mc, Preambulo = pream)

# D2HH combiners

class D2HH0_ee_Combiner(Hlt2Combiner):
    def __init__(self, name, shared = False, nickname = None):
        e_daughter_cuts  = ("( PT > %(Elec_PT)s ) &" +
                 "(TRCHI2DOF< %(Track_Chi2)s ) &"+
                 "(MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s) ")
        daug_cuts = { 'e+' : e_daughter_cuts,
                      'e-' : e_daughter_cuts }

        from HltTracking.HltPVs import PV3D
        from Inputs import Hlt2NoPIDsElectrons
        inputs = [ Hlt2NoPIDsElectrons ]
        Hlt2Combiner.__init__(self, name, "KS0 -> e+ e-", inputs,
                              dependencies = [ PV3D('Hlt2')],tistos = 'TisTosSpec',
                              DaughtersCuts = daug_cuts, CombinationCut =  "AALL",
                              MotherCut = "ALL", Preambulo = [],
                              shared = shared, nickname = nickname)


## Shared instances of D2HH0_ee_Combiner
## ------------------------------------------------------------------------- ##
Conv_Photon_2EmEp = D2HH0_ee_Combiner( 'CharmHadGToEmEp_Conv'
        , shared = True, nickname = 'Conv_Photon' )      ## def in D2HHLines.py


## ------------------------------------------------------------------------- ##
class D2HH0_3Body_Combiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs, shared = False, nickname = None) :
        pi_daughters_cut    = ("( PT > %(Daug_PT)s ) &" +
                               "( P > %(Daug_P)s ) &" +
                               "(TRCHI2DOF< %(Track_Chi2)s ) &" +
                               "(MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s) ")
        K_daughters_cut     = ("( PT > %(Daug_PT)s ) &" +
                               "( P  >  %(Daug_P)s ) &" +
                               "(TRCHI2DOF< %(Track_Chi2)s ) &" +
                               "(MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s) ")
        gamma_daughters_cut = ("( PT > %(Gamma_PT)s  ) &" +
                               "( P  > %(Gamma_P)s   )")
        e_daughters_cut     = ("( PT > %(Daug_PT)s ) &" +
                               "( P  >  %(Daug_P)s ) &" +
                               "(TRCHI2DOF< %(Track_Chi2)s ) &"+
                               "(MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s) ")
        daug_cuts =    {'KS0'   : "ALL",
                        'pi+'   : pi_daughters_cut,
                        'K+'    : K_daughters_cut,
                        'gamma' : gamma_daughters_cut,
                        'e+'    : e_daughters_cut }

        D_combination_cut   = ("(AM>%(D_MassWinLow)s) &" +
                               "(AM<%(D_MassWinHigh)s)")

        neutral_combination_cut = ("(AM>%(Neutral_MassWinLow)s) &" +
                               "(AM<%(Neutral_MassWinHigh)s) &" +
                               "(APT > %(Neutral_PT)s)")

        D_mother_cut   = ("( (PT > %(D_PT)s ) &" +
                          "(VFASPF(VCHI2/VDOF)< %(Vertex_Chi2)s ) )")

        from HltTracking.HltPVs import PV3D
        from Configurables import DaVinci__N3BodyDecays

        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [ PV3D('Hlt2')], tistos = 'TisTosSpec',
                              combiner = DaVinci__N3BodyDecays, DaughtersCuts = daug_cuts,
                              CombinationCut =  D_combination_cut, MotherCut = D_mother_cut,
                              Combination12Cut = neutral_combination_cut, Preambulo = [],
                              shared = shared, nickname = nickname )


class D02_ee_Combiner(Hlt2Combiner):
    def __init__(self, name):
        e_daughter_cuts  = ("( PT > %(Elec_PT)s ) &" +
                 "(TRCHI2DOF< %(Track_Chi2)s ) &"+
                 "(MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s) ")

        daug_cuts = { 'e+' : e_daughter_cuts,
                      'e-' : e_daughter_cuts }

        D0_combination_cut   = ("(AM>%(D0_AM_MassWinLow)s) &" +
                               "(AM<%(D0_AM_MassWinHigh)s)")

        D0_mother_cut   = ("( (PT > %(D0_PT)s ) &" +
                          "(VFASPF(VCHI2/VDOF)< %(Vertex_Chi2)s ) ) &"+
                          "(M>%(D0_MassWinLow)s) &" +
                          "(M<%(D0_MassWinHigh)s)")

        from HltTracking.HltPVs import PV3D
        from Inputs import Hlt2NoPIDsElectrons
        inputs = [ Hlt2NoPIDsElectrons ]
        Hlt2Combiner.__init__(self, name, "D0 -> e+ e-", inputs,
                              dependencies = [ PV3D('Hlt2')],tistos = 'TisTosSpec',
                              DaughtersCuts = daug_cuts, CombinationCut =  D0_combination_cut,
                              MotherCut = D0_mother_cut, Preambulo = [])

class D0_gg_NeutralCombiner(Hlt2Combiner):
    """ Combines converted photon ('KS0') with another photon  """
    def __init__(self, name):
        gamma_daughters_cut = ("( PT > %(Gamma_PT)s  ) &" +
                               "( P  > %(Gamma_P)s   )")

        daug_cuts = { 'gamma' : gamma_daughters_cut,
                      'KS0'    : "ALL"  }

        combination_cut = ("(AM>%(D0_AM_MassWinLow)s) &" +
                           "(AM<%(D0_AM_MassWinHigh)s)")

        mother_cut = ("(PT > %(D0_PT)s) & " +
                     "(M>%(D0_MassWinLow)s) &" +
                     "(M<%(D0_MassWinHigh)s)")

        from Inputs import Hlt2NoPIDsPhotons
        inputs = [ Hlt2NoPIDsPhotons, Conv_Photon_2EmEp]
        Hlt2Combiner.__init__(self, name,"D0 -> KS0 gamma ",
                              inputs,nickname=name, DaughtersCuts=daug_cuts,
                              CombinationCut=combination_cut, MotherCut=mother_cut,
                              ParticleCombiners={'':'ParticleAdder'})


# D2EtaH Combiners
class D2RhoHG_3Body_Combiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs):

        inPartCuts = ( "(PT> %(Trk_PT_MIN)s)" +
                       "& (MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s )" )

        neutPartCuts = ( "(PT> %(Neut_ALL_PT_MIN)s )" )

        rho_cuts = ( "(M>%(MRho_MIN)s)" +
                     "&  (M<%(MRho_MAX)s)" )


        daug_cuts =    {'rho(770)0'   : rho_cuts,
                        'pi+'   : inPartCuts,
                        'K+'    : inPartCuts,
                        'gamma' : neutPartCuts,
                        'pi0'   : neutPartCuts,
                        'eta'   : neutPartCuts,
                        }

        charged_combination_cut = ("(AM>%(AMRhoH_MIN)s) " +
                                   "& (AM<%(AMRhoH_MAX)s) " +
                                   "& ( ACUTDOCACHI2( %(DOCACHI2_MAX)s , '') ) "
                                   )

        D_combination_cut   = ("(AM>%(AM_MIN)s) &" +
                               "(AM<%(AM_MAX)s) &" +
                               "(APT > %(APT_MIN)s) &" +
                               "(AM13>%(AMEta_MIN)s) &" +
                               "(AM13<%(AMEta_MAX)s) " )

        D_mother_cut   = ("( in_range( %(DMASS_MIN)s, M, %(DMASS_MAX)s )" +
                          " & ( VFASPF(VCHI2PDOF) < %(VCHI2PDOF_MAX)s)" +
                          " & ( BPVLTIME() > %(BPVLTIME_MIN)s ) " +
                          " & ( M13>%(MEta_MIN)s) " +
                          " & ( M13<%(MEta_MAX)s) ) "
                          )

        from HltTracking.HltPVs import PV3D
        from Configurables import DaVinci__N3BodyDecays

        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec',
                              combiner = DaVinci__N3BodyDecays, DaughtersCuts = daug_cuts,
                              CombinationCut =  D_combination_cut,
                              MotherCut = D_mother_cut,
                              Combination12Cut = charged_combination_cut, Preambulo = [])


class DetachedRhoChild(Hlt2Combiner):
    def __init__(self, name, decay, inputs, shared = False):

        inPartCuts = ( "(PT> %(Trk_PT_MIN)s)" +
                       "& (MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s )"
                       "& ( PIDK < %(Trk_PIDK_MAX)s ) " )

        dc = {   'pi+' : inPartCuts
                 , 'K+' : inPartCuts
                              }

        cc = (" ( AM < %(AM_MAX)s ) " +
              "&( (APT1+APT2) > %(ASUMPT_MIN)s )" +
              "&( ACUTDOCA( %(ADOCA_MAX)s , '' ) )" +
              "&( AALLSAMEBPV )" )
        mc = (" ( VFASPF(VCHI2)<%(VCHI2_MAX)s ) " +
              "&( BPVVD > %(BPVVD_MIN)s )" +
              "&( BPVCORRM < %(BPVCORRM_MAX)s )" +
              "&( BPVVDCHI2 > %(BPVVDCHI2_MIN)s )")

        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__(self, name, decay, inputs,
                              dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                              tistos = 'TisTosSpec',
                              shared = shared,
                              DaughtersCuts = dc, CombinationCut = cc,
                              MotherCut = mc, Preambulo = [])

# Shared
D2HH_RhoToPipPim = DetachedRhoChild('DiPion_forD2HH'
                                    , decay=["rho(770)0 -> pi+ pi-"]
                                    , inputs = [Hlt2LoosePions]
                                    , shared = True )


# Hc->hhhh lines for lifetime measurement

class Xic02PKKPi_LTUNB(HHHHCombiner) :
    def __init__(self,name) :
        decay = "[Xi_c0 -> p+ K- K- pi+]cc"
        inputs = [SharedPromptChild_K,
                  SharedPromptChild_pi,
                  SharedTighterPromptChild_p]
        HHHHCombiner.__init__(self,name,decay,inputs)



##  The first argument must be a unique name as HHKshCombiner is "shared = True".
##  All these instances use the same nickname, 'D02HHKsh', to point to a single, common,
##  dictionary in Lines.py This is hidden in the HHKshCombiner class code.

## Note that all the CP eigenstate lines are defined as D0 decays, not D0bar.  This avoids "duplication"
## that would be produced using decay="[ xxx ]cc". The non-CP-eigenstate final states
## are made with DecayDescriptors of the form"[  xxx  ]cc"  rather than " xxx "


D02KsPiPi_LL = HHKshCombiner('KshPiPiLL', decay="D0 ->  pi- pi+ KS0",
                   inputs=[CharmHadSharedKsLL, SharedDetachedDpmChild_pi])
D02KsPiPi_DD = HHKshCombiner('KshPiPiDD', decay="D0 ->  pi- pi+ KS0",
                   inputs=[CharmHadSharedKsDD, SharedDetachedDpmChild_pi])
##
##  These lines produce D0 --> K-,pi+,Kshort and D0bar --> K+,pi-,Kshort candidates
##  The candidates can also be used for the charge conjugate decays by
##  using D0bar in place of D0, and vice versa, in decay descriptors.
##  this is done in D2HHKsLines.py
D02KsKPi_LL  = HHKshCombiner('KshKPiLL', decay="[D0 ->  K- pi+ KS0]cc",
                   inputs=[CharmHadSharedKsLL, SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi])
D02KsKPi_DD  = HHKshCombiner('KshKPiDD', decay="[D0 ->  K- pi+ KS0]cc",
                   inputs=[CharmHadSharedKsDD, SharedDetachedDpmChild_K, SharedDetachedDpmChild_pi])

D02KsKK_LL   = HHKshCombiner('KshKKLL', decay="D0 ->  K- K+ KS0",
                   inputs=[CharmHadSharedKsLL, SharedDetachedDpmChild_K])
D02KsKK_DD   = HHKshCombiner('KshKKDD', decay="D0 ->  K- K+ KS0",
                   inputs=[CharmHadSharedKsDD, SharedDetachedDpmChild_K])

## ------------------------------------------------------------------------- ##
##
##  now, create the "lifetime-unbiased" versions of these combiners
##  note that the first arguments here are the same as those for the
##  corresponding "generic" (not LTUNB) lines. These "names" correspond
##  to dictionaries which define mother mass cuts.
##  Question:  are they "superseded" by the nicknames in the combiner
##  class definition?  If so, they are probably irrelevant
D02KsPiPi_LL_LTUNB = LTUNB_HHKshCombiner('LTUNB_KshPiPiLL', decay="D0 ->  pi- pi+ KS0",
                   inputs=[CharmHadSharedKsLL, SharedPromptChild_pi])
D02KsPiPi_DD_LTUNB = LTUNB_HHKshCombiner('LTUNB_KshPiPiDD', decay="D0 ->  pi- pi+ KS0",
                   inputs=[CharmHadSharedKsDD, SharedPromptChild_pi])
D02KsKPi_LL_LTUNB  = LTUNB_HHKshCombiner('LTUNB_KshKPiLL', decay="[D0 ->  K- pi+ KS0]cc",
                   inputs=[CharmHadSharedKsLL, SharedPromptChild_K, SharedPromptChild_pi])
D02KsKPi_DD_LTUNB  = LTUNB_HHKshCombiner('LTUNB_KshKPiDD', decay="[D0 ->  K- pi+ KS0]cc",
                   inputs=[CharmHadSharedKsDD, SharedPromptChild_K, SharedPromptChild_pi])

D02KsKK_LL_LTUNB   = LTUNB_HHKshCombiner('LTUNB_KshKKLL', decay="D0 ->  K- K+ KS0",
                   inputs=[CharmHadSharedKsLL, SharedPromptChild_K])
D02KsKK_DD_LTUNB   = LTUNB_HHKshCombiner('LTUNB_KshKKDD', decay="D0 ->  K- K+ KS0",
                   inputs=[CharmHadSharedKsDD, SharedPromptChild_K])

## ------------------------------------------------------------------------- ##
class DetachedD02HHInclCombiner(Hlt2Combiner) : # {
    """
    Displaced combiner for inclusive D*+ line.

    Configuration dictionaries must contain the following keys:
        'Trk_TRCHI2DOF_MAX' : upper limit on TRCHI2DOF of each input particle
        'Trk_PT_MIN'        : lower limit on PT of each input particle
        'Trk_MIPCHI2DV_MIN' : lower limit on MIPCHI2DV(PRIMARY) of each input
        'D0_VCHI2PDOF_MAX'  : upper limit on VFASPF(VCHI2PDOF) in MotherCut
        'D0_BPVVDCHI2_MIN'  : lower limit on BPVVDCHI2 in MotherCut
        'D0_BPVCORRM_MAX'   : upper limit on BPVCORRM in MotherCut
        'TisTosSpec'        : configuration string of the Hlt1 TISTOS filter.

    It is expected that the cuts on input particles will eventually be removed
    when the inputs are made into shared instances of DetachedInParticleFilter.
    """
    def __init__(self, name, decay, inputs, nickname = None, shared = False) : # {

        inPartCuts = "(TRCHI2DOF< %(Trk_TRCHI2DOF_MAX)s )" \
                     "& (PT> %(Trk_PT_MIN)s)" \
                     "& (MIPCHI2DV(PRIMARY)> %(Trk_MIPCHI2DV_MIN)s )"

        dc = {   'pi+' : inPartCuts }

        combCut = "(APT > %(D0_comb_PT_MIN)s)" \
                  "& (AM < %(D0_BPVCORRM_MAX)s)" \
                  "& (ACUTDOCACHI2(%(D0_VCHI2PDOF_MAX)s, ''))"

        parentCut = "(VFASPF(VCHI2PDOF) < %(D0_VCHI2PDOF_MAX)s)" \
                    "& (BPVVDCHI2 > %(D0_BPVVDCHI2_MIN)s)" \
                    "& (BPVCORRM < %(D0_BPVCORRM_MAX)s)"

        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__( self, name, decay, inputs,
                               dependencies = [PV3D('Hlt2')],
                               tistos = 'TisTosSpec',
                               nickname = nickname,
                               shared = shared,
                               DaughtersCuts = dc,
                               CombinationCut = combCut,
                               MotherCut = parentCut,
                               Preambulo = [] )

    # }
# }

## Shared instances of DetachedD02HHInclCombiner
## ------------------------------------------------------------------------- ##
InclHc2HHX = DetachedD02HHInclCombiner( 'CharmHadInclHc2HHX'
        , decay = [    "D0 -> pi+ pi-"
                    ,  "D0 -> pi- pi-"
                    ,  "D0 -> pi+ pi+" ]

        , inputs = [ Hlt2NoPIDsPions ]
        , nickname = 'InclHc2HHX' ## 'InclHc2HHX' def in Dst2PiD02HHXBDTLines.py
        , shared = True )


## ------------------------------------------------------------------------- ##
class Dstp2D0PiInclCombiner(Hlt2Combiner) : # {
    """
    Combiner for inclusive D*+ line.

    Configuration dictionaries must contain the following keys:
        'Spi_TRCHI2DOF_MAX' : Upper limit of TRCHI2DOF on soft pion
        'Spi_PT_MIN'        : Lower limit of PT on soft pion
        'Dst_VCHI2PDOF_MAX' : Upper limit on VFASPF(VCHI2PDOF) in MotherCut
        'Dst_PT_MIN'        : Lower limit on PT in MotherCut
        'Dst_M_MAX'         : Upper limit on M in MotherCut
        'Dst_D0_DeltaM_MAX' : Upper limit on M - M1 in MotherCut

    It is expected that the cuts on input particles will eventually be removed
    when the inputs are made into shared instances of DetachedInParticleFilter.
    Use of a generic soft pion tagging combiner should be investigated.
    """
    def __init__(self, name, decay, inputs, nickname = None, shared = False) : # {

        inPartCuts = "(TRCHI2DOF< %(Spi_TRCHI2DOF_MAX)s )" \
                     "& (PT> %(Spi_PT_MIN)s)"

        dc = {  'pi+' : inPartCuts }

        combCut = "AALL"

        parentCut = "(VFASPF(VCHI2PDOF) < %(Dst_VCHI2PDOF_MAX)s)" \
                    "& (PT > %(Dst_PT_MIN)s)" \
                    "& (M < %(Dst_M_MAX)s)" \
                    "& (M - M1 < %(Dst_D0_DeltaM_MAX)s)" \

        Hlt2Combiner.__init__( self, name, decay, inputs,
                               #tistos = 'TisTosSpec',
                               nickname = nickname,
                               shared = shared,
                               DaughtersCuts = dc,
                               CombinationCut = combCut,
                               MotherCut = parentCut,
                               Preambulo = [] )

    # }
# }

## Shared instances of Dstp2D0PiInclCombiner
## ------------------------------------------------------------------------- ##
## 'InclHcst2PiHc2HHX' defined in Dst2PiD02HHXBDTLines.py
InclHcst2PiHc2HHX = Dstp2D0PiInclCombiner( 'CharmHadInclHcst2PiHc2HHX'
        , decay = [ "D*(2010)+ -> D0 pi+", "D*(2010)- -> D0 pi-" ]
        , inputs = [ Hlt2NoPIDsPions, InclHc2HHX ]
        , nickname = 'InclHcst2PiHc2HHX'
        , shared = True )



# prompt charm filter (really, prompt exclusive filter)
# designed for charmed hadrons being combined with bachelor
# tracks, also from the PV. See "PromptTrackFilter" below for bachelors.
## ------------------------------------------------------------------------- ##
class PromptSpectroscopyFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs, nickname = None, shared = False ):
        ipchi2_cut = "(BPVIPCHI2() < %(IPCHI2_MAX)s )"
        decaytime_cut = "(BPVLTIME() > %(D_BPVLTIME_MIN)s )"
        mass_cut = "(%(DMASS_MIN)s < M) & (M < %(DMASS_MAX)s)"
        pt_cut   = "(%(DPT_MIN)s < PT)"
        p_cut    = "(%(DMom_MIN)s < P)"
        cut = ipchi2_cut + " & " + decaytime_cut + " & " + mass_cut
        cut = cut + "&" + pt_cut + "&" + p_cut
        nickname = name if nickname == None else nickname
        name     = name if not shared       else 'CharmHad%sPromptSpectroscopy' % name
        Hlt2ParticleFilter.__init__(self, name, cut, inputs,
                                    nickname = nickname , shared = shared )
##
class DplusFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs, nickname = None, shared = False ):
        decaytime_cut = "(BPVLTIME() > %(D_BPVLTIME_MIN)s )"
        mass_cut = "(%(DMASS_MIN)s < M) & (M < %(DMASS_MAX)s)"
        pt_cut = "(PT > %(PT_MIN)s)"
        cut =  decaytime_cut + " & " + mass_cut + " & " + pt_cut
        nickname = name if nickname == None else nickname
        name     = name if not shared       else 'CharmHad%sDplusFiltered' % name
        Hlt2ParticleFilter.__init__(self, name, cut, inputs,
                                    nickname = nickname , shared = shared )
##

## ------------------------------------------------------------------------- ##
class PromptBachelorFilter(Hlt2ParticleFilter):
    def __init__(self, name, inputs, nickname = None, shared = False ):
        ipchi2_cut = "(BPVIPCHI2() < %(IPCHI2_MAX)s )"
        cut = ipchi2_cut
        nickname = name if nickname == None else nickname
        name     = name if not shared       else 'CharmHad%sPromptSpectroscopy' % name
        Hlt2ParticleFilter.__init__(self, name, cut, inputs,
                                    nickname = nickname , shared = shared )

## ------------------------------------------------------------------------- ##
class InclHcst2PiHc2HHXFilter( Hlt2ParticleFilter ) : # {
    """
    A filter for candidates of two-body decays intended for the use in
    slow-pion tagged inclusive lines.  The goal is provide some additional
    kinematic filtering to a general (h h') + pi+ reconstruction that might
    support several partially specialized inclusive lines.

    !!!NOTE!!! The implementation assumes that the 'heavy' (h h') combination
    is the first decay product and the slow pion the second decay product of
    the reconstruction that came before the filter.
        GOOD:      [Sigma_c0 -> Lambda_c+ pi-]cc
        NOT GOOD:  [Sigma_c0 -> pi- Lambda_c+]cc

    Configuration dictionaries must contain the following keys:
        'D0_BPVVDCHI2_MIN'  : lower limit on BPVVDCHI2 for CHILD 1
        'D0_BPVCORRM_MAX'   : upper limit on BPVCORRM for CHILD 1
        'Dst_M_MAX'         : Upper limit on M
        'Dst_D0_DeltaM_MAX' : Upper limit on M - M1
    """
    def __init__(self, name, inputs, nickname = None, shared = False ) : # {
        cut = "(M < %(Dst_M_MAX)s)" \
              "& (M - M1 < %(Dst_D0_DeltaM_MAX)s)" \
              "& (CHILDCUT( ( (BPVVDCHI2 > %(D0_BPVVDCHI2_MIN)s)" \
                " & (BPVCORRM < %(D0_BPVCORRM_MAX)s)) , 1 ) )"

        Hlt2ParticleFilter.__init__(self, name, cut, inputs,
                                    nickname = nickname , shared = shared )
    # }
# }


## ------------------------------------------------------------------------- ##
class BDTFilter( Hlt2ParticleFilter ) : # {
    """
    Filter with a BBDecTreeTool.

    The part of the configuration dictionary that configures the lookup
    table and filter threshold must be passed as constructor argument 'props'
    and must contain the following keys:
        'BDT_Lookup_Filename' : the name of the file that contains the lookup
                                table.  Assumed to be in $PARAMFILESROOT/data/.
        'BDT_Lookup_VarMap'   : the map of nickname:functor strings to define
                                the input variables of the lookup table.
        'BDT_Threshold'       : Minimum response value accepted by the filter.
    """
    def __init__( self, name, inputs, props ) : # {

        ## Configure the DictOfFunctors variable handler
        from HltLine.HltLine import Hlt1Tool
        from Configurables import LoKi__Hybrid__DictOfFunctors
        varHandler = Hlt1Tool( type = LoKi__Hybrid__DictOfFunctors
                               , name = 'VarHandler'
                               , Variables = props['BDT_Lookup_VarMap']
                             )
        from Configurables import BBDecTreeTool
        bdtTool = Hlt1Tool( type = BBDecTreeTool
                            , name = 'DstBBDT'
                            , Threshold = props['BDT_Threshold']
                            , ParamFile = props['BDT_Lookup_Filename']
                            , ParticleDictTool = "%s/%s" % (varHandler.Type.getType(), varHandler.Name )
                            , tools = [varHandler]
                          )
        filtCut = "FILTER('%s/%s')" % (bdtTool.Type.getType(), bdtTool.Name )
        ## The dependence on PV3D('Hlt2') is there assuming that PVs might
        ##   be needed for some of the input variables.
        from HltTracking.HltPVs import PV3D
        Hlt2ParticleFilter.__init__( self, name, filtCut, inputs
                                     , dependencies = [ PV3D('Hlt2') ]
                                     , tools = [bdtTool ] )
    # }
# }



## Combiner class for Xi_cc -> Hc H H decays
## ------------------------------------------------------------------------- ##
class Xicc2HcHHCombiner(Hlt2Combiner) : # {
    """
    Combiner for Xicc -> Hc + h h'.
    The selection variables are those used by the 2012 stripping for these
    decays.
    Like PromptSpectroscopyFilter, it relies of other 'standard' selections
    for Cabibbo-favored singly charmed hadrons as input and to provide
    selectivity.
    !!!NOTE!!! The implementation dowstream vertex requirement requires that
    the singly charmed hadron Hc is the first member of the decay descriptor.

    Configuration dictionaries must contain the following keys:
        'Trk_1of2_PT_MIN'      : Lower limit on PT for one of the 2 tracks
                                 produced in the Xicc decay.
        'Comb_AM_MAX'          : Upper limit on combination mass, AM
        'Comb_ACHI2DOCA_MAX'   : Upper limit on pairwise DOCACHI2 of all Xicc
                                 products, including the singly-charmed hadron.
        'Comb_APT_MIN'         : Lower limit on the combination APT
        'Xicc_VCHI2PDOF_MAX'   : Upper limit on Xicc decay vertex
        'Xicc_VZ1VZdiff_MIN'   : Minimum difference between Hc vertex VZ and
                                 Xicc vertex VZ.
        'Xicc_BPVVDCHI2_MIN'   : Minimum vertex displacement BPVVDCHI2
        'Xicc_acosBPVDIRA_MAX' :
        'TisTosSpec'        : configuration string of the Hlt1 TISTOS filter.
    """
    def __init__(self, name, decay, inputs, nickname = None, shared = False) : # {
        ## No DaughterCuts.  Assume that all filtering of input particles is
        ## already done.
        comb12 = "( AM < %(Comb_AM_MAX)s ) " \
                 "& ( ACHI2DOCA(1,2) < %(Comb_ACHI2DOCA_MAX)s ) "
        comb   = "(in_range( %(Comb_AM_MIN)s, AM, %(Comb_AM_MAX)s ))" \
                 "& ((APT2 > %(Trk_1of2_PT_MIN)s) | (APT3 > %(Trk_1of2_PT_MIN)s))" \
                 "& ( APT > %(Comb_APT_MIN)s )" \
                 "& ( ACHI2DOCA(1,3) < %(Comb_ACHI2DOCA_MAX)s ) " \
                 "& ( ACHI2DOCA(2,3) < %(Comb_ACHI2DOCA_MAX)s ) "

        parent = "(VFASPF(VCHI2PDOF) < %(Xicc_VCHI2PDOF_MAX)s)" \
                 "& (CHILD(VFASPF(VZ),1) - VFASPF(VZ) > %(Xicc_VZ1VZdiff_MIN)s)" \
                 "& (BPVVDCHI2 > %(Xicc_BPVVDCHI2_MIN)s )" \
                 "& (BPVDIRA > lcldira )"

        pream = [
                "import math"
                , "lcldira = math.cos( %(Xicc_acosBPVDIRA_MAX)s )"
        ]

        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__( self, name, decay, inputs,
                               dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                               tistos = 'TisTosSpec',
                               combiner = N3BodyDecays,
                               nickname = nickname,
                               shared = shared,
                               Combination12Cut =  comb12,
                               CombinationCut = comb,
                               MotherCut = parent,
                               Preambulo = pream)
    # }
# }


## Combiner class for Xi_cc -> Hc H H H decays
## ------------------------------------------------------------------------- ##
class Xicc2HcHHHCombiner(Hlt2Combiner) : # {
    """
    Combiner for Xicc -> Hc + h h' h''.
    The selection variables are those used by the 2012 stripping for these
    decays.
    Like PromptSpectroscopyFilter, it relies of other 'standard' selections
    for Cabibbo-favored singly charmed hadrons as input and to provide
    selectivity.
    !!!NOTE!!! The implementation dowstream vertex requirement requires that
    the singly charmed hadron Hc is the first member of the decay descriptor.

    Configuration dictionaries must contain the following keys:
        'Trk_2of3_PT_MIN'      :
        'Trk_1of3_PT_MIN'      : Lower limit on PT for one (or two) of the
                                 3 tracks produced in the Xicc decay.
        'Comb_AM_MAX'          : Upper limit on combination mass, AM
        'Comb_ACHI2DOCA_MAX'   : Upper limit on pairwise DOCACHI2 of all Xicc
                                 products, including the singly-charmed hadron.
        'Comb_APT_MIN'         : Lower limit on the combination APT
        'Xicc_VCHI2PDOF_MAX'   : Upper limit on Xicc decay vertex
        'Xicc_VZ1VZdiff_MIN'   : Minimum difference between Hc vertex VZ and
                                 Xicc vertex VZ.
        'Xicc_BPVVDCHI2_MIN'   : Minimum vertex displacement BPVVDCHI2
        'Xicc_acosBPVDIRA_MAX' :
        'TisTosSpec'        : configuration string of the Hlt1 TISTOS filter.
    """
    def __init__(self, name, decay, inputs, nickname = None, shared = False) : # {
        ## No DaughterCuts.  Assume that all filtering of input particles is
        ## already done.
        comb12  = "( AM < %(Comb_AM_MAX)s ) " \
                 "& ( ACHI2DOCA(1,2) < %(Comb_ACHI2DOCA_MAX)s ) "
        comb123 = "( AM < %(Comb_AM_MAX)s ) " \
                 "& ((APT2 > %(Trk_2of3_PT_MIN)s) | (APT3 > %(Trk_2of3_PT_MIN)s))" \
                 "& ( ACHI2DOCA(1,3) < %(Comb_ACHI2DOCA_MAX)s ) " \
                 "& ( ACHI2DOCA(2,3) < %(Comb_ACHI2DOCA_MAX)s ) "
        comb   = "(in_range( %(Comb_AM_MIN)s, AM, %(Comb_AM_MAX)s ))" \
                 "& (ANUM( ISBASIC & (PT > %(Trk_1of3_PT_MIN)s) ) >= 1)" \
                 "& (ANUM( ISBASIC & (PT > %(Trk_2of3_PT_MIN)s) ) >= 2)" \
                 "& ( APT > %(Comb_APT_MIN)s )" \
                 "& ( ACHI2DOCA(1,4) < %(Comb_ACHI2DOCA_MAX)s ) " \
                 "& ( ACHI2DOCA(2,4) < %(Comb_ACHI2DOCA_MAX)s ) " \
                 "& ( ACHI2DOCA(3,4) < %(Comb_ACHI2DOCA_MAX)s ) "


        parent = "(VFASPF(VCHI2PDOF) < %(Xicc_VCHI2PDOF_MAX)s)" \
                 "& (CHILD(VFASPF(VZ),1) - VFASPF(VZ) > %(Xicc_VZ1VZdiff_MIN)s)" \
                 "& (BPVVDCHI2 > %(Xicc_BPVVDCHI2_MIN)s )" \
                 "& (BPVDIRA > lcldira )"

        pream = [
                "import math"
                , "lcldira = math.cos( %(Xicc_acosBPVDIRA_MAX)s )"
        ]

        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__( self, name, decay, inputs,
                               dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                               tistos = 'TisTosSpec',
                               combiner = DaVinci__N4BodyDecays,
                               nickname = nickname,
                               shared = shared,
                               Combination12Cut =  comb12,
                               Combination123Cut = comb123,
                               CombinationCut = comb,
                               MotherCut = parent,
                               Preambulo = pream)
    # }
# }


## Combiner class for Xi_cc -> Hc H H H H decays
## ------------------------------------------------------------------------- ##
class Xicc2HcHHHHCombiner(Hlt2Combiner) : # {
    """
    Combiner for Xicc -> Hc + h h' h'' h'''.
    The selection variables are those used by the 2012 stripping for these
    decays.
    Like PromptSpectroscopyFilter, it relies of other 'standard' selections
    for Cabibbo-favored singly charmed hadrons as input and to provide
    selectivity.
    !!!NOTE!!! The implementation dowstream vertex requirement requires that
    the singly charmed hadron Hc is the first member of the decay descriptor.

    Configuration dictionaries must contain the following keys:
        'Trk_3of4_PT_MIN'      :
        'Trk_2of4_PT_MIN'      :
        'Trk_1of4_PT_MIN'      : Lower limit on PT for one, two, or three  of
                                 the 4 tracks produced in the Xicc decay.
        'Comb_AM_MAX'          : Upper limit on combination mass, AM
        'Comb_ACHI2DOCA_MAX'   : Upper limit on pairwise DOCACHI2 of all Xicc
                                 products, including the singly-charmed hadron.
        'Comb_APT_MIN'         : Lower limit on the combination APT
        'Xicc_VCHI2PDOF_MAX'   : Upper limit on Xicc decay vertex
        'Xicc_VZ1VZdiff_MIN'   : Minimum difference between Hc vertex VZ and
                                 Xicc vertex VZ.
        'Xicc_BPVVDCHI2_MIN'   : Minimum vertex displacement BPVVDCHI2
        'Xicc_acosBPVDIRA_MAX' :
        'TisTosSpec'        : configuration string of the Hlt1 TISTOS filter.
    """
    def __init__(self, name, decay, inputs, nickname = None, shared = False) : # {
        ## No DaughterCuts.  Assume that all filtering of input particles is
        ## already done.
        comb12  = "( AM < %(Comb_AM_MAX)s ) " \
                 "& ( ACHI2DOCA(1,2) < %(Comb_ACHI2DOCA_MAX)s ) "
        comb123 = "( AM < %(Comb_AM_MAX)s ) " \
                 "& ((APT2 > %(Trk_3of4_PT_MIN)s) | (APT3 > %(Trk_3of4_PT_MIN)s))" \
                 "& ( ACHI2DOCA(1,3) < %(Comb_ACHI2DOCA_MAX)s ) " \
                 "& ( ACHI2DOCA(2,3) < %(Comb_ACHI2DOCA_MAX)s ) "
        comb1234 = "( AM < %(Comb_AM_MAX)s ) " \
                 "& (ANUM( ISBASIC & (PT > %(Trk_2of4_PT_MIN)s) ) >= 1)" \
                 "& (ANUM( ISBASIC & (PT > %(Trk_3of4_PT_MIN)s) ) >= 2)" \
                 "& ( ACHI2DOCA(1,4) < %(Comb_ACHI2DOCA_MAX)s ) " \
                 "& ( ACHI2DOCA(2,4) < %(Comb_ACHI2DOCA_MAX)s ) " \
                 "& ( ACHI2DOCA(3,4) < %(Comb_ACHI2DOCA_MAX)s ) "
        comb   = "(in_range( %(Comb_AM_MIN)s, AM, %(Comb_AM_MAX)s ))" \
                 "& (ANUM( ISBASIC & (PT > %(Trk_1of4_PT_MIN)s) ) >= 1)" \
                 "& (ANUM( ISBASIC & (PT > %(Trk_2of4_PT_MIN)s) ) >= 2)" \
                 "& (ANUM( ISBASIC & (PT > %(Trk_3of4_PT_MIN)s) ) >= 3)" \
                 "& ( APT > %(Comb_APT_MIN)s )" \
                 "& ( ACHI2DOCA(1,5) < %(Comb_ACHI2DOCA_MAX)s ) " \
                 "& ( ACHI2DOCA(2,5) < %(Comb_ACHI2DOCA_MAX)s ) " \
                 "& ( ACHI2DOCA(3,5) < %(Comb_ACHI2DOCA_MAX)s ) " \
                 "& ( ACHI2DOCA(4,5) < %(Comb_ACHI2DOCA_MAX)s ) "

        parent = "(VFASPF(VCHI2PDOF) < %(Xicc_VCHI2PDOF_MAX)s)" \
                 "& (CHILD(VFASPF(VZ),1) - VFASPF(VZ) > %(Xicc_VZ1VZdiff_MIN)s)" \
                 "& (BPVVDCHI2 > %(Xicc_BPVVDCHI2_MIN)s )" \
                 "& (BPVDIRA > lcldira )"

        pream = [
                "import math"
                , "lcldira = math.cos( %(Xicc_acosBPVDIRA_MAX)s )"
        ]

        from HltTracking.HltPVs import PV3D
        Hlt2Combiner.__init__( self, name, decay, inputs,
                               dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                               tistos = 'TisTosSpec',
                               combiner = DaVinci__N5BodyDecays,
                               nickname = nickname,
                               shared = shared,
                               Combination12Cut =  comb12,
                               Combination123Cut = comb123,
                               Combination1234Cut = comb1234,
                               CombinationCut = comb,
                               MotherCut = parent,
                               Preambulo = pream)
    # }
# }

## ------------------------------------------------------------------------- ##
# The HHLambdaCombiner is designed for H-dibaryon --> Lambda,p,pi- below
# threshold for Lambda,Lambda decay.  Such dibaryons are expected to have
# have lifetimes a bit short than those of the Lamba as there are
# two stange quarks which can decay rather than one. A similar combiner
# might be useful for Lambda_b or Cascade_b^0 --> Lambda,pi-,pi+;
# Lambda,K-,K+; or Lambda,p+,pi- decays. Am not sure if the topo
# triggers use downstream Lambdas.

from Configurables import DaVinci__N3BodyDecays as N3BodyDecays
class HHLambdaCombiner(Hlt2Combiner):
    def __init__(self, name, decay, inputs, nickname = None):

## HHLambda mother cuts
        mc =    (
                 "   (PT > %(Mother_PT_MIN)s )" +
                 " & (CHI2VXNDOF < %(Mother_VCHI2PDOF_MAX)s)" +
                 " & (BPVDIRA > %(Mother_BPVDIRA_MIN)s )" +
                 " & (BPVVDCHI2 > %(Mother_BPVVDCHI2_MIN)s )" +
                 " & (BPVLTIME() > %(Mother_BPVLTIME_MIN)s )" +
                 " & (CHILD(VFASPF(VZ),3)-VFASPF(VZ) > %(LambdaDeltaZ_MIN)s )")

        c12Cut = ("(AM <  %(HHMass_MAX)s)" +
                  " & (ADOCAMAX('') < %(HHMaxDOCA)s)")

        cc =    ("(in_range( %(AM_MIN)s, AM, %(AM_MAX)s ))" +
                 " & ((APT1+APT2+APT3) > %(ASUMPT_MIN)s )")

        from HltTracking.HltPVs import PV3D

        Hlt2Combiner.__init__(self, name, decay, inputs,
                     dependencies = [TrackGEC('TrackGEC'), PV3D('Hlt2')],
                     shared = True,
                     tistos = 'TisTosSpec', combiner = N3BodyDecays,
                     nickname = nickname,
                     Combination12Cut =  c12Cut,
                     CombinationCut = cc,
                     MotherCut = mc, Preambulo = [])


## ------------------------------------------------------------------------- ##
##
##  and now, some instantiations
H2LambdaPrPi_LLLL  = HHLambdaCombiner('LamPrPiLLLL', decay="[D0 ->  Lambda0 p+ pi-]cc",
                     inputs=[SharedTighterDetachedLcChild_p, SharedDetachedLcChild_p,
                             CharmHadSharedSecondaryLambdaLL], nickname='LambdaPrPiLLLL')
H2LambdaPrPi_DDLL  = HHLambdaCombiner('LamPrPiDDLL', decay="[D0 ->  Lambda0 p+ pi-]cc",
                     inputs=[SharedTighterDetachedLcChild_p, SharedDetachedLcChild_p,
                             CharmHadSharedSecondaryLambdaDD], nickname='LambdaPrPiDDLL')

from Inputs import Hlt2DownProtons

H2LambdaPrPi_DDDD  = HHLambdaCombiner('LamPrPiDDDD', decay="[D0 ->  Lambda0 p+ pi-]cc",
                     inputs=[Hlt2DownProtons, Hlt2DownPions,
                             CharmHadSharedSecondaryLambdaDD], nickname='LambdaPrPiDDDD')


##  Let's do the same for Lambda,pi+,p
H2LambdaPiPr_LLLL  = HHLambdaCombiner('LamPiPrLLLL', decay="[D0 ->  Lambda0 p~- pi+]cc",
                     inputs=[SharedTighterDetachedLcChild_p, SharedDetachedLcChild_p,
                             CharmHadSharedSecondaryLambdaLL], nickname='LambdaPrPiLLLL')
H2LambdaPiPr_DDLL  = HHLambdaCombiner('LamPiPrDDLL', decay="[D0 ->  Lambda0 p~- pi+]cc",
                     inputs=[SharedTighterDetachedLcChild_p, SharedDetachedLcChild_p,
                             CharmHadSharedSecondaryLambdaDD], nickname='LambdaPrPiDDLL')

from Inputs import Hlt2DownProtons

H2LambdaPiPr_DDDD  = HHLambdaCombiner('LamPiPrDDDD', decay="[D0 ->  Lambda0 p~- pi+]cc",
                     inputs=[Hlt2DownProtons, Hlt2DownPions,
                             CharmHadSharedSecondaryLambdaDD], nickname='LambdaPrPiDDDD')
