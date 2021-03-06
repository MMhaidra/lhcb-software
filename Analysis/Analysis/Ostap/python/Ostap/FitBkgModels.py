#!/usr/bin/env python
# -*- coding: utf-8 -*-
# =============================================================================
# $Id$
# =============================================================================
## @file FitBkgModels.py
#
#  Set of useful smooth 1D-models to describe background distribtions
#
#  @author Vanya BELYAEV Ivan.Belyaeve@itep.ru
#  @date 2011-07-25
# 
#                    $Revision$
#  Last modification $Date$
#                 by $Author$
# =============================================================================
"""Set of useful smooth 1D-models to describe background distributions"""
# =============================================================================
__version__ = "$Revision:"
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__date__    = "2011-07-25"
__all__     = (
    'Bkg_pdf'         , ## An exponential function, modulated by positive polynomial
    'PSPol_pdf'       , ## A phase space  function, modulated by positive polynomial
    'PolyPos_pdf'     , ## A positive polynomial
    'Monothonic_pdf'  , ## A positive monothonic polynomial
    'Convex_pdf'      , ## A positive polynomial with fixed sign first and second derivatives 
    'ConvexOnly_pdf'  , ## A positive polynomial with fixed sign second derivatives 
    'Sigmoid_pdf'     , ## Background: sigmoid modulated by positive polynom 
    'TwoExpoPoly_pdf' , ## difference of two exponents, modulated by positive polynomial
    ##
    'PSpline_pdf'     , ## positive            spline 
    'MSpline_pdf'     , ## positive monothonic spline 
    'CSpline_pdf'     , ## positive monothonic convex or concave spline 
    'CPSpline_pdf'    , ## positive convex or concave spline 
    ##
    'PS2_pdf'         , ## 2-body phase space (no parameters)
    'PSLeft_pdf'      , ## Low  edge of N-body phase space 
    'PSRight_pdf'     , ## High edge of L-body phase space from N-body decays  
    'PSNL_pdf'        , ## L-body phase space from N-body decays  
    'PS23L_pdf'       , ## 2-body phase space from 3-body decays with orbital momenta
    ##
    "makeBkg"         , ## helper function to create "background"
    )
