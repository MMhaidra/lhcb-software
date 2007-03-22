# =============================================================================
# $Id: bendergcuts.py,v 1.5 2007-03-22 18:50:47 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.5 $
# =============================================================================
## This is helper module for decoration of LoKi Gen particle functions/cuts 
#
#  This file is a part of 
#  <a href="http://cern.ch/lhcb-comp/Analysis/Bender/index.html">Bender project</a>
#  <b>"Python-based Interactive Environment for Smart and Friendly 
#   Physics Analysis"</b>
#
#  The package has been designed with the kind help from
#  Pere MATO and Andrey TSAREGORODTSEV. 
#  And it is based on the 
#  <a href="http://cern.ch/lhcb-comp/Analysis/LoKi/index.html">LoKi project:</a>
#  "C++ ToolKit for Smart and Friendly Physics Analysis"
#
#  By usage of this code one clearly states the disagreement 
#  with the campain of Dr.O.Callot et al.: 
#  "No Vanya's lines are allowed in LHCb/Gaudi software."
#
#  @date   2004-07-11
#  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
# =============================================================================
""" This is helper module for decoration of LoKi Gen particle functions/cuts  """
# =============================================================================
__author__ = 'Vanya BELYAEV belyaev@lapp.in2p3.fr'
# =============================================================================

import gaudimodule

_LoKi   = gaudimodule.gbl.LoKi
_Bender = gaudimodule.gbl.Bender
_LHCb   = gaudimodule.gbl.LHCb
_GENP   = _LoKi.GenParticles 


# =============================================================================
_GP = 'const HepMC::GenParticle*'
# =============================================================================

