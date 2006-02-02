#!/usr/bin/env python
# =============================================================================
# $Id: bendertools.py,v 1.5 2006-02-02 13:07:39 ibelyaev Exp $ 
# =============================================================================
# CVS tag $Name: not supported by cvs2svn $ version $Revison:$
# =============================================================================
# @file 
# @author Vanya BeLYAEV Ivan.BElyaev@itep.ru
# @date 2005-01-20
# =============================================================================
"""
Helper module  to locate/cast to different tools 
"""
# =============================================================================
__author__  = "Vanya BELYAEV Ivan.Belyaev@itep.ru"
__version__ = "CVS tag $Name: not supported by cvs2svn $ version: $Revision: 1.5 $ "
# =============================================================================

import gaudimodule

gaudimodule.loaddict('BenderDict')

_gbl = gaudimodule.gbl

IAlgTool       = _gbl.IAlgTool

IBestParticles           = _gbl.IBestParticles
IFilterCriterion         = _gbl.IFilterCriterion
IKFFitTool               = _gbl.IKFFitTool
IParticleStuffer         = _gbl.IParticleStuffer
IPlotTool                = _gbl.IPlotTool 
ICheckOverlap            = _gbl.ICheckOverlap    
IGeomDispCalculator      = _gbl.IGeomDispCalculator
ILifetimeFitter          = _gbl.ILifetimeFitter 
IParticleTransporter     = _gbl.IParticleTransporter
IOnOffline               = _gbl.IOnOffline          
IDecayFinder             = _gbl.IDecayFinder       
IGeomVertexFitter        = _gbl.IGeomVertexFitter  
IMassVertexFitter        = _gbl.IMassVertexFitter  
IPhotonFromMergedParams  = _gbl.IPhotonFromMergedParams
IVertexFitter            = _gbl.IVertexFitter      
IDecodeSimpleDecayString = _gbl.IGeomVertexFitter  
IParticleFilter          = _gbl.IParticleFilter    
IPhotonParams            = _gbl.IPhotonParams      
IDirectionFitter         = _gbl.IDirectionFitter
IParticleMaker           = _gbl.IParticleMaker
IPhysDesktop             = _gbl.IPhysDesktop
ITrExtrapolator          = _gbl.ITrExtrapolator
IMCDecayFinder           = _gbl.IMCDecayFinder
IMCEffBreakdown          = _gbl.IMCEffBreakdown
IMCEffReconstructed      = _gbl.IMCEffReconstructed
IMCEffReconstructible    = _gbl.IMCEffReconstructible
IMCParticleSelector      = _gbl.IMCParticleSelector
IVisPrimVertTool         = _gbl.IVisPrimVertTool 
IDebugTool               = _gbl.IDebugTool
IBTaggingTool            = _gbl.IBTaggingTool

Tools = ( IBestParticles           ,
          IFilterCriterion         ,
          IKFFitTool               ,
          IParticleStuffer         ,
          IPlotTool                ,
          ICheckOverlap            ,
          IGeomDispCalculator      ,
          ILifetimeFitter          ,
          IParticleTransporter     ,
          IOnOffline               ,
          IDecayFinder             ,
          IGeomVertexFitter        ,
          IMassVertexFitter        ,
          IPhotonFromMergedParams  ,
          IVertexFitter            ,
          IDecodeSimpleDecayString ,
          IParticleFilter          ,
          IPhotonParams            ,
          IDirectionFitter         ,
          IParticleMaker           ,
          IPhysDesktop             ,
          #
          ITrExtrapolator          ,
          #
          IMCDecayFinder           ,
          IMCEffBreakdown          ,
          IMCEffReconstructed      ,
          IMCEffReconstructible    ,
          IMCParticleSelector      ,
          IVisPrimVertTool         ,
          #
          IDebugTool               ,
          IBTaggingTool            )

Interface = _gbl.Bender.Interface

# =============================================================================
# The END 
# =============================================================================

