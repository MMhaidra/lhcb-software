//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtSLPoleFF.cc
//
// Description: Routine to implement semileptonic form factors
//              according to the model SLPoles
//
// Modification history:
//
//    DJL       April 17,1998       Module created
//
//------------------------------------------------------------------------
// 
#ifdef WIN32 
  #pragma warning( disable : 4786 ) 
  // Disable anoying warning about symbol size 
#endif 
#include "EvtGenBase/EvtPatches.hh"
#include "EvtGenBase/EvtReport.hh"
#include "EvtGenModels/EvtSLPoleFF.hh"
#include <string>
#include "EvtGenBase/EvtPDL.hh"
#include <math.h>

EvtSLPoleFF::EvtSLPoleFF(int numarg, double *arglist) {
   numSLPoleargs = numarg;
   for (int i=0; i<numarg; i++) {
     SLPoleargs[i] = arglist[i]; }

   return;
}


void EvtSLPoleFF::getscalarff(EvtId parent,EvtId /*daught*/,
                              double t, double /*mass*/, double *fpf,
                              double *f0f ) {

// Form factors have a general form, with parameters passed in
// from the arguements.

   if ( numSLPoleargs !=8 ) {
     report(ERROR,"EvtGen") << "Problem in EvtSLPoleFF::getscalarff\n";
     report(ERROR,"EvtGen") << "wrong number of arguements!!!\n";
   }

  double mb=EvtPDL::getMeanMass(parent);
  double mb2 = mb*mb;

  double f0,af,bf,powf;

  f0 = SLPoleargs[0];
  af = SLPoleargs[1];
  bf = SLPoleargs[2];
  powf = SLPoleargs[3];
  *fpf = f0/(pow( 1.0 + (af*t/mb2) + (bf*((t/mb2)*(t/mb2))),powf));

  f0 = SLPoleargs[4];
  af = SLPoleargs[5];
  bf = SLPoleargs[6];
  powf = SLPoleargs[7];

  *f0f = f0/(pow( 1.0 + (af*t/mb2) + (bf*((t/mb2)*(t/mb2))),powf)); 

  return;
}

void EvtSLPoleFF::getvectorff(EvtId parent,EvtId /*daught*/,
                              double t, double /*mass*/, double *a1f,
                              double *a2f, double *vf, double *a0f ){

  if ( numSLPoleargs !=16 ) {
     report(ERROR,"EvtGen") << "Problem in EvtSLPoleFF::getvectorff\n";
     report(ERROR,"EvtGen") << "wrong number of arguements!!!\n";
     report(ERROR,"EvtGen") << numSLPoleargs<<"\n";
  }

  double mb=EvtPDL::getMeanMass(parent);
  double mb2 = mb*mb;

  double f0,af,bf,powf;

  f0 = SLPoleargs[0];
  af = SLPoleargs[1];
  bf = SLPoleargs[2];
  powf = SLPoleargs[3];
  *a1f = f0/(pow( 1.0 + (af*t/mb2) + (bf*((t/mb2)*(t/mb2))),powf));

  f0 = SLPoleargs[4];
  af = SLPoleargs[5];
  bf = SLPoleargs[6];
  powf = SLPoleargs[7];

  *a2f = f0/(pow( 1.0 + (af*t/mb2) + (bf*((t/mb2)*(t/mb2))),powf));

  f0 = SLPoleargs[8];
  af = SLPoleargs[9];
  bf = SLPoleargs[10];
  powf = SLPoleargs[11];

  *vf = f0/(pow( 1.0 + (af*t/mb2) + (bf*((t/mb2)*(t/mb2))),powf));

  f0 = SLPoleargs[12];
  af = SLPoleargs[13];
  bf = SLPoleargs[14];
  powf = SLPoleargs[15];

  *a0f = f0/(pow( 1.0 + (af*t/mb2) + (bf*((t/mb2)*(t/mb2))),powf));
  return;
 }



void EvtSLPoleFF::gettensorff(EvtId parent,EvtId /*daught*/,
                              double t, double /*mass*/, double *hf,
                              double *kf, double *bpf, double *bmf ){

  if ( numSLPoleargs !=16 ) {
     report(ERROR,"EvtGen") << "Problem in EvtSLPoleFF::gettensorff\n";
     report(ERROR,"EvtGen") << "wrong number of arguements!!!\n";
  }

  double mb=EvtPDL::getMeanMass(parent);
  double mb2 = mb*mb;

  double f0,af,bf,powf;

  f0 = SLPoleargs[0];
  af = SLPoleargs[1];
  bf = SLPoleargs[2];
  powf = SLPoleargs[3];
  *hf = f0/(pow( 1.0 + (af*t/mb2) + (bf*((t/mb2)*(t/mb2))),powf));

  f0 = SLPoleargs[4];
  af = SLPoleargs[5];
  bf = SLPoleargs[6];
  powf = SLPoleargs[7];

  *kf = f0/(pow( 1.0 + (af*t/mb2) + (bf*((t/mb2)*(t/mb2))),powf));

  f0 = SLPoleargs[8];
  af = SLPoleargs[9];
  bf = SLPoleargs[10];
  powf = SLPoleargs[11];

  *bpf = f0/(pow( 1.0 + (af*t/mb2) + (bf*((t/mb2)*(t/mb2))),powf));

  f0 = SLPoleargs[12];
  af = SLPoleargs[13];
  bf = SLPoleargs[14];
  powf = SLPoleargs[15];

  *bmf = f0/(pow( 1.0 + (af*t/mb2) + (bf*((t/mb2)*(t/mb2))),powf));
  return;
 }