## @see LoKi::Cuts::GTRUE
GTRUE     = _LoKi.BooleanConstant( _GP )(True)
## @see LoKi::Cuts::GFALSE
GFALSE    = _LoKi.BooleanConstant( _GP )(False)
## @see LoKi::Cuts::GALL
GALL      = GTRUE
## @see LoKi::Cuts::GNONE
GNONE     = GFALSE
## @see LoKi::Cuts::GONE
GONE      = _LoKi.Constant ( _GP )(1.0)
## @see LoKi::Cuts::GZERO
GZERO     = _LoKi.Constant ( _GP )(0.0)
## @see LoKi::Cuts::GMIN
GMIN      = _LoKi.Min( _GP )
## @see LoKi::Cuts::GMAX
GMAX      = _LoKi.Max( _GP )
## @see LoKi::Cuts::GSWITCH
GSWITCH   = _LoKi.Switch( _GP )
## @see LoKi::Cuts::GSSWITCH
GSSWITCH  = _LoKi.SimpleSwitch( _GP )
## @see LoKi::Cuts::GVALID
GVALID    = _LoKi.Valid(_GP)()
## @see LoKi::Cuts::GSAME
GSAME     = _LoKi.TheSame(_GP)
## @see LoKi::Cuts::GBAR
GBAR      = _LoKi.GenParticles.BarCode()
## @see LoKi::Cuts::GBARCODE 
GBARCODE  = GBAR
## @see LoKi::Cuts::GID
GID       = _LoKi.GenParticles.Identifier()
## @see LoKi::Cuts::GABSID
GABSID    = _LoKi.GenParticles.AbsIdentifier()
## @see LoKi::Cuts::GSTATUS
GSTATUS   = _LoKi.GenParticles.Status()
## @see LoKi::Cuts::GETA
GETA      = _LoKi.GenParticles.PseudoRapidity()
## @see LoKi::Cuts::GPHI
GPHI      = _LoKi.GenParticles.Phi()
## @see LoKi::Cuts::GTHETA
GTHETA    = _LoKi.GenParticles.Theta()
## @see LoKi::Cuts::GVEV
GVEV      = _LoKi.GenParticles.ValidEndVertex()
## @see LoKi::Cuts::GMOMDIST
GMOMDIST  = _LoKi.GenParticles.MomentumDistance
## @see LoKi::Cuts::GPTDIR
GPTDIR    = _LoKi.GenParticles.TransverseMomentumRel
## @see LoKi::Cuts::GNINTREE
GNINTREE  = _LoKi.GenParticles.NInTree
## @see LoKi::Cuts::GFROMTREE
GFROMTREE = _LoKi.GenParticles.FromHepMCTree
## @see LoKi::Cuts::GANCESTOR
GANCESTOR = _LoKi.GenParticles.IsAnAncestor
## @see LoKi::Cuts::GQUARK
GQUARK    = _LoKi.GenParticles.HasQuark
## @see LoKi::Cuts::GBEAUTY
##GBEAUTY   = GQUARK( _LHCb.ParticleID.bottom  )
GBEAUTY   = GQUARK( 5 )
## @see LoKi::Cuts::GCHARM
##GCHARM    = GQUARK( _LHCb.ParticleID.charm   )
GCHARM    = GQUARK( 4 )
## @see LoKi::Cuts::GSTRANGE
## GSTRANGE  = GQUARK( _LHCb.ParticleID.strange )
GSTRANGE  = GQUARK( 3 )
## @see LoKi::Cuts::GTOP
##GTOP      = GQUARK( _LHCb.ParticleID.top     )
GTOP      = GQUARK( 6 )
## @see LoKi::Cuts::GNEUTRAL
GNEUTRAL  = _LoKi.GenParticles.IsNeutral() 
## @see LoKi::Cuts::GLEPTON
GLEPTON   = _LoKi.GenParticles.IsLepton() 
## @see LoKi::Cuts::GMESON
GMESON   = _LoKi.GenParticles.IsMeson() 
## @see LoKi::Cuts::GBARYON
GBARYON  = _LoKi.GenParticles.IsBaryon() 
## @see LoKi::Cuts::GHADRON
GHADRON  = _LoKi.GenParticles.IsHadron() 
## @see LoKi::Cuts::GNUCLEUS
GNUCLEUS = _LoKi.GenParticles.IsNucleus() 
## @see LoKi::Cuts::GP
GP       = _LoKi.GenParticles.Momentum() 
## @see LoKi::Cuts::GPT
GPT      = _LoKi.GenParticles.TransverseMomentum() 
## @see LoKi::Cuts::GE
GE       = _LoKi.GenParticles.Energy()
## @see LoKi::Cuts::GM
GM       = _LoKi.GenParticles.Mass() 
## @see LoKi::Cuts::GPX
GPX      = _LoKi.GenParticles.MomentumX() 
## @see LoKi::Cuts::GPY
GPY      = _LoKi.GenParticles.MomentumY() 
## @see LoKi::Cuts::GPZ
GPZ      = _LoKi.GenParticles.MomentumZ() 
## @see LoKi::Cuts::GTIME
GTIME    = _LoKi.GenParticles.ProperLifeTime() 
## @see LoKi::Cuts::GCTAU
GTIME    = GTIME
## @see LoKi::Cuts::GNLT
GNLT     = _LoKi.GenParticles.NominalLifeTime() 
## @see LoKi::Cuts::GFAPVX
GFAPVX   = _LoKi.GenParticles.AdapterToProductionVertex
## @see LoKi::Cuts::GFAEVX
GFAEVX   = _LoKi.GenParticles.AdapterToEndVertex
## @see LoKi::Cuts::G3Q
G3Q      = _LoKi.GenParticles.ThreeCharge() 
## @see LoKi::Cuts::GMCMOTH
GMCMOTH  = _LoKi.GenParticles.IsAMotherForMC



if __name__ == '__main__' :
    print __doc__
    print "dir(%s) : %s" % ( __name__ , dir() ) 

# =============================================================================
# $Log: not supported by cvs2svn $
# Revision 1.4  2006/11/28 18:24:17  ibelyaev
#  prepare for v6r1
#
# =============================================================================
# The END 
# =============================================================================
