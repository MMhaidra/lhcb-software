#!/usr/bin/env python
# =============================================================================
# $Id: functions.py,v 1.12 2010-05-17 16:01:39 ibelyaev Exp $ 
# =============================================================================
## @file LoKiHlt/functions.py
#  The full set of useful objects from LoKiHlt library 
#  The file is a part of LoKi and Bender projects
#  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
#  @date 2008-09-17
# =============================================================================
"""
The full set of useful objects from LoKiHlt library
"""
__author__  = "Vanya BELYAEV Ivan.Belyaev@nikhef.nl   "
__version__ = " CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.12 $  "
# =============================================================================

import LoKiCore.decorators as _LoKiCore 

# Namespaces:
cpp      = _LoKiCore.cpp
std      = _LoKiCore.std
LoKi     = _LoKiCore.LoKi
LHCb     = _LoKiCore.LHCb

_o1  = 'const LHCb::ODIN*'
_l0  = 'const LHCb::L0DUReport*'
_hlt = 'const LHCb::HltDecReports*'

# =============================================================================
## "The main" types
# =============================================================================

## @see LoKi::Types::L0_Func
L0_Func  = LoKi.Functor                ( _l0 + ',double' ) 
## @see LoKi::Types::L0_Cuts
L0_Cuts  = LoKi.Functor                ( _l0 + ',bool'   ) 
## @see LoKi::Types::L0_Fun
L0_Fun   = LoKi.FunctorFromFunctor     ( _l0 + ',double' ) 
## @see LoKi::Types::L0_Cut
L0_Cut   = LoKi.FunctorFromFunctor     ( _l0 + ',bool'   ) 

## @see LoKi::Types::ODIN_Func
ODIN_Func  = LoKi.Functor                ( _o1 + ',double' ) 
## @see LoKi::Types::ODIN_Cuts
ODIN_Cuts  = LoKi.Functor                ( _o1 + ',bool'   ) 
## @see LoKi::Types::ODIN_Fun
ODIN_Fun   = LoKi.FunctorFromFunctor     ( _o1 + ',double' ) 
## @see LoKi::Types::ODIN_Cut
ODIN_Cut   = LoKi.FunctorFromFunctor     ( _o1 + ',bool'   ) 

## @see LoKi::Types::HLT_Func
HLT_Func = LoKi.Functor                ( _hlt + ',double' ) 
## @see LoKi::Types::HLT_Cuts
HLT_Cuts = LoKi.Functor                ( _hlt + ',bool'   ) 
## @see LoKi::Types::HLT_Fun
HLT_Fun  = LoKi.FunctorFromFunctor     ( _hlt + ',double' ) 
## @see LoKi::Types::HLT_Cut
HLT_Cut  = LoKi.FunctorFromFunctor     ( _hlt + ',bool'   ) 



# =============================================================================
## The concrete L0-functions 
# =============================================================================

## @see LoKi::Cuts::L0_ALL 
L0_ALL           = LoKi.Constant ( _l0 + ',bool'   ) ( True  )

## @see LoKi::Cuts::L0_CHANNEL 
L0_CHANNEL        = LoKi.L0.ChannelDecision
## @see LoKi::Cuts::L0_CHANNEL_SUBSTR 
L0_CHANNEL_SUBSTR = LoKi.L0.ChannelDecisionSubString
## @see LoKi::Cuts::L0_CHANNEL_RE
L0_CHANNEL_RE     = LoKi.L0.ChannelDecisionRegex 

## @see LoKi::Cuts::L0_CHDECISION
L0_CHDECISION    = LoKi.L0.ChannelDecision

## @see LoKi::Cuts::L0_CHPREDECISION
L0_CHPREDECISION = LoKi.L0.ChannelPreDecision
## @see LoKi::Cuts::L0_CHPREDECISION_SUBSTR
L0_CHPREDECISION_SUBSTR = LoKi.L0.ChannelPreDecisionSubString
## @see LoKi::Cuts::L0_CHPREDECISION_RE
L0_CHPREDECISION_RE     = LoKi.L0.ChannelPreDecisionRegex 

