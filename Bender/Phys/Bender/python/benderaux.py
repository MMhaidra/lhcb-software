# =============================================================================
# $Id: benderaux.py,v 1.20 2006-11-16 13:42:13 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.20 $
# =============================================================================
""" Auxillary module  to keep some helper fuctions for bender """
# =============================================================================
## @file
#  Auxillary module  to keep some helper fuctions for Bender
#  @date   2004-07-11
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
# =============================================================================
__author__ = "Vanya BELYAEV ibelyaev@physics.syr.edu"

import os,sets
import gaudimodule

## Load all defined dictionary libraries
#  @param  lst list of additional dictionaries to be load
#  @param  verbose flag for verbose execution 
#  @return list of properly loaded dictionaries 
def _loadDict_ ( lst = [] , verbose = True ) :
    """
    Load all defined dictionary libraries
    Parameters:
    
    - lst      list of additional dictionaries to be load
    - verbose  flag for verbose execution 

    return list of properly loaded dictionaries 
    """
    if type(lst) is str : lst = [lst] 
    ## get the libraries from argument
    _libs_ = sets.Set( lst )
    ## get the patterns form the environment 
    for k in os.environ.keys() :
        i = k.find('DictShr')
        if 0 < i : _libs_.add( k[0:k.find('Shr')] )
    if verbose : print ' Libraries to be loaded: %s' % list(_libs_)
    good = sets.Set()
    # for ROOT < 5.13.04c EventAssocLoad must be loaded AFTER 
    if 'EventAssocDict' in _libs_ :
        _libs_.remove ( 'EventAssocDict' )
    for _lib_ in _libs_ :
        try    :
            gaudimodule.loaddict( _lib_ )
            good.add(_lib_)
        except :
            print 'Error Loading the Dictionary "%s"' % _lib_
    ## return the list of properly loaded dictionaries
    good = list(good) 
    if verbose: print ' Successfully loaded dictionaries: %s' % good
    return good 

## Load list of DLLs
#  @param lst list of DLLs to be loaded or library name 
#  @param appMgr application manager
#  @return list of successfully loaded libraries 
def _loadDll_ ( lst , appMgr = None ) :
    """
    Load list of DLLs
    
    - lst list of DLLs to be loaded or library name 
    - appMgr application manager
    
    return list of successfully loaded DLLs
    
    """
    ## library or list of libraries? 
    if type(lst) == str : lst= [lst] 
    ## create application manager if needed 
    if not appMgr : appMgr = gaudimodule.AppMgr()
    ## get the loaded libraries from ApplicationManager
    _DLLs = sets.Set( appMgr.DLLs )    
    ## get the libraries from argument, except already loaded libraries 
    libs = [ l for l in lst if not l in _DLLs ]
    ## remove duplicates (if any) 
    libs = list ( sets.Set ( libs ) )
    ## load libraries (if needed) 
    if libs : appMgr.DLLs += libs
    ## return list of successfully loaded libraries 
    return libs

# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.19  2006/11/13 10:33:03  ibelyaev
#  add python/benderfunctions.py
#
# Revision 1.18  2006/11/09 19:04:23  ibelyaev
#  fix
#
# =============================================================================
# The END 
# =============================================================================
