// $Id: GlobalToLocalDelta.cpp,v 1.11 2007-06-25 14:52:20 jpalac Exp $
// Include files 
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/IGeometryInfo.h"
// local
#include "DetDesc/GlobalToLocalDelta.h"
#include "DetDesc/3DTransformationFunctions.h"
#include "RotationZYX.h"
#include "3DConversions.h"
namespace DetDesc {

const Gaudi::Transform3D localDeltaMatrix(const IDetectorElement* DE,
                                          const Gaudi::Transform3D& globalDelta) {
  return localDeltaMatrix(DE->geometry(), globalDelta);
}

const Gaudi::Transform3D localDeltaMatrix(const IGeometryInfo* GI,
                                          const Gaudi::Transform3D& toMisaligned_global) {

  const Gaudi::Transform3D d_0 = GI->localDeltaMatrix().Inverse();
  const Gaudi::Transform3D T   = GI->matrixInv();
  return d_0 * T.Inverse() * toMisaligned_global * T;

}

void localDeltaParameters(const IDetectorElement* DE,
                          std::vector<double>& trans,
                          std::vector<double>& rot,
                          std::vector<double>& pivot) {

  DetDesc::localDeltaParameters(DE->geometry(), trans, rot, pivot);

}

void localDeltaParameters(const IGeometryInfo* GI,
                          std::vector<double>& trans,
                          std::vector<double>& rot,
                          std::vector<double>& pivot) {
  
  const Gaudi::Transform3D globalDelta = DetDesc::localToGlobalTransformation(trans, rot, pivot);
  const Gaudi::Transform3D localDelta  = DetDesc::localDeltaMatrix(GI, globalDelta);
  // get the pivot from the alignment condition
  // @todo find a way not to require this awful hard-wired string.
  const std::string pivotString("pivotXYZ");

  std::vector<double> tmp = (GI->alignmentCondition()->exists(pivotString) ) ? 
    GI->alignmentCondition()->paramAsDoubleVect(pivotString) : std::vector<double>(3, 0);

  std::copy( tmp.begin(), tmp.end(), pivot.begin() );

  DetDesc::getZYXTransformParameters(localDelta, trans, rot, pivot);

}
  
} // namespace DetDesc
//=============================================================================
