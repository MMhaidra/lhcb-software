#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file
#  Module with decoration of many ROOT objects for efficient use in python
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2011-06-07
#
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""Decoration of some ROOT objects for efficient use in python

Many native  root classes are equipped with new useful methods and operators,
in particular TH1(x) , TH2(x) , TAxis, TGraph(Errors), etc...

see also GaudiPython.HistoUtils, LHCbMath.math_ve, LHCbMath.Types 
"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-06-07"
# =============================================================================
__all__     = (
    #
    'cpp'             , ## global C++ namespace 
    'rootID'          , ## construct the (global) unique ROOT identifier
    'funcID'          , ## construct the (global) unique ROOT identifier
    'funID'           , ## construct the (global) unique ROOT identifier
    'hID'             , ## construct the (global) unique ROOT identifier
    'histoID'         , ## construct the (global) unique ROOT identifier
    'dsID'            , ## construct the (global) unique ROOT identifier
    #
    'VE'              , ## Gaudi::Math::ValueWithError
    'histoGuess'      , ## guess the simple histo parameters
    #'useLL'           , ## use LL for histogram fit?
    #'allInts'         , ## natural histogram with natural entries?
    'SE'              , ## StatEntity
    'WSE'             , ## StatEntity with weights
    'iszero'          , ## is almost zero  ?
    'isequal'         , ## is almost equal ?
    #
    'binomEff'        , ## calculate binomial efficiency
    'binomEff2'       , ## calculate binomial efficiency
    'zechEff'         , ## calculete binomial efficiency using Zech's          prescription
    'wilsonEff'       , ## calculete binomial efficiency using Wilson's        prescription
    'agrestiCoullEff' , ## calculete binomial efficiency using Agresti-Coull's prescription
    #
    'binomEff_h1'     , ## calculate binomial efficiency for 1D-histos
    'binomEff_h2'     , ## calculate binomial efficiency for 2D-ihstos
    'binomEff_h3'     , ## calculate binomial efficiency for 3D-ihstos
    #
    'makeGraph'       , ## make ROOT Graph from input data
    'hToGraph'        , ## convert historgam to graph
    'hToGraph2'       , ## convert historgam to graph
    'hToGraph3'       , ## convert historgam to graph
    'lw_graph'        , ## make Lafferty-Wyatt's graph from the histo 
    'h1_axis'         , ## book 1D-histogram from axis 
    'h2_axes'         , ## book 2D-histogram from axes
    'h3_axes'         , ## book 3D-histogram from axes
    'axis_bins'       , ## convert list of bin edges to axis
    've_adjust'       , ## adjust the efficiency to be in physical range
    #
    )
# =============================================================================
import ROOT
# =============================================================================
# logging 
# =============================================================================
from Ostap.Logger import getLogger 
logger = getLogger( __name__ )
# =============================================================================
logger.info ( 'Zillions of decorations for ROOT   objects')
# =============================================================================
from Ostap.Core import ( cpp      ,
                         ROOTCWD  , rootID    , 
                         funcID   , funID     , fID             ,
                         histoID  , hID       , dsID            ,
                         cwd      , pwd       ,
                         VE       , SE        , WSE             ,
                         binomEff , binomEff2 ,
                         zechEff  , wilsonEff , agrestiCoullEff , 
                         iszero   , isequal   ,
                         isint    , islong    , natural_entry   ) 
## silently load RooFit library 
from Ostap.Utils import mute
with mute() : _tmp = ROOT.RooRealVar
del mute

iLevel = int( ROOT.gErrorIgnoreLevel ) 
ROOT.gROOT.ProcessLine("gErrorIgnoreLevel = 2001; " ) 


# =============================================================================
## decorate histograms 
# =============================================================================    
from   Ostap.HistoDeco import ( binomEff_h1 , binomEff_h2 , binomEff_h3 ,
                                h1_axis     , h2_axes     , h3_axes     ,
                                axis_bins   , ve_adjust   , histoGuess  )

# =============================================================================
# Other decorations 
# =============================================================================
import Ostap.TreeDeco
import Ostap.MinuitDeco
import Ostap.HTextDeco
import Ostap.HParamDeco
import Ostap.HCmpDeco
import Ostap.TFileDeco
import Ostap.MiscDeco
import Ostap.Models 
import Ostap.HepDATA 
import Ostap.Canvas 
import Ostap.RooFitDeco

# =============================================================================
## graphs 
# =============================================================================
from Ostap.GraphDeco import makeGraph, hToGraph, hToGraph2, hToGraph3, lw_graph  

## restore the warnings level 
ROOT.gROOT.ProcessLine("gErrorIgnoreLevel = %d; " % iLevel ) 

# =============================================================================
if '__main__' == __name__ :
            
    import Ostap.Line
    logger.info ( __file__  + '\n' + Ostap.Line.line  ) 
    logger.info ( 80*'*'   )
    logger.info ( __doc__  )
    logger.info ( 80*'*' )
    logger.info ( ' Author  : %s' %         __author__    ) 
    logger.info ( ' Version : %s' %         __version__   ) 
    logger.info ( ' Date    : %s' %         __date__      )
    logger.info ( ' Symbols : %s' %  list ( __all__     ) )
    logger.info ( 80*'*' ) 
    
# =============================================================================
# The END 
# =============================================================================
