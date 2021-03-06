#!/usr/bin/env python
# =============================================================================
# $Id$ 
# =============================================================================
## @file LoKiCore/math.py
#  helper file for redefine the standard mathematical functions
#  using pseudo-operators
#
#        This file is a part of LoKi project - 
#    "C++ ToolKit  for Smart and Friendly Physics Analysis"
#
#  The package has been designed with the kind help from
#  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
#  contributions and advices from G.Raven, J.van Tilburg, 
#  A.Golutvin, P.Koppenburg have been used in the design.
#
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
#
#  @author Vanya BELYAEV ibelyaev@physics.syr.edu
#  @date 2007-07-17
# =============================================================================
"""
The set of basic math for LoKi objects

      This file is a part of LoKi project - 
``C++ ToolKit  for Smart and Friendly Physics Analysis''

The package has been designed with the kind help from
Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
contributions and advices from G.Raven, J.van Tilburg, 
A.Golutvin, P.Koppenburg have been used in the design.

"""
# =============================================================================
__author_   = "Vanya BELYAEV ibelyaev@physics.syr.edu"
__date__    = "2007-01-17"
__version__ = "CVS Tag: $Name: not supported by cvs2svn $, version $Revision$ "
# =============================================================================
import math


def sin   ( o ) :
    if hasattr ( o , "__sin__" ) : return o.__sin__()
    return math.sin(o)
def cos   ( o ) :
    if hasattr ( o , "__cos__" ) : return o.__cos__()
    return math.cos(o)
def tan   ( o ) :
    if hasattr ( o , "__tan__" ) : return o.__tan__()
    return math.tan(o)

def sinh  ( o ) :
    if hasattr ( o , "__sinh__"  ) : return o.__sinh__()
    return math.sinh(o)
def cosh  ( o ) :
    if hasattr ( o , "__cosh__"  ) : return o.__cosh__()
    return math.cosh(o)
def tanh  ( o ) :
    if hasattr ( o , "__tanh__"  ) : return o.__tanh__()
    return math.tanh(o)

def asin  ( o ) :
    if hasattr ( o , "__asin__"  ) : return o.__asin__()
    return math.sin(o)
def acos  ( o ) :
    if hasattr ( o , "__acos__"  ) : return o.__acos__()
    return math.cos(o)
def atan  ( o , *a ) :
    if hasattr ( o , "__atan__"  ) : return o.__atan__( *a )
    if 0 == len(*a)                : return math.tan(o)
    return math.atan2(o,*a)
def atan2 ( o ,  a ) :
    if hasattr ( o , "__atan2__" ) : return o.__atan__( a )
    return math.atan2(o,a)

def exp   ( o ) :
    if hasattr ( o , "__exp__"   ) : return o.__exp__   ()
    return math.exp(o)
def log   ( o ) :
    if hasattr ( o , "__log__"   ) : return o.__log__   ()
    return math.log(o)
def log10 ( o ) :
    if hasattr ( o , "__log10__" ) : return o.__log10__ ()
    return math.log10(o)
def sqrt  ( o ) :
    if hasattr ( o , "__sqrt__"  ) : return o.__sqrt__  ()
    return math.sqrt(o)
def cbrt  ( o ) :
    if hasattr ( o , "__cbrt__"  ) : return o.__cbrt__  ()
    return pow( o , 1.0/3.0 ) 
def pow2  ( o ) :
    if hasattr ( o , "__pow2__"  ) : return o.__pow2__  ()
    return pow(o,2)
def pow3  ( o ) :
    if hasattr ( o , "__pow3__"  ) : return o.__pow3__  ()
    return pow(o,3)
def pow4  ( o ) :
    if hasattr ( o , "__pow4__"  ) : return o.__pow4__  ()
    return pow(o,4)


def round ( o ) :
    if hasattr ( o , '__round__' ) : return o.__round__()
    return long ( o )

def jbit  ( o , j )       : return o.__jbit__  ( j       ) 
def jbits ( o , j1 , j2 ) : return o.__jbits__ ( j1 , j2 ) 

_std_min_ = min 
def min  ( o , *a ) :
    if not a : return _std_min_ ( o )  
    elif                 hasattr ( o    , "__min__"  ) :
        return o.__min__( *a )
    elif 2 == len(a) and hasattr ( a[0] , "__min__"  ) :
        return a[0].__min__( o )
    return _std_min_( o , *a )

_std_max_ = max 
def max  ( o , *a ) :
    if not a : return _std_max_ ( o )  
    elif                 hasattr ( o    , "__max__"  ) :
        return o.__max__( *a )
    elif 1 == len(a) and hasattr ( a[0] , "__max__"  ) :
        return a[0].__max__( o )
    return _std_max_( o , *a ) 




# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print 80*'*'
    for i in dir() : print i
    
# =============================================================================

# =============================================================================
# The end 
# =============================================================================