## @see LoKi::Cuts::L0_CONDITION
L0_CONDITION     = LoKi.L0.ConditionValue 

## @see LoKi::Cuts::L0_DATA
L0_DATA          = LoKi.L0.DataValue

## @see LoKi::Cuts::L0_DECISION
L0_DECISION         = LoKi.L0.Decision
## @see LoKi::Cuts::L0_DECISION_PHYSICS 
L0_DECISION_PHYSICS = LoKi.L0.Decision( LHCb.L0DUDecision.Physics ) 
## @see LoKi::Cuts::L0_DECISION_BEAM1
L0_DECISION_BEAM1   = LoKi.L0.Decision( LHCb.L0DUDecision.Beam1   ) 
## @see LoKi::Cuts::L0_DECISION_BEAM2
L0_DECISION_BEAM2   = LoKi.L0.Decision( LHCb.L0DUDecision.Beam2   ) 


## @see LoKi::Cuts::L0_DIGIT
L0_DIGIT         = LoKi.L0.DataDigit

## @see LoKi::Cuts::L0_FALSE
L0_FALSE         = LoKi.Constant ( _l0 + ',bool'   ) ( False )

## @see LoKi::Cuts::L0_FORCEBIT
L0_FORCEBIT      = LoKi.L0.ForceBit () 

## @see LoKi::Cuts::L0_NONE 
L0_NONE          = LoKi.Constant ( _l0 + ',bool'   ) ( False )

## @see LoKi::Cuts::L0_NULL
L0_NULL          = LoKi.Constant ( _l0 + ',double' ) ( 0 )

## @see LoKi::Cuts::L0_ONE 
L0_ONE           = LoKi.Constant ( _l0 + ',double' ) ( 1 )


## @see LoKi::Cuts::L0_SUMET
L0_SUMET         = LoKi.L0.SumEt

## @see LoKi::Cuts::L0_SUMET0
L0_SUMET0        = LoKi.L0.SumEt ( 0 ) 

## @see LoKi::Cuts::L0_SUMDECISION
L0_SUMDECISION          = LoKi.L0.SumDecision  
## @see LoKi::Cuts::L0_SUMDECISION_PHYSICS 
L0_SUMDECISION_PHYSICS  = LoKi.L0.SumDecision ( LHCb.L0DUDecision.Physics )
## @see LoKi::Cuts::L0_SUMDECISION_BEAM1 
L0_SUMDECISION_BEAM1    = LoKi.L0.SumDecision ( LHCb.L0DUDecision.Beam1   )
## @see LoKi::Cuts::L0_SUMDECISION_BEAM2
L0_SUMDECISION_BEAM2    = LoKi.L0.SumDecision ( LHCb.L0DUDecision.Beam2   )


## @see LoKi::Cuts::L0_TCK
L0_TCK           = LoKi.L0.Tck ()

## @see LoKi::Cuts::L0_TIMINGBIT 
L0_TIMINGBIT     = LoKi.L0.TimingBit ()

## @see LoKi::Cuts::L0_TRIGGER 
L0_TRIGGER       = LoKi.L0.TriggerDecision 
## @see LoKi::Cuts::L0_TRIGGER_SUBSTR 
L0_TRIGGER_SUBSTR = LoKi.L0.TriggerDecisionSubString
## @see LoKi::Cuts::L0_TRIGGER 
L0_TRIGGER_RE     = LoKi.L0.TriggerDecisionRegex 

## @see LoKi::Cuts::L0_TRUE
L0_TRUE          = LoKi.Constant ( _l0 + ',bool'   ) ( True  )

## @see LoKi::Cuts::L0_ZERO
L0_ZERO          = LoKi.Constant ( _l0 + ',double' ) ( 0 )

## @see LoKi::Cuts::L0_VALID
L0_VALID         = LoKi.L0.Valid() 


# =============================================================================
## The concrete O1-functions 
# =============================================================================

## @see LoKi::Cuts::ODIN_ALL
ODIN_ALL       = LoKi.Constant( _o1 + ',bool'  ) ( True  )

