'''Collection of functions and callable classes to perform standard MicroDST cloning.

'''

from copy import copy
from dstwriterutils import setCloneFilteredParticlesToTrue, ConfigurableList

class MicroDSTExtras(object) :
    '''
    Wrap a set of MicroDST callables in a list-like interface.
    Set the TES branch of each to the input branch.
    Export the output location trunk of all the callables.
    '''
    def __init__(self, branch = 'MicroDST', callables = []) :
        self.callables = callables
        self.branch = branch
        self.output = '/Event/'+branch+'#99'
        for c in self.callables :
            c.branch = self.branch
    def __getitem__(self, index) :
        return self.callables[index]

class CopyWithBranch(object) :
    def __init__(self, branch = '') :
        self.branch = branch
    def setOutputPrefix(self,alg) :
        alg.OutputPrefix = self.branch


class CloneRecHeader(CopyWithBranch) :
    def __call__(self, sel):
        from Configurables import CopyRecHeader
        cloner = CopyRecHeader(_personaliseName(sel, "CopyRecHeader"))
        self.setOutputPrefix(cloner)
        return [cloner]

class CloneODIN(CopyWithBranch) :
    def __call__(self, sel) :
        from Configurables import CopyODIN
        cloner = CopyODIN(_personaliseName(sel,"CopyODIN"))
        self.setOutputPrefix(cloner)
        return [cloner]

class CloneParticleTrees(CopyWithBranch) :

    def __init__(self, branch='', copyProtoParticles = True) :
        CopyWithBranch.__init__(self, branch)
        self.copyPP = copyProtoParticles

    def __call__(self, sel) :
        from Configurables import (CopyParticles,
                                   VertexCloner,
                                   ParticleCloner,
                                   ProtoParticleCloner )
        cloner = CopyParticles(_personaliseName(sel,
                                                'CopyParticles'))
        cloner.InputLocations = _dataLocations(sel,"Particles")
        cloner.OutputLevel=4
        if self.copyPP == False :
            cloner.addTool(ParticleCloner, name="ParticleCloner")
            cloner.ParticleCloner.ICloneProtoParticle="NONE"
        self.setOutputPrefix(cloner)

        confList = ConfigurableList(sel)
        setCloneFilteredParticlesToTrue( confList.flatList() )

        return [cloner]

class ClonePVs(CopyWithBranch) :
    def __call__(self, sel) :
        from Configurables import CopyPrimaryVertices
        cloner=CopyPrimaryVertices(_personaliseName(sel,
                                                    'CopyPrimaryVertices'))
        cloner.OutputLevel = 4
        self.setOutputPrefix(cloner)
        return [cloner]

class CloneMCInfo(CopyWithBranch) :
    def __call__(self, sel) :
        """
        Copy related MC particles of candidates plus daughters
        """
        from Configurables import P2MCRelatorAlg, CopyParticle2MCRelations
        from Configurables import MCParticleCloner, MCVertexCloner
        # first, get matches MCParticles for selected candidates.
        # This will make a relations table in mainLocation+"/P2MCPRelations"
        p2mcRelator = P2MCRelatorAlg(_personaliseName(sel,'P2MCRel'))
        p2mcRelator.ParticleLocations = _dataLocations(sel,'Particles')
        p2mcRelator.OutputLevel=4
        # Now copy relations table + matched MCParticles to MicroDST
        cloner = CopyParticle2MCRelations(_personaliseName(sel,
                                                           "CopyP2MCRel"))
        cloner.addTool(MCParticleCloner)
        cloner.MCParticleCloner.addTool(MCVertexCloner,
                                        name = 'ICloneMCVertex')
        cloner.InputLocations = _dataLocations(sel,"P2MCPRelations")
        cloner.OutputLevel = 4
        self.setOutputPrefix(cloner)
        return [p2mcRelator, cloner]

class CloneBTaggingInfo(CopyWithBranch) :
    def __call__(self, sel) :
        from Configurables import BTagging
        from Configurables import CopyFlavourTag
        importOptions('$FLAVOURTAGGINGOPTS/BTaggingTool.py')
        BTagAlgo = BTagging(_personaliseName(sel,'BTagging'))
        BTagAlgo.InputLocations=_dataLocations(sel,"")
        BTagAlgo.OutputLevel = 4
        cloner = CopyFlavourTag(_personaliseName(sel,
                                                 "CopyFlavourTag"))
        cloner.InputLocations = _dataLocations(sel,"FlavourTags")
        cloner.OutputLevel=4
        self.setOutputPrefix(cloner)
        return [BTagAlgo, cloner]


class ClonePVRelations(CopyWithBranch) :
    def __init__(self, PVRelationsMap, branch='') :
        CopyWithBranch.__init__(self, branch)
        self.PVRelMap = PVRelationsMap
        
    def __call__(self, sel) :
        """
        loop over related PV locations and copy each table.
        If no PV copying, keep original PV, but only if no PV re-fitting
        has been performed. If it has, then copy the PV too.
        """
        cloners = []
        for loc, copyPV in self.PVRelMap.iteritems() :
            print "Copy PV relations ", loc
            fullLoc = _dataLocations(sel, loc)
            cloner = _copyP2PVRelations(sel,"CopyP2PV_"+loc, fullLoc)
            clonerType = cloner.getProp('ClonerType')
            if copyPV == False :
                cloner.ClonerType = 'NONE'
                if hasattr(sel,'algorithm') :
                    alg = sel.algorithm()
                    refitPVs = False
                    if alg != None and alg.properties().has_key('ReFitPVs') :
                        refitPVs =  alg.getProp('ReFitPVs')
                    if refitPVs :
                        cloner.ClonerType = clonerType
            self.setOutputPrefix(cloner)
            cloners += [cloner]
            return cloners

class CloneL0DUReport(CopyWithBranch) :
    def __call__(self, sel) :
        from Configurables import CopyL0DUReport
        cloner = CopyL0DUReport(_personaliseName(sel,'CopyL0DUReport'))
        self.setOutputPrefix(cloner)
        return [cloner]
    
class CloneHltDecReports(CopyWithBranch) :
    def __call__(self, sel) :
        from Configurables import CopyHltDecReports
        cloner = CopyHltDecReports(_personaliseName(sel,'CopyHltDecReports'))
        self.setOutputPrefix(cloner)
        return [cloner]

class CloneBackCat(CopyWithBranch) :
    def __call__(self, sel) :
        from Configurables import Particle2BackgroundCategoryRelationsAlg
        from Configurables import CopyParticle2BackgroundCategory
        backCatAlg = Particle2BackgroundCategoryRelationsAlg(_personaliseName(sel,'BackCatAlg'))
        backCatAlg.InputLocations=_dataLocations(sel,"Particles")
        cloner =  CopyParticle2BackgroundCategory(_personaliseName(sel, 'CopyP2BackCat'))
        cloner.InputLocations = _dataLocations(sel,"P2BCRelations")
        cloner.OutputLevel=4
        self.setOutputPrefix(cloner)
        return [backCatAlg, cloner]

def _copyP2PVRelations(sel, name, locations) :
    from Configurables import CopyParticle2PVRelations
    cloner = CopyParticle2PVRelations(_personaliseName(sel,name))
    cloner.InputLocations = locations
    cloner.OutputLevel=4
    return cloner

def _personaliseName(sel, name) :
    return name + "_" + sel.name()

def _dataLocations(sel, extension) :
    loc = []
    for output in sel.outputLocations() :
        location = output+"/"+extension
        location = location.replace("//", "/")
        if location.endswith('/') :
            location = location[:len(location)-1]
        loc += [location]
    return loc