# =============================================================================
import ROOT, math
from   Ostap.Core       import cpp,iszero  
from   Ostap.FitBasic   import makeVar, PDF 
# =============================================================================
from   Ostap.Logger     import getLogger
if '__main__' ==  __name__ : logger = getLogger ( 'Ostap.FitBkgModels' )
else                       : logger = getLogger ( __name__             )
# =============================================================================
logger.debug ( __doc__ )
models = []
# =============================================================================
## @class  Bkg_pdf
#  The exponential modified with the positive polynomial 
#  @see Analysis::Models::ExpoPositive
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class Bkg_pdf(PDF) :
    """
    Exponential function, modulated by the positive polynomial:
    
    f(x) ~ exp(-tau*X) * Pol_n(x)
    where Pol_n(x) is POSITIVE polynomial (Pol_n(x)>=0 over the whole range) 
    
    >>>  mass = ROOT.RooRealVar( ... ) 
    >>>  bkg  = Bkg_pdf ( 'B' , mass , power = 3 )
    
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the variable
                   power = 0        ,   ## degree of polynomial
                   tau   = None     ) : ##  
        #
        PDF.__init__  ( self , name )
        #                
        self.mass  = mass
        self.power = power
        #
        mn,mx   = mass.minmax()
        mc      = 0.5 * ( mn + mx )
        taumax  = 100
        #
        if not iszero ( mn ) : taumax =                100.0 / abs ( mn ) 
        if not iszero ( mc ) : taumax = min ( taumax , 100.0 / abs ( mc ) )
        if not iszero ( mx ) : taumax = min ( taumax , 100.0 / abs ( mx ) )
        # 
        ## the exponential slope
        #
        self.tau  = makeVar ( tau              ,
                              "tau_%s"  % name ,
                              "tau(%s)" % name , tau , 0 , -taumax, taumax )
        #
        # 
        if 0 >= self.power :
            
            self.phis     = []
            self.phi_list = ROOT.RooArgList ()
            self.pdf      = ROOT.RooExponential (
                'exp_%s' % name  , 'exp(%s)' % name , mass , self.tau )
            
        else :
            
            # 
            self.makePhis ( power ) 
            #
            
            self.pdf  = cpp.Analysis.Models.ExpoPositive (
                'expopos_%s'  % name ,
                'expopos(%s)' % name ,
                mass                 ,
                self.tau             ,
                self.phi_list        ,
                mass.getMin()        ,
                mass.getMax()        )

models.append ( Bkg_pdf ) 
# =============================================================================
## @class  PSPol_pdf
#  Phase space function modulated with the positive polynomial 
#  @see Analysis::Models::PhaseSpacePol
#  @see Gaudi::Math::PhaseSpacePol
#  @see Gaudi::Math::PhaseSpaceNL 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class PSPol_pdf(PDF) :
    """
    The phase space function modified with positive polynomial 
    
    f(x) ~ PS( x ) * Pol_n(x)
    where
    - PS       is a phase-space function
    - Pol_n(x) is POSITIVE polynomial (Pol_n(x)>=0 over the whole range) 

    >>>  mass = ROOT.RooRealVar( ... )
    >>>  ps   = cpp.Gaudi.Math.PhaseSpaceNL ( low , high , 3 , 4 )  
    >>>  bkg  = PSPol_pdf ( 'B' , mass , ps , power = 2 )
    
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the varibale 
                   phasespace       ,   ## Gaudi::Math::PhaseSpaceNL 
                   power = 1        ) : ## degree of the polynomial
        
        #
        PDF.__init__  ( self , name )
        #
        self.mass  = mass  
        self.ps    = phasespace  ## Gaudi::Math::PhaseSpaceNL
        self.power = power
        
        # 
        self.makePhis ( power ) 
        #
            
        self.pdf  = cpp.Analysis.Models.PhaseSpacePol (
            'pspol_%s'          % name ,
            'PhaseSpacePol(%s)' % name ,
            self.mass            ,
            self.ps              ,  ## Gaudi::Math::PhaseSpaceNL 
            self.phi_list        )

models.append ( PSPol_pdf ) 
# =============================================================================
## @class  TwoExpoPoly_pdf
#  Difference of two exponents, modulated by positive polynomial 
#  @see Analysis::Models::TwoExpoPositive
#  @see Gaudi::Math::TwoExpoPositive
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2015-03-26
class TwoExpoPoly_pdf(PDF) :
    """
    Difference of two exponential function, modulated by the positive polynomial:
    
    f(x) ~ ( exp(-alpha*x) - exp(-(alpha_delta)*x) *  Pol_n(x)
    where Pol_n(x) is POSITIVE polynomial (Pol_n(x)>=0 over the whole range) 
    
    >>>  mass = ROOT.RooRealVar( ... ) 
    >>>  bkg  = TwoExpoPolu_pdf ( 'B' , mass , alpah = 1 , delta = 1 , x0 = 0 , power = 3 )
    
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the variable
                   alpha = None     ,   ## the slope of the first exponent 
                   delta = None     ,   ## (alpha+delta) is the slope of the first exponent
                   x0    = 0        ,   ## f(x)=0 for x<x0 
                   power = 0        ,   ## degree of polynomial
                   tau   = None     ) : ##  
        #
        PDF.__init__  ( self , name )
        #                
        self.mass  = mass
        self.power = power
        #
        mn,mx   = mass.minmax()
        mc      = 0.5 * ( mn + mx )
        taumax  = 100
        #
        if not iszero ( mn ) : taumax =                100.0 / abs ( mn ) 
        if not iszero ( mc ) : taumax = min ( taumax , 100.0 / abs ( mc ) )
        if not iszero ( mx ) : taumax = min ( taumax , 100.0 / abs ( mx ) )
        # 
        ## the exponential slope
        #
        self.alpha  = makeVar ( alpha               ,
                                "alpha_%s"   % name ,
                                "#alpha(%s)" % name , alpha , 1 , 0 , taumax )
        
        self.delta  = makeVar ( delta               ,
                                "delta_%s"   % name ,
                                "#delta(%s)" % name , delta , 1 , 0 , taumax )
        
        self.x0     = makeVar ( x0                 ,
                                "x0_%s"     % name ,
                                "x_{0}(%s)" % name , x0  ,
                                mn , mn-0.5*(mx-mn) , mx+0.5*(mx-mn) ) 
        #
        # 
        if 0 >= self.power :
            
            self.phis     = []
            self.phi_list = ROOT.RooArgList ()
            self.pdf      = ROOT.RooExponential (
                'exp_%s' % name  , 'exp(%s)' % name , mass , self.tau )
            
        else :
            
            # 
            self.makePhis ( power ) 
            #
            
            self.pdf  = cpp.Analysis.Models.TwoExpoPositive (
                '2expopos_%s'  % name ,
                '2expopos(%s)' % name ,
                mass                  ,
                self.alpha            ,
                self.delta            ,
                self.x0               ,
                self.phi_list         ,
                mass.getMin()         ,
                mass.getMax()         )

models.append ( TwoExpoPoly_pdf ) 
# =============================================================================
## @class  PolyPos_pdf
#  A positive polynomial 
#  @see Analysis::Models::PolyPositive 
#  @see Gaudi::Math::Positive
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class PolyPos_pdf(PDF) :
    """
    Positive (Bernstein) polynomial: 
    
    f(x) = Pol_n(x)
    with Pol_n(x)>= 0 over the whole range 
    
    >>>  mass = ROOT.RooRealVar( ... )
    >>>  bkg  = PolyPos_pdf ( 'B' , mass , power = 2 )
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the varibale 
                   power = 1        ) : ## degree of the polynomial
        #
        PDF.__init__ ( self , name )
        #
        self.power = power
        self.mass  = mass 
        
        #
        self.makePhis ( power ) 
            
        self.pdf  = cpp.Analysis.Models.PolyPositive (
            'pp_%s'            % name ,
            'PolyPositive(%s)' % name ,
            self.mass            ,
            self.phi_list        ,
            self.mass.getMin()   ,
            self.mass.getMax()   ) 
        