## @see LoKi::Cuts::ODIN_BUNCH
ODIN_BUNCH     = LoKi.Odin.BunchId      ()

## @see LoKi::Cuts::ODIN_BXCURRENT
ODIN_BXCURRENT = LoKi.Odin.BunchCurrent ()

## @see LoKi::Cuts::ODIN_BXID 
ODIN_BXID      = LoKi.Odin.BXId 

## @see LoKi::Cuts::ODIN_BXTYP
ODIN_BXTYP     = LoKi.Odin.BXType () 

## @see LoKi::Cuts::ODIN_CALSTEP
ODIN_CALSTEP   = LoKi.Odin.CalibrationStep () 

## @see LoKi::Cuts::ODIN_ForceBit 
ODIN_FORCEBIT  = LoKi.Odin.ForceBit () 

## @see LoKi::Cuts::ODIN_EVTTYP
ODIN_EVTTYP    = LoKi.Odin.EventType () 

## @see LoKi::Cuts::ODIN_ERRBITS
ODIN_ERRBITS   = LoKi.Odin.ErrorBits () 



## @see LoKi::Cuts::ODIN_FALSE
ODIN_FALSE     = LoKi.Constant( _o1 + ',bool'  ) ( False )

## @see LoKi::Cuts::ODIN_NONE
ODIN_NONE      = LoKi.Constant( _o1 + ',bool'   ) ( False )

## @see LoKi::Cuts::ODIN_NULL
ODIN_NULL      = LoKi.Constant( _o1 + ',double') ( 0 )

## @see LoKi::Cuts::ODIN_ONE 
ODIN_ONE       = LoKi.Constant( _o1 + ',double') ( 1 )

## @see LoKi::Cuts::ODIN_ORBIT 
ODIN_ORBIT     = LoKi.Odin.Orbit  () 

## @see LoKi::Cuts::ODIN_PRESCALE
ODIN_PRESCALE  = LoKi.Odin.Prescale

## @see LoKi::Cuts::ODIN_ROTYP
ODIN_ROTYP     = LoKi.Odin.ReadOutType () 

## @see LoKi::Cuts::ODIN_RUN 
ODIN_RUN       = LoKi.Odin.Run ()

## @see LoKi::Cuts::ODIN_RUNNUMBER 
ODIN_RUNNUMBER = LoKi.Odin.RunNumber 
ODIN_RUNNUMBER.run_list   = std.vector     ('unsigned int') 

## @see LoKi::Cuts::ODIN_EVTNUMBER 
ODIN_EVTNUMBER = LoKi.Odin.EvtNumber 
ODIN_EVTNUMBER.event_list   = std.vector   ('unsigned long long') 

## @see LoKi::Cuts::ODIN_RUNEVTNUMBER 
ODIN_RUNEVTNUMBER = LoKi.Odin.RunEvtNumber 
ODIN_RUNEVTNUMBER.runevt_pair = std.pair   ( 'unsigned int' , 'unsigned long long' )
ODIN_RUNEVTNUMBER.runevt_list = std.vector (  ODIN_RUNEVTNUMBER.runevt_pair )

## @see LoKi::Cuts::ODIN_TCK
ODIN_TCK       = LoKi.Odin.TrgConfKey () 

## @see LoKi::Cuts::ODIN_TCKEYS 
ODIN_TCKEYS    = LoKi.Odin.Tck 

## @see LoKi::Cuts::ODIN_TIME
ODIN_TIME      = LoKi.Odin.InTime

## @see LoKi::Cuts::ODIN_TGRTYP
ODIN_TRGTYP    = LoKi.Odin.TriggerType () 

## @see LoKi::Cuts::ODIN_TRUE
ODIN_TRUE      = LoKi.Constant( _o1 + ',bool'  ) ( True )

## @see LoKi::Cuts::ODIN_ZERO
ODIN_ZERO      = LoKi.Constant( _o1 + ',double') ( 0 )

## @see LoKi::Cuts::ODIN_ROUTINBITS
ODIN_ROUTINGBITS    = LoKi.Odin.RoutingBits



# =============================================================================
# concrete HLT functions 
# =============================================================================

