#!/usr/bin/env gaudirun.py
# =============================================================================
# $Id: Hlt1.py,v 1.4 2008-09-17 19:44:40 graven Exp $
# =============================================================================
## @file
#  Configuration of HLT1
#  @author Gerhard Raven Gerhard.Raven@nikhef.nl
#  @date 2008-08-25
# =============================================================================
"""
 script to configure HLT1 trigger lines
 Note: the actual line definitions are NOT in this file, 
 this script relies on them being available through hlt1Lines()
"""
# =============================================================================
__author__  = "Gerhard Raven Gerhard.Raven@nikhef.nl"
__version__ = "CVS Tag $Name: not supported by cvs2svn $, $Revision: 1.4 $"
# =============================================================================

from Gaudi.Configuration import * 
from Configurables       import GaudiSequencer as Sequence
from Configurables       import HltSummaryWriter
from Configurables       import HltSelectionFilter, HltSelectionToTES
from Configurables       import HltDecisionFilter
from Configurables       import HltDecReportsMaker, HltDecReportsWriter
from Configurables       import HltVertexReportsMaker, HltVertexReportsWriter
from Configurables       import HltRoutingBitsWriter
from Configurables       import HltLumiWriter
from Configurables       import HltGlobalMonitor
from Configurables       import bankKiller
from HltConf.HltLine     import hlt1Lines
from HltConf.HltLine     import hlt1Decisions
from HltConf.HltLine     import hlt1Selections
from HltConf.HltLine     import addHlt1Prop

# add a few thing to our printout
addHlt1Prop('routingBitDefinitions')
addHlt1Prop('Accept')

importOptions('$HLTCONFROOT/options/HltInit.opts')
# importOptions('$HLTCONFROOT/options/HltMain.py')


Hlt1Global =  HltDecisionFilter('Hlt1Global'
                                , Accept = ' | '.join( hlt1Decisions() )
                                )

### TODO: check dependencies, and re-order if needed! 
Hlt1 = Sequence('Hlt1',  ModeOR = True, ShortCircuit = False
               , Members = [ i.sequencer() for i in  hlt1Lines() ] + [ Hlt1Global ] 
               )

## needed to feed HltVertexReportsMaker... needed for Velo!
# run for all selections which have 'velo' in them
summaryWriter = HltSummaryWriter( Save = list(hlt1Selections()['All']) )
vertexMaker = HltVertexReportsMaker(  )
vertexWriter =  HltVertexReportsWriter( )

veloVertex = Sequencer( 'VeloVertex',  Members = [ summaryWriter, vertexMaker, vertexWriter ])


## set triggerbits
#  0-31: reserved for L0  // need to add L0DU support to routing bit writer
# 32-63: reserved for Hlt1
# 64-91: reserved for Hlt2
triggerBits = HltRoutingBitsWriter( routingBitDefinitions = 
                                  { 32 : 'Hlt1Global'
                                  , 34 : 'Hlt1RandomDecision|Hlt1PhysicsDecision|Hlt1LumiDecision'
                                  , 35 : 'Hlt1RandomDecision'
                                  , 36 : 'Hlt1PhysicsDecision'
                                  , 37 : 'Hlt1LumiDecision'
                                  })


def XOnly( dec ) :
    return dec + '&!(' + '|'.join([ i for i in hlt1Decisions() if i != dec ]) + ') '


rawbankLumiStripper = Sequence( 'LumiStripper'
                              , IgnoreFilterPassed = True
                              , Members = 
                              [ HltDecisionFilter('LumiOnlyFilter' , Accept = XOnly( 'Hlt1LumiDecision' ) )
                              , bankKiller( BankTypes=[ 'ODIN','HltLumiSummary'],  DefaultIsKill=True )
                              ])

Hlt  = Sequence('Hlt')
Hlt.Members = [ Hlt1
              , Sequence( 'HltEndSequence', ModeOR = True, ShortCircuit = False
                        , Members = [ HltGlobalMonitor( Hlt1Decisions = list( hlt1Decisions() ) )
                                    , HltDecReportsWriter()
                                    , veloVertex
                                    , triggerBits
                                    , HltLumiWriter()
                                    #, rawbankLumiStripper
                                    ] )
              ]

print Hlt