models.append ( PolyPos_pdf ) 
# =============================================================================
## @class  Monothonic_pdf
#  A positive monothonic polynomial 
#  @see Analysis::Models::PolyMonothonic 
#  @see Gaudi::Math::Monothonic
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class Monothonic_pdf(PDF) :
    """
    Positive monothonic (Bernstein) polynomial:
    
    f(x) = Pol_n(x)
    with f(x)>= 0 over the whole range and
    the derivative f' do not change the sign
    
    >>>  mass = ROOT.RooRealVar( ... )
    
    # increasing background 
    >>>  bkg_inc  = Monothonic_pdf ( 'B1' , mass , power = 2 , increasing = True  )
    
    # decreasing background 
    >>>  bkg_dec  = Monothonic_pdf ( 'B2' , mass , power = 2 , increasing = False  )
    """
    ## constructor
    def __init__ ( self              ,
                   name              ,   ## the name 
                   mass              ,   ## the variable
                   power = 2         ,   ## degree of the polynomial
                   increasing = True ) : ## increasing or decreasing ?
        #
        PDF.__init__ ( self , name )
        #
        self.power      = power
        self.mass       = mass 
        self.increasing = increasing
        # 
        self.makePhis ( power ) 
            
        self.pdf  = cpp.Analysis.Models.PolyMonothonic (
            'pp_%s'              % name ,
            'PolyMonothonic(%s)' % name ,
            self.mass            ,
            self.phi_list        ,
            self.mass.getMin()   ,
            self.mass.getMax()   ,
            self.increasing      )
        
        
