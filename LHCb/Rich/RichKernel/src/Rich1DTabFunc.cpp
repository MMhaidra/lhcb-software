// $Id: Rich1DTabFunc.cpp,v 1.2 2004-06-29 19:27:30 jonrob Exp $

// GaudiKernel
#include "GaudiKernel/Kernel.h" // Suppress "debug information truncated" warnings on Windows
#include "GaudiKernel/GaudiException.h"

// local
#include "RichKernel/Rich1DTabFunc.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Rich1DTabFunc
//
// 2003-08-13 : C. Jones       Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

//============================================================================

// Default Constructor
Rich1DTabFunc::Rich1DTabFunc() :
  m_OK                 ( false ),
  m_mainDistAcc        ( 0 ),
  m_mainDistSpline     ( 0 ),
  m_weightedDistAcc    ( 0 ),
  m_weightedDistSpline ( 0 ) { }

//============================================================================

// Constructor from arrays
Rich1DTabFunc::Rich1DTabFunc( const double x[],
                              const double y[],
                              const int size,
                              const gsl_interp_type * interType ) :
  m_OK                 ( false ),
  m_mainDistAcc        ( 0 ),
  m_mainDistSpline     ( 0 ),
  m_weightedDistAcc    ( 0 ),
  m_weightedDistSpline ( 0 )
{

  // copy data to internal container
  for ( int i = 0; i < size; ++i ) { m_data[ x[i] ] = y[i]; }

  // initialise interpolation
  m_OK = initInterpolator( interType );
}

//============================================================================

// Constructor from std::vector
Rich1DTabFunc::Rich1DTabFunc( const std::vector<double> & x,
                              const std::vector<double> & y,
                              const gsl_interp_type * interType ) :
  m_OK                 ( false ),
  m_mainDistAcc        ( 0 ),
  m_mainDistSpline     ( 0 ),
  m_weightedDistAcc    ( 0 ),
  m_weightedDistSpline ( 0 )
{

  // Check on size of containers
  if ( x.size() != y.size() ) {
    m_OK = false;
  } else {

    // copy data to internal container
    std::vector<double>::const_iterator ix,iy;
    for ( ix = x.begin(), iy = y.begin();
          ix != x.end(); ++ix, ++iy ) { m_data[*ix] = *iy; }

    // initialise interpolation
    m_OK = initInterpolator( interType );

  }

}

//============================================================================

// Constructor from std::map
Rich1DTabFunc::Rich1DTabFunc( const std::map<double,double> & data,
                              const gsl_interp_type * interType ) :
  m_data               ( data ),
  m_OK                 ( false ),
  m_mainDistAcc        ( 0 ),
  m_mainDistSpline     ( 0 ),
  m_weightedDistAcc    ( 0 ),
  m_weightedDistSpline ( 0 )
{
  // initialise interpolation
  m_OK = initInterpolator( interType );
}

//============================================================================

// initialise the interpolator
bool Rich1DTabFunc::initInterpolator( const gsl_interp_type * interType ) 
{

  // clean up first
  clearInterpolator();

  // Needs at least 2 points to work...
  const int size = m_data.size();
  if ( size < 2 ) return false;

  // Copy data to temporary initialisation arrays
  double * x  = new double[size];
  double * y  = new double[size];
  double * xy = new double[size];
  unsigned int i = 0;
  for ( std::map<double,double>::const_iterator iD = m_data.begin();
        iD != m_data.end(); ++iD, ++i ) {
    x[i] = (*iD).first;
    y[i] = (*iD).second;
    xy[i] = x[i]*y[i];
  }

  // Initialise the GSL interpolators
  m_mainDistAcc        = gsl_interp_accel_alloc();
  m_weightedDistAcc    = gsl_interp_accel_alloc();
  m_mainDistSpline     = gsl_spline_alloc ( interType, size );
  m_weightedDistSpline = gsl_spline_alloc ( interType, size );
  const int err1 = gsl_spline_init ( m_mainDistSpline,     x, y,  size );
  const int err2 = gsl_spline_init ( m_weightedDistSpline, x, xy, size );

  // delete temporary arrays
  delete[] x;
  delete[] y;
  delete[] xy;

  if ( err1 || err2 ) {
    throw GaudiException( "Error whilst initialising GSL interpolators",
                          "*Rich1DTabFunc*", StatusCode::FAILURE );
    return false;
  }

  return true;
}

//============================================================================

void Rich1DTabFunc::clearInterpolator()
{

  // Free GSL components
  if ( m_mainDistSpline ) {
    gsl_spline_free( m_mainDistSpline );
    m_mainDistSpline = 0;
  }
  if ( m_mainDistAcc ) {
    gsl_interp_accel_free( m_mainDistAcc );
    m_mainDistAcc = 0;
  }
  if ( m_weightedDistSpline ) {
    gsl_spline_free( m_weightedDistSpline );
    m_weightedDistSpline = 0;
  }
  if ( m_weightedDistAcc ) {
    gsl_interp_accel_free( m_weightedDistAcc );
    m_weightedDistAcc = 0;
  }

}
