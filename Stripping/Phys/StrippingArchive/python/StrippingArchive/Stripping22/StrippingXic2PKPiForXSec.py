"""
Stripping lines for selection of
    [Xi_c+ -> p+ K- pi+]cc
for open charm cross section measurement.
"""

__author__ = ['Alex Pearce']
__date__ = '2015/02/02'
__version__ = '$Revision: 1.0 $'

__all__ = (
    'default_config',
    'StrippingXic2PKPiForXSecConf'
)

from GaudiKernel.SystemOfUnits import MeV, GeV, mm, mrad
from Configurables import CombineParticles, FilterDesktop
from StandardParticles import (
    StdAllNoPIDsProtons,
    StdAllNoPIDsKaons,
    StdAllNoPIDsPions
)
from PhysSelPython.Wrappers import Selection
from StrippingUtils.Utils import LineBuilder
from StrippingConf.StrippingLine import StrippingLine
from MVADictHelpers import addTMVAclassifierValue

default_config = {
    'NAME': 'Xic2PKPiForXSec',
    'WGs': ['Charm'],
    'BUILDERTYPE': 'StrippingXic2PKPiForXSecConf',
    'STREAMS': ['Charm'],
    'CONFIG': {
        # Minimum transverse momentum all Xic+ daughters must satisfy
        'Daug_All_PT_MIN': 200.0*MeV,
        # Minimum transverse momentum at least 2 Xic+ daughters must satisfy
        'Daug_2of3_PT_MIN': 400.0*MeV,
        # Minimum transverse momentum at least 1 Xic+ daughter must satisfy
        'Daug_1of3_PT_MIN': 1000.0*MeV,
        # Minimum best primary vertex IP chi^2 all Xic+ daughters must satisfy
        'Daug_All_BPVIPCHI2_MIN': 4.0,
        # Minimum best PV IP chi^2 at least 2 Xic+ daughters must satisfy
        'Daug_2of3_BPVIPCHI2_MIN': 4.0,
        # Minimum best PV IP chi^2 at least 1 Xic+ daughter must satisfy
        'Daug_1of3_BPVIPCHI2_MIN': 4.0,
        # Minimum Xic+ daughter momentum
        'Daug_P_MIN': 3.0*GeV,
        # Maximum Xic+ daughter momentum
        'Daug_P_MAX': 100.0*GeV,
        # Minimum Xic+ daughter pseudorapidity
        'Daug_ETA_MIN': 2.0,
        # Maximum Xic+ daughter pseudorapidity
        'Daug_ETA_MAX': 5.0,
        # Minimum Xic+ daughter proton DLLp
        'Proton_PIDpPIDpi_MIN': 10.0,
        # Minimum Xic+ daughter proton DLLp - DLLK
        'Proton_PIDpPIDK_MIN': 5.0,
        # Minimum Xic+ daughter kaon DLLK
        'K_PIDK_MIN': 5.0,
        # Maximum Xic+ daughter pion DLLK
        'Pi_PIDK_MAX': 3.0,
        # Xic+ mass window around the nominal Xic+ mass before the vertex fit
        'Comb_ADAMASS_WIN': 90.0*MeV,
        # Maximum distance of closest approach of Xic+ daughters
        'Comb_ADOCAMAX_MAX': 0.5*mm,
        # Maximum Xic+ vertex chi^2 per vertex fit DoF
        'Xic_VCHI2VDOF_MAX': 25.0,
        # Maximum angle between Xic+ momentum and Xic+ direction of flight
        'Xic_acosBPVDIRA_MAX': 35.0*mrad,
        # Primary vertex displacement requirement, either that the Xic+ is some
        # sigma away from the PV, or it has a minimum flight time
        'Xic_PVDispCut': (
            '((BPVVDCHI2 > 16.0) |'
            '(BPVLTIME() > 0.150*picosecond))'
        ),
        # Minimum Xic MVA discriminant value
        'Xic_MVA_MIN': -0.3,
        # Path to the Xic MVA weights file
        # BDT is not applied if this is the empty string or None
        'Xic_MVA_Weights': None,
        # Dictionary of LoKi functors defining the Xic MVA input variables
        # The keys must match those used when training the MVA
        'Xic_MVA_Variables': {
        },
        # HLT filters, only process events firing triggers matching the RegEx
        'Hlt1Filter': None,
        'Hlt2Filter': None,
        # Fraction of candidates to randomly throw away before stripping
        'PrescaleXic2PKPi': 1.0,
        # Fraction of candidates to randomly throw after before stripping
        'PostscaleXic2PKPi': 1.0
    }
}