models.append ( Monothonic_pdf ) 
# =============================================================================
## @class  Convex_pdf
#  A positive polynomial with fixed signs of the first and second derivative 
#  @see Analysis::Models::PolyConvex 
#  @see Gaudi::Math::Convex
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class Convex_pdf(PDF) :
    """
    Positive monothonic (Bernstein) polynomial with fixed-sign second derivative:
    
    f(x) = Pol_n(x)
    with f(x)>= 0 over the whole range and
    the both frist derivative f' and second derivative f'' do not change the sign
    
    >>>  mass = ROOT.RooRealVar( ... )
    
    # increasing convex background 
    >>>  bkg  = Convex_pdf ( 'B1' , mass , power = 2 , increasing = True  , convex = True  )

    # dereasing convex background 
    >>>  bkg  = Convex_pdf ( 'B2' , mass , power = 2 , increasing = False , convex = True  )

    # increasing concave background 
    >>>  bkg  = Convex_pdf ( 'B3' , mass , power = 2 , increasing = True  , convex = False )

    # dereasing concave background 
    >>>  bkg  = Convex_pdf ( 'B3' , mass , power = 2 , increasing = False , convex = False )   
    """
    ## constructor
    def __init__ ( self              ,
                   name              ,   ## the name 
                   mass              ,   ## the variable
                   power = 2         ,   ## degree of the polynomial
                   increasing = True ,   ## increasing or decreasing ?
                   convex     = True ) : ## convex or concave ?
        #
        PDF.__init__ ( self , name )
        #
        self.power      = power
        self.mass       = mass 
        self.increasing = increasing
        self.convex     = convex  
        # 
        self.makePhis ( power ) 
            
        self.pdf  = cpp.Analysis.Models.PolyConvex (
            'pp_%s'          % name ,
            'PolyConvex(%s)' % name ,
            self.mass            ,
            self.phi_list        ,
            self.mass.getMin()   ,
            self.mass.getMax()   ,
            self.increasing      ,
            self.convex          )

models.append ( Convex_pdf ) 
# =============================================================================
## @class  ConvexOnly_pdf
#  A positive polynomial with fixed signs of the second derivative 
#  @see Analysis::Models::PolyConvexOnly 
#  @see Gaudi::Math::ConvexOnly
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class ConvexOnly_pdf(PDF) :
    """ Positive (Bernstein) polynomial with fixed-sign second derivative:
    
    f(x) = Pol_n(x)
    with f(x)>= 0 over the whole range and
    the second derivative f'' do not change the sign
    
    >>>  mass = ROOT.RooRealVar( ... )
    
    >>>  bkg  = ConvexOnly_pdf ( 'B1' , mass , power = 2 , convex = True  )
    >>>  bkg  = ConvexOnly_pdf ( 'B1' , mass , power = 2 , convex = False  )
    """
    ## constructor
    def __init__ ( self              ,
                   name              ,   ## the name 
                   mass              ,   ## the variable
                   power = 2         ,   ## degree of the polynomial
                   convex     = True ) : ## convex or concave ?
        #
        PDF.__init__ ( self , name )
        #
        self.power      = power
        self.mass       = mass 
        self.convex     = convex  
        # 
        self.makePhis ( power ) 
            
        self.pdf  = cpp.Analysis.Models.PolyConvexOnly (
            'pp_%s'          % name ,
            'PolyConvex(%s)' % name ,
            self.mass            ,
            self.phi_list        ,
            self.mass.getMin()   ,
            self.mass.getMax()   ,
            self.convex          )

models.append ( ConvexOnly_pdf ) 
# =============================================================================
## @class  Sigmoid_pdf
#  Sigmoid function modulated wit hpositive polynomial
#  @see Analysis::Models::PolySigmoid
#  @see Gaudi::Math::Sigmoid
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class Sigmoid_pdf(PDF) :
    """
    A sigmoid function modulated by positive (Bernstein) polynomial 
    
    f(x) = 0.5*(1+tahn(alpha(x-x0))*Pol_n(x)
    
    """
    ## constructor
    def __init__ ( self              ,
                   name              ,   ## the name 
                   mass              ,   ## the variable
                   power = 2         ,   ## degree of the polynomial
                   alpha = None      ,   ##
                   x0    = None      ) :
        #
        PDF.__init__ ( self , name )
        #
        self.power      = power
        self.mass       = mass

        xmin  = mass.getMin()
        xmax  = mass.getMax()
        dx    = xmax - xmin 
        alpmx = 2000.0/dx 
        
        self.alpha  = makeVar ( alpha               ,
                                  'alpha_%s'  % name  ,
                                  'alpha(%s)' % name  ,
                                  alpha               , 0 , -alpmx , alpmx ) 
        
        self.x0    = makeVar  ( x0               ,
                                'x0_%s'  % name  ,
                                'x0(%s)' % name  ,
                                x0               ,
                                0.5*(xmax+xmin)  ,
                                xmin - 0.1 * dx  ,
                                xmax + 0.1 * dx  ) 
        # 
        self.makePhis ( power ) 
            
        self.pdf  = cpp.Analysis.Models.PolySigmoid (
            'ps_%s'           % name ,
            'PolySigmoid(%s)' % name ,
            self.mass            ,
            self.phi_list        ,
            self.mass.getMin()   ,
            self.mass.getMax()   ,
            self.alpha           ,
            self.x0              )

        
