#!/usr/bin/env python
# =============================================================================
# $Id: Phi.py,v 1.12 2006-02-02 13:09:25 ibelyaev Exp $
# =============================================================================
# CVS version $Revision: 1.12 $
# =============================================================================
# CVS tag     $Name: not supported by cvs2svn $
# =============================================================================
# @file 
# "Demo" algorithm for Python based phi -> K+ K- "analysis"
# =============================================================================
# @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# @date   2003-12-04
# =============================================================================

# import the Bender itself  
from   bendermodule  import *
import benderPreLoad as preload

global h1 
# =============================================================================
# Specific physics analysis algorithm 
# =============================================================================

class Phi(Algo):
    " My own analysis algorithm for selection of phi -> K+ K- "
    def analyse ( self ) :
        # get kaons 
        kplus  = self.select ( tag = "K+" , cuts = 'K+' == ID )
        kminus = self.select ( tag = "K-" , cuts = 'K-' == ID )
        # create loop
        phis   = self.loop ( formula = "K+ K-" , pid = "phi(1020)" )
        # loop over KK combinations 
        for phi in phis:
            if phi.mass(1,2) > 1050 * MeV : continue
            vxChi2 = VCHI2 ( phi )
            if 100 < vxChi2  or  0 > vxChi2  : continue
            h1 = self.plot ( title = "K+ K- mass"      ,
                             value = M ( phi ) / MeV   ,
                             low   = 1000              ,
                             high  = 1050              )
            phi.save('phi')

        if not self.selected('phi').empty() :
            self.setFilterPassed ( True )
                
        return SUCCESS 
        

# =============================================================================
# Generic job configuration 
# =============================================================================
def configure() :
    gaudi.config( files   =
                  [ '$DAVINCIROOT/options/DaVinciCommon.opts'   ,   # common options 
                    '$DAVINCIROOT/options/DaVinciReco.opts'     ,   # general 'Reco' options 
                    '$DAVINCIROOT/options/DaVinciTestData.opts' ] ) 
    
    # specific job configuration 
    gaudi.DLLs += ["HbookCnv"]
    
    # preload algorithm(s)
    gaudi.addAlgorithm ( 'LoKiPreLoad/Hadrons' ) 
    preload.Hadrons( Particles = [ 'kaon' , 'pion'] )
    
    # create analysis algorithm and add it to the list of
    phi = Phi('Phi')    
    gaudi.addAlgorithm ( phi ) 
    
    desktop = gaudi.tool('Phi.PhysDesktop')
    desktop.InputLocations  = [ "/Event/Phys/Hadrons"]
    
    # output histogram file 
    hsvc = gaudi.service( 'HistogramPersistencySvc' )
    hsvc.HistogramPersistency = "HBOOK" 
    gaudi.HistogramPersistency = "HBOOK" 
    hsvc.OutputFile = 'phi.hbook'
    
    # add the printout of the histograms
    hsvc = gaudi.service( 'HbookHistSvc' )
    hsvc.PrintHistos = True

    # switch off native DaVinci histograms
    dv = gaudi.algorithm('DaVinci')
    dv.HistoProduce = False
    
    return SUCCESS

# =============================================================================
# job execution 
# =============================================================================

if __name__ == '__main__' :
    import sys 
    # configure the job
    configure() 
    # run job 
    gaudi.run  ( 500 )
    
# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.11  2005/08/01 09:50:19  ibelyaev
#  update for v5r2
#
# =============================================================================
# The END 
# =============================================================================
