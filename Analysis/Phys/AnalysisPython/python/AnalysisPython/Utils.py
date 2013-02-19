#!/usr/bin/env python
# =============================================================================
# $Id: Utils.py 151154 2013-01-27 11:55:52Z ibelyaev $
# =============================================================================
## @file
#  Module with some simple but useful utilities 
#
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date   2013-02-10
#  
#                    $Revision: 151154 $
#  Last modification $Date: 2013-01-27 12:55:52 +0100 (Sun, 27 Jan 2013) $
#  by                $Author: ibelyaev $
# =============================================================================
"""
Module with some simple but useful utilities 

"""
# =============================================================================
__version__ = "$Revision: 151154 $"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2013-02-10"
# =============================================================================
__all__     = (
    #
    'virtualMemory'  , ## context manager to count virtual memory increase 
    'memory'         , ## ditto
    'clocks'         , ## context manager to count clocks 
    'timing'         , ## context manager to count time 
    'timer'          , ## ditto
    'mute'           , ## contex manager to suppress stdout/strerr printout 
    'silence'        , ## ditto 
)    
# =============================================================================
import ROOT, time, os  
from   GaudiPython.Bindings   import gbl as cpp
# =============================================================================
## @class _Memory
#  Simple context manager to measure the virtual memory increase
#
#  @see System::virtualMemory
#  @code
#
#  with Memory() :
#     <whatever action is>
#     at the exit it prints the chaneg in virtual memory 
#  @endcode
#
# Or:
#
#  @code
#
#  with Memory() as Q :
#     <whatever action is>
#     at the exit it prints the chaneg in virtual memory
#
#  print Q.delta 
# 
#  @endcode
#
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-02-10
class Memory(object):
    """
    Simple class to evaluate the change in virtual memory
    to be used as context manager:
    
    with Memory('here...') :
    #   <whatever action is>
    #    at the exit it prints the change in virtual memory
    
    with Memory('here...') as M :
    #   <whatever action is>
    #    at the exit it prints the change in virtual memory
    
    >>> delta = M.delta
    
    """    
    def __init__  ( self , name = '' ) :
        self.name   = name
    def __enter__ ( self ) :
        self.memory = cpp.System.virtualMemory()
    def __exit__  ( self, exc_type, exc_value, traceback) :
        if not exc_type :
            if not  exc_value :
                if not traceback :
                    self.delta = cpp.System.virtualMemory() - self.memory
                    print  '%s Memory %.1fMB ' % ( self.name , 0.001*self.delta ) 


# ============================================================================
## create the context manager to monitor the virtual memory increase  
def virtualMemory ( name = '' ) :
    """
    Create the context manager to monitor the virtual memory increase:

    with memory('here...') :
    #     <whatever action is>
    #      at the exit it prints the change in virtual memory
          
    with memory('here...') as m :
    #     <whatever action is>
    #      at the exit it prints the change in virtual memory
          
    >>> delta = m.delta
    
    """
    return Memory( name )

## ditto 
memory = virtualMemory  ## ditto

# =============================================================================
## @class Clock
#  Smple context manager to measure the clock counts
#
#  @code
#
#  with Clock() :
#     <whatever action is>
#     at the exit it prints the clock counts 
#  @endcode
#
# Or:
#
#  @code
#
#  with Clock() as c :
#     <whatever action is>
#     at the exit it prints the clock counts 
#
#  print c.delta 
# 
#  @endcode
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-02-10                    
class Clock(object):
    """
    Smple context manager to measure the clock counts
    
    with Clock() :
    #    <whatever action is>
    #    at the exit it prints the clock counts 
    
    with Clock() as c :
    #     <whatever action is>
    #     at the exit it prints the clock counts 
    
    >>> print c.delta 
    """
    def __init__  ( self , name = '' ) :
        self.name   = name
    def __enter__ ( self ) :
            self.clock = time.clock() 
    def __exit__  ( self, exc_type, exc_value, traceback) :
        if not exc_type :
            if not  exc_value :
                if not traceback :
                    self.delta = time.clock() - self.clock
                    print  '%s Clocks %s ' % ( self.name , self.delta ) 