models.append ( Sigmoid_pdf ) 
# =============================================================================
## @class  PSpline_pdf
#  The special spline for non-negative function
#  Actually it is a sum of M-splines with non-negative coefficients 
#  \f$ f(x) = \sum_i \alpha_i * M_i^k(x) \f$,
#  with constraints  \f$  \sum_i \alpha_i=1\f$ and \f$ 0 \le \alpha_i\f$.
#  @see Analysis::Models::PositiveSpline 
#  @see Gaudi::Math::PositiveSpline 
#  @see http://en.wikipedia.org/wiki/M-spline
#  @see http://en.wikipedia.org/wiki/B-spline
#  The constrains are implemented as N-sphere
#  @see Gaudi::Math::NSphere
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class PSpline_pdf(PDF) :
    """
    A positive spline, a composion of M-splines with non-negative coefficients

    >>> mass   = ... ## the variable
    >>> order  = 3   ## spline order
    
    ## create uniform spline 
    >>> inner  = 3   ## number of inner knots between min and max 
    >>> spline = Gaudi.Math.PositiveSpline( mass.xmin() , mass.xmax() , inner , order )

    ## create non-uniform spline with
    >>> knots = std.vector('double)()
    >>> knots.push_back ( mass.xmin() )
    >>> knots.push_back ( mass.xmax() )
    >>> knots.push_back ( ... )
    >>> spline = Gaudi.Math.PositiveSpline( knots , order )

    >>> bkg = PSpline_pdf ( 'Spline' , mass , spline ) 
    
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the variable
                   spline           ) : ## the spline object Gaudi::Math::PositiveSpline
        #
        PDF.__init__ ( self , name )
        #
        self.spline = spline 
        self.mass   = mass 
        
        # 
        self.makePhis ( spline.npars()  ) 
        #
            
        self.pdf  = cpp.Analysis.Models.PositiveSpline (
            'ps_%s'              % name ,
            'PositiveSpline(%s)' % name ,
            self.mass            ,
            self.spline          , 
            self.phi_list        )

models.append ( PSpline_pdf ) 
# =============================================================================
## @class  MSpline_pdf
#  The special spline for non-negative monothonic function
#  @see Analysis::Models::MonothonicSpline 
#  @see Gaudi::Math::MonothonicSpline 
#  @see http://en.wikipedia.org/wiki/I-spline
#  @see http://en.wikipedia.org/wiki/M-spline
#  @see http://en.wikipedia.org/wiki/B-spline
#  @see http://link.springer.com/chapter/10.1007%2F978-3-0348-7692-6_6
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class MSpline_pdf(PDF) :
    """
    A positive monothonic spline

    >>> mass   = ... ## the variable
    >>> order  = 3   ## spline order
    
    ## create uniform spline 
    >>> inner  = 3   ## number of inner knots between min and max 
    >>> spline = Gaudi.Math.MonothonicSpline( mass.xmin() , mass.xmax() , inner , order , True )

    ## create non-uniform spline with
    >>> knots = std.vector('double)()
    >>> knots.push_back ( mass.xmin() )
    >>> knots.push_back ( mass.xmax() )
    >>> knots.push_back ( ... )
    >>> spline = Gaudi.Math.MonothonicSpline( knots , order , True )

    >>> bkg = MSpline_pdf ( 'Spline' , mass , spline ) 
    
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the variable
                   spline           ) : ## the spline object Gaudi::Math::MonothonicSpline
        #
        PDF.__init__ ( self , name )
        #
        self.spline = spline 
        self.mass   = mass 
        
        # 
        self.makePhis ( spline.npars()  ) 
        #
        self.pdf  = cpp.Analysis.Models.MonothonicSpline (
            'is_%s'                % name ,
            'MonothonicSpline(%s)' % name ,
            self.mass                     ,
            self.spline                   , 
            self.phi_list                 )

