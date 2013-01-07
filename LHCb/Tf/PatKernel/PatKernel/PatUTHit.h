#ifndef PATFORWARD_PAUTTHIT_H 
#define PATFORWARD_PAUTTHIT_H 1

#include "TfKernel/HitExtension.h"
#include "TfKernel/LineHit.h"
#include "TfKernel/STHit.h"
#include "Kernel/MemPoolAlloc.h"


  /** @class PatUTHit
   *  Extended hit class for PatAddUT algorithms
   *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
   *  @date   2007-06-019
   **/

  class PatUTHit : public Tf::HitExtension<Tf::LineHit>,
                   public LHCb::MemPoolAlloc<PatUTHit>
  {

  public:

    /// Constructor from an ST hit
    PatUTHit ( const Tf::STHit & stHit ) :
      Tf::HitExtension<Tf::LineHit>(stHit),
      m_x(stHit.xMid()),
      m_z(stHit.zMid()),
      m_projection(-999),
      m_planeCode( 2 * stHit.station() + (stHit.layer()%2) ) { }

    /// Desctructor
    ~PatUTHit() { }

    /// Accessors
    double x()             const { return m_x; }
    double z()             const { return m_z; }   
    int    mask()          const { return 1 << m_planeCode; }
    double projection()    const { return m_projection; }
    int    planeCode()     const { return m_planeCode; }

    /// Setters
    void setX( double x )                     { m_x = x; }
    void setZ( double z )                     { m_z = z; }
    void setProjection( double proj )         { m_projection = proj; }

  private:
    double  m_x;
    double  m_z;
    double  m_projection;
    int     m_planeCode;
 
  };
 
  typedef std::vector<PatUTHit*> PatUTHits;
 

#endif // PATFORWARD_PAUTTHit_H