## @see LoKi::Cuts::HLT_ALL 
HLT_ALL      =    LoKi.Constant ( _hlt + ',bool'   ) ( True  )

## @see LoKi::Cuts::HLT_DECISION
HLT_DECISION =    LoKi.HLT.Decision ()  

## @see LoKi::Cuts::HLT_FALSE 
HLT_FALSE    =    LoKi.Constant ( _hlt + ',bool'   ) ( False )

## @see LoKi::Cuts::HLT_HAS
HLT_HAS        =    LoKi.HLT.HasDecision
## @see LoKi::Cuts::HLT_HAS_SUBSTR
HLT_HAS_SUBSTR =    LoKi.HLT.HasDecisionSubString
## @see LoKi::Cuts::HLT_HAS_RE
HLT_HAS_RE     =    LoKi.HLT.HasDecisionRegex

## @see LoKi::Cuts::HLT_NONE
HLT_NONE     =    LoKi.Constant ( _hlt + ',bool'   ) ( False )

## @see LoKi::Cuts::HLT_NPASS
HLT_NPASS    =    LoKi.HLT.NPass () 

## @see LoKi::Cuts::HLT_NULL
HLT_NULL     =    LoKi.Constant ( _hlt + ',double' ) ( 0 )

## @see LoKi::Cuts::HLT_ONE
HLT_ONE      =    LoKi.Constant ( _hlt + ',double' ) ( 1 )

## @see LoKi::Cuts::HLT_PASS
HLT_PASS        =    LoKi.HLT.PassDecision 
## @see LoKi::Cuts::HLT_PASS_SUBSTR
HLT_PASS_SUBSTR =    LoKi.HLT.PassDecisionSubString
## @see LoKi::Cuts::HLT_PASS_RE
HLT_PASS_RE     =    LoKi.HLT.PassDecisionRegex

## @see LoKi::Cuts::HLT_PASSIGNORING 
HLT_PASSIGNORING         =    LoKi.HLT.DecisionBut
## @see LoKi::Cuts::HLT_PASSIGNORING_SUBSTR 
HLT_PASSIGNORING_SUBSTR  =    LoKi.HLT.DecisionButSubString
## @see LoKi::Cuts::HLT_PASSIGNORING_RE
HLT_PASSIGNORING_RE      =    LoKi.HLT.DecisionButRegex

## @see LoKi::Cuts::HLT_SIZE
HLT_SIZE     =    LoKi.HLT.Size () 

## @see LoKi::Cuts::HLT_TRUE
HLT_TRUE     =    LoKi.Constant ( _hlt + ',bool'   ) ( True  )

## @see LoKi::Cuts::HLT_ZERO
HLT_ZERO     =    LoKi.Constant ( _hlt + ',double' ) ( 0 )


## @see LoKi::Cuts::HLT_ERRORBITS 
HLT_ERRORBITS        = LoKi.HLT.ErrorBits

## @see LoKi::Cuts::HLT_EXECUTIONSTAGE
HLT_EXECUTIONSTAGE   = LoKi.HLT.ExecutionStage

## @see LoKi::Cuts::HLT_NCANDIDATES
HLT_NCANDIDATES       = LoKi.HLT.NumberOfCandidates

## @see LoKi::Cuts::HLT_SATURATED
HLT_SATURATED          = LoKi.HLT.Saturated 

## @see LoKi::Cuts::HLT_COUNTERRORBITS 
HLT_COUNT_ERRORBITS    = LoKi.HLT.CountErrorBits  

## @see LoKi::Cuts::HLT_COUNTERRORBITS_RE 
HLT_COUNT_ERRORBITS_RE = LoKi.HLT.CountErrorBitsRegex  

## @see LoKi::Cuts::HLT_ROUTINBITS
HLT_ROUTINGBITS        = LoKi.HLT.HltRoutingBits


## @see LoKi::Cuts::ROUTINBITS
ROUTINGBITS            = LoKi.HLT.RoutingBits