models.append ( MSpline_pdf )

# =============================================================================
## @class  CSpline_pdf
#  The special spline for non-negative monothonic convex/concave function
#  @see Analysis::Models::ConvexSpline 
#  @see Gaudi::Math::ConvexSpline 
#  @see http://en.wikipedia.org/wiki/I-spline
#  @see http://en.wikipedia.org/wiki/M-spline
#  @see http://en.wikipedia.org/wiki/B-spline
#  @see http://link.springer.com/chapter/10.1007%2F978-3-0348-7692-6_6
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class CSpline_pdf(PDF) :
    """
    A positive monothonic spline

    >>> mass   = ... ## the variable
    >>> order  = 3   ## spline order
    
    ## create uniform spline 
    >>> inner  = 3   ## number of inner knots between min and max 
    >>> spline = Gaudi.Math.ConvexSpline( mass.xmin() , mass.xmax() , inner , order , True , True )

    ## create non-uniform spline with
    >>> knots = std.vector('double)()
    >>> knots.push_back ( mass.xmin() )
    >>> knots.push_back ( mass.xmax() )
    >>> knots.push_back ( ... )
    >>> spline = Gaudi.Math.ConvexSpline( knots , order, True  , True )

    >>> bkg = MSpline_pdf ( 'Spline' , mass , spline ) 
    
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the variable
                   spline           ) : ## the spline object Gaudi::Math::ConvexSpline
        #
        PDF.__init__ ( self , name )
        #
        self.spline = spline 
        self.mass   = mass 
        
        # 
        self.makePhis ( spline.npars()  ) 
        #
        self.pdf  = cpp.Analysis.Models.ConvexSpline (
            'is_%s'            % name ,
            'ConvexSpline(%s)' % name ,
            self.mass                ,
            self.spline              , 
            self.phi_list            )

models.append ( CSpline_pdf ) 


# =============================================================================
## @class  CPSpline_pdf
#  The special spline for non-negative convex/concave function
#  @see Analysis::Models::ConvexOnlySpline 
#  @see Gaudi::Math::ConvexOnlySpline 
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class CPSpline_pdf(PDF) :
    """
    A positive convex/concave spline

    >>> mass   = ... ## the variable
    >>> order  = 3   ## spline order
    
    ## create uniform spline 
    >>> inner  = 3   ## number of inner knots between min and max 
    >>> spline = Gaudi.Math.ConvexOnlySpline( mass.xmin() , mass.xmax() , inner , order , True )

    ## create non-uniform spline with
    >>> knots = std.vector('double)()
    >>> knots.push_back ( mass.xmin() )
    >>> knots.push_back ( mass.xmax() )
    >>> knots.push_back ( ... )
    >>> spline = Gaudi.Math.ConvexonlySpline( knots , order, True )

    >>> bkg = CPSpline_pdf ( 'Spline' , mass , spline ) 
    
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the variable
                   spline           ) : ## the spline object Gaudi::Math::ConvexOnlySpline
        #
        PDF.__init__ ( self , name )
        #
        self.spline = spline 
        self.mass   = mass 
        
        # 
        self.makePhis ( spline.npars()  ) 
        #
        self.pdf  = cpp.Analysis.Models.ConvexOnlySpline (
            'is_%s'                % name ,
            'ConvexOnlySpline(%s)' % name ,
            self.mass                ,
            self.spline              , 
            self.phi_list            )

models.append ( CPSpline_pdf ) 

