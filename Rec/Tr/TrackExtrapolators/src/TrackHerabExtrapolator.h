#ifndef TRACKHERABEXTRAPOLATOR_H
#define TRACKHERABEXTRAPOLATOR_H 1

// Include files
#include "TrackExtrapolator.h"

// Forward declarations

class IMagneticFieldSvc;

/** @class TrackHerabExtrapolator TrackHerabExtrapolator.h "TrackHerabExtrapolator.h"
 *
 *  A TrackHerabExtrapolator is a ITrackExtrapolator which does a 'HerabRK5'
 *  extrapolation of a State.
 *  It doesn't take into account Multiple Scattering.
 *
 *  @author Edwin Bos
 *  @date   06/07/2005
 *  @author Jose A. Hernando (14-03-05)
 *  @author Matt Needham
 *  @date   22-04-2000
 */

class TrackHerabExtrapolator: public TrackExtrapolator
{

public:
  /// Constructor
  TrackHerabExtrapolator( const std::string& type, 
                          const std::string& name, 
                          const IInterface* parent );

  /// destructor
  virtual ~TrackHerabExtrapolator();

  /// initialize
  virtual StatusCode initialize();


  /// Propagate a state to a given z-position
  virtual StatusCode propagate( State& pState, 
                                double z,
                                ParticleID partId = ParticleID(211) );

private:
 
  /// interface to Hera-b code
  void extrapolate( double& zIn,
                    double pIn[5], 
                    double& zNew, 
                    double pOut[5],
                    double fQp[25], 
                    int& istat);


  /// Interface to 4th order Runga-Kutta
  void rk4order( double& z_in ,
                 double* p_in	,
                 double& z_out,
                 double* p_out,
                 double* rkd,
                 int& ierror);

  /// Interface to fast 4th order Runga-Kutta  
  void rk4fast(  double& z_in ,
                 double* p_in ,
                 double& z_out ,
                 double* p_out,
                 double* rkd,
                 int& ierror );

  /// Interface to 5th order Runga-Kutta
  void rk5order( double& z_in ,
                 double* p_in ,
                 double& error ,
                 double& z_out ,
                 double* p_out,
                 double* rkd,
                 int& ierror);

  /// Interface to fast 5th order Runga-Kutta
  void rk5fast(  double& z_in ,
                 double* p_in,
                 double& error,
                 double& z_out,
                 double* p_out,
                 double* rkd,
                 int& ierror);

 /// interface to 5th order with derivatives caculated by numerical derivatives
  void rk5numde( double& z_in ,
                 double* p_in,
                 double& error,
                 double& z_out,
                 double* p_out,
                 double* rkd,
                 int& ierror);

  /// Without derivatives rkd and ierror flag 
  void rk5fast(  double& z_in, 
                 double* p_in, 
                 double& error, 
                 double& z_out,
                 double* p_out);

  /// Without derivatives rkd and ierror flag
  void rk4fast(  double& z_in, 
                 double* p_in, 
                 double& z_out, 
                 double* p_out);
 
  int m_extrapolatorID;

  double             m_error;  ///< Error
  HepPoint3D         m_point;  ///< to compute the field
  HepVector3D        m_B;      ///< returned field
  IMagneticFieldSvc* m_pIMF;   ///< Pointer to the magnetic field service

  // Parameters for Runge-Kutta
  double             m_qpCurls;    ///< Maximum curvature
  double             m_stepMin;    ///<
  double             m_stepMinRK5; ///<

};

#endif // TRACKHERABEXTRAPOLATOR_H