# =============================================================================
## @class Timer
#  Simple context manager to measure the time 
#  @code
#
#  with Timer() :
#     <whatever action is>
#     at the exit it prints the time 
#  @endcode
#
# Or:
#
#  @code
#
#  with Timer() as t :
#     <whatever action is>
#     at the exit it prints the clock counts 
#
#  print ct.delta 
# 
#  @endcode
#
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-02-10
#
class Timer(object):
    """
    Simple context manager to measure the time
    
    with Timer() :
    #     <whatever action is>
    #     at the exit it prints the time 
    
    Or:
    
    with Timer() as t :
    #     <whatever action is>
    #     at the exit it prints the clock counts 
    
    >>> print ct.delta 
    """
    def __init__  ( self , name = '' ) :
        self.name   = name
    def __enter__ ( self ) :
        self.time = time.time() 
    def __exit__  ( self, exc_type, exc_value, traceback) :
        if not exc_type :
            if not  exc_value :
                if not traceback :
                    self.delta = time.time() - self.time
                    print  '%s Time   %s ' % ( self.name , self.delta ) 
                    

# =============================================================================
## Simple context manager to measure the clock counts
#
#  @code
#
#  with clocks () :
#     <whatever action is>
#     at the exit it prints the clock counts
#
#  @endcode
#
# Or:
#
#  @code
#
#  with clocks () as c :
#     <whatever action is>
#     at the exist it prints the clock counts 
#
#  print c.delta 
# 
#  @endcode
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-02-10                    
def clocks ( name = '' ) :
    """
    Simple context manager to measure the clock counts 
    
    with clocks () :
    #     <whatever action is>
    #     at the exit it prints the clock counts 
    
    with clocks () as c :
    #     <whatever action is>
    #     at the exit it prints the clock counts 
    
    >>>print c.delta
    """
    return Clock ( name )


# =============================================================================
## Simple context manager to measure the time
#
#  @code
#
#  with timer () :
#     <whatever action is>
#     at the exit it prints the time
#
#  @endcode
#
# Or: 
#
#  @code
#
#  with timer () as t :
#     <whatever action is>
#     at the exit it prints the clock counts 
#
#  print t.delta 
# 
#  @endcode
#  @author Vanya Belyaev Ivan.Belyaev@itep.ru
#  @date 2013-02-10                    
def timing ( name = '' ) :
    """
    Simple context manager to measure the clock counts 
    
    with timing () :
    #    <whatever action is>
    #    at the exit it prints the clock counts 
    
    with timing () as c :
    #    <whatever action is>
    #    at the exit it prints the clock counts 
    
    >>> print c.delta
    """
    return Timer ( name )

# =============================================================================
## ditto 
timer = timing   # ditto

# ============================================================================
## @class Mute
#  context manager to suppress the printour
#
#  The code is copied from
#    http://stackoverflow.com/questions/11130156/suppress-stdout-stderr-print-from-python-functions
class Mute(object):
    """
    A context manager for doing a ``deep suppression'' of stdout and stderr in 
    Python, i.e. will suppress all print, even if the print originates in a 
    compiled C/Fortran sub-function.
    This will not suppress raised exceptions, since exceptions are printed
    to stderr just before a script exits, and after the context manager has
    exited (at least, I think that is why it lets exceptions through).      
    
    stallen from  
    http://stackoverflow.com/questions/11130156/suppress-stdout-stderr-print-from-python-functions
    """
    def __init__( self , out = True , err = False ):
        # Open a pair of null files
        self.null_fds =  [os.open(os.devnull,os.O_RDWR) for x in range(2)]
        # Save the actual stdout (1) and stderr (2) file descriptors.
        self.save_fds = (os.dup(1), os.dup(2))
        self._out = out
        self._err = err 
        
    def __enter__(self):
        # Assign the null pointers to stdout and stderr.
        if self._out : os.dup2 ( self.null_fds[0] , 1 )
        if self._err : os.dup2 ( self.null_fds[1] , 2 )

    def __exit__(self, *_):
        # Re-assign the real stdout/stderr back to (1) and (2)
        os.dup2(self.save_fds[0],1)
        os.dup2(self.save_fds[1],2)
        # Close the null files
        os.close(self.null_fds[0])
        os.close(self.null_fds[1])

# =============================================================================
## simple context manager to suppress the printout
#
#  @code
#  with mute () :
#          <some code here>
#  @endcode 
def mute ( cout = True , cerr = False )   :
    """
    Simple context manager to suppress the printout
    
    with mute () :
    # <some code here>
    #
    """
    return Mute ( cout , cerr )

# ==============================================================================
## ditto 
silence = mute  # ditto

# =============================================================================
if '__main__' == __name__ :
    
    print 80*'*'
    print __doc__
    print ' Author  : ' , __author__
    print ' Version : ' , __version__
    print ' Date    : ' , __date__    
    print ' Symbols : ' , __all__    
    print 80*'*'
    
# =============================================================================
# The END 
# =============================================================================