# =============================================================================
## @class  PS2_pdf
#  Primitive 2-body phase space function 
#  @see Analysis::Models::PhaseSpace2
#  @see Gaudi::Math::PhaseSpace2
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class PS2_pdf(PDF) :
    """
    Primitive 2-body phase space function
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the varibale
                   m1               ,   ## the first  mass (constant)
                   m2               ) : ## the second mass (constant)
        #
        ## initialize the base 
        PDF.__init__ ( self , name )
        #
        if mass.getMax() < abs ( m1 ) + abs ( m2 ) :
            logger.error('PS2_pdf(%s): senseless setting of edges/threshold' % self.name ) 

        self.mass = mass
        
        self.pdf  = cpp.Analysis.Models.PhaseSpace2 (
            'ps2_%s'          % name ,
            'PhaseSpace2(%s)' % name ,
            self.mass            ,
            m1  , m2 )

models.append ( PS2_pdf ) 
# =============================================================================
## @class  PSLeft_pdf
#  Left edge of N-body phase space
#  @see Analysis::Models::PhaseSpaceLeft
#  @see Gaudi::Math::PhaseSpaceLeft
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class PSLeft_pdf(PDF) :
    """
    Left edge of N-body phase space function
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the variable
                   N                ,   ## N 
                   left   = None    ) : 
        #
        ## initialize the base 
        PDF.__init__ ( self , name )
        #
        self.mass = mass
        self.left = makeVar ( left                ,
                              'left_%s'    % name ,
                              'm_left(%s)' % name ,
                              None , mass.getMin() , mass.getMax() )

        if self.left.getMin() >= self.mass.getMax() :
            logger.error('PSLeft_pdf(%s): invalid setting!' % name )
            
        self.pdf  = cpp.Analysis.Models.PhaseSpaceLeft (
            'psl_%s'             % name ,
            'PhaseSpaceLeft(%s)' % name ,
            self.mass ,
            self.left ,
            N         ) 

models.append ( PSLeft_pdf ) 
# =============================================================================
## @class  PSRight_pdf
#  Right edge of L-body phase space fro N-body decay 
#  @see Analysis::Models::PhaseSpaceRight
#  @see Gaudi::Math::PhaseSpaceRight
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class PSRight_pdf(PDF) :
    """
    Right edge of L-body phase space for N-body decay 
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the variable
                   L                ,   ## L  
                   N                ,   ## N
                   right   = None   ) : 
        #
        ## initialize the base 
        PDF.__init__ ( self , name )
        #
        self.mass  = mass
        self.right = makeVar ( right ,
                               'right_%s'      % name ,
                               'm_{right}(%s)' % name ,
                               None , mass.getMin() , mass.getMax() )
        
        if self.right.getMax() <= self.mass.getMax() :
            logger.error('PSRight_pdf(%s): invalid setting!' % name )
            
        self.pdf  = cpp.Analysis.Models.PhaseSpaceRight (
            'psr_%s'              % name ,
            'PhaseSpaceRight(%s)' % name ,
            self.mass  ,
            self.right ,
            L          , 
            N          ) 

models.append ( PSRight_pdf ) 
# =============================================================================
## @class  PSNL_pdf
#  L-body phase space from N-body decay 
#  @see Analysis::Models::PhaseSpaceNL
#  @see Gaudi::Math::PhaseSpaceNL
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class PSNL_pdf(PDF) :
    """
    L-body phase space from N-body decay 
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the variable 
                   L                ,   ## L  
                   N                ,   ## N
                   left  = None     , 
                   right = None     ) : 
        ##
        #
        ## initialize the base 
        PDF.__init__ ( self , name )
        #
        self.mass  = mass
        #
        mmin = mass.getMin()
        mmax = mass.getMax()
        #
        self.left  = makeVar ( left ,
                               'left_%s'        % name ,
                               'm_{left}(%s)'   % name , left  , 
                               0.9 * mmin + 0.1 * mmax ,
                               mmin ,
                               mmax ) 
        
        self.right = makeVar ( right ,
                               'right_%s'       % name ,
                               'm_{right}(%s)'  % name , right , 
                               0.1 * mmin + 0.9 * mmax ,
                               mmin ,
                               mmax ) 
        
        if self.left.getMin()  >= self.mass.getMax() :
            logger.error('PSNL_pdf(%s): invalid setting!' % name )
            
        if self.right.getMax() <= self.mass.getMax() :
            logger.error('PSNL_pdf(%):  invalid setting!' % name )

        self.pdf  = cpp.Analysis.Models.PhaseSpaceNL (
            'psnl_%s'          % name ,
            'PhaseSpaceNL(%s)' % name ,
            self.mass  ,
            self.left  ,
            self.right ,
            L          , 
            N          ) 