# =========================================================================
## helper function to define properly ODIN_EVTNUMBER predicate
#  @see LoKi::Cuts::ODIN_EVTNUMBER
#  @see LoKi::Odin::EvtNumber
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2010-03-07
def odin_events ( arg1 , *args ) :
    """
    Helper function to define properly ODIN_EVTNUMBER predicate
    """
    
    if   0 == len ( args ) and issubclass ( type(arg1) , ( int , long ) ) : 
        return ODIN_EVTNUMBER ( arg1 )
    elif 1 == len ( args ) :
        if issubclass ( type(arg1) , ( int , long ) ) :
            arg2=args[0]
            if issubclass ( type(arg2) , ( int , long ) ) :
                return ODIN_EVTNUMBER ( arg1 , arg2 )
        raise TypeError ("invalid signature") 
                        
    from LoKiCore.functions import ullongs    
    evts = ullongs ( arg1 , *args )
    
    return ODIN_EVTNUMBER ( evts )

# =========================================================================
## helper function to define properly ODIN_RUNNUMBER predicate
#  @see LoKi::Cuts::ODIN_RUNNUMBER
#  @see LoKi::Odin::RunNumber
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2010-03-07
def odin_runs ( arg1 , *args ) :
    """
    Helper function to define properly ODIN_RUNNUMBER predicate
    """
    
    if   0 == len ( args ) and issubclass ( type(arg1) , ( int , long ) ) : 
        return ODIN_RUNNUMBER ( arg1 )
    elif 1 == len ( args ) :
        if issubclass ( type(arg1) , ( int , long ) ) :
            arg2=args[0]
            if issubclass ( type(arg2) , ( int , long ) ) :
                return ODIN_RUNNUMBER ( arg1 , arg2 )
        raise TypeError ("invalid signature") 
                        
    from LoKiCore.functions import uints
    evts = uints ( arg1 , *args )
    
    return ODIN_RUNNUMBER ( evts )


# =========================================================================
## helper function to define properly ODIN_RUNEVTNUMBER predicate
#  @see LoKi::Cuts::ODIN_RUNEVTNUMBER
#  @see LoKi::Odin::RunEvtNumber
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2010-03-07
def odin_runevts ( arg1 ) :
    """
    Helper function to define properly ODIN_RUNEVTNUMBER predicate
    """
    _rep = ODIN_RUNEVTNUMBER.RunEvtPair
    _rel = ODIN_RUNEVTNUMBER.RunEvtList

    if issubclass ( type ( arg1 ) , _rel ) :
        return ODIN_RUNEVTNUMBER ( arg1 )
    
    rel = _rel()
    for a in arg1 :
        rel.push_back( _rep( a[0] , a[1] ) ) 
        
    return ODIN_RUNEVTNUMBER( rel )
    
# =============================================================================
## helper function to define (void)functor  for routing bits 
#  @see LoKi::Cuts::ROUTINGBITS
#  @see LoKi::Cuts::ROUTINGBIT
#  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
#  @date 2010-05-17
def routingBits ( arg1 , *args ) :
    """
    Helper function to define (void)functor  for routing bits
    """
    from LoKiCore.functions import uints
    bits = uints ( arg1 , *args )
    return ROUTINGBITS ( bits )

# =============================================================================
## helper function to define HLT-functor  for routing bits 
#  @see LoKi::Cuts::HLT_ROUTINGBITS
#  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
#  @date 2010-05-17
def HLT_routingBits ( arg1 , *args ) :
    """
    Helper function to define HLT-functor  for routing bits
    """
    return HLT_ROUTINGBITS ( routingBits ( arg1 , *args ) ) 

# =============================================================================
## helper function to define ODIN-functor  for routing bits 
#  @see LoKi::Cuts::ODIN_ROUTINGBITS
#  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
#  @date 2010-05-17
def ODIN_routingBits ( arg1 , *args ) :
    """
    Helper function to define ODIN-functor  for routing bits
    """
    return ODIN_ROUTINGBITS ( routingBits ( arg1 , *args ) ) 

# =============================================================================
if '__main__' == __name__ :
    for o in dir() : print o

# =============================================================================
# The END 
# =============================================================================
