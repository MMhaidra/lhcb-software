#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file
#  Helper function to extract luminosity 
#
#  @code
#
#  >>> l1 = getLumi ( 'myfile.root' )
#  >>> l2 = getLumi ( tree  )
#  >>> l3 = getLumi ( chain )
#  >>> l4 = getLumi ( file  )
#  >>> l5 = getLumi ( [ any sequence of above ]  )
#
#  @endcode
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-10-16
#  
#                    $Revision$
#  Last modification $Date$
#  by                $Author$
# =============================================================================
"""Helper function to extract luminosity 

Get lumi :

>>> l1 = getLumi ( 'myfile.root' )
>>> l2 = getLumi ( tree  )
>>> l3 = getLumi ( chain )
>>> l4 = getLumi ( file  )
>>> l5 = getLumi ( [ any sequence of above ]  )

"""
# =============================================================================
__version__ = "$Revision$"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2012-10-16"
# =============================================================================
__all__     = (
    'getLumi'  ,  ## get the lumi
    )
# =============================================================================
# logging 
# =============================================================================
from AnalysisPython.Logger import getLogger 
if '__main__' ==  __name__ : logger = getLogger ( 'Ostap.GetLumi' )
else                       : logger = getLogger ( __name__        )
# =============================================================================
## get luminosity from Lumi tuple
#
#  @param data  (INPUT) tree, chain, file, filename or sequence
#  @return the luminosity
#  @attention Linear addition of uncertainties is used here 
#
#  @code
#
#  >>> l1 = getLumi ( 'myfile.root' )
#  >>> l2 = getLumi ( tree  )
#  >>> l3 = getLumi ( chain )
#  >>> l4 = getLumi ( file  )
#  >>> l5 = getLumi ( [ any sequence of above ]  )
#
#  @endcode
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2012-09-20
def getLumi ( data , *args ) :
    """
    Get lumi :
    
    >>> l1 = getLumi ( 'myfile.root' )
    >>> l2 = getLumi ( tree  )
    >>> l3 = getLumi ( chain )
    >>> l4 = getLumi ( file  )
    >>> l5 = getLumi ( [ any sequence of above ]  )

    """
    tree_name = 'GetIntegratedLuminosity/LumiTuple'
    #
    import ROOT 
    from   Ostap.PyRoUts import VE, hID  
    #
    if args :
        data = [ data ]
        for a in args : data.append ( a )
        return getLumi ( data ) 
        
    if isinstance ( data , str ) :
        ## expand the actual file name
        import os 
        data = os.path.expandvars ( data )
        data = os.path.expanduser ( data )
        data = os.path.expandvars ( data )
        data = os.path.expandvars ( data )
        
        try :    
            tree = ROOT.TChain ( tree_name ) 
            tree.Add ( data )   
            lumi = getLumi ( tree )
            return lumi
        except :
            logger.error('Unable to get lumi(1) for %s' % data )
            return VE()
        
        #
    if isinstance ( data , ROOT.TFile ) :
        try :
            tree = data.Get( tree_name ) 
            return getLumi ( tree ) 
        except:
            logger.error('Unable to get lumi(2) for %s' % data.GetName() )
            return VE()
        
    if isinstance ( data , ROOT.TTree ) :
        
        try:
            #
            ## @attention here we are using sumVar! 
            l1 = data.sumVar ( '1.0*IntegratedLuminosity+0.0*IntegratedLuminosityErr' )
            l2 = data.sumVar ( '1.0*IntegratedLuminosity+1.0*IntegratedLuminosityErr' )
            l3 = data.sumVar ( '1.0*IntegratedLuminosity-1.0*IntegratedLuminosityErr' )            
            #
            l1.setError ( 0.5 * abs ( l2.value () - l3.value () ) )
            #
            # 
            return l1
        except :
            logger.error('Unable to get lumi(3) for %s' % data.GetName() )
            return VE()
        
    l = VE() 
    for i in data :
        k = getLumi ( i )
        ## @attention: linear addition of uncertainties: 
        l = VE ( l.value() + k.value() , ( l.error() + k.error () ) ** 2 ) 

    return l 


# =============================================================================
if '__main__' == __name__ :
    
    import ostapline
    logger.info ( __file__  + '\n' + ostapline.line  ) 
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