class StrippingXic2PKPiForXSecConf(LineBuilder):
    """Creates LineBuilder object containing the stripping lines."""
    # Allowed configuration keys
    __configuration_keys__ = default_config['CONFIG'].keys()

    # Decay descriptors
    Xic2PKPi = ['[Xi_c+ -> p+ K- pi+]cc']

    def __init__(self, name, config):
        """Initialise this LineBuilder instance."""
        self.name = name
        self.config = config
        LineBuilder.__init__(self, name, config)

        xic_pkpi_name = '{0}Xic2PKPi'.format(name)

        self.inPions = StdAllNoPIDsPions
        self.inKaons = StdAllNoPIDsKaons
        self.inProtons = StdAllNoPIDsProtons

        self.selXic2PKPi = self.makeXic2PKPi(
            name=xic_pkpi_name,
            inputSel=[self.inPions, self.inKaons, self.inProtons],
            decDescriptors=self.Xic2PKPi
        )
        self.selXic2PKPiMVA = self.makeMVASelection(
            '{0}MVASelection'.format(xic_pkpi_name),
            self.selXic2PKPi
        )

        self.line_Xic2PKPi = self.make_line(
            name='{0}Line'.format(xic_pkpi_name),
            selection=self.selXic2PKPiMVA,
            prescale=config['PrescaleXic2PKPi'],
            postscale=config['PostscaleXic2PKPi'],
            HLT1=config['Hlt1Filter'],
            HLT2=config['Hlt2Filter']
        )

    def make_line(self, name, selection, prescale, postscale, **kwargs):
        """Create the stripping line defined by the selection.

        Keyword arguments:
        name -- Base name for the Line
        selection -- Selection instance
        prescale -- Fraction of candidates to randomly drop before stripping
        postscale -- Fraction of candidates to randomly drop after stripping
        **kwargs -- Keyword arguments passed to StrippingLine constructor
        """
        # Only create the line with positive pre- and postscales
        # You can disable each line by setting either to a negative value
        if prescale > 0 and postscale > 0:
            line = StrippingLine(
                name,
                selection=selection,
                prescale=prescale,
                postscale=postscale,
                **kwargs
            )
            self.registerLine(line)
            return line
        else:
            return False

    def makeXic2PKPi(self, name, inputSel, decDescriptors):
        """Return a Selection instance for a Xi_c+ -> p+ h- h+ decay.

        Keyword arguments:
        name -- Name to give the Selection instance
        inputSel -- List of inputs passed to Selection.RequiredSelections
        decDescriptors -- List of decay descriptors for CombineParticles
        """
        lclPreambulo = [
            'from math import cos'
        ]

        daugCuts = (
            '(PT > {0[Daug_All_PT_MIN]})'
            '& (BPVIPCHI2() > {0[Daug_All_BPVIPCHI2_MIN]})'
        ).format(self.config)

        pidFiducialCuts = (
            '(in_range({0[Daug_P_MIN]}, P, {0[Daug_P_MAX]}))'
            '& (in_range({0[Daug_ETA_MIN]}, ETA, {0[Daug_ETA_MAX]}))'
        ).format(self.config)

        protonPIDCuts = (
            pidFiducialCuts +
            '& ((PIDp - PIDpi) > {0[Proton_PIDpPIDpi_MIN]})'
            '& ((PIDp - PIDK) > {0[Proton_PIDpPIDK_MIN]})'
        ).format(self.config)

        kaonPIDCuts = (
            pidFiducialCuts +
            '& ((PIDK - PIDpi) > {0[K_PIDK_MIN]})'
        ).format(self.config)

        pionPIDCuts = (
            pidFiducialCuts +
            '& ((PIDK - PIDpi) < {0[Pi_PIDK_MAX]})'
        ).format(self.config)

        combCuts = (
            "(ADAMASS('Xi_c+') < {0[Comb_ADAMASS_WIN]})"
            '& (AMAXCHILD(PT) > {0[Daug_1of3_PT_MIN]})'
            '& (AMAXCHILD(BPVIPCHI2()) > {0[Daug_1of3_BPVIPCHI2_MIN]})'
            '& (ANUM(PT > {0[Daug_2of3_PT_MIN]}) >= 2)'
            '& (ANUM(BPVIPCHI2() > {0[Daug_2of3_BPVIPCHI2_MIN]}) >= 2)'
            "& (ACUTDOCA({0[Comb_ADOCAMAX_MAX]}, ''))"
        ).format(self.config)

        xicCuts = (
            '(VFASPF(VCHI2/VDOF) < {0[Xic_VCHI2VDOF_MAX]})'
            '& ({0[Xic_PVDispCut]})'
            '& (BPVDIRA > cos({0[Xic_acosBPVDIRA_MAX]}))'
        ).format(self.config)

        _Xic = CombineParticles(
            name='Combine{0}'.format(name),
            DecayDescriptors=decDescriptors,
            Preambulo=lclPreambulo,
            DaughtersCuts={
                'pi+': '{0} & {1}'.format(daugCuts, pionPIDCuts),
                'K+': '{0} & {1}'.format(daugCuts, kaonPIDCuts),
                'p+': '{0} & {1}'.format(daugCuts, protonPIDCuts)
            },
            CombinationCut=combCuts,
            MotherCut=xicCuts
        )

        return Selection(name, Algorithm=_Xic, RequiredSelections=inputSel)

    def makeMVASelection(self, name, inputSel):
        # Don't apply a BDT if the weights file has not been specified
        if not self.config['Xic_MVA_Weights']:
            return inputSel

        cut = "VALUE('LoKi::Hybrid::DictValue/{0}') > {1[Xic_MVA_MIN]}".format(
            name, self.config
        )
        mva = FilterDesktop('{0}Filter'.format(name), Code=cut)

        addTMVAclassifierValue(
            Component=mva,
            XMLFile=self.config['Xic_MVA_Weights'],
            Variables=self.config['Xic_MVA_Variables'],
            ToolName=name
        )

        return Selection(name, Algorithm=mva, RequiredSelections=[inputSel])