models.append ( PSNL_pdf ) 
# =============================================================================
## @class  PS23L_pdf
#  2-body phase space from 3-body decay with orbital momenta 
#  @see Analysis::Models::PhaseSpace23L
#  @see Gaudi::Math::PhaseSpace23L
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2011-07-25
class PS23L_pdf(PDF) :
    """
    2-body phase space from 3-body decay with orbital momenta 
    """
    ## constructor
    def __init__ ( self             ,
                   name             ,   ## the name 
                   mass             ,   ## the variable
                   m1               ,   ## mass the first particle  (const)
                   m2               ,   ## mass the second particle (const)
                   m3               ,   ## mass the third particle  (const)
                   m                ,   ## mass of the whole system (const)
                   L                ,   ## orbital momenutm between (1,2) and 3
                   l  = 0           ) : ## orbital momentum between 1 and 2

        #
        ## initialize the base 
        PDF.__init__ ( self , name )
        #
        self.mass = mass        
        self.pdf  = cpp.Analysis.Models.PhaseSpace23L (
            'ps23l_%s'          % name ,
            'PhaseSpace23L(%s)' % name ,
            self.mass  ,
            m1 , m2 , m3 , m , L , l )
        
models.append ( PS23L_pdf ) 



# =============================================================================
## helper function to create "background"

# =============================================================================
## create simple background model
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
#  @date 2015-04-03
def makeBkg ( bkg , name , xvar , **kwargs ) :
    """
    Helper function to create background models (around Bkg_pdf)
    
    >>> x =   .. ## the variable
    
    ## non-negative integer, construct PDF using Bkg_pdf 
    >>> bkg1  = makeBkg ( 3      , 'B1' , x ) ## use Bkg_pdf ( 'B1' , x , power = 3 )
    
    ## generic RooAbsPdf: use this PDF
    >>> pdf   = RooPolynomial( ... )
    >>> bkg2  = makeBkg ( pdf    , 'B2' , x ) ## use Generic1D_pdf ( pdf , x , 'B2' )
    
    ## some Ostap-based model, use it as it is  
    >>> model = Convex_pdf ( ... )
    >>> bkg3  = makeBkg ( models , 'B3' , x )  
    
    ## some RooAbsReal, use is as exponenial slope for Bkg_pdf
    >>> tau  = RooRealVar( ....  )
    >>> bkg4 = makeBkg ( tau     , 'B4' , x ) 
    
    """

    ## Regular case: degree of polynom in Bkg_pdf 
    if isinstance ( bkg , ( int , long ) ) and 0 <= bkg :
        
        model = Bkg_pdf ( name , power = bkg , mass = xvar , **kwargs )
        return model
    
    ## native RooFit pdf ? 
    elif isinstance ( bkg , ROOT.RooAbsPdf ) :
        
        from Ostap.FitBasic import Generic1D_pdf 
        model = Generic1D_pdf ( bkg , varx = xvar , name = name ) 
        return model
    
    ## some Ostap-based model ?
    elif hasattr    ( bkg , 'pdf' ) and isinstance ( bkg.pdf , ROOT.RooAbsPdf ) :
        
        model = bkg
        return model
    
    ## interprete it as exponential slope for Bkg-pdf 
    elif isinstance ( bkg , ROOT.RooAbsReal ) :

        model = Bkg_pdf ( name , mass = xvar , tau = bkg , **kwargs )
        return model
    
    raise  TypeError("Wrong type of bkg object: %s/%s " % ( bkg , type(bkg) ) )


# =============================================================================
if '__main__' == __name__ :
    
    import Ostap.Line
    logger.info ( __file__  + '\n' + Ostap.Line.line  ) 
    logger.info ( 80*'*'   )
    logger.info ( __doc__  )
    logger.info ( 80*'*'   )
    logger.info ( ' Author  : %s' %         __author__    ) 
    logger.info ( ' Version : %s' %         __version__   ) 
    logger.info ( ' Date    : %s' %         __date__      )
    logger.info ( ' Symbols : %s' %  list ( __all__     ) )
    logger.info ( 80*'*' ) 
    for m in models : logger.info ( 'Model %s: %s' % ( m.__name__ ,  m.__doc__  ) ) 
    logger.info ( 80*'*' ) 
        
# =============================================================================
# The END 
# =============================================================================
