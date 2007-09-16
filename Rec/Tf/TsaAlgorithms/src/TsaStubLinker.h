// $ Exp $
#ifndef _TsaStubLinker_H
#define _TsaStubLinker_H

#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"

#include <algorithm>

#include "TsaKernel_Tf/ITsaStubLinker.h"
#include "TsaKernel_Tf/SeedFun.h"
#include "TsaKernel_Tf/SeedStub.h"
#include "TsaKernel_Tf/SeedTrack.h"
#include "TsaKernel_Tf/SeedPnt.h"
#include "TsaKernel_Tf/TsaConstants.h"

#include "SeedParabolaFit.h"
#include "SeedLineFit.h"

#include "LHCbMath/GeomFun.h"

namespace Tf
{
  namespace Tsa
  {

    /** @class StubLinker TsaStubLinker.h
     * Link stubs in the IT
     * @author M. Needham
     **/

    class StubLinker:  public GaudiTool,
                       virtual public ITsaStubLinker
    {

    public:

      /// constructer
      StubLinker(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

      // destructer
      virtual ~StubLinker();

      virtual StatusCode initialize();

      virtual StatusCode execute(std::vector<SeedStub*> stubs[], std::vector<SeedTrack*>& seeds)  ;

    private:

      double xSlopeCon(const SeedStub* point1, const SeedStub* point2) const;

      SeedLineFit* m_fitLine;
      SeedParabolaFit* m_parabolaFit;

      double m_deltaSx;
      double m_deltaXCon;

    };

    inline double StubLinker::xSlopeCon(const SeedStub* point1,
                                        const SeedStub* point2) const
    {
      double slopeChord = (point1->x() - point2->x())/ (point1->z1()-point2->z1());
      return(slopeChord - 0.5*(point1->sx()+point2->sx()));
    }

  }
}

#endif  // _TsaStubLinker_H



